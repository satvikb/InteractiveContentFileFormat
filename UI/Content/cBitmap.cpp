#include "cBitmap.h"

cBitmap::cBitmap(cContainer* parent) : wxPanel((wxWindow*) parent, wxID_ANY) {
	//wxPanel::Create((wxWindow*)parent, wxID_ANY);

    // load the file... ideally add a check to see if loading was successful
    w = -1;
    h = -1;
}

void cBitmap::interpretContent() {
	if (content->data.size() > 0) {

        
	}
}

BEGIN_EVENT_TABLE(cBitmap, wxPanel)

 // catch paint events
EVT_PAINT(cBitmap::paintEvent)
//Size event
EVT_SIZE(cBitmap::OnSize)
END_EVENT_TABLE()

void cBitmap::paintEvent(wxPaintEvent& evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

void cBitmap::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

void cBitmap::drawContent(wxDC& dc) {
    std::vector<uint8_t> bytes = content->data;
    int numBytes = bytes.size();
    if (numBytes > 0) {
        int w, h;
        dc.GetSize(&w, &h);

        int i = 0;

        while (i < numBytes && bytes[i] != 0x0) {
            switch (bytes[i]) {
                case BITMAP_LINE:
                    readAndDrawLine(dc, w, h, bytes, &i);
                break;
                case BITMAP_RECTANGLE:
                    readAndDrawRectangle(dc, w, h, bytes, &i);
                break;
            }

        }
    }
}

void cBitmap::readAndDrawLine(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int *index) {
    int i = *index;
    // Skip the type byte
    i++;
    uint8_t startX = bytes[i];
    i++;
    uint8_t startY = bytes[i];
    i++;
    uint8_t endX = bytes[i];
    i++;
    uint8_t endY = bytes[i];
    i++;
    std::pair<uint8_t, uint32_t> styleChunk = readChunkTypeAndID((char*)&bytes[0], &i);
    struct Style* style = FileManager::getStyleByID(styleChunk.second);
    // apply style
    if (style != nullptr) {

    }

    float startXPercent = (float)startX / 100.f;
    float startYPercent = (float)startY / 100.f;
    float endXPercent = (float)endX / 100.f;
    float endYPercent = (float)endY / 100.f;

    dc.DrawLine(startXPercent*w, startYPercent*h, endXPercent*w, endYPercent*h);
    *index = i;
}

void cBitmap::readAndDrawRectangle(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index) {
    int i = *index;
    // Skip the type byte
    i++;
    uint8_t startX = bytes[i];
    i++;
    uint8_t startY = bytes[i];
    i++;
    uint8_t endX = bytes[i];
    i++;
    uint8_t endY = bytes[i];
    i++;
    std::pair<uint8_t, uint32_t> styleChunk = readChunkTypeAndID((char*)&bytes[0], &i);
    struct Style* style = FileManager::getStyleByID(styleChunk.second);
    // apply style
    if (style != nullptr) {

    }

    float startXPercent = (float)startX / 100.f;
    float startYPercent = (float)startY / 100.f;
    float endXPercent = (float)endX / 100.f;
    float endYPercent = (float)endY / 100.f;

    float topLeftX = startXPercent * w;
    float topLeftY = startYPercent * h;
    float bottomRightX = endXPercent * w;
    float bottomRightY = endYPercent * h;

    dc.DrawRectangle(topLeftX, topLeftY, (bottomRightX - topLeftX), (bottomRightY - topLeftY));
    *index = i;
}


void cBitmap::render(wxDC& dc)
{
    drawContent(dc);
}

/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void cBitmap::OnSize(wxSizeEvent& event) {
    Refresh();
    //skip the event.
    event.Skip();
}


cBitmap::~cBitmap() {

}