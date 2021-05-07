#pragma once

#include <wx/wx.h>
#include <map>
#include "FileManager.h"
#include "WindowManager.h"
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

