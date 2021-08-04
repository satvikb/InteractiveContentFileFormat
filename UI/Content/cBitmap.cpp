#include "cBitmap.h"

cBitmap::cBitmap(cContainer* parent, struct Content* content, struct Style* style) : cStyledPanel((wxWindow*) parent, style, wxID_ANY) {
    SetContent(content);
	//wxPanel::Create((wxWindow*)parent, wxID_ANY);
    w = -1;
    h = -1;
}

void cBitmap::interpretContent() {
	if (content->data.size() > 0) {

        
	}
}

void cBitmap::ApplyContentStyle(struct Style* style) {

}


void cBitmap::RenderContent(wxDC& dc) {
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
                case BITMAP_CIRCLE:
                    readAndDrawCircle(dc, w, h, bytes, &i);
                break;
                default:
                goto exit_drawing;
                break;
            }

        }
    }
    // if encountered an unknown shape type
    exit_drawing:;
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
        // TODO
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

void cBitmap::readAndDrawCircle(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index) {
    int i = *index;
    // Skip the type byte
    i++;
    uint8_t centerX = bytes[i];
    i++;
    uint8_t centerY = bytes[i];
    i++;
    uint8_t radius = bytes[i];
    i++;
    uint8_t radiusMode = bytes[i];
    i++;
    std::pair<uint8_t, uint32_t> styleChunk = readChunkTypeAndID((char*)&bytes[0], &i);
    struct Style* style = FileManager::getStyleByID(styleChunk.second);
    // apply style
    if (style != nullptr) {
        // TODO
    }

    float centerXPercent = (float)centerX / 100.f;
    float centerYPercent = (float)centerY / 100.f;
    float radiusPercent = (float)radius / 100.f;

    float centerXPixels = centerXPercent * w;
    float centerYPixels = centerYPercent * h;
    float radiusPixels = 0;
    switch (radiusMode) {
        case BITMAP_CIRCLE_RADIUS_WIDTH:
        radiusPixels = radiusPercent * w;
        break;
        case BITMAP_CIRCLE_RADIUS_HEIGHT:
        radiusPixels = radiusPercent * h;
        break;
    }

    dc.DrawCircle(centerXPixels, centerYPixels, radiusPixels);
    *index = i;
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