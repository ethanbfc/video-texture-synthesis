#include "Rendering.h"

using namespace cv;

//--------------------------------------------------------------------------------------
// Static Methods (Public)
//--------------------------------------------------------------------------------------

// Applies cross-fading to transitions then saves list of frames to video
string Rendering::CreateVideoTextureWithCrossFading(string inputVideoFilePath, CompoundLoop transitions, int windowSize) {

	int windowSizeSplit = (windowSize - 1) / 2;

	// Get sequence of frames
	vector<vector<int>> sequencesOfFrames = GetFrameSequence(transitions);

	// Write sequence of frames to video
	VideoCapture inputVideo(inputVideoFilePath);

	string outputVideoFilePath = ComputeOutputFilePath(inputVideoFilePath);
	VideoWriter outputVideo(outputVideoFilePath, inputVideo.get(CAP_PROP_FOURCC), inputVideo.get(CAP_PROP_FPS), Size2i(inputVideo.get(CAP_PROP_FRAME_WIDTH), inputVideo.get(CAP_PROP_FRAME_HEIGHT)));

	if (inputVideo.isOpened() && outputVideo.isOpened()) {

		for (int i = 0; i < sequencesOfFrames.size(); i++) {
			for (int j = windowSizeSplit; j < sequencesOfFrames[i].size(); j++) {

				// Perform cross-fading
				if (j == sequencesOfFrames[i].size() - windowSizeSplit - 1) {

					// If next sequence overflows, then we loop back to the first sequence
					int nextSequence = (i + 1 < sequencesOfFrames.size()) ? i + 1 : 0;

					// Get frame pairs
					for (int k = 0; k < windowSize; k++) {
						Mat frame1, frame2;

						inputVideo.set(1, sequencesOfFrames[i][j] + k);
						bool success = inputVideo.read(frame1);
						if (!success) break;

						inputVideo.set(1, sequencesOfFrames[nextSequence][k] - windowSizeSplit - 1);
						success = inputVideo.read(frame2);
						if (!success) break;

						// Get new frame via cross-fading
						double weight = (k + 1) / (windowSize + 1.0);
						Mat newFrame = ((1 - weight) * frame1) + (weight * frame2);
						outputVideo << newFrame;
					}

					break;
				}
				else {
					Mat frame;

					inputVideo.set(1, sequencesOfFrames[i][j]);
					bool success = inputVideo.read(frame);
					if (!success) break;

					outputVideo << frame;
				}
			}
		}

		outputVideo.release();
	}

	return outputVideoFilePath;
}

// Applies morphing to transitions then saves list of frames to video
string Rendering::CreateVideoTextureWithMorphing(string inputVideoFilePath, CompoundLoop transitions, int parameters[]) {

	// Get sequence of frames
	vector<vector<int>> sequencesOfFrames = GetFrameSequence(transitions);

	// Write sequence of frames to video
	VideoCapture inputVideo(inputVideoFilePath);

	string outputVideoFilePath = ComputeOutputFilePath(inputVideoFilePath);
	VideoWriter outputVideo(outputVideoFilePath, inputVideo.get(CAP_PROP_FOURCC), inputVideo.get(CAP_PROP_FPS), Size2i(inputVideo.get(CAP_PROP_FRAME_WIDTH), inputVideo.get(CAP_PROP_FRAME_HEIGHT)));

	if (inputVideo.isOpened() && outputVideo.isOpened()) {

		for (int i = 0; i < sequencesOfFrames.size(); i++) {
			for (int j = 1; j < sequencesOfFrames[i].size(); j++) {

				// Perform morphing
				if (j == sequencesOfFrames[i].size() - 1) {

					// If next sequence overflows, then we loop back to the first sequence
					int nextSequence = (i + 1 < sequencesOfFrames.size()) ? i + 1 : 0;

					// Get transition frames
					Mat prev, prevGrey, current, currentGrey, opticalFlow, newFrame;

					inputVideo.set(1, sequencesOfFrames[i][j]);
					bool success = inputVideo.read(prev);
					if (!success) break;

					inputVideo.set(1, sequencesOfFrames[nextSequence][0]);
					success = inputVideo.read(current);
					if (!success) break;

					// Compute optical flow between frames
					cvtColor(prev, prevGrey, COLOR_BGR2GRAY);
					cvtColor(current, currentGrey, COLOR_BGR2GRAY);

					double polySigma = 1.1 + (0.2 * (parameters[2] - 5));
					calcOpticalFlowFarneback(currentGrey, prevGrey, opticalFlow, 0.5, 5, parameters[1], 10, parameters[2], polySigma, 0);

					// Create new frames via interpolation
					for (int k = 0; k < parameters[0]; k++) {

						double weight = double(k) / parameters[0];

						Mat mapX(opticalFlow.size(), CV_32FC1);
						Mat mapY(opticalFlow.size(), CV_32FC1);

						for (int y = 0; y < mapX.rows; ++y)
						{
							for (int x = 0; x < mapX.cols; ++x)
							{
								Point2f f = opticalFlow.at<Point2f>(y, x);
								mapX.at<float>(y, x) = x + (weight * f.x);
								mapY.at<float>(y, x) = y + (weight * f.y);
							}
						}

						remap(prev, newFrame, mapX, mapY, parameters[3], BORDER_REFLECT);
						outputVideo << newFrame;
					}

					break;
					outputVideo << current;
				}
				else {
					Mat frame;

					inputVideo.set(1, sequencesOfFrames[i][j]);
					bool success = inputVideo.read(frame);
					if (!success) break;

					outputVideo << frame;
				}
			}
		}

		outputVideo.release();
	}

	return outputVideoFilePath;
}

//--------------------------------------------------------------------------------------
// Static Methods (Private)
//--------------------------------------------------------------------------------------

// Converts transition set into multiple sequences of frames
vector<vector<int>> Rendering::GetFrameSequence(CompoundLoop compoundLoopofTransitions) {

	// Get sequence of frames to write to video
	vector<Transition> transitions = compoundLoopofTransitions.GetTransitions();
	vector<vector<int>> sequencesOfFrames;
	vector<int> currentSequenceOfFrames;

	// Start from the destination frame of the transition with the latest source frame (i.e. transitions[0].destinationFrame)
	int startFrame = transitions[0].GetDestinationFrame();

	for (int i = 1; i < transitions.size(); i++) {

		// Go from startFrame to the source frame of the next transition
		for (int j = startFrame; j <= transitions[i].GetSourceFrame(); j++) {
			currentSequenceOfFrames.push_back(j);
		}

		sequencesOfFrames.push_back(currentSequenceOfFrames);
		currentSequenceOfFrames.clear();

		// Take transition i
		startFrame = transitions[i].GetDestinationFrame() + 1;
	}

	// Final transition is to frame before source frame of the original starting transition
	for (int j = startFrame; j <= transitions[0].GetSourceFrame() - 1; j++) {
		currentSequenceOfFrames.push_back(j);
	}

	sequencesOfFrames.push_back(currentSequenceOfFrames);

	return sequencesOfFrames;
}

// Computes file path for output video texture
string Rendering::ComputeOutputFilePath(string inputFilePath) {
	string filename = (inputFilePath.substr(inputFilePath.find_last_of("/\\") + 1));
	filename = filename.substr(0, filename.find('.'));

	return (filename + "_VIDEO_TEXTURE_RENDERING.mp4");
}