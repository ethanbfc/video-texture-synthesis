#include "App.h"
#include "HomeFrame.h"

wxIMPLEMENT_APP(App);

// Application entry point
bool App::OnInit() {
	HomeFrame* homeFrame = new HomeFrame("Video Texture Synthesis");
	homeFrame->SetClientSize(620, 715);
	homeFrame->Center();
	homeFrame->Show();
	return true;
}