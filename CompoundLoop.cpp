#include "CompoundLoop.h"

using namespace cv;
using namespace std;

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

// Default constructor - creates an empty compound loop
CompoundLoop::CompoundLoop() {
	cost = 0;
	range = Point(0, 0);
}

// Creates a compound loop containing a single transition
CompoundLoop::CompoundLoop(Transition t) {
	AddTransition(t);
}

//--------------------------------------------------------------------------------------
// Getters & Setters
//--------------------------------------------------------------------------------------

vector<Transition> CompoundLoop::GetTransitions() {
	return transitions;
}

int CompoundLoop::GetCost() {
	return cost;
}

void CompoundLoop::SetCost(int c) {
	cost = c;
}

Point CompoundLoop::GetRange() {
	return range;
}

void CompoundLoop::SetRange(Point r) {
	range = r;
}

//--------------------------------------------------------------------------------------
// Instance Methods (Public)
//--------------------------------------------------------------------------------------

// Add a tranisition to the compound loop (also recalculates cost and range)
void CompoundLoop::AddTransition(Transition t) {
	transitions.push_back(t);

	CalculateCost();
	CalculateRange();
}

// Removes a transition from the compound loop - ensures valid ranges by returning either one or two compound loops
vector<CompoundLoop> CompoundLoop::RemoveTransition(Transition t) {

	vector<Transition> newTransitionSet;

	// Convert this compound loop into a vector of transitions EXCLUDING transition t
	for (int i = 0; i < transitions.size(); i++) {
		if ((transitions[i] != t) || ((transitions[i] == t) && (newTransitionSet.size() != i)))
			newTransitionSet.push_back(transitions[i]);
	}

	// Sort this vector of transitions by their source frames (ascending)
	sort(newTransitionSet.begin(), newTransitionSet.end(), [](Transition& a, Transition& b) {
		return a.GetSourceFrame() < b.GetSourceFrame();
	});

	// Convert this vector of transitions into compound loop(s) - ensure legal ranges
	vector<CompoundLoop> compoundLoopsWithLegalTransitions;
	CompoundLoop currentCompoundLoop;

	for (int i = 0; i < newTransitionSet.size(); i++) {
		Transition t = newTransitionSet[i];

		if (i == 0)
			currentCompoundLoop.AddTransition(t);
		else if (CheckIfOverlappingRanges(currentCompoundLoop, CompoundLoop(t)))
			currentCompoundLoop.AddTransition(t);
		else {
			compoundLoopsWithLegalTransitions.push_back(currentCompoundLoop);
			currentCompoundLoop = currentCompoundLoop.RemoveAllTransitions();
			currentCompoundLoop.AddTransition(t);
		}

		if (i == newTransitionSet.size() - 1)
			compoundLoopsWithLegalTransitions.push_back(currentCompoundLoop);
	}

	return compoundLoopsWithLegalTransitions;
}

// Removes all transitions from the compound loop
CompoundLoop CompoundLoop::RemoveAllTransitions() {
	CompoundLoop empty;
	return empty;
}

// Returns number of transitions in compound loop
int CompoundLoop::GetNumberOfTransitions() {
	return transitions.size();
}

// Returns transition with latest source frame
Transition CompoundLoop::GetLatestTransition() {
	Transition last;

	for (Transition& t : transitions) {
		if (t.GetSourceFrame() > last.GetSourceFrame())
			last = t;
	}

	return last;
}

// Checks if the compound loop contains a specified transition
bool CompoundLoop::ContainsTransition(Transition inputTransition) {

	for (Transition& t : transitions) {
		if (t == inputTransition)
			return true;
	}

	return false;
}

// Checks if the compound loop contains a specified transition
bool CompoundLoop::ContainsTransitions(CompoundLoop c) {

	for (Transition& current : transitions) {
		for (Transition& t : c.GetTransitions()) {
			if (current == t)
				return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------
// Instance Methods (Private)
//--------------------------------------------------------------------------------------

// Calculate cost of compound loop
void CompoundLoop::CalculateCost() {
	int newCost = 0;

	for (Transition& t : transitions) {
		newCost += t.GetTransitionCost();
	}

	SetCost(newCost);
}

// Calculates range of compound loop
void CompoundLoop::CalculateRange() {
	int start = INT_MIN;
	int end = INT_MAX;

	for (Transition& t : transitions) {
		if (t.GetSourceFrame() > start)
			start = t.GetSourceFrame();

		if (t.GetDestinationFrame() < end)
			end = t.GetDestinationFrame();
	}

	SetRange(Point(start, end));
}

//--------------------------------------------------------------------------------------
// Static Methods (Public)
//--------------------------------------------------------------------------------------

// Checks if two compound loops have overlapping ranges
bool CompoundLoop::CheckIfOverlappingRanges(CompoundLoop c1, CompoundLoop c2) {
	Point range1 = c1.GetRange();
	Point range2 = c2.GetRange();

	if ((range2.y - range1.x) * (range1.y - range2.x) >= 0)
		return true;

	return false;
}

// Merges two compound loops into a new compound loop
CompoundLoop CompoundLoop::MergeCompoundLoops(CompoundLoop c1, CompoundLoop c2) {
	CompoundLoop merged = c1;

	for (Transition& t : c2.GetTransitions()) {
		merged.AddTransition(t);
	}

	return merged;
}