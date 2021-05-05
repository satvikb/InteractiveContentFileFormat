#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp() {
	//readFile("example.ic");

}

cApp::~cApp() {

}

bool cApp::OnInit() {
	m_frame1 = new cMain();
	m_frame1->Show();

	return true;
}