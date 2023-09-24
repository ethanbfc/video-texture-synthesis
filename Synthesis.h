#pragma once
#include "Transition.h"
#include "CompoundLoop.h"
#include <opencv2/opencv.hpp>

using namespace std;

// Synthesis
// - Tranforms a similarity matrix into a video texture
// - A video texture is a video with a looping property (such that it can be played on a loop with no/minimal visual discontinuities)

class Synthesis {
	public:
		// Static Methods
		static CompoundLoop GetTransitionSet(cv::Mat motionDistanceMatrix, cv::Mat futureCostDistanceMatrix, int lengthMultiplier);
		static string CreateVideoTexture(string inputVideoFilePath, CompoundLoop transitions);

	private:
		// Static Methods
		static vector<Transition> PruneTransitions(cv::Mat motionDistanceMatrix, cv::Mat futureCostDistanceMatrix);
		static CompoundLoop GetSetOfTransitions(vector<Transition> transitionMatrix, int lengthMultiplier);
		static vector<CompoundLoop> GetLongestSetOfCompoundLoopsWithTransitions(vector<vector<CompoundLoop>> compoundLoops);
		static CompoundLoop ScheduleTransitions(CompoundLoop transitionSet);
		static CompoundLoop ScheduleBeforeStartPoint(CompoundLoop rangeSet);
		static CompoundLoop ScheduleAfterStartPoint(CompoundLoop rangeSet);
		static vector<int> GetFrameSequence(CompoundLoop transitions);
		static string ComputeOutputFilePath(string inputFilePath);
};

