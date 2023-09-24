#pragma once
#include "SimilarityMatrix.h"
#include <opencv2/opencv.hpp>

using namespace std;

// SimilarityMeasure
// - Measures similarity between video frames

class SimilarityMeasure {
	public:
		// Static Methods
		static SimilarityMatrix ComputeEuclideanSimilarityMatrix(string videoFilePath);
		static SimilarityMatrix ComputeMotionSimilarityMatrix(string videoFilePath, SimilarityMatrix euclideanSimilarityMatrix);
		static SimilarityMatrix ComputeFutureCostSimilarityMatrix(string videoFilePath, SimilarityMatrix motionSimilarityMatrix);
		
	private:
		// Static Methods
		static string ComputeNewFilePath(string originalFilePath, string matrixRepresentation, string matrixType, string extension);
		static cv::Mat RemoveInvalidFrames(cv:: Mat input);
};

