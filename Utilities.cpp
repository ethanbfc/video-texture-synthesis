#include <string>
#include <fstream>
#include <limits>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Utilities
// - Holds general purpose methods to be used elsewhere

namespace utils_ {

    //--------------------------------------------------------------------------------------
    // String Operations
    //--------------------------------------------------------------------------------------

    // Converts any input type to string
    template <typename T> string ToStr(const T& t) {
        ostringstream os;
        os << t;
        return os.str();
    }

    // Converts string to uppercase
    static string ToUpper(string input) {
        std::transform(input.begin(), input.end(), input.begin(), ::toupper);
        return input;
    }

    //--------------------------------------------------------------------------------------
    // File Operations
    //--------------------------------------------------------------------------------------

    // Reads .csv file and stores in Mat
    static Mat ReadCSVFile(string filePath) {
        vector<vector<float>> data;
        string currentLine;

        ifstream input(filePath);

        while (getline(input, currentLine)) {
            vector<float> lineValues;
            stringstream tempLine(currentLine);
            string this_value;
            while (getline(tempLine, this_value, ',')) {
                lineValues.push_back(atof(this_value.c_str()));
            }
            data.push_back(lineValues);
        }

        Mat matrix(int(data.size()), int(data[0].size()), CV_32F);
        for (int row = 0; row < int(data.size()); row++) {
            for (int col = 0; col < int(data[0].size()); col++) {
                matrix.at<float>(row, col) = data[row][col];
            }
        }

        return matrix;
    }

    //--------------------------------------------------------------------------------------
    // Matrix Operations
    //--------------------------------------------------------------------------------------

    // Removes column at specified index from matrix
    static Mat RemoveMatrixColumn(Mat input, int index) {
        Mat output(input.rows, input.cols - 1, CV_32F);

        if (index > 0) {
            Rect rect(0, 0, index, input.rows);
            input(rect).copyTo(output(rect));
        }

        if (index < input.cols - 1) {
            Rect rect1(index + 1, 0, input.cols - index - 1, input.rows);
            Rect rect2(index, 0, input.cols - index - 1, input.rows);
            input(rect1).copyTo(output(rect2));
        }
        
        return output;
    }

    // Removes row at specified index from matrix
    static Mat RemoveMatrixRow(Mat input, int index) {
        Mat output(input.rows - 1, input.cols, CV_32F);

        if (index > 0) {
            Rect rect(0, 0, input.cols, index);
            input(rect).copyTo(output(rect));
        }

        if (index < input.rows - 1) {
            Rect rect1(0, index + 1, input.cols, input.rows - index - 1);
            Rect rect2(0, index, input.cols, input.rows - index - 1);
            input(rect1).copyTo(output(rect2));
        }

        return output;
    }

    // Gets the lowest value in each row of the matrix
    static Mat GetMatrixRowLowestValues(Mat input) {
        Mat output(1, input.rows, CV_32F);

        for (int j = 0; j < input.rows; j++) {
            float lowest = FLT_MAX;
            for (int i = 0; i < input.cols; i++) {
                if ((i != j) && (input.at<float>(i, j) < lowest))
                    lowest = input.at<float>(i, j);
            }
            output.at<float>(j) = lowest;
        }

        return output;
    }
}