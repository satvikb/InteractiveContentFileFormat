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
            wxWindow* win = children[i];
            children[i]->Show(false);
            // TODO crashes because of using smart pointers, maybe try wxWindowPtr?
            children[i]->Destroy();
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

    wxWindow* newContent = nullptr;

    switch (contentType) {
    case CONTENT_TEXT: {
        //std::shared_ptr<cRichTextView> richTextCtrl = std::make_shared<cRichTextView>((cContainer*)this);
        newContent = new cRichTextView((cContainer*)this);
        static_cast<cRichTextView*>(newContent)->SetContent(content);
        static_cast<cRichTextView*>(newContent)->applyComponentStyle(FileManager::getStyleByID(pos->styleID));
    }
    break;
    case CONTENT_IMAGE:
        newContent = new cImageView((cContainer*)this);
        static_cast<cImageView*>(newContent)->SetContent(content);
        //static_cast<cImageView*>(newContent)->applyComponentStyle(FileManager::getStyleByID(pos->styleID));
    break;
    case CONTENT_WEB:
        newContent = new cWebView((cContainer*)this);
        static_cast<cWebView*>(newContent)->SetContent(content);
        break;
    case CONTENT_STREAMED:
        CreateContent(FileManager::getStreamedContent(content), pos, index);
        return;
        break;
    }

    if (newContent != nullptr) {
        // content was actually created (valid content type)

        if (index >= 0 && index < children.size()) {
            //delete children[index];
            children[index] = newContent;
        }
        else {
            children.push_back(newContent);
        }

        wxConstraintPosition wxPos = FileManager::convertLayoutPositionToConstraint(pos);
        cContainer::applyParentConstraints(newContent, this, wxPos);
        Layout();
    }
}

// replaces existing element (either a container or content) at index with the given content ID.
// For now only replacing with a single content ID is supported (not with infinite containers)
// The 0 is because infinite containers are not supported when replacing, so only first is selected.
void cContainer::ReplaceElementAtIndexWithContent(uint8_t index, uint16_t contentID) {
    if (index < children.size()) {
        wxWindow* windowObj = children[index];
        elementPosition* pos = layout->positions[index];

        std::vector<Chunk> subelements = container->elementIDs[index];
        bool isContainer = subelements.size() > 1 || (subelements.size() > 0 && subelements[0].chunkID == CHUNK_CONTAINER);
        if (isContainer) {
            // destroy container
            cContainer* c = dynamic_cast<cContainer*>(windowObj);
            WindowManager::DestroyContainer(c);
        }
        else {
            // destroy content
            windowObj->Show(false);
            windowObj->Destroy(); // TODO
            // shared_ptr doesnt need delete
        }

        Content* newContentData = FileManager::getContentByID(contentID);
        container->elementIDs[index] = std::vector<Chunk>{ { CHUNK_CONTENT, contentID } };

        //if (container->elementIDs[index].size() > 0) {
        //    container->elementIDs[index][0]->chunkType = CHUNK_CONTENT;
        //    container->elementIDs[index][0]->chunkID = contentID;
        //}
        //else {
        //    // TODO is this ever reached?
        //    // if it is reached set to a new vector with 1 element, the new chunk (pointer)
        //    //container->elementIDs[index] = std::vector<Chunk*>{ { CHUNK_CONTENT, contentID } };
        //}
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

void cContainer::applyParentConstraints(wxWindow* applyTo, wxWindow* parent, struct wxConstraintPosition constraints) {
    wxLayoutConstraints* posCons = new wxLayoutConstraints();
    posCons->left.PercentOf(parent, wxWidth, constraints.x);
    posCons->top.PercentOf(parent, wxHeight, constraints.y);
    posCons->width.PercentOf(parent, wxWidth, constraints.width);
    posCons->height.PercentOf(parent, wxHeight, constraints.height);
    applyTo->SetConstraints(posCons);
}
