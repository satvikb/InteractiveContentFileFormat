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

	// Instead of initalizing all handlers, lets just init the handlers IC currently supports.
	//wxInitAllImageHandlers();
	//wxImage::AddHandler(new wxPNGHandler);
	//wxImage::AddHandler(new wxJPEGHandler);
	//wxImage::AddHandler(new wxGIFHandler);

	m_frame1 = new cMain();
	m_frame1->Show();

	return true;
}