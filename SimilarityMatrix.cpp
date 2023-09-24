#include "SimilarityMatrix.h"
#include "Utilities.cpp"
#include <fstream>

using namespace cv;
using namespace utils_;

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

SimilarityMatrix::SimilarityMatrix() {}

SimilarityMatrix::SimilarityMatrix(Mat dm) {
    SetDistanceMatrix(dm);
}

//--------------------------------------------------------------------------------------
// Getters & Setters
//--------------------------------------------------------------------------------------

Mat SimilarityMatrix::GetDistanceMatrix() {
	return distanceMatrix;
}

void SimilarityMatrix::SetDistanceMatrix(Mat dm) {
	distanceMatrix = dm;
	MapDistancesToProbabilities();
    NormaliseProbabilityMatrix();
}

Mat SimilarityMatrix::GetProbabilityMatrix() {
	return probabilityMatrix;
}

//--------------------------------------------------------------------------------------
// Instance Methods (Public)
//--------------------------------------------------------------------------------------

// Saves distance matrix as CSV file
void SimilarityMatrix::SaveDistanceMatrixAsCSV(string filePath) {
    ofstream output(filePath);

    for (int i = 0; i < distanceMatrix.rows; i++) {
        for (int j = 0; j < distanceMatrix.cols; j++) {
            output << ToStr(distanceMatrix.at<float>(i, j));
            if (j + 1 < distanceMatrix.cols)
                output << ", ";
        }
        output << endl;
    }

    output.close();
}

// Saves probability matrix as CSV file
void SimilarityMatrix::SaveProbabilityMatrixAsCSV(string filePath) {
    ofstream output(filePath);

    for (int i = 0; i < probabilityMatrix.rows; i++) {
        for (int j = 0; j < probabilityMatrix.cols; j++) {
            output << ToStr(probabilityMatrix.at<float>(i, j));
            if (j + 1 < probabilityMatrix.cols)
                output << ", ";
        }
        output << endl;
    }

    output.close();
}

// Saves distance matrix as image file
void SimilarityMatrix::SaveDistanceMatrixAsImage(string filePath) {
    Mat image;
    normalize(distanceMatrix, image, 0, 255, NORM_MINMAX);
    imwrite(filePath, image);
}

// Saves probability matrix as image file
void SimilarityMatrix::SaveProbabilityMatrixAsImage(string filePath) {
    Mat image;
    normalize(probabilityMatrix, image, 0, 255, NORM_MINMAX);
    imwrite(filePath, image);
}

//--------------------------------------------------------------------------------------
// Instance Methods (Private)
//--------------------------------------------------------------------------------------

// Maps distances to probabilities to create the probability matrix
void SimilarityMatrix::MapDistancesToProbabilities() {

    // Calculate sigma
    float sigma = 0.1 * GetAverageValue();

    // Map to probabilities
    Mat probabilities(distanceMatrix.rows - 1, distanceMatrix.cols, CV_32F);

    for (int i = 0; i < distanceMatrix.rows - 1; i++) {
        for (int j = 0; j < distanceMatrix.cols; j++) {
            probabilities.at<float>(i, j) = exp((-1 * distanceMatrix.at<float>(i + 1, j)) / sigma);
        }
    }

    probabilityMatrix = probabilities;
}

// Get average (non-zero) value of distance matrix
float SimilarityMatrix::GetAverageValue() {

    float total = 0;
    int count = 0;

    for (int row = 0; row < distanceMatrix.rows; row++) {
        for (int col = 0; col < distanceMatrix.cols; col++) {
            if (row != col) {
                total += distanceMatrix.at<float>(row, col);
                count++;
            }
        }
    }

    return (total / count);
}

// Normalises values in each row of the probability matrix such that the sum of each row is one
void SimilarityMatrix::NormaliseProbabilityMatrix() {

    Mat normalised(probabilityMatrix.rows, probabilityMatrix.cols, CV_32F);

    for (int i = 0; i < probabilityMatrix.rows; i++) {
        float sum = 0;
        for (int j = 0; j < probabilityMatrix.cols; j++) {
            sum += probabilityMatrix.at<float>(i, j);
        }
        normalised.row(i) = probabilityMatrix.row(i) / sum;
    }

    probabilityMatrix = normalised;
}