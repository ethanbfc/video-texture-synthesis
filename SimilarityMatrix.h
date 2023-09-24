#pragma once
#include <opencv2/opencv.hpp>

using namespace std;

// SimilarityMatrix
// - Stores distance and probabilistic representations of similarities between frames

class SimilarityMatrix {
	public:
		// Constructors
		SimilarityMatrix();
		SimilarityMatrix(cv::Mat dm);

		// Getters & Setters
		cv::Mat GetDistanceMatrix();
		void SetDistanceMatrix(cv::Mat dm);
		cv::Mat GetProbabilityMatrix();

		// Instance Methods
		void SaveDistanceMatrixAsCSV(string filePath);
		void SaveProbabilityMatrixAsCSV(string filePath);
		void SaveDistanceMatrixAsImage(string filePath);
		void SaveProbabilityMatrixAsImage(string filePath);

	private:
		// Parameters
		cv::Mat distanceMatrix;
		cv::Mat probabilityMatrix;

		// Instance Methods
		void MapDistancesToProbabilities();
		float GetAverageValue();
		void NormaliseProbabilityMatrix();
};

