#pragma once
#include "Video.h"
#include "VideoTexture.h"
#include <wx/wx.h>

using namespace std;

// HomeFrame
// - Represents the top-level (main) frame in the application

class HomeFrame : public wxFrame {

	public:
		// Constructors
		HomeFrame(const wxString& title);

	private:
		// Parameters
		Video input;
		VideoTexture output;

		// Instance Methods (UI)
		void BuildInputUI(wxPanel* parent);
		void BuildPreprocessingUI(wxPanel* parent);
		void BuildSimilarityMeasureUI(wxPanel* parent);
		void BuildSynthesisUI(wxPanel* parent);
		void BuildRenderingUI(wxPanel* parent);
		void UpdateUI();

		// Event Handlers
		void BtnHelpClick(wxCommandEvent& e);
		void BtnVideoSelectClick(wxCommandEvent& e);
		void BtnEuclideanSimilarityMatrixSelectClick(wxCommandEvent& e);
		void BtnEuclideanSimilarityMatrixViewClick(wxCommandEvent& e);
		void BtnMotionSimilarityMatrixSelectClick(wxCommandEvent& e);
		void BtnMotionSimilarityMatrixViewClick(wxCommandEvent& e);
		void BtnFutureCostSimilarityMatrixSelectClick(wxCommandEvent& e);
		void BtnFutureCostSimilarityMatrixViewClick(wxCommandEvent& e);
		void BtnBeginPreprocessingClick(wxCommandEvent& e);
		void BtnBeginSimilarityMeasureClick(wxCommandEvent& e);
		void BtnBeginSynthesisClick(wxCommandEvent& e);
		void BtnBeginRenderingClick(wxCommandEvent& e);
		void BtnRenderingOptionSelection(wxCommandEvent& e);
};