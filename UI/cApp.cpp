#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp() {
	//readFile("example.ic");

}

cApp::~cApp() {

}

bool cApp::OnInit() {
	wxImage::AddHandler(new CustomImageReader);
	m_frame1 = new cMain();
	m_frame1->Show();

	return true;
}