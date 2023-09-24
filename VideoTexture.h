#pragma once
#include "Video.h"
#include "CompoundLoop.h"

// VideoTexture
// - Subclass of video with additional attribute for the set of transitions that made up the video texture

class VideoTexture : public Video {
	public:
		// Constructors
		VideoTexture();

		// Getters & Setters
		CompoundLoop GetScheduledTransitions();
		void SetScheduledTransitions(CompoundLoop t);

	private:
		// Parameters
		CompoundLoop scheduledTransitions;
};

