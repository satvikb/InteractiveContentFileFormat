#include "cMain.h"
#if !wxUSE_CONSTRAINTS
#error You must set wxUSE_CONSTRAINTS to 1 in setup.h!
#endif

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Interactive Content Viewer") {
	SetAutoLayout(true);

	m_pMenuBar = new wxMenuBar();
	// File Menu
	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(wxID_OPEN, _T("&Open"));
	m_pFileMenu->Append(wxID_SAVE, _T("&Open"));

	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(wxID_EXIT, _T("&Quit"));
	m_pMenuBar->Append(m_pFileMenu, _T("&File"));
	// About menu
	m_pHelpMenu = new wxMenu();
	m_pHelpMenu->Append(wxID_ABOUT, _T("&About"));
	m_pMenuBar->Append(m_pHelpMenu, _T("&Help"));

	SetMenuBar(m_pMenuBar);

	FileManager::loadFile("styles.ic");
	//FileManager::loadFile("example.ic");

	aspectRatio = FileManager::getHeader()->windowAspectRatio;

	uint16_t startContainerID = FileManager::getStartContainerID();
	mainContainer = new cContainer(true, this);
	mainContainer->CreateContainerUI(FileManager::getContainerAndLayoutByID(startContainerID));

	wxLayoutConstraints* fullScreenCons = new wxLayoutConstraints();
	fullScreenCons->left.PercentOf(this, wxWidth, 0);
	fullScreenCons->top.PercentOf(this, wxHeight, 0);
	fullScreenCons->width.PercentOf(this, wxWidth, 100);
	fullScreenCons->height.PercentOf(this, wxHeight, 100);
	mainContainer->SetConstraints(fullScreenCons);

	WindowManager::SetTopContainer(mainContainer);
}

bool cMain::ShouldMaintainAspectRatio() {
	// TODO if this ever becomes more complictated, this check is done every size change frame for now
	return aspectRatio > 0.f;
}

BEGIN_EVENT_TABLE(cMain, wxPanel)
// Size event
EVT_SIZE(cMain::OnSize)
EVT_MENU(wxID_EXIT, cMain::OnExit)

END_EVENT_TABLE()

void cMain::OnSize(wxSizeEvent& event) {
	if (ShouldMaintainAspectRatio()) {
		wxSize newSize = event.GetSize();
		// skip the event.
		float RATIO = 1.f / aspectRatio;

		float width = (float)GetSize().GetWidth();
		float height = (float)GetSize().GetHeight();

		float newWidth = newSize.GetWidth();
		float newHeight = newSize.GetHeight();

		wxSize scaledSize = wxSize(100, 100);
		if (RATIO >= 1.f) {
			bool widthLarger = newWidth > width;

			if (widthLarger) {
				scaledSize = wxSize(newWidth, newWidth / RATIO);
			} else {
				scaledSize = wxSize(newHeight / RATIO, newHeight);
			}
		} else {
			bool heightLarger = newHeight != height;
			if (heightLarger) {
				scaledSize = wxSize(newHeight * RATIO, newHeight);
			} else {
				scaledSize = wxSize(newWidth, newWidth * RATIO);
			}
		}
		SetSize(scaledSize);
	}
	event.Skip();
}

void cMain::OnExit(wxCommandEvent& WXUNUSED(event)) {
	// when the user selects "Exit" from the menu we should close
	Close(true);
}

cMain::~cMain() {}