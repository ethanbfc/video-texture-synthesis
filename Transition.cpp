#include "Transition.h"
#include <opencv2/opencv.hpp>

using namespace std;

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

// Default constructor - creates an empty transition
Transition::Transition() {
	source = INT_MIN;
	destination = INT_MAX;
	length = 0;
	cost = INT_MAX;
}

// Creates transition
Transition::Transition(int i, int j, float c) {
	source = i;
	destination = j;
	length = i - j + 1;
	cost = c;
}

//--------------------------------------------------------------------------------------
// Getters & Setters
//--------------------------------------------------------------------------------------

int Transition::GetSourceFrame() {
	return source;
}

void Transition::SetSourceFrame(int i) {
	source = i;
}

int Transition::GetDestinationFrame() {
	return destination;
}

void Transition::SetDestinationFrame(int j) {
	destination = j;
}

int Transition::GetTransitionLength() {
	return length;
}

float Transition::GetTransitionCost() {
	return cost;
}

void Transition::SetTransitionCost(float c) {
	cost = c;
}

//--------------------------------------------------------------------------------------
// Static Methods (Public)
//--------------------------------------------------------------------------------------

int Transition::GetLongestLength(vector<Transition> transitions) {
	sort(transitions.begin(), transitions.end(), [](Transition& a, Transition& b) {
		return a.GetTransitionLength() > b.GetTransitionLength();
	});

	return transitions[0].GetTransitionLength();
}

//--------------------------------------------------------------------------------------
// Overloaded Operators
//--------------------------------------------------------------------------------------
 
bool operator==(const Transition& t1, const Transition& t2) {
	return ((t1.source == t2.source) && (t1.destination == t2.destination) && (t1.cost == t2.cost));
}

bool operator!=(const Transition& t1, const Transition& t2) {
	return !(t1 == t2);
}