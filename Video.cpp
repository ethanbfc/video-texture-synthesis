#include "Video.h"

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

Video::Video() {}

//--------------------------------------------------------------------------------------
// Getters & Setters
//--------------------------------------------------------------------------------------

string Video::GetVideoFilePath() {
	return videoFilePath;
}

void Video::SetVideoFilePath(string fp) {
	videoFilePath = fp;
}

string Video::GetEuclideanFilePath() {
	return euclideanFilePath;
}

void Video::SetEuclideanFilePath(string fp) {
	euclideanFilePath = fp;
}

string Video::GetMotionFilePath() {
	return motionFilePath;
}

void Video::SetMotionFilePath(string fp) {
	motionFilePath = fp;
}

string Video::GetFutureCostFilePath() {
	return futureCostFilePath;
}

void Video::SetFutureCostFilePath(string fp) {
	futureCostFilePath = fp;
}

SimilarityMatrix Video::GetEuclidean() {
	return euclidean;
}

void Video::SetEuclidean(SimilarityMatrix e) {
	euclidean = e;
}

SimilarityMatrix Video::GetMotion() {
	return motion;
}

void Video::SetMotion(SimilarityMatrix m) {
	motion = m;
}

SimilarityMatrix Video::GetFutureCost() {
	return futureCost;
}

void Video::SetFutureCost(SimilarityMatrix fc) {
	futureCost = fc;
}

//--------------------------------------------------------------------------------------
// Instance Methods (Public)
//--------------------------------------------------------------------------------------

// Sets file paths for all matrices
void Video::SetFilePaths(string option) {
	// option - binary code to determine which file paths to set (euclidean, motion, future)

	string filename = (GetVideoFilePath().substr(GetVideoFilePath().find_last_of("/\\") + 1));
	filename = filename.substr(0, filename.find('.'));

	if (option.substr(0, 1) == "1")
		SetEuclideanFilePath(filename + "_" + "DISTANCE_MATRIX_(EUCLIDEAN).csv");
	if (option.substr(1, 1) == "1")
		SetMotionFilePath(filename + "_" + "DISTANCE_MATRIX_(MOTION).csv");
	if (option.substr(2, 1) == "1")
		SetFutureCostFilePath(filename + "_" + "DISTANCE_MATRIX_(FUTURE).csv");
}