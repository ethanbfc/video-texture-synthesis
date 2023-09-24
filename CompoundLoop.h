#pragma once
#include "Transition.h"
#include <opencv2/opencv.hpp>

using namespace std;

// CompoundLoop
// - Representation of an unordered set of transitions
// - Tracks total cost and overall range of all transitions (must ensure all transitions can fit within a single range)

class CompoundLoop {
	public:
		// Constructors
		CompoundLoop();
		CompoundLoop(Transition t);

		// Getters & Setters
		vector<Transition> GetTransitions();
		int GetCost();
		void SetCost(int c);
		cv::Point GetRange();
		void SetRange(cv::Point r);

		// Instance Methods
		void AddTransition(Transition t);
		vector<CompoundLoop> RemoveTransition(Transition t);
		CompoundLoop RemoveAllTransitions();
		int GetNumberOfTransitions();
		Transition GetLatestTransition();
		bool ContainsTransition(Transition t);
		bool ContainsTransitions(CompoundLoop c);

		// Static Methods
		static bool CheckIfOverlappingRanges(CompoundLoop c1, CompoundLoop c2);
		static CompoundLoop MergeCompoundLoops(CompoundLoop c1, CompoundLoop c2);

	private:
		// Parameters
		vector<Transition> transitions;
		int cost;
		cv::Point range;

		// Instance Methods
		void CalculateCost();
		void CalculateRange();
};

