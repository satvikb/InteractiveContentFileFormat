#include "cContainer.h"

cContainer::cContainer(bool mainContainer, wxWindow* parent) {
	Create(parent, wxID_ANY);
	isMainContainer = mainContainer;
}

cContainer::cContainer(cContainer *parent) {
	// create this container itself
	Create(parent, wxID_ANY);

	// create subelements according to layout
}

void cContainer::CreateContainerUI(std::pair<struct Container*, struct Layout*> dataPair) {
    CreateContainerUI(dataPair.first, dataPair.second);
}

void cContainer::CreateContainerUI(struct Container* container, struct Layout* layout) {

    if (children.size() > 0) {
        // TODO: loop through existing windows and delete (memory)
        for (int i = 0; i < children.size(); i++) {
            children[i]->Show(false);
            children[i]->Destroy();
            delete children[i];
        }
    }

    for (int i = 0; i < container->elementIDs.size(); i++) {
        elementPosition* pos = layout->positions[i];

        std::vector<uint16_t> subelements = container->elementIDs[i];

        for (int j = 0; j < subelements.size(); j++) {
            // and do what with this id...? maybe have to pass in the actual file? maybe have a static FileReader class?
            uint16_t ID = subelements[j];

            struct Container* container = FileManager::getContainerByID(ID);
            if (container != nullptr) {
                // create another cContainer
            }
            else {
                // try content
                struct Content* content = FileManager::getContentByID(ID);
                if (content != nullptr) {

                }
                else {
                    // ?
                }
            }
        }
    }

    ////m_panel = new wxPanel(this, wxID_ANY);
    cRichTextView *richTextCtrl = new cRichTextView((cContainer *)this);

    //wxPanel* m_panel2 = new wxPanel(this, wxID_ANY);

    ////m_panel->SetAutoLayout(true);

    //wxStaticText* text = new wxStaticText(this, wxID_ANY, "Resizable");
    //wxStaticText* text2 = new wxStaticText(m_panel2, wxID_ANY, "Child");

    //// (0.1, 0.1, 0.8, 0.2)
    //wxLayoutConstraints* cons = new wxLayoutConstraints();
    //cons->left.PercentOf(this, wxWidth, 10);
    //cons->top.PercentOf(this, wxHeight, 10);
    //cons->width.PercentOf(this, wxWidth, 80);
    //cons->height.PercentOf(this, wxHeight, 20);
    //text->SetConstraints(cons);

    //// (0.1, 0.3, 0.8, 0.6)
    wxLayoutConstraints* cons2 = new wxLayoutConstraints();
    cons2->left.PercentOf(this, wxWidth, 10);
    cons2->top.PercentOf(this, wxHeight, 30);
    cons2->width.PercentOf(this, wxWidth, 80);
    cons2->height.PercentOf(this, wxHeight, 60);
    richTextCtrl->SetConstraints(cons2);

    //// (0.92, 0.1, 0.06, 0.9)
    //wxLayoutConstraints* cons3 = new wxLayoutConstraints();
    //cons3->left.PercentOf(this, wxWidth, 92);
    //cons3->top.PercentOf(this, wxHeight, 10);
    //cons3->width.PercentOf(this, wxWidth, 6);
    //cons3->height.PercentOf(this, wxHeight, 90);

    //m_panel2->SetConstraints(cons3);

    //// child text (0, 0.4, 1, 0.6)
    //wxLayoutConstraints* cons4 = new wxLayoutConstraints();
    //cons4->left.PercentOf(m_panel2, wxWidth, 0);
    //cons4->top.PercentOf(m_panel2, wxHeight, 40);
    //cons4->width.PercentOf(m_panel2, wxWidth, 100);
    //cons4->height.PercentOf(m_panel2, wxHeight, 60);

    //text2->SetConstraints(cons4);
}

cContainer::~cContainer() {

}
