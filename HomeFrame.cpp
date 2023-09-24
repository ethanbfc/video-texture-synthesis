#include "HomeFrame.h"
#include "MatrixFrame.h"
#include "VideoPreprocessing.h"
#include "SimilarityMeasure.h"
#include "SimilarityMatrix.h"
#include "Synthesis.h"
#include "Rendering.h"
#include "Utilities.cpp"
#include <wx/filedlg.h>
#include <wx/cshelp.h>
#include <wx/msgdlg.h>
#include <wx/statline.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace utils_;

//--------------------------------------------------------------------------------------
// Constructors
//--------------------------------------------------------------------------------------

// Constructs UI for home frame
HomeFrame::HomeFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	wxPanel* panel = new wxPanel(this, 100);

	BuildInputUI(panel);
	BuildPreprocessingUI(panel);
	BuildSimilarityMeasureUI(panel);
	BuildSynthesisUI(panel);
	BuildRenderingUI(panel);

	CreateStatusBar();
}

//--------------------------------------------------------------------------------------
// UI Instance Methods (Private)
//--------------------------------------------------------------------------------------

void HomeFrame::BuildInputUI(wxPanel* parent) {

	// Heading
	wxStaticText* lblInputHeading = new wxStaticText(parent, 500, "INPUT", wxPoint(10, 5));
	lblInputHeading->SetFont(lblInputHeading->GetFont().Scale(2));
	
	// Help button
	wxContextHelpButton* btnInputHelp = new wxContextHelpButton(parent, 505, wxPoint(82, 10), wxSize(20, 20));
	btnInputHelp->Bind(wxEVT_BUTTON, &HomeFrame::BtnHelpClick, this);

	// Video - Label
	wxStaticText* lblSelectVideo = new wxStaticText(parent, 510, "Video File*:", wxPoint(10, 40));
	lblSelectVideo->SetFont(lblSelectVideo->GetFont().Scale(1.5));

	// Video - Text Box
	wxTextCtrl* txtVideoFilePath = new wxTextCtrl(parent, 515, "", wxPoint(105, 45), wxSize(435, 20), wxTE_READONLY);

	// Video - Selection Button
	wxButton* btnVideoSelect = new wxButton(parent, 520, "...", wxPoint(545, 44), wxSize(20, 22));
	btnVideoSelect->Bind(wxEVT_BUTTON, &HomeFrame::BtnVideoSelectClick, this);

	// Euclidean Similarity Matrix - Label
	wxStaticText* lblSelectEuclideanSimilarityMatrix = new wxStaticText(parent, 525, "Euclidean Similarity Matrix:", wxPoint(10, 70));
	lblSelectEuclideanSimilarityMatrix->SetFont(lblSelectEuclideanSimilarityMatrix->GetFont().Scale(1.5));

	// Euclidean Similarity Matrix - Text Box
	wxTextCtrl* txtEuclideanSimilarityMatrixFilePath = new wxTextCtrl(parent, 530, "", wxPoint(225, 75), wxSize(315, 20), wxTE_READONLY);

	// Euclidean Similarity Matrix - Selection Button
	wxButton* btnEuclideanSimilarityMatrixSelect = new wxButton(parent, 535, "...", wxPoint(545, 74), wxSize(20, 22));
	btnEuclideanSimilarityMatrixSelect->Bind(wxEVT_BUTTON, &HomeFrame::BtnEuclideanSimilarityMatrixSelectClick, this);

	// Euclidean Similarity Matrix - View Button
	wxButton* btnDisplayEuclideanSimilarityMatrix = new wxButton(parent, 540, "SHOW", wxPoint(570, 74), wxSize(45, 22));
	btnDisplayEuclideanSimilarityMatrix->Bind(wxEVT_BUTTON, &HomeFrame::BtnEuclideanSimilarityMatrixViewClick, this);
	btnDisplayEuclideanSimilarityMatrix->Enable(false);

	// Motion Similarity Matrix - Label
	wxStaticText* lblMotionSelectSimilarityMatrix = new wxStaticText(parent, 545, "Motion Similarity Matrix:", wxPoint(10, 100));
	lblMotionSelectSimilarityMatrix->SetFont(lblMotionSelectSimilarityMatrix->GetFont().Scale(1.5));

	// Motion Similarity Matrix - Text Box
	wxTextCtrl* txtMotionSimilarityMatrixFilePath = new wxTextCtrl(parent, 550, "", wxPoint(210, 105), wxSize(330, 20), wxTE_READONLY);

	// Motion Similarity Matrix - Selection Button
	wxButton* btnMotionSimilarityMatrixSelect = new wxButton(parent, 555, "...", wxPoint(545, 104), wxSize(20, 22));
	btnMotionSimilarityMatrixSelect->Bind(wxEVT_BUTTON, &HomeFrame::BtnMotionSimilarityMatrixSelectClick, this);

	// Motion Similarity Matrix - View Button
	wxButton* btnDisplayMotionSimilarityMatrix = new wxButton(parent, 560, "SHOW", wxPoint(570, 104), wxSize(45, 22));
	btnDisplayMotionSimilarityMatrix->Bind(wxEVT_BUTTON, &HomeFrame::BtnMotionSimilarityMatrixViewClick, this);
	btnDisplayMotionSimilarityMatrix->Enable(false);

	// Future Cost Similarity Matrix - Label
	wxStaticText* lblFutureCostSelectSimilarityMatrix = new wxStaticText(parent, 565, "Future Cost Similarity Matrix:", wxPoint(10, 130));
	lblFutureCostSelectSimilarityMatrix->SetFont(lblFutureCostSelectSimilarityMatrix->GetFont().Scale(1.5));

	// Future Cost Similarity Matrix - Text Box
	wxTextCtrl* txtFutureCostSimilarityMatrixFilePath = new wxTextCtrl(parent, 570, "", wxPoint(240, 135), wxSize(300, 20), wxTE_READONLY);

	// Future Cost Similarity Matrix - Selection Button
	wxButton* btnFutureCostSimilarityMatrixSelect = new wxButton(parent, 575, "...", wxPoint(545, 134), wxSize(20, 22));
	btnFutureCostSimilarityMatrixSelect->Bind(wxEVT_BUTTON, &HomeFrame::BtnFutureCostSimilarityMatrixSelectClick, this);

	// Future Cost Similarity Matrix - View Button
	wxButton* btnDisplayFutureCostSimilarityMatrix = new wxButton(parent, 580, "SHOW", wxPoint(570, 134), wxSize(45, 22));
	btnDisplayFutureCostSimilarityMatrix->Bind(wxEVT_BUTTON, &HomeFrame::BtnFutureCostSimilarityMatrixViewClick, this);
	btnDisplayFutureCostSimilarityMatrix->Enable(false);

	// Dividing Line
	wxStaticLine* inputLineDivider = new wxStaticLine(parent, 585, wxPoint(0, 160), wxSize(620, 5));
}

void HomeFrame::BuildPreprocessingUI(wxPanel* parent) {
	// Heading
	wxStaticText* lblPreprocessingHeading = new wxStaticText(parent, 600, "VIDEO PREPROCESSING", wxPoint(10, 165));
	lblPreprocessingHeading->SetFont(lblPreprocessingHeading->GetFont().Scale(2));

	// Optional Indicator
	wxStaticText* lblPreprocessingOptionalIndicator = new wxStaticText(parent, 605, "OPTIONAL", wxPoint(545, 165));
	lblPreprocessingOptionalIndicator->SetFont(lblPreprocessingOptionalIndicator->GetFont().Scale(1.2));

	// Help Button
	wxContextHelpButton* btnPreprocessingHelp = new wxContextHelpButton(parent, 610, wxPoint(268, 170), wxSize(20, 20));
	btnPreprocessingHelp->Bind(wxEVT_BUTTON, &HomeFrame::BtnHelpClick, this);

	// Options - Label
	wxStaticText* lblOptions = new wxStaticText(parent, 612, "Option:", wxPoint(10, 200));
	lblOptions->SetFont(lblOptions->GetFont().Scale(1.2));

	// Options - Selection Box
	wxArrayString options;
	options.Add("Video Stabilisation");
	options.Add("Reduce Video Resolution (144p)");
	options.Add("Reduce Video Resolution (360p)");
	options.Add("Reduce Video Resolution (480p)");
	wxChoice* cmbOptions = new wxChoice(parent, 613, wxPoint(60, 200), wxSize(220, 40), options);
	cmbOptions->SetSelection(0);

	// Begin Button
	wxButton* btnBeginPreprocessing = new wxButton(parent, 615, "BEGIN", wxPoint(10, 230), wxSize(150, 50));
	btnBeginPreprocessing->Bind(wxEVT_BUTTON, &HomeFrame::BtnBeginPreprocessingClick, this);
	btnBeginPreprocessing->Enable(false);

	// Finished Indicator
	wxButton* btnPreprocessingFinishedIndicator = new wxButton(parent, 620, "", wxPoint(170, 230), wxSize(50, 50));
	btnPreprocessingFinishedIndicator->Enable(false);
	btnPreprocessingFinishedIndicator->SetBackgroundColour(*wxRED);

	// Output - Label
	wxStaticText* lblOutputPreprocessedVideo = new wxStaticText(parent, 625, "Output:", wxPoint(230, 225));
	lblOutputPreprocessedVideo->SetFont(lblOutputPreprocessedVideo->GetFont().Scale(1.5));

	// Output - Text Box
	wxTextCtrl* txtOutputPreprocessedVideoFilePath = new wxTextCtrl(parent, 630, "", wxPoint(230, 255), wxSize(300, 20), wxTE_READONLY);

	// Dividing Line
	wxStaticLine* preprocessingLineDivider = new wxStaticLine(parent, 635, wxPoint(0, 290), wxSize(620, 5));
}

void HomeFrame::BuildSimilarityMeasureUI(wxPanel* parent) {
	// Heading
	wxStaticText* lblSimilarityMeasureHeading = new wxStaticText(parent, 700, "SIMILARITY_MEASURE", wxPoint(10, 295));
	lblSimilarityMeasureHeading->SetFont(lblSimilarityMeasureHeading->GetFont().Scale(2));

	// Help Button
	wxContextHelpButton* btnSimilarityMeasureHelp = new wxContextHelpButton(parent, 705, wxPoint(250, 300), wxSize(20, 20));
	btnSimilarityMeasureHelp->Bind(wxEVT_BUTTON, &HomeFrame::BtnHelpClick, this);

	// Begin Button
	wxButton* btnBeginSimilarityMeasure = new wxButton(parent, 710, "BEGIN", wxPoint(10, 335), wxSize(150, 50));
	btnBeginSimilarityMeasure->Bind(wxEVT_BUTTON, &HomeFrame::BtnBeginSimilarityMeasureClick, this);
	btnBeginSimilarityMeasure->Enable(false);

	// Finished Indicator
	wxButton* btnSimilarityMeasureFinishedIndicator = new wxButton(parent, 715, "", wxPoint(170, 335), wxSize(50, 50));
	btnSimilarityMeasureFinishedIndicator->Enable(false);
	btnSimilarityMeasureFinishedIndicator->SetBackgroundColour(*wxRED);

	// Start Point - Label
	wxStaticText* lblSimilarityMeasureStartPoint = new wxStaticText(parent, 720, "Start Point:", wxPoint(230, 330));
	lblSimilarityMeasureStartPoint->SetFont(lblSimilarityMeasureStartPoint->GetFont().Scale(1.5));

	// Start Point - Selection Box
	wxArrayString choices;
	choices.Add("Video");
	choices.Add("Euclidean");
	choices.Add("Motion");
	wxChoice* cmbSimilarityMeasureStartPoint = new wxChoice(parent, 725, wxPoint(230, 360), wxSize(300, 20), choices);
	cmbSimilarityMeasureStartPoint->SetSelection(0);

	// Dividing Line
	wxStaticLine* similarityMeasureLineDivider = new wxStaticLine(parent, 730, wxPoint(0, 395), wxSize(620, 5));
}

void HomeFrame::BuildSynthesisUI(wxPanel* parent) {
	// Heading
	wxStaticText* lblSynthesisHeader = new wxStaticText(parent, 800, "SYNTHESIS", wxPoint(10, 400));
	lblSynthesisHeader->SetFont(lblSynthesisHeader->GetFont().Scale(2));

	// Help Button
	wxContextHelpButton* btnSynthesisHelp = new wxContextHelpButton(parent, 805, wxPoint(133, 405), wxSize(20, 20));
	btnSynthesisHelp->Bind(wxEVT_BUTTON, &HomeFrame::BtnHelpClick, this);

	// Length - Label
	wxStaticText* lblSynthesisLength = new wxStaticText(parent, 807, "Length (Multiplier):", wxPoint(10, 435));
	lblSynthesisLength->SetFont(lblSynthesisLength->GetFont().Scale(1.2));

	// Length - Selection Box
	wxArrayString length;
	for (int i = 1; i <= 10; i++) {
		length.Add(wxString(ToStr(i)));
	}
	wxChoice* cmbSynthesisLength = new wxChoice(parent, 808, wxPoint(130, 435), wxSize(30, 40), length);
	cmbSynthesisLength->SetSelection(2);

	// Begin Button
	wxButton* btnBeginSynthesis = new wxButton(parent, 810, "BEGIN", wxPoint(10, 465), wxSize(150, 50));
	btnBeginSynthesis->Bind(wxEVT_BUTTON, &HomeFrame::BtnBeginSynthesisClick, this);
	btnBeginSynthesis->Enable(false);

	// Finished Indicator
	wxButton* btnSynthesisFinishedIndicator = new wxButton(parent, 815, "", wxPoint(170, 465), wxSize(50, 50));
	btnSynthesisFinishedIndicator->Enable(false);
	btnSynthesisFinishedIndicator->SetBackgroundColour(*wxRED);

	// Output - Label
	wxStaticText* lblOutputVideoTexture = new wxStaticText(parent, 820, "Output:", wxPoint(230, 460));
	lblOutputVideoTexture->SetFont(lblOutputVideoTexture->GetFont().Scale(1.5));

	// Output - Text Box
	wxTextCtrl* txtOutputVideoTextureFilePath = new wxTextCtrl(parent, 825, "", wxPoint(230, 490), wxSize(300, 20), wxTE_READONLY);

	// Dividing Line
	wxStaticLine* synthesisLineDivider = new wxStaticLine(parent, 830, wxPoint(0, 525), wxSize(620, 5));
}

void HomeFrame::BuildRenderingUI(wxPanel* parent) {
	// Heading
	wxStaticText* lblRenderingHeader = new wxStaticText(parent, 900, "RENDERING", wxPoint(10, 530));
	lblRenderingHeader->SetFont(lblRenderingHeader->GetFont().Scale(2));

	// Optional Indicator
	wxStaticText* lblRenderingOptionalIndicator = new wxStaticText(parent, 905, "OPTIONAL", wxPoint(545, 530));
	lblRenderingOptionalIndicator->SetFont(lblRenderingOptionalIndicator->GetFont().Scale(1.2));

	// Help Button
	wxContextHelpButton* btnRenderingHelp = new wxContextHelpButton(parent, 910, wxPoint(142, 535), wxSize(20, 20));
	btnRenderingHelp->Bind(wxEVT_BUTTON, &HomeFrame::BtnHelpClick, this);

	// Options - Label
	wxStaticText* lblRenderingOptions = new wxStaticText(parent, 911, "Option:", wxPoint(10, 565));
	lblRenderingOptions->SetFont(lblRenderingOptions->GetFont().Scale(1.2));

	// Options - Selection Box
	wxArrayString options;
	options.Add("Cross-Fading");
	options.Add("Morphing");
	wxChoice* cmbRenderingOptions = new wxChoice(parent, 912, wxPoint(60, 565), wxSize(100, 40), options);
	cmbRenderingOptions->SetSelection(0);
	cmbRenderingOptions->Bind(wxEVT_CHOICE, &HomeFrame::BtnRenderingOptionSelection, this);

	// Window Size (Cross-Fading) - Label
	wxStaticText* lblCrossFadingWindowSize = new wxStaticText(parent, 913, "Window Size:", wxPoint(170, 565));
	lblCrossFadingWindowSize->SetFont(lblCrossFadingWindowSize->GetFont().Scale(1.2));

	// Window Size (Cross-Fading) - Selection Box
	wxArrayString crossFadingSize;
	crossFadingSize.Add("3x3");
	crossFadingSize.Add("5x5");
	crossFadingSize.Add("7x7");
	crossFadingSize.Add("9x9");
	wxChoice* cmbCrossFadingWindowSize = new wxChoice(parent, 914, wxPoint(255, 565), wxSize(45, 40), crossFadingSize);
	cmbCrossFadingWindowSize->SetSelection(0);

	// Number of Interpolated Frames (Morphing) - Label
	wxStaticText* lblNumberOfInterpolatedFrames = new wxStaticText(parent, 915, "Number of Interpolated Frames:", wxPoint(10, 595));
	lblNumberOfInterpolatedFrames->SetFont(lblNumberOfInterpolatedFrames->GetFont().Scale(1.2));

	// Number of Interpolated Frames (Morphing) - Selection Box
	wxArrayString numberOfInterpolatedFrames;
	numberOfInterpolatedFrames.Add("1");
	numberOfInterpolatedFrames.Add("2");
	numberOfInterpolatedFrames.Add("3");
	numberOfInterpolatedFrames.Add("4");
	numberOfInterpolatedFrames.Add("5");
	wxChoice* cmbNumberOfInterpolatedFrames = new wxChoice(parent, 916, wxPoint(210, 595), wxSize(30, 40), numberOfInterpolatedFrames);
	cmbNumberOfInterpolatedFrames->SetSelection(0);
	cmbNumberOfInterpolatedFrames->Disable();

	// Window Size (Morphing) - Label
	wxStaticText* lblMorphingWindowSize = new wxStaticText(parent, 917, "Window Size:", wxPoint(250, 595));
	lblMorphingWindowSize->SetFont(lblMorphingWindowSize->GetFont().Scale(1.2));

	// Window Size (Morphing) - Selection Box
	wxArrayString morphingSize;
	for (int i = 3; i <= 29; i += 2) {
		morphingSize.Add(ToStr(i) + "x" + ToStr(i));
	}
	wxChoice* cmbMorphingWindowSize = new wxChoice(parent, 918, wxPoint(335, 595), wxSize(55, 40), morphingSize);
	cmbMorphingWindowSize->SetSelection(5);
	cmbMorphingWindowSize->Disable();

	// Pixel Neighbourhood (Morphing) - Label
	wxStaticText* lblPixelNeighbourhood = new wxStaticText(parent, 919, "Pixel Neighbourhood:", wxPoint(400, 595));
	lblPixelNeighbourhood->SetFont(lblPixelNeighbourhood->GetFont().Scale(1.2));

	// Pixel Neighbourhood (Morphing) - Selection Box
	wxArrayString pixelNeighbourhoods;
	pixelNeighbourhoods.Add("5x5");
	pixelNeighbourhoods.Add("7x7");
	wxChoice* cmbPixelNeighbourhood = new wxChoice(parent, 921, wxPoint(535, 595), wxSize(45, 40), pixelNeighbourhoods);
	cmbPixelNeighbourhood->SetSelection(0);
	cmbPixelNeighbourhood->Disable();

	// Interpolation (Morphing) - Label
	wxStaticText* lblInterpolation = new wxStaticText(parent, 922, "Interpolation:", wxPoint(10, 625));
	lblInterpolation->SetFont(lblInterpolation->GetFont().Scale(1.2));

	// Interpolation (Morphing) - Selection Box
	wxArrayString interpolations;
	interpolations.Add("Nearest Neighbour");
	interpolations.Add("Bilinear");
	interpolations.Add("Bicubic");
	wxChoice* cmbInterpolation = new wxChoice(parent, 923, wxPoint(95, 625), wxSize(130, 40), interpolations);
	cmbInterpolation->SetSelection(1);
	cmbInterpolation->Disable();

	// Begin Button
	wxButton* btnBeginRendering = new wxButton(parent, 920, "BEGIN", wxPoint(10, 655), wxSize(150, 50));
	btnBeginRendering->Bind(wxEVT_BUTTON, &HomeFrame::BtnBeginRenderingClick, this);
	btnBeginRendering->Enable(false);

	// Finished Indicator
	wxButton* btnRenderingFinishedIndicator = new wxButton(parent, 925, "", wxPoint(170, 655), wxSize(50, 50));
	btnRenderingFinishedIndicator->Enable(false);
	btnRenderingFinishedIndicator->SetBackgroundColour(*wxRED);

	// Output - Label
	wxStaticText* lblOutputRenderedVideoTexture = new wxStaticText(parent, 930, "Output:", wxPoint(230, 650));
	lblOutputRenderedVideoTexture->SetFont(lblOutputRenderedVideoTexture->GetFont().Scale(1.5));

	// Output - Text Box
	wxTextCtrl* txtRenderedOutputVideoTextureFilePath = new wxTextCtrl(parent, 935, "", wxPoint(230, 680), wxSize(300, 20), wxTE_READONLY);
}

// Updates UI elements after each user interaction
void HomeFrame::UpdateUI() {
	// Update file paths
	this->FindWindowById(515)->SetLabel(input.GetVideoFilePath());
	this->FindWindowById(530)->SetLabel(input.GetEuclideanFilePath());
	this->FindWindowById(550)->SetLabel(input.GetMotionFilePath());
	this->FindWindowById(570)->SetLabel(input.GetFutureCostFilePath());
	this->FindWindowById(825)->SetLabel(output.GetVideoFilePath());

	// Euclidean Display
	if ((input.GetVideoFilePath() != "") && (input.GetEuclideanFilePath() != ""))
		this->FindWindowById(540)->Enable(true);

	// Motion Display
	if ((input.GetVideoFilePath() != "") && (input.GetMotionFilePath() != ""))
		this->FindWindowById(560)->Enable(true);

	// Future Cost Display
	if ((input.GetVideoFilePath() != "") && (input.GetFutureCostFilePath() != ""))
		this->FindWindowById(580)->Enable(true);

	// Video Processing Begin
	if (input.GetVideoFilePath() != "")
		this->FindWindowById(615)->Enable(true);
	
	// Similarity Measure Begin
	if ((input.GetVideoFilePath() != "") || (input.GetMotionFilePath() != "") || (input.GetMotionFilePath() != ""))
		this->FindWindowById(710)->Enable(true);

	// Synthesis Begin
	if ((input.GetVideoFilePath() != "") && (input.GetMotionFilePath() != "") && (input.GetFutureCostFilePath() != ""))
		this->FindWindowById(810)->Enable(true);

	// Rendering Begin
	if (output.GetVideoFilePath() != "")
		this->FindWindowById(920)->Enable(true);
}

//--------------------------------------------------------------------------------------
// Event Handlers
//--------------------------------------------------------------------------------------

// Event handler for any help button click
void HomeFrame::BtnHelpClick(wxCommandEvent& e) {
	int id = e.GetId();
	string message;

	if (id == 505)
		message = "Select your inputs for the program.\n\n"
				  "A video is necessary however similarity matrices (in the form of distance matrices) are optional since they can be generated from the video.";
	else if (id == 610)
		message = "Apply preprocessing techniques to your input video.\n\n" 
			      "Video stabilisation will reduce the amount of jitter in your video from camera movement, resulting in a smoother video.\n\n"
				  "Reducing the resolution of the video will reduce the amount of detail present, making the video smaller and (in theory) increasing the similarity of frames.";
	else if (id == 705)
		message = "Finds the similarity matrices for your input.\n\n"
				  "We can start this process from different stages, and it may not be necessary if you start with full inputs.";
	else if (id == 805)
		message = "Creates a video texture from the similarity matrices.\n\n"
				  "This is only possible if we have a video, motion matrix and future cost matrix.\n\n"
				  "The length multplier helps to determine the length of the video texture (this is calculated as the product of the multiplier and the longest transition).";
	else if (id == 910)
		message = "Applies rendering techniques to the synthesis process of the video texture. Number of frames signifies the number of extra frames to be added to the video texture\n\n"
				  "Cross-fading will create a simple fade at transition points.\n\n"
				  "Morphing uses the optical flow between frames to interpolate new frames.";
	else
		message = "An error has occurred";

	wxMessageBox(wxString(message), "Help", wxOK);
}

// Event handler for select video button
void HomeFrame::BtnVideoSelectClick(wxCommandEvent& e) {
	// Open file dialog so user can select video
	wxFileDialog* fileDialog = new wxFileDialog(this);

	if (fileDialog->ShowModal() == wxID_OK)
	{
		// Save file path selected by user
		string filePath = string(fileDialog->GetPath());
		input.SetVideoFilePath(filePath);

		// Update UI elements
		UpdateUI();
	}

	fileDialog->Destroy();
}

// Event handler for select Euclidean similarity matrix button
void HomeFrame::BtnEuclideanSimilarityMatrixSelectClick(wxCommandEvent& e) {
	// Open file dialog so user can select similarity matrix
	wxFileDialog* fileDialog = new wxFileDialog(this);

	if (fileDialog->ShowModal() == wxID_OK)
	{
		// Save Euclidean matrix selected by user
		string filePath = string(fileDialog->GetPath());
		input.SetEuclideanFilePath(filePath);
		input.SetEuclidean(ReadCSVFile(filePath));

		// Update UI elements
		UpdateUI();
	}

	fileDialog->Destroy();
}

// Event handler for display Euclidean similarity matrix button
void HomeFrame::BtnEuclideanSimilarityMatrixViewClick(wxCommandEvent& e) {
	MatrixFrame* matrixFrame = new MatrixFrame("Euclidean Similarity Matrix", input.GetVideoFilePath(), input.GetEuclidean().GetDistanceMatrix(), input.GetEuclidean().GetProbabilityMatrix(), "euclidean");
	matrixFrame->SetClientSize(1010, 625);
	matrixFrame->Center();
	matrixFrame->Show(true);
}

// Event handler for select motion similarity matrix button
void HomeFrame::BtnMotionSimilarityMatrixSelectClick(wxCommandEvent& e) {
	// Open file dialog so user can select similarity matrix
	wxFileDialog* fileDialog = new wxFileDialog(this);

	if (fileDialog->ShowModal() == wxID_OK)
	{
		// Save motion matrix selected by user
		string filePath = string(fileDialog->GetPath());
		input.SetMotionFilePath(filePath);
		input.SetMotion(ReadCSVFile(filePath));

		// Update UI elements
		UpdateUI();
	}

	fileDialog->Destroy();
}

// Event handler for display motion similarity matrix button
void HomeFrame::BtnMotionSimilarityMatrixViewClick(wxCommandEvent& e) {
	MatrixFrame* matrixFrame = new MatrixFrame("Motion Similarity Matrix", input.GetVideoFilePath(), input.GetMotion().GetDistanceMatrix(), input.GetMotion().GetProbabilityMatrix(), "motion");
	matrixFrame->SetClientSize(1010, 625);
	matrixFrame->Center();
	matrixFrame->Show(true);
}

// Event handler for select future cost similarity matrix button
void HomeFrame::BtnFutureCostSimilarityMatrixSelectClick(wxCommandEvent& e) {
	// Open file dialog so user can select similarity matrix
	wxFileDialog* fileDialog = new wxFileDialog(this);

	if (fileDialog->ShowModal() == wxID_OK)
	{
		// Save future cost matrix selected by user
		string filePath = string(fileDialog->GetPath());
		input.SetFutureCostFilePath(filePath);
		input.SetFutureCost(ReadCSVFile(filePath));

		// Update UI elements
		UpdateUI();
	}

	fileDialog->Destroy();
}

// Event handler for display future cost similarity matrix button
void HomeFrame::BtnFutureCostSimilarityMatrixViewClick(wxCommandEvent& e) {
	MatrixFrame* matrixFrame = new MatrixFrame("Future Cost Similarity Matrix", input.GetVideoFilePath(), input.GetFutureCost().GetDistanceMatrix(), input.GetFutureCost().GetProbabilityMatrix(), "future");
	matrixFrame->SetClientSize(1010, 625);
	matrixFrame->Center();
	matrixFrame->Show(true);
}

// Event handler for begin preprocessing button
void HomeFrame::BtnBeginPreprocessingClick(wxCommandEvent& e) {
	wxLogStatus("VIDEO PREPROCESSING: Begin");

	string outputVideoFilePath;
	int option = wxDynamicCast(this->FindWindowById(613), wxChoice)->GetSelection();

	if (option == 0)
		outputVideoFilePath = VideoPreprocessing::VideoStabilisation(input.GetVideoFilePath());
	else if (option == 1)
		outputVideoFilePath = VideoPreprocessing::ReduceVideoResolution(input.GetVideoFilePath(), 144);
	else if (option == 2)
		outputVideoFilePath = VideoPreprocessing::ReduceVideoResolution(input.GetVideoFilePath(), 360);
	else if (option == 3)
		outputVideoFilePath = VideoPreprocessing::ReduceVideoResolution(input.GetVideoFilePath(), 480);

	wxLogStatus("VIDEO PREPROCESSING: Finish");

	// Update UI
	this->FindWindowById(630)->SetLabel(outputVideoFilePath);
	this->FindWindowById(620)->SetBackgroundColour(*wxGREEN);
}

// Event handler for begin similarity measure button
void HomeFrame::BtnBeginSimilarityMeasureClick(wxCommandEvent& e) {
	wxLogStatus("SIMILARITY MEASURE: Begin");

	// Get start point for similarity measure
	int selection = wxDynamicCast(this->FindWindowById(725), wxChoice)->GetSelection();

	// Perform similarity measure
	if (selection == 2) { // Motion
		input.SetFutureCost(SimilarityMeasure::ComputeFutureCostSimilarityMatrix(input.GetVideoFilePath(), input.GetMotion()));
		wxLogStatus("SIMILARITY MEASURE: Future Cost Similarity Matrix Complete");
		input.SetFilePaths("001");
	}
	else if (selection == 1) { // Euclidean
		input.SetMotion(SimilarityMeasure::ComputeMotionSimilarityMatrix(input.GetVideoFilePath(), input.GetEuclidean()));
		wxLogStatus("SIMILARITY MEASURE: Motion Similarity Matrix Complete");
		input.SetFutureCost(SimilarityMeasure::ComputeFutureCostSimilarityMatrix(input.GetVideoFilePath(), input.GetMotion()));
		wxLogStatus("SIMILARITY MEASURE: Future Cost Similarity Matrix Complete");
		input.SetFilePaths("011");
	}
	else { // Video
		input.SetEuclidean(SimilarityMeasure::ComputeEuclideanSimilarityMatrix(input.GetVideoFilePath()));
		wxLogStatus("SIMILARITY MEASURE: Euclidean Similarity Matrix Complete");
		input.SetMotion(SimilarityMeasure::ComputeMotionSimilarityMatrix(input.GetVideoFilePath(), input.GetEuclidean()));
		wxLogStatus("SIMILARITY MEASURE: Motion Similarity Matrix Complete");
		input.SetFutureCost(SimilarityMeasure::ComputeFutureCostSimilarityMatrix(input.GetVideoFilePath(), input.GetMotion()));
		wxLogStatus("SIMILARITY MEASURE: Future Cost Similarity Matrix Complete");
		input.SetFilePaths("111");
	}
	wxLogStatus("SIMILARITY MEASURE: Finish");

	// Update UI
	this->FindWindowById(715)->SetBackgroundColour(*wxGREEN);
	UpdateUI();
}

// Event handler for begin synthesis button
void HomeFrame::BtnBeginSynthesisClick(wxCommandEvent& e) {
	this->FindWindowById(815)->SetBackgroundColour(*wxRED);
	wxLogStatus("SYNTHESIS: Begin");

	int lengthMultiplier = wxDynamicCast(this->FindWindowById(808), wxChoice)->GetSelection();

	// Create video texture
	output.SetScheduledTransitions(Synthesis::GetTransitionSet(input.GetMotion().GetDistanceMatrix(), input.GetFutureCost().GetDistanceMatrix(), lengthMultiplier));
	output.SetVideoFilePath(Synthesis::CreateVideoTexture(input.GetVideoFilePath(), output.GetScheduledTransitions()));
	wxLogStatus("SYNTHESIS: Finish");

	// Update UI
	this->FindWindowById(815)->SetBackgroundColour(*wxGREEN);
	UpdateUI();
}

// Event handler for begin rendering button
void HomeFrame::BtnBeginRenderingClick(wxCommandEvent& e) {
	this->FindWindowById(925)->SetBackgroundColour(*wxRED);
	wxLogStatus("RENDERING: Begin");

	// Get parameters
	int option = wxDynamicCast(this->FindWindowById(912), wxChoice)->GetSelection();
	int crossFadingWindowSize = 3 + (2 * wxDynamicCast(this->FindWindowById(914), wxChoice)->GetSelection());
	int morphingInterpolatedFrames = wxDynamicCast(this->FindWindowById(916), wxChoice)->GetSelection() + 1;
	int morphingWindowSize = 3 + (2 * wxDynamicCast(this->FindWindowById(918), wxChoice)->GetSelection());
	int morphingPixelNeighbourhood =  5 + (2 * wxDynamicCast(this->FindWindowById(921), wxChoice)->GetSelection());
	int morphingInterpolation = wxDynamicCast(this->FindWindowById(923), wxChoice)->GetSelection();
	int morphingParameters[4] = { morphingInterpolatedFrames, morphingWindowSize, morphingPixelNeighbourhood, morphingInterpolation };

	string outputVideoFilePath;
	if (option == 0)
		outputVideoFilePath = Rendering::CreateVideoTextureWithCrossFading(input.GetVideoFilePath(), output.GetScheduledTransitions(), crossFadingWindowSize);
	else if (option == 1)
		outputVideoFilePath = Rendering::CreateVideoTextureWithMorphing(input.GetVideoFilePath(), output.GetScheduledTransitions(), morphingParameters);

	wxLogStatus("RENDERING: Finish");

	// Update UI
	this->FindWindowById(935)->SetLabel(outputVideoFilePath);
	this->FindWindowById(925)->SetBackgroundColour(*wxGREEN);
}

// Event handler for rendering method selection box
void HomeFrame::BtnRenderingOptionSelection(wxCommandEvent& e) {

	// Disable all fields initially
	this->FindWindowById(914)->Enable(false);
	this->FindWindowById(916)->Enable(false);
	this->FindWindowById(918)->Enable(false);
	this->FindWindowById(921)->Enable(false);
	this->FindWindowById(923)->Enable(false);

	// Enable fields based on current selection
	if (e.GetSelection() == 0)
		this->FindWindowById(914)->Enable(true);
	else {
		this->FindWindowById(916)->Enable(true);
		this->FindWindowById(918)->Enable(true);
		this->FindWindowById(921)->Enable(true);
		this->FindWindowById(923)->Enable(true);
	}
}