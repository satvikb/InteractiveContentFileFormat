#include "cMain.h"
#if !wxUSE_CONSTRAINTS
#error You must set wxUSE_CONSTRAINTS to 1 in setup.h!
#endif

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Title") {
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


    m_panel = new wxPanel(this, wxID_ANY, wxPoint(100, 100), wxSize(100, 100));
    //m_panel->SetAutoLayout(true);

    wxStaticText* text = new wxStaticText(this, wxID_ANY, "Ungrowable:", wxPoint(120, 0), wxSize(100, 100));
    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, "sdafse:", wxPoint(120, 0), wxSize(100, 100));

    wxLayoutConstraints* cons = new wxLayoutConstraints();
    //wxIndividualLayoutConstraint* w = new wxIndividualLayoutConstraint();
    ////w->Set(wxPercentOf, this, wxWidth, 20);
    //w->PercentOf(this, wxWidth, 20);

    //wxIndividualLayoutConstraint* x = new wxIndividualLayoutConstraint();
    //x->Set(wxPercentOf, this, wxWidth, 50);

    //cons->width = *w;
    //cons->left = *x;
    cons->centreX.SameAs(this, wxCentreX);
    cons->top.SameAs(this, wxTop, 5);
    cons->width.PercentOf(this, wxWidth, 80);
    cons->height.PercentOf(this, wxHeight, 10);

    text->SetConstraints(cons);

    wxLayoutConstraints* cons2 = new wxLayoutConstraints();
    //wxIndividualLayoutConstraint* w = new wxIndividualLayoutConstraint();
    ////w->Set(wxPercentOf, this, wxWidth, 20);
    //w->PercentOf(this, wxWidth, 20);

    //wxIndividualLayoutConstraint* x = new wxIndividualLayoutConstraint();
    //x->Set(wxPercentOf, this, wxWidth, 50);

    //cons->width = *w;
    //cons->left = *x;
    cons2->centreX.SameAs(this, wxCentreX);
    cons2->top.SameAs(this, wxTop, 5);
    cons2->width.PercentOf(this, wxWidth, 80);
    cons2->height.PercentOf(this, wxHeight, 10);

    m_panel->SetConstraints(cons2);
    //text2->SetConstraints(cons);

    //int c = 0;
    //cons->SatisfyConstraints(m_panel, &c);
    //m_panel->Layout();
    //Layout();
    //std::cout << c << std::endl;
}

cMain::~cMain(){

}