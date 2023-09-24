#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <opencv2/opencv.hpp>

using namespace std;

// MatrixFrame
// - Frame to display similarity matrix and the associated image representation

class MatrixFrame : public wxFrame {
	public:
		// Constructors
		MatrixFrame(const wxString& title, string videoFilePath, cv::Mat distance, cv::Mat probability, string type);

	private:
		// Parameters
		string videoFilePath;
		string type;
		int offset;

		// Getters & Setters
		string GetVideoFilePath();
		void SetVideoFilePath(string fp);
		string GetType();
		void SetType(string t);
		int GetOffset();
		void SetOffset();

		// Instance Methods
		void BuildMatrixPanel(wxPanel* parent, cv::Mat mat);
		void BuildImagePanel(wxPanel* parent, cv::Mat mat);

		// Event Handlers
		void MatrixCellClick(wxGridEvent& e);
};