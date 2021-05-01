#pragma once

#include <wx/wx.h>

class cMain : public wxFrame
{

public:
	cMain();
	~cMain();

private:
	wxMenuBar* m_pMenuBar;
	wxMenu* m_pFileMenu;
	wxMenu* m_pHelpMenu;

	wxPanel* m_panel;
};

