#include "VideoPreprocessing.h"

using namespace cv;
using namespace std;

//--------------------------------------------------------------------------------------
// Static Methods (Public)
//--------------------------------------------------------------------------------------

// Performs video stabilisation
string VideoPreprocessing::VideoStabilisation(string inputVideoFilePath) {
	string outputFilePath = ComputeOutputVideoFilePath(inputVideoFilePath);

	VideoCapture inputVideo(inputVideoFilePath);
	int frameCount = inputVideo.get(CAP_PROP_FRAME_COUNT);
	VideoWriter output(outputFilePath, inputVideo.get(CAP_PROP_FOURCC), inputVideo.get(CAP_PROP_FPS), Size2i(inputVideo.get(CAP_PROP_FRAME_WIDTH), inputVideo.get(CAP_PROP_FRAME_HEIGHT)));

	// 1. Get motion between consecutive frames by tracking feature points between them
	Mat currentFrame, previousFrame, previousTransformationMatrix;
    vector<vector<double>> transformations; // [motion in x-direction, motion in y-direction, change in angle]

	inputVideo >> previousFrame;
	cvtColor(previousFrame, previousFrame, COLOR_BGR2GRAY);

	for (int i = 1; i < frameCount; i++) {
		// Find feature points to track between frames
		vector<Point2f> currentFrameFeaturePoints, previousFrameFeaturePoints;
		goodFeaturesToTrack(previousFrame, previousFrameFeaturePoints, 200, 0.01, 30);

		bool success = inputVideo.read(currentFrame);
		if (!success) break;
		cvtColor(currentFrame, currentFrame, COLOR_BGR2GRAY);

		// Track feature points from previous frame into current frame
		vector<uchar> status;
		vector<float> errors;
		calcOpticalFlowPyrLK(previousFrame, currentFrame, previousFrameFeaturePoints, currentFrameFeaturePoints, status, errors);

		// Remove feature points that could not be tracked between frames
		auto previousFeaturePoint = previousFrameFeaturePoints.begin();
		auto currentFeaturePoint = currentFrameFeaturePoints.begin();

		for (int k = 0; k < status.size(); k++) {
			if (status[k]) {
				previousFeaturePoint++;
				currentFeaturePoint++;
			}
			else {
				previousFeaturePoint = previousFrameFeaturePoints.erase(previousFeaturePoint);
				currentFeaturePoint = currentFrameFeaturePoints.erase(currentFeaturePoint);
			}
		}

		// Find transformation matrix that maps previous frame to current frame
		Mat transformationMatrix = estimateAffinePartial2D(previousFrameFeaturePoints, currentFrameFeaturePoints);

		// If we cannot find a transformation, we will just use the last known good transformation
		if (transformationMatrix.data == NULL) {
			previousTransformationMatrix.copyTo(transformationMatrix);
			transformationMatrix.copyTo(previousTransformationMatrix);
		}

		// Add this transformation to vector of all transformations
		vector<double> currentTransformations;
		currentTransformations.push_back(transformationMatrix.at<double>(0, 2));
		currentTransformations.push_back(transformationMatrix.at<double>(1, 2));
		currentTransformations.push_back((atan2(transformationMatrix.at<double>(1, 0), transformationMatrix.at<double>(0, 0))));
		transformations.push_back(currentTransformations);

		// Move to next frame
		currentFrame.copyTo(previousFrame);
	}

	// 2. Calculate overall trajectory of motion in the video
	vector<vector<double>> trajectory = GetOverallTrajectoryOfMotion(transformations);

	// 3. Smooth trajectory of motion & calculate the smooth transformations for each frame
	vector<vector<double>> smoothedTrajectory = SmoothTrajectoryOfMotion(trajectory);

	vector<vector<double>> smoothTransformations;
	for (int i = 0; i < transformations.size(); i++) {
		vector<double> currentSmoothTransformations;

		currentSmoothTransformations.push_back(transformations[i][0] + (smoothedTrajectory[i][0] - trajectory[i][0]));
		currentSmoothTransformations.push_back(transformations[i][1] + (smoothedTrajectory[i][1] - trajectory[i][1]));
		currentSmoothTransformations.push_back(transformations[i][2] + (smoothedTrajectory[i][2] - trajectory[i][2]));

		smoothTransformations.push_back(currentSmoothTransformations);
	}

	// 4. Apply smooth transformations to frames of the input video
	inputVideo.set(CAP_PROP_POS_FRAMES, 1);
	Mat transformationMatrix(2, 3, CV_64F);
	Mat frame, stabilisedFrame;

	for (int i = 0; i < frameCount - 1; i++) {;
		bool success = inputVideo.read(frame);
		if (!success) break;

		transformationMatrix = GetTransformationMatrix(smoothTransformations[i]);
		warpAffine(frame, stabilisedFrame, transformationMatrix, frame.size());

		// 6. Fix border artifacts that may have arisen due to the stabilisation
		Mat transformationMatrix = getRotationMatrix2D(Point2f(stabilisedFrame.cols / 2, stabilisedFrame.rows / 2), 0, 1.04);
		warpAffine(stabilisedFrame, stabilisedFrame, transformationMatrix, stabilisedFrame.size());

		output.write(stabilisedFrame);
	}

	output.release();

	return outputFilePath;
}

// Reduces video resolution whilst maintaining aspect ratio
string VideoPreprocessing::ReduceVideoResolution(string inputVideoFilePath, int newResolution) {
	string outputFilePath = ComputeOutputVideoFilePath(inputVideoFilePath);

	VideoCapture inputVideo(inputVideoFilePath);

	// Determine new resolution whilst maintaining aspect ratio
	int height = newResolution;
	int width = inputVideo.get(CAP_PROP_FRAME_WIDTH) / (inputVideo.get(CAP_PROP_FRAME_HEIGHT) / newResolution);

	// Resize all frames in input video
	VideoWriter outputVideo(outputFilePath, inputVideo.get(CAP_PROP_FOURCC), inputVideo.get(CAP_PROP_FPS), Size2i(width, height));

	if (inputVideo.isOpened() && outputVideo.isOpened()) {

		for (int i = 0; i < inputVideo.get(CAP_PROP_FRAME_COUNT); i++) {
			Mat frame, resizedFrame;

			inputVideo.set(1, i);
			bool success = inputVideo.read(frame);
			if (!success) break;

			resize(frame, resizedFrame, Size(width, height));
			outputVideo << resizedFrame;
		}

		outputVideo.release();
	}


	return outputFilePath;
}

//--------------------------------------------------------------------------------------
// Static Methods (Private)
//--------------------------------------------------------------------------------------

// Computes overall trajectory of motion in the video
vector<vector<double>> VideoPreprocessing::GetOverallTrajectoryOfMotion(vector<vector<double>> transformations) {
	vector<vector<double>> trajectory;
	double x = 0;
	double y = 0;
	double angle = 0;

	for (int i = 0; i < transformations.size(); i++) {
		x += transformations[i][0];
		y += transformations[i][1];
		angle += transformations[i][2];

		vector<double> currentTrajectory = { x, y, angle };
		trajectory.push_back(currentTrajectory);
	}

	return trajectory;
}

// Smooth overall trajectory of motion
vector<vector<double>> VideoPreprocessing::SmoothTrajectoryOfMotion(vector<vector<double>> trajectory) {
	vector<vector<double>> smoothedTrajectory;
	int radius = 50;

	for (int i = 0; i < trajectory.size(); i++) {
		double sum_x = 0;
		double sum_y = 0;
		double sum_angle = 0;
		int count = 0;

		for (int j = 0; j <= radius; j++) {
			if ((i + j >= 0) && (i + j < trajectory.size())) {
				sum_x += trajectory[i + j][0];
				sum_y += trajectory[i + j][1];
				sum_angle += trajectory[i + j][2];
				count++;
			}
		}

		vector<double> currentSmoothedTrajectory = { sum_x / count, sum_y / count, sum_angle / count };
		smoothedTrajectory.push_back(currentSmoothedTrajectory);
	}
	return smoothedTrajectory;
}

// Converts changes in motion/angle to transformation matrix
cv::Mat VideoPreprocessing::GetTransformationMatrix(vector<double> transformations) {
	Mat output(2, 3, CV_64F);

	output.at<double>(0, 0) = cos(transformations[2]);
	output.at<double>(0, 1) = -sin(transformations[2]);
	output.at<double>(1, 0) = sin(transformations[2]);
	output.at<double>(1, 1) = cos(transformations[2]);
	output.at<double>(0, 2) = transformations[0];
	output.at<double>(1, 2) = transformations[1];

	return output;
}

// Computes file path for output video
string VideoPreprocessing::ComputeOutputVideoFilePath(string inputVideoFilePath) {
	string filename = (inputVideoFilePath.substr(inputVideoFilePath.find_last_of("/\\") + 1));
	filename = filename.substr(0, filename.find('.'));

	return (filename + "_PREPROCESSING.mp4");
}