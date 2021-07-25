#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp() {
	//readFile("example.ic");

}

cApp::~cApp() {

}

bool cApp::OnInit() {
	CustomImageReader* imgReader = new CustomImageReader;
	imgReader->SetMimeType("application/icimage");
	wxImage::AddHandler(imgReader);

	m_frame1 = new cMain();
	m_frame1->Show();

	return true;
}