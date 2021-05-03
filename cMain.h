#pragma once

#include <wx/wx.h>

#include "FileManager.h"
#include "cContainer.h"

class cMain : public wxFrame
{

public:
	cMain();
	~cMain();

private:
	wxMenuBar* m_pMenuBar;
	wxMenu* m_pFileMenu;
	wxMenu* m_pHelpMenu;

	cContainer* mainContainer;
};

