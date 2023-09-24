#include "VideoTexture.h"

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

VideoTexture::VideoTexture() {}

//--------------------------------------------------------------------------------------
// Getters & Setters
//--------------------------------------------------------------------------------------

CompoundLoop VideoTexture::GetScheduledTransitions() {
	return scheduledTransitions;
}
void VideoTexture::SetScheduledTransitions(CompoundLoop t) {
	scheduledTransitions = t;
}