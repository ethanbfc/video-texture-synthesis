#include "Synthesis.h"

using namespace cv;
using namespace std;

//--------------------------------------------------------------------------------------
// Static Methods (Public)
//--------------------------------------------------------------------------------------

// Computes an ordered set of transitions for a video texture
CompoundLoop Synthesis::GetTransitionSet(Mat motionDistanceMatrix, Mat futureCostDistanceMatrix, int lengthMultiplier) {
	vector<Transition> prunedTransitionSet = PruneTransitions(motionDistanceMatrix, futureCostDistanceMatrix);
	CompoundLoop unscheduledTransitionSet = GetSetOfTransitions(prunedTransitionSet, lengthMultiplier);
	CompoundLoop scheduledTransitionSet = ScheduleTransitions(unscheduledTransitionSet);

	return scheduledTransitionSet;
}

// Saves list of frames to video
string Synthesis::CreateVideoTexture(string inputVideoFilePath, CompoundLoop compoundLoopOfTransitions) {

	// Get sequence of frames
	vector<int> sequenceOfFrames = GetFrameSequence(compoundLoopOfTransitions);

	// Write sequence of frames to video
	VideoCapture inputVideo(inputVideoFilePath);

	string outputVideoFilePath = ComputeOutputFilePath(inputVideoFilePath);
	VideoWriter outputVideo(outputVideoFilePath, inputVideo.get(CAP_PROP_FOURCC), inputVideo.get(CAP_PROP_FPS), Size2i(inputVideo.get(CAP_PROP_FRAME_WIDTH), inputVideo.get(CAP_PROP_FRAME_HEIGHT)));

	if (inputVideo.isOpened() && outputVideo.isOpened()) {

		for (int i = 0; i < sequenceOfFrames.size(); i++) {
			Mat frame;

			inputVideo.set(1, sequenceOfFrames[i]);
			bool success = inputVideo.read(frame);
			if (!success) break;

			outputVideo << frame;
		}

		outputVideo.release();
	}

	return outputVideoFilePath;
}

//--------------------------------------------------------------------------------------
// Static Methods (Private)
//--------------------------------------------------------------------------------------

// Prunes matrix of transitions for synthesis
vector<Transition> Synthesis::PruneTransitions(Mat motionDistanceMatrix, Mat futureCostDistanceMatrix) {
	// Essentially find primitive loops that will be used to form compound loops
	// In order to create a cycle for transition i->j: range = [j, i] (i.e. i >= j) and cost = D''_ij (motion distance matrix)

	vector<Transition> localMinimaTransitions, transitions;
	double cost = 0;
	Point frame(0, 0);

	// 1. Select local minima (i.e. lowest cost transition) for each source frame
	for (int i = 1; i < futureCostDistanceMatrix.rows; i++) {
		futureCostDistanceMatrix.at<float>(i, i) = FLT_MAX;
		bool validRange = false;

		while (!validRange) { // Ensures range [j, i]
			minMaxLoc(futureCostDistanceMatrix.row(i), &cost, NULL, &frame, NULL);
			if (frame.x <= i)
				validRange = true;
			else
				futureCostDistanceMatrix.at<float>(i, frame.x) = FLT_MAX;
		}

		localMinimaTransitions.push_back(Transition(i, frame.x, cost));
	}

	// 2. Remove transitions with length <= 2 & compute average cost for each transition
	for (Transition& t : localMinimaTransitions) {
		if (t.GetTransitionLength() > 2) {
			float cost = motionDistanceMatrix.at<float>(t.GetSourceFrame(), t.GetDestinationFrame());
			t.SetTransitionCost(cost);
			transitions.push_back(t);
		}
	}

	// Ensure we have enough transitions to create texture
	if (transitions.size() < 2)
		transitions = localMinimaTransitions;

	// 3. We only want to use the best transitions (20)
	sort(transitions.begin(), transitions.end(), [](Transition& a, Transition& b) {
		return a.GetTransitionCost() < b.GetTransitionCost();
	});

	if (transitions.size() > 20) {
		vector<Transition> bestTransitions(transitions.begin(), transitions.begin() + 20);
		return bestTransitions;
	}

	return transitions;
}

// Finds the lowest cost compound loop of a given length
CompoundLoop Synthesis::GetSetOfTransitions(vector<Transition> transitions, int lengthMultiplier) {

	int maxLoopLength = lengthMultiplier * Transition::GetLongestLength(transitions);
	vector<vector<CompoundLoop>> compoundLoops;

	// Dynamic programming algorithm to find compound loops

	// For each cell:
	// - Look at shorter compound loops that sum to this length
	// - Ensure these compound loops have overlapping ranges
	// - Select compound loop with lowest cost
	// - Combine with cell's compound loop for this entry

	// OUTPUT - List of compound loops -> each compound loop is a list of transitions

	for (int length = 1; length <= maxLoopLength; length++) {
		vector<CompoundLoop> compoundLoopsOfLengthN;
		for (Transition& primitiveLoop : transitions) {
			CompoundLoop current;
			// Look at length of primitive loop - determine difference from current length
			int lengthDifference = length - primitiveLoop.GetTransitionLength();

			// Do nothing if current length is less than length of primitive loop

			if (lengthDifference == 0) // Current length = loop length, so the compound loop is simply the primitive loop
				current.AddTransition(primitiveLoop);

			if (lengthDifference > 0) {
				current.AddTransition(primitiveLoop);
				// Need to combine primitive loop with compound loop to get current length
				vector<CompoundLoop> possibleCompoundLoops = compoundLoops[lengthDifference - 1];

				// Sort set of compound loops by cost
				sort(possibleCompoundLoops.begin(), possibleCompoundLoops.end(), [](CompoundLoop& a, CompoundLoop& b) {
					return a.GetCost() < b.GetCost();
				});

				// Add lowest-cost compound loop with overlapping range to the primitive loop (ensure no duplicate transitions in new compound loop)
				for (CompoundLoop& c : possibleCompoundLoops) {
					if (!(current.ContainsTransitions(c)) && (c.GetNumberOfTransitions() > 0) && (CompoundLoop::CheckIfOverlappingRanges(current, c))) {
						current = CompoundLoop::MergeCompoundLoops(current, c);
						break;
					}
				}

				// If only one transition in the compound loop, then no smaller one has been found - this column is therefore empty
				if (current.GetNumberOfTransitions() == 1) {
					CompoundLoop empty;
					current = empty;
				}
			}
			compoundLoopsOfLengthN.push_back(current);
		}
		compoundLoops.push_back(compoundLoopsOfLengthN);
	}

	// Get compound loop of largest length which contains transitions
	vector<CompoundLoop> compoundLoopsOfFinalLength = GetLongestSetOfCompoundLoopsWithTransitions(compoundLoops);

	sort(compoundLoopsOfFinalLength.begin(), compoundLoopsOfFinalLength.end(), [](CompoundLoop& a, CompoundLoop& b) {
		return a.GetCost() < b.GetCost();
	});

	// Return compound loop with lowest cost (> 0)
	CompoundLoop compoundLoopWithNonZeroCost;
	for (CompoundLoop& c : compoundLoopsOfFinalLength) {
		if (c.GetCost() > 0) {
			compoundLoopWithNonZeroCost = c;
			break;
		}
	}

	return compoundLoopWithNonZeroCost;
}

// Returns set of compound loops of the longest length that contains transitions
vector<CompoundLoop> Synthesis::GetLongestSetOfCompoundLoopsWithTransitions(vector<vector<CompoundLoop>> compoundLoops) {
	while (compoundLoops.size() > 0) {
		vector<CompoundLoop> longestCompoundLoops = compoundLoops.back();

		for (CompoundLoop& c : longestCompoundLoops) {
			if (c.GetNumberOfTransitions() > 0)
				return compoundLoops.back();
		}

		compoundLoops.pop_back();
	}
}

// Schedules transitions to form valid video structure
CompoundLoop Synthesis::ScheduleTransitions(CompoundLoop transitionSet) {

	CompoundLoop orderedCompoundLoop;

	// Transition at end of sequence is first transition to be taken (i.e. find largest value of i) - add to ordered compound loop
	Transition latestTransition = transitionSet.GetLatestTransition();
	orderedCompoundLoop.AddTransition(latestTransition);

	// Remove this transition from the compound loop - we will now have one or more new compound loops (each must have legal range)
	vector<CompoundLoop> newLoops = transitionSet.RemoveTransition(latestTransition);

	// Transitions that occur before the destination transition of the latest starting transition
	CompoundLoop lowerRangeSet = newLoops[0];
	orderedCompoundLoop = CompoundLoop::MergeCompoundLoops(orderedCompoundLoop, ScheduleBeforeStartPoint(lowerRangeSet));

	// Transitions that occur after the destination transition of the latest starting transition
	for (int i = 1; i < newLoops.size(); i++) {
		CompoundLoop rangeSet = newLoops[i];
		orderedCompoundLoop = CompoundLoop::MergeCompoundLoops(orderedCompoundLoop, ScheduleAfterStartPoint(rangeSet));
	}

	return orderedCompoundLoop;
}

// Schedules transitions before the starting point
CompoundLoop Synthesis::ScheduleBeforeStartPoint(CompoundLoop rangeSet) {
	// We want to go backwards here to the earliest source frame
	vector<Transition> toSchedule = rangeSet.GetTransitions();

	// Sort by source - highest first
	sort(toSchedule.begin(), toSchedule.end(), [](Transition& a, Transition& b) {
		return a.GetSourceFrame() > b.GetSourceFrame();
	});

	// Create compound loop
	CompoundLoop sorted;
	for (Transition& t : toSchedule) {
		sorted.AddTransition(t);
	}

	return sorted;
}

// Schedules transitions after the starting point
CompoundLoop Synthesis::ScheduleAfterStartPoint(CompoundLoop rangeSet) {
	// We want to go forwards here to largest frame
	vector<Transition> toSchedule = rangeSet.GetTransitions();

	// Sort by source - highest first
	sort(toSchedule.begin(), toSchedule.end(), [](Transition& a, Transition& b) {
		return a.GetSourceFrame() < b.GetSourceFrame();
		});

	// Create compound loop
	CompoundLoop sorted;
	for (Transition& t : toSchedule) {
		sorted.AddTransition(t);
	}

	return sorted;
}

// Converts transition set into list of frames
vector<int> Synthesis::GetFrameSequence(CompoundLoop compoundLoopOfTransitions) {
	// Get sequence of frames to write to video
	vector<Transition> transitions = compoundLoopOfTransitions.GetTransitions();
	vector<int> sequenceOfFrames;

	// Start from the destination frame of the transition with the latest source frame (i.e. transitions[0].destinationFrame)
	int startFrame = transitions[0].GetDestinationFrame();

	for (int i = 1; i < transitions.size(); i++) {
		// Go from startFrame to the source frame of the next transition
		for (int j = startFrame; j <= transitions[i].GetSourceFrame(); j++) {
			sequenceOfFrames.push_back(j);
		}

		// Take transition i
		startFrame = transitions[i].GetDestinationFrame() + 1;
	}

	// Final transition is to frame before source frame of the original starting transition
	for (int j = startFrame; j <= transitions[0].GetSourceFrame() - 1; j++) {
		sequenceOfFrames.push_back(j);
	}

	return sequenceOfFrames;
}

// Computes file path for output video texture
string Synthesis::ComputeOutputFilePath(string inputFilePath) {
	string filename = (inputFilePath.substr(inputFilePath.find_last_of("/\\") + 1));
	filename = filename.substr(0, filename.find('.'));

	return (filename + "_VIDEO_TEXTURE.mp4");
}
