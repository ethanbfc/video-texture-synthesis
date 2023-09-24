#pragma once
#include "CompoundLoop.h"
#include <opencv2/opencv.hpp>

using namespace std;

// Rendering
// - Creates a video texture with rendering techniques applied

class Rendering {
	public:
		// Static Methods
		static string CreateVideoTextureWithCrossFading(string inputVideoFilePath, CompoundLoop transitions, int windowSize);
		static string CreateVideoTextureWithMorphing(string inputVideoFilePath, CompoundLoop transitions, int parameters[]);

	private:
		// Static Methods
		static vector<vector<int>> GetFrameSequence(CompoundLoop compoundLoopofTransitions);
		static string ComputeOutputFilePath(string inputFilePath);
};

