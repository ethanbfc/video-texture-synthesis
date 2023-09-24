#include "MatrixFrame.h"
#include "HomeFrame.h"
#include "Utilities.cpp"
#include <wx/notebook.h>
#include <wx/image.h>

using namespace cv;
using namespace std;
using namespace utils_;

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

// Frame construction
MatrixFrame::MatrixFrame(const wxString& title, string videoFilePath, Mat distanceMatrix, Mat probabilityMatrix, string type) : wxFrame(nullptr, wxID_ANY, title) {

	SetVideoFilePath(videoFilePath);
	SetType(type);

	wxPanel* mainPanel = new wxPanel(this, 2000);
	wxNotebook* tabs = new wxNotebook(mainPanel, 2100, wxPoint(0, 0), wxSize(1600, 1000));

	// Define panels for notebook pages
	wxPanel* distanceMatrixPanel = new wxPanel(tabs, wxID_ANY);
	wxPanel* probabilityMatrixPanel = new wxPanel(tabs, wxID_ANY);
	wxPanel* distanceImagePanel = new wxPanel(tabs, wxID_ANY);
	wxPanel* probabilityImagePanel = new wxPanel(tabs, wxID_ANY);

	// Create UI on each panel
	BuildMatrixPanel(distanceMatrixPanel, distanceMatrix);
	BuildMatrixPanel(probabilityMatrixPanel, probabilityMatrix);
	BuildImagePanel(distanceImagePanel, distanceMatrix);
	BuildImagePanel(probabilityImagePanel, probabilityMatrix);

	// Add panels to notebook
	tabs->AddPage(distanceMatrixPanel, wxT("Distance Matrix"));
	tabs->AddPage(probabilityMatrixPanel, wxT("Probability Matrix"));
	tabs->AddPage(distanceImagePanel, wxT("Distance Image"));
	tabs->AddPage(probabilityImagePanel, wxT("Probability Image"));

	CreateStatusBar();
}

//--------------------------------------------------------------------------------------
// Getters & Setters
//--------------------------------------------------------------------------------------

string MatrixFrame::GetVideoFilePath() {
	return videoFilePath;
}

void MatrixFrame::SetVideoFilePath(string fp) {
	videoFilePath = fp;
}

string MatrixFrame::GetType() {
	return type;
}

void MatrixFrame::SetType(string t) {
	type = t;
	SetOffset();
}

int MatrixFrame::GetOffset() {
	return offset;
}

void MatrixFrame::SetOffset() {
	if (ToUpper(GetType()) == "EUCLIDEAN")
		offset = 1;
	else
		offset = 2;
}

//--------------------------------------------------------------------------------------
// Instance Methods (Private)
//--------------------------------------------------------------------------------------

// Displays input matrix as clickable grid
void MatrixFrame::BuildMatrixPanel(wxPanel* parent, cv::Mat mat) {

	// Create grid
	wxGrid* matrixGrid = new wxGrid(parent, wxID_ANY, wxPoint(0, 0), wxSize(1000, 600));
	matrixGrid->CreateGrid(mat.rows, mat.cols);
	matrixGrid->SetCornerLabelValue("Frames");
	matrixGrid->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
	matrixGrid->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
	matrixGrid->SetCornerLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);

	// Populate grid with data from matrix
	for (int i = 0; i < mat.rows; i++) {
		matrixGrid->SetRowLabelValue(i, ToStr(i + GetOffset()));
		for (int j = 0; j < mat.cols; j++) {
			if (i == 0)
				matrixGrid->SetColLabelValue(j, ToStr(j + GetOffset()));
			if (i != j) {
				matrixGrid->SetCellAlignment(i, j, wxALIGN_CENTRE, wxALIGN_CENTRE);
				matrixGrid->SetCellValue(i, j, ToStr(mat.at<float>(i, j)));
				matrixGrid->SetReadOnly(i, j);
			}
			else
				matrixGrid->SetCellBackgroundColour(i, j, *wxBLACK);
		}
	}

	matrixGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &MatrixFrame::MatrixCellClick, this);
}

// Displays input matrix as clickable image
void MatrixFrame::BuildImagePanel(wxPanel* parent, cv::Mat mat) {
	Mat image;
	normalize(mat, image, 0, 255, NORM_MINMAX, CV_8UC1);

	// Create grid
	wxGrid* imageGrid = new wxGrid(parent, wxID_ANY, wxPoint(0, 0), wxSize(1000, 600));
	imageGrid->CreateGrid(image.rows, image.cols);
	imageGrid->SetDefaultColSize(15);
	imageGrid->SetDefaultRowSize(15);
	imageGrid->SetRowLabelSize(0);
	imageGrid->SetColLabelSize(0);

	// Set cell background colour to image pixel colour
	for (int i = 0; i < image.rows; i++) {
		imageGrid->SetRowLabelValue(i, ToStr(i + 2));
		for (int j = 0; j < image.cols; j++) {
			imageGrid->SetReadOnly(i, j);
			imageGrid->SetCellBackgroundColour(i, j, wxColor(image.at<uchar>(i, j), image.at<uchar>(i, j), image.at<uchar>(i, j)));
		}
	}

	imageGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &MatrixFrame::MatrixCellClick, this);
}

//--------------------------------------------------------------------------------------
// Event Handlers
//--------------------------------------------------------------------------------------

// Click event for matrix elements - displays the frames associated with this element
void MatrixFrame::MatrixCellClick(wxGridEvent& e) {

	// Determine offset
	wxNotebook* nb = wxDynamicCast(this->FindWindowById(2100), wxNotebook);
	int offset = GetOffset();
	if (nb->GetSelection() % 2 != 0)
		offset++;

	// Ignore transition frame i -> frame i
	if (e.GetCol() != e.GetRow()) {
		int col = e.GetCol() + offset;
		int row = e.GetRow() + offset;

		wxLogStatus(this, "Cell (%d, %d) clicked", col, row);
		VideoCapture inputVideo(GetVideoFilePath());

		// Show frames
		if (!inputVideo.isOpened())
			wxLogStatus(this, "ERROR: Cannot open video file");
		else {
			Mat frame1, frame2;
			bool success;

			// Frame 1
			inputVideo.set(1, col);
			success = inputVideo.read(frame1);
			if (success)
				imshow("Frame " + ToStr(col), frame1);
			else
				wxLogStatus(this, "Error opening frame %d", col);

			// Frame 2
			inputVideo.set(1, row);
			success = inputVideo.read(frame2);
			if (success)
				imshow("Frame " + ToStr(row), frame2);
			else
				wxLogStatus(this, "Error opening frame %d", row);
		}
	}
}