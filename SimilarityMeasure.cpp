#include "SimilarityMeasure.h"
#include "Utilities.cpp"

using namespace std;
using namespace cv;
using namespace utils_;

//--------------------------------------------------------------------------------------
// Static Methods (Public)
//--------------------------------------------------------------------------------------

// Creates similarity matrix by calculating Euclidean distance between individual frames
SimilarityMatrix SimilarityMeasure::ComputeEuclideanSimilarityMatrix(string videoFilePath) {
    VideoCapture inputVideo(videoFilePath);

    if (inputVideo.isOpened()) {
        int frameCount = int(inputVideo.get(CAP_PROP_FRAME_COUNT));
        Mat distanceMatrix(frameCount, frameCount, CV_32F), frame1, frame2;

        // Calculate Euclidean distance between frames
        for (int i = 0; i < frameCount; i++) {
            inputVideo.set(1, i);
            bool success = inputVideo.read(frame1);
            if (!success) break;
            for (int j = 0; j < frameCount; j++) {
                inputVideo.set(1, j);
                bool success = inputVideo.read(frame2);
                if (!success) break;
                distanceMatrix.at<float>(i, j) = norm(frame1, frame2, NORM_L2);
            }
        }

        SimilarityMatrix output(distanceMatrix);

        // Save matrices as CSV and image files
        output.SaveDistanceMatrixAsCSV(ComputeNewFilePath(videoFilePath, "distance", "euclidean", "csv"));
        output.SaveProbabilityMatrixAsCSV(ComputeNewFilePath(videoFilePath, "probability", "euclidean", "csv"));
        output.SaveDistanceMatrixAsImage(ComputeNewFilePath(videoFilePath, "distance", "euclidean", "png"));
        output.SaveProbabilityMatrixAsImage(ComputeNewFilePath(videoFilePath, "probability", "euclidean", "png"));

        return output;
    }

    return SimilarityMatrix();
}

// Creates similarity matrix by calculating Euclidean distance between sequences of frames
SimilarityMatrix SimilarityMeasure::ComputeMotionSimilarityMatrix(string videoFilePath, SimilarityMatrix euclideanSimilarityMatrix) {

    Mat euclideanDistanceMatrix = euclideanSimilarityMatrix.GetDistanceMatrix();
    int frameCount = euclideanDistanceMatrix.rows;
    Mat distanceMatrix(frameCount, frameCount, CV_32F);

    int m = 2;
    int weights[] = { 1, 1, 1, 1 };

    for (int i = 0; i < frameCount; i++) {
        for (int j = 0; j < frameCount; j++) {

            // Calculate Dij
            float newDistance = 0;
            for (int k = -1 * m; k < m; k++) {
                if ((k + i < 0) || (k + j < 0) || (k + i >= frameCount) || (k + j >= frameCount)) {
                    newDistance = -1;
                    break;
                }
                else
                    newDistance += weights[k + 2] * euclideanDistanceMatrix.at<float>(i + k, j + k);
            }
            distanceMatrix.at<float>(i, j) = newDistance;
        }
    }

    distanceMatrix = RemoveInvalidFrames(distanceMatrix);

    SimilarityMatrix output(distanceMatrix);

    // Save matrices as CSV and image files
    output.SaveDistanceMatrixAsCSV(ComputeNewFilePath(videoFilePath, "distance", "motion", "csv"));
    output.SaveProbabilityMatrixAsCSV(ComputeNewFilePath(videoFilePath, "probability", "motion", "csv"));
    output.SaveDistanceMatrixAsImage(ComputeNewFilePath(videoFilePath, "distance", "motion", "png"));
    output.SaveProbabilityMatrixAsImage(ComputeNewFilePath(videoFilePath, "probability", "motion", "png"));

    return output;
}

// Incorporates future cost into distance calculations
SimilarityMatrix SimilarityMeasure::ComputeFutureCostSimilarityMatrix(string videoFilePath, SimilarityMatrix motionSimilarityMatrix) {
    Mat distanceMatrix, motionDistanceMatrixPowP, probabilityMatrix;
    Mat motionDistanceMatrix = motionSimilarityMatrix.GetDistanceMatrix();

    double p = 0.75;
    float alpha = 0.995;

    pow(motionDistanceMatrix, p, motionDistanceMatrixPowP);
    distanceMatrix = motionDistanceMatrixPowP;
    Mat distanceMatrixLowestValuePerRow = GetMatrixRowLowestValues(distanceMatrix);

    bool change = true;
    while (change) {
        change = false;
        Mat prev = distanceMatrixLowestValuePerRow;
        distanceMatrixLowestValuePerRow = GetMatrixRowLowestValues(distanceMatrix);

        Mat dst;
        bitwise_xor(distanceMatrixLowestValuePerRow, prev, dst);
        if (countNonZero(dst) > 0)
            break;

        for (int i = distanceMatrix.rows - 1; i >= 0; i--) {
            for (int j = 0; j < distanceMatrix.cols; j++) {
                // Calculate D''_ij = (D'_ij)^p + alpha * min_k(D''_jk)
                distanceMatrix.at<float>(i, j) = motionDistanceMatrixPowP.at<float>(i, j) + (alpha * distanceMatrixLowestValuePerRow.at<float>(j));
            }
        }
    }

    SimilarityMatrix output(distanceMatrix);

    // Save matrices as CSV and image files
    output.SaveDistanceMatrixAsCSV(ComputeNewFilePath(videoFilePath, "distance", "future", "csv"));
    output.SaveProbabilityMatrixAsCSV(ComputeNewFilePath(videoFilePath, "probability", "future", "csv"));
    output.SaveDistanceMatrixAsImage(ComputeNewFilePath(videoFilePath, "distance", "future", "png"));
    output.SaveProbabilityMatrixAsImage(ComputeNewFilePath(videoFilePath, "probability", "future", "png"));

    return output;
}

//--------------------------------------------------------------------------------------
// Static Methods (Private)
//--------------------------------------------------------------------------------------

// Computes new file path when saving distance/probability matrix
string SimilarityMeasure::ComputeNewFilePath(string originalFilePath, string matrixRepresentation, string matrixType, string extension) {
    // Parameters:
    // - originalFilePath: file path for video
    // - matrixRepresentation: distance or probability matrix
    // - matrixType: Euclidean, motion or future cost
    // - extension: file format

    string filename = (originalFilePath.substr(originalFilePath.find_last_of("/\\") + 1));
    filename = filename.substr(0, filename.find('.'));

    if (extension == "csv")
        filename = filename + "_" + ToUpper(matrixRepresentation) + "_MATRIX_(" + ToUpper(matrixType) + ")." + extension;
    else if (extension == "png")
        filename = filename + "_" + ToUpper(matrixRepresentation) + "_IMAGE_(" + ToUpper(matrixType) + ")." + extension;

    return filename;
}

// Removes frames from matrix that are not used in calculations
Mat SimilarityMeasure::RemoveInvalidFrames(Mat input) {
    Mat output(input.rows, input.cols, CV_32F);

    output = RemoveMatrixColumn(input, 0);
    output = RemoveMatrixColumn(output, 0);
    output = RemoveMatrixColumn(output, output.cols - 1);
    output = RemoveMatrixRow(output, 0);
    output = RemoveMatrixRow(output, 0);
    output = RemoveMatrixRow(output, output.rows - 1);

    return output;
}