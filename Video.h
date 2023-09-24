#pragma once
#include "SimilarityMatrix.h"
#include <string>

using namespace std;

// Video
// - Stores related data to an input video

class Video {
	public:
		// Constructors
		Video();
		
		// Getters & Setters
		string GetVideoFilePath();
		void SetVideoFilePath(string fp);
		string GetEuclideanFilePath();
		void SetEuclideanFilePath(string fp);
		string GetMotionFilePath();
		void SetMotionFilePath(string fp);
		string GetFutureCostFilePath();
		void SetFutureCostFilePath(string fp);
		SimilarityMatrix GetEuclidean();
		void SetEuclidean(SimilarityMatrix e);
		SimilarityMatrix GetMotion();
		void SetMotion(SimilarityMatrix m);
		SimilarityMatrix GetFutureCost();
		void SetFutureCost(SimilarityMatrix fc);

		// Instance Methods
		void SetFilePaths(string option);

	private:
		// Parameters
		string videoFilePath;
		string euclideanFilePath;
		string motionFilePath;
		string futureCostFilePath;
		SimilarityMatrix euclidean;
		SimilarityMatrix motion;
		SimilarityMatrix futureCost;
};

