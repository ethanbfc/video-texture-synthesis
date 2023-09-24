#pragma once
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;

// VideoPreprocessing
// - Holds methods for preprocessing procedures

class VideoPreprocessing {
	public:
		// Static Methods
		static string VideoStabilisation(string inputVideoFilePath);
		static string ReduceVideoResolution(string inputVideoFilePath, int newResolution);

	private:
		// Static Methods
		static vector<vector<double>> GetOverallTrajectoryOfMotion(vector<vector<double>> transformations);
		static vector<vector<double>> SmoothTrajectoryOfMotion(vector<vector<double>> trajectory);
		static cv::Mat GetTransformationMatrix(vector<double> transformations);
		static string ComputeOutputVideoFilePath(string inputVideoFilePath);
};

