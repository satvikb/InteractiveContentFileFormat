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
                    richTextCtrl->applyComponentStyle(FileManager::getStyleByID(pos->styleID));

                    children.push_back(richTextCtrl);
                }
                else {
                    // ?
                }
            }
        }
    }
}

void cContainer::SetContainerStyle(struct Style* style) {
    // c++17 style loop
    for (auto const& [key, val] : style->styles) {
        switch (key) {
        case STYLE_COMPONENT_BACKGROUND_COLOR: {
            SetBackgroundColour(std::any_cast<wxColour>(val));
        }
        break;
        }
    }
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
