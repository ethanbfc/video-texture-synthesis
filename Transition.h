#pragma once
#include <vector>

using namespace std;

// Transition
// - Representation of a transition between two frames
// - Format: Transition from frame i to frame j with the length and associated cost

class Transition {
	public:
		// Constructors
		Transition();
		Transition(int i, int j, float c);

		// Getters & Setters
		int GetSourceFrame();
		void SetSourceFrame(int i);
		int GetDestinationFrame();
		void SetDestinationFrame(int j);
		int GetTransitionLength();
		float GetTransitionCost();
		void SetTransitionCost(float c);

		// Static Methods
		static int GetLongestLength(vector<Transition> transitions);

		// Overloaded Operators
		friend bool operator==(const Transition& t1, const Transition& t2);
		friend bool operator!=(const Transition& t1, const Transition& t2);

	private:
		// Parameters
		int source;
		int destination;
		int length;
		float cost;
};

