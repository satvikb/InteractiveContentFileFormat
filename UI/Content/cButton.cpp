#include "cButton.h"

cButton::cButton(cContainer* parent) : wxPanel((wxWindow*) parent, wxID_ANY) {
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

// how should this work in addition to the state styles of the button?
void cButton::ApplyComponentStyle(struct Style* style) {

}


BEGIN_EVENT_TABLE(cButton, wxPanel)

 // catch paint events
EVT_PAINT(cButton::paintEvent)
//Size event
EVT_SIZE(cButton::OnSize)
END_EVENT_TABLE()

void cButton::paintEvent(wxPaintEvent& evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

void cButton::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

void cButton::drawContent(wxDC& dc) {
    std::vector<uint8_t> bytes = content->data;
    int numBytes = bytes.size();
    if (numBytes > 0) {
        int w, h;
        dc.GetSize(&w, &h);

        int i = 0;
    }
}

void cButton::render(wxDC& dc)
{
    drawContent(dc);
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


cButton::~cButton() {

}