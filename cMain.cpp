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
    m_pFileMenu->Append(wxID_SAVE, _T("&Save"));
    m_pFileMenu->AppendSeparator();
    m_pFileMenu->Append(wxID_EXIT, _T("&Quit"));
    m_pMenuBar->Append(m_pFileMenu, _T("&File"));
    // About menu
    m_pHelpMenu = new wxMenu();
    m_pHelpMenu->Append(wxID_ABOUT, _T("&About"));
    m_pMenuBar->Append(m_pHelpMenu, _T("&Help"));

    SetMenuBar(m_pMenuBar);

    FileManager::loadFile("example.ic");
    
    mainContainer = new cContainer(true, this);
    mainContainer->CreateContainerUI(FileManager::getContainerAndLayoutByID(FileManager::getStartContainerID()));

    
}



void clickHandler(wxRichTextEvent& event) {

}

cMain::~cMain(){

}