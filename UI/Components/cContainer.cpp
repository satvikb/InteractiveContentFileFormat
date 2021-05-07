#include "cContainer.h"

cContainer::cContainer(bool mainContainer, wxWindow* parent) {
	Create(parent, wxID_ANY);
	isMainContainer = mainContainer;
}

void cContainer::CreateContainerUI(std::pair<struct Container*, struct Layout*> dataPair) {
    CreateContainerUI(dataPair.first, dataPair.second);
}

void cContainer::CreateContainerUI(struct Container* container, struct Layout* layout) {
    this->container = container;
    this->layout = layout;
    if (children.size() > 0) {
        // TODO: loop through existing windows and delete (memory)
        for (int i = 0; i < children.size(); i++) {
            children[i]->Show(false);
            children[i]->Destroy();
            
        }
        children.clear();
    }

    for (int i = 0; i < container->elementIDs.size(); i++) {
        elementPosition* pos = layout->positions[i];

        std::vector<uint16_t> subelements = container->elementIDs[i];

        // should be one, unless infinite
        for (int j = 0; j < subelements.size(); j++) {
            uint16_t ID = subelements[j];

            struct Container* container = FileManager::getContainerByID(ID);
            if (container != nullptr) {
                // create another cContainer
            }
            else {
                // try content
                struct Content* content = FileManager::getContentByID(ID);
                if (content != nullptr) {
                    // TODO decide how to handle inviidual content types
                    // testing for positioning

                    wxConstraintPosition wxPos = FileManager::convertLayoutPositionToConstraint(pos);
                    cRichTextView* richTextCtrl = new cRichTextView((cContainer*)this);

                    wxLayoutConstraints* posCons = new wxLayoutConstraints();
                    posCons->left.PercentOf(this, wxWidth, wxPos.x);
                    posCons->top.PercentOf(this, wxHeight, wxPos.y);
                    posCons->width.PercentOf(this, wxWidth, wxPos.width);
                    posCons->height.PercentOf(this, wxHeight, wxPos.height);
                    richTextCtrl->SetConstraints(posCons);

                    richTextCtrl->SetContent(content);

                    children.push_back(richTextCtrl);
                }
                else {
                    // ?
                }
            }
        }
    }

    ////m_panel = new wxPanel(this, wxID_ANY);

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

bool cContainer::getIsMainContainer() {
    return isMainContainer;
}

wxWindow* cContainer::getParentWindow() {
    return parentWindow;
}

Container* cContainer::getContainer() {
    return container;
}

cContainer::~cContainer() {
    // TODO
}



void cContainer::applyParentContraints(wxWindow* applyTo, wxWindow* parent, struct wxConstraintPosition constraints) {
    wxLayoutConstraints* posCons = new wxLayoutConstraints();
    posCons->left.PercentOf(parent, wxWidth, constraints.x);
    posCons->top.PercentOf(parent, wxHeight, constraints.y);
    posCons->width.PercentOf(parent, wxWidth, constraints.width);
    posCons->height.PercentOf(parent, wxHeight, constraints.height);
    applyTo->SetConstraints(posCons);
}
