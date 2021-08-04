#include "cButton.h"

cButton::cButton(cContainer* parent, struct Content* content, struct Style* style) : cStyledPanel((wxWindow*)parent, style, wxID_ANY) {
	//wxPanel::Create((wxWindow*)parent, wxID_ANY);

    // load the file... ideally add a check to see if loading was successful
    w = -1;
    h = -1;
}

void cButton::interpretContent() {
	if (content->data.size() > 0) {
        std::vector<uint8_t> bytes = content->data;

        int i = 0;
        std::pair<uint8_t, uint32_t> actionID = readChunkTypeAndID((char*)&bytes[0], &i);
        struct Action *_action = FileManager::getActionByID(actionID.second);
        if (_action != nullptr) {
            this->action = _action;
        }

        // start with the key value pairs
        while (bytes[i] != 0x0) {
            uint8_t key = (unsigned char)bytes[i];
            switch (key) {
                case BUTTON_DEFAULT_STYLE: case BUTTON_HOVER_STYLE: case BUTTON_CLICK_STYLE:
                {
                    std::pair<uint8_t, uint32_t> styleID = readChunkTypeAndID((char*)&bytes[0], &i);
                    struct Style* _style = FileManager::getStyleByID(styleID.second);
                    if (_style != nullptr) {
                        if (key == BUTTON_DEFAULT_STYLE) {
                            this->defaultStyle = _style;
                        } else if (key == BUTTON_HOVER_STYLE) {
                            this->hoverStyle = _style;
                        } else if (key == BUTTON_CLICK_STYLE) {
                            this->clickStyle = _style;
                        }
                    }
                }
                break;
                default:
                // unknown key, skip the rest
                goto finish_interpret;
                break;
            }
        }
    finish_interpret:;
	}
}

void cButton::ApplyContentStyle(struct Style* style) {

}

BEGIN_EVENT_TABLE(cButton, wxPanel)

// mouse events
EVT_LEFT_DOWN(cButton::mouseDown)
EVT_LEFT_UP(cButton::mouseReleased)
EVT_ENTER_WINDOW(cButton::windowEnter)
EVT_LEAVE_WINDOW(cButton::windowLeave)

END_EVENT_TABLE()


void cButton::RenderContent(wxDC& dc) {
    std::vector<uint8_t> bytes = content->data;
    int numBytes = bytes.size();
    if (numBytes > 0) {
        int w, h;
        dc.GetSize(&w, &h);

        int i = 0;
    }
}

/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void cButton::OnSize(wxSizeEvent& event) {
    Refresh();
    //skip the event.
    event.Skip();
}

void cButton::mouseDown(wxMouseEvent& event) {
    ApplyContentStyle(this->clickStyle);
}

void cButton::mouseReleased(wxMouseEvent& event) {
    ApplyContentStyle(this->defaultStyle);
}

void cButton::windowEnter(wxMouseEvent& event) {
    ApplyContentStyle(this->hoverStyle);
}

void cButton::windowLeave(wxMouseEvent& event) {
    ApplyContentStyle(this->defaultStyle);
}

cButton::~cButton() {

}