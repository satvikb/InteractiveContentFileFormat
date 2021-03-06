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
            
            win->Show(false);
            
            cContainer* existingContainer = dynamic_cast<cContainer*>(win);
            if (existingContainer != NULL) {
                WindowManager::DestroyContainer(existingContainer);
            } else {
                // TODO crashes because of using smart pointers, maybe try wxWindowPtr?
                win->Destroy();
            }
        }

        // TODO MEMORY MANAGEMENT, DELETE THE WINDOWS
        children.clear();
    }

    for (int i = 0; i < container->elementIDs.size(); i++) {
        elementPosition* pos = layout->positions[i];

        Chunk ID = container->elementIDs[i];

        struct Container* container = FileManager::getContainerByID(ID.chunkID);
        if (ID.chunkType == CHUNK_CONTAINER && container != nullptr) {
            // create another cContainer
            CreateSubContainer(ID.chunkID, pos);
        } else {
            // try content
            struct Content* content = FileManager::getContentByID(ID.chunkID);
            if (content != nullptr) {
                CreateContent(content, pos);
            } else {
                // ?
            }
        }
        
    }
}

void cContainer::CreateContent(struct Content* content, struct elementPosition* pos, uint8_t index) {
    uint8_t contentType = content->type;
    struct Style* style = FileManager::getStyleByID(pos->styleID);
    wxWindow* newContent = nullptr;

    switch (contentType) {
    case CONTENT_TEXT:
        // TODO does it matter which direct base class it converts to...? cStyledPanel or wxRichTextCtrl
        newContent = (wxRichTextCtrl*)(new cRichTextView((cContainer*)this, content, style));
    break;
    case CONTENT_IMAGE:
        newContent = new cImageView((cContainer*)this, content, style);
    break;
    case CONTENT_BITMAP:
        newContent = new cBitmap((cContainer*)this, content, style);
    break;
    case CONTENT_BUTTON:
        newContent = new cButton((cContainer*)this, content, style);
    break;
    case CONTENT_WEB:
        newContent = new cWebView((cContainer*)this, content, style);
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

void cContainer::CreateSubContainer(uint32_t containerID, struct elementPosition* pos, uint8_t index) {
    cContainer* newContainer = WindowManager::CreateContainer(containerID, this, FileManager::convertLayoutPositionToConstraint(pos));
    newContainer->SetContainerStyle(FileManager::getStyleByID(pos->styleID));
    if (index >= 0 && index < children.size()) {
        // delete the old container that is what is there currently
        cContainer* existingContainer = dynamic_cast<cContainer*>(children[index]);
        if (existingContainer != NULL) {
            WindowManager::DestroyContainer(existingContainer);
        }
        children[index] = newContainer;
    } else {
        children.push_back(newContainer);
    }
}

// replaces existing element (either a container or content) at index with the given content ID.
// For now only replacing with a single content ID is supported (not with infinite containers)
// The 0 is because infinite containers are not supported when replacing, so only first is selected.
void cContainer::ReplaceElementAtIndexWithContent(uint8_t index, uint16_t elementID) {
    if (index < children.size()) {
        wxWindow* windowObj = children[index];
        elementPosition* pos = layout->positions[index];

        Chunk chunk = container->elementIDs[index];
        bool isContainer = chunk.chunkID == CHUNK_CONTAINER;
        if (isContainer) {
            // destroy container
            cContainer* c = dynamic_cast<cContainer*>(windowObj);
            WindowManager::DestroyContainer(c);
        } else {
            // destroy content
            windowObj->Show(false);
            windowObj->Destroy(); // TODO
            // shared_ptr doesnt need delete
        }

        
        Content* newContentData = FileManager::getContentByID(elementID);

        if (newContentData != nullptr) {
            container->elementIDs[index] = {CHUNK_CONTENT, elementID};

            CreateContent(newContentData, pos, index);
        } else {
            // try to make container
            Container* newContainerData = FileManager::getContainerByID(elementID);
            if (newContainerData != nullptr) {
                container->elementIDs[index] = {CHUNK_CONTENT, elementID};
                CreateSubContainer(newContainerData->chunkID, pos, index);
            } else {
                // element ID not found
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

void cContainer::applyParentConstraints(wxWindow* applyTo, wxWindow* parent, struct wxConstraintPosition constraints) {
    wxLayoutConstraints* posCons = new wxLayoutConstraints();
    posCons->left.PercentOf(parent, wxWidth, constraints.x);
    posCons->top.PercentOf(parent, wxHeight, constraints.y);
    posCons->width.PercentOf(parent, wxWidth, constraints.width);
    posCons->height.PercentOf(parent, wxHeight, constraints.height);
    applyTo->SetConstraints(posCons);
}
