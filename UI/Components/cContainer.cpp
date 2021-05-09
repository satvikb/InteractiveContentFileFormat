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
            std::shared_ptr<wxWindow> win = children[i];
            children[i]->Show(false);
            // TODO crashes because of using smart pointers, maybe try wxWindowPtr?
            //children[i]->Destroy();
        }

        // TODO MEMORY MANAGEMENT, DELETE THE WINDOWS
        children.clear();
    }

    for (int i = 0; i < container->elementIDs.size(); i++) {
        elementPosition* pos = layout->positions[i];

        std::vector<Chunk> subelements = container->elementIDs[i];

        // should be one, unless infinite
        for (int j = 0; j < subelements.size(); j++) {
            Chunk ID = subelements[j];

            struct Container* container = FileManager::getContainerByID(ID.chunkID);
            if (container != nullptr) {
                // create another cContainer
            }
            else {
                // try content
                struct Content* content = FileManager::getContentByID(ID.chunkID);
                if (content != nullptr) {
                    // TODO decide how to handle inviidual content types
                    // testing for positioning
                    CreateContent(content, pos);
                }
                else {
                    // ?
                }
            }
        }
    }
}

void cContainer::CreateContent(struct Content* content, struct elementPosition* pos, uint8_t index) {
    uint8_t contentType = content->type;

    std::shared_ptr<wxWindow> newContent(nullptr);

    switch (contentType) {
    case CONTENT_TEXT: {
        //std::shared_ptr<cRichTextView> richTextCtrl = std::make_shared<cRichTextView>((cContainer*)this);
        newContent = std::make_shared<cRichTextView>((cContainer*)this);


        std::static_pointer_cast<cRichTextView>(newContent)->SetContent(content);
        std::static_pointer_cast<cRichTextView>(newContent)->applyComponentStyle(FileManager::getStyleByID(pos->styleID));
    }
    break;
    }

    if (index >= 0 && index < children.size()) {
        children[index] = newContent;
    }
    else {
        children.push_back(newContent);
    }

    wxConstraintPosition wxPos = FileManager::convertLayoutPositionToConstraint(pos);
    wxLayoutConstraints* posCons = new wxLayoutConstraints();
    posCons->left.PercentOf(this, wxWidth, wxPos.x);
    posCons->top.PercentOf(this, wxHeight, wxPos.y);
    posCons->width.PercentOf(this, wxWidth, wxPos.width);
    posCons->height.PercentOf(this, wxHeight, wxPos.height);
    newContent->SetConstraints(posCons);
}

// replaces existing element (either a container or content) at index with the given content ID.
// For now only replacing with a single content ID is supported (not with infinite containers)
void cContainer::ReplaceElementAtIndexWithContent(uint8_t index, uint16_t contentID) {
    if (index < children.size()) {
        std::shared_ptr<wxWindow> windowObj = children[index];
        elementPosition* pos = layout->positions[index];

        std::vector<Chunk> subelements = container->elementIDs[index];
        bool isContainer = subelements.size() > 1 || (subelements.size() > 0 && subelements[0].chunkID == CHUNK_CONTAINER);
        if (isContainer) {
            // destroy container
            cContainer* c = dynamic_cast<cContainer*>(windowObj.get());
            WindowManager::DestroyContainer(c);
        }
        else {
            // destroy content
            windowObj->Show(false);
            //windowObj->Destroy(); // TODO
            // shared_ptr doesnt need delete
        }

        Content* newContentData = FileManager::getContentByID(contentID);
        container->elementIDs[index] = std::vector<Chunk>{ { CHUNK_CONTENT, contentID } };
        CreateContent(newContentData, pos, index);
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
