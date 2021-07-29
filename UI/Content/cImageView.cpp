#include "cImageView.h"

cImageView::cImageView(cContainer* parent) : wxPanel((wxWindow*) parent, wxID_ANY) {
	//wxPanel::Create((wxWindow*)parent, wxID_ANY);

    // load the file... ideally add a check to see if loading was successful
    imageWidth = -1;
    imageHeight = -1;
}

void cImageView::interpretContent() {
    std::vector<uint8_t> bytes = content->data;
    int numBytes = bytes.size();
    if (numBytes > 0) {
        int i = 0;

        while (i < numBytes) {
            uint8_t controlByte = bytes[i];
            i += 1;

            switch (controlByte) {
                case IMAGE_TYPE:
                {
                    imageType = bytes[i];
                    i += 1;
                }
                    
                break;
                case IMAGE_DATA_LENGTH:
                {
                    dataLength = read32BitInt((char*)&bytes[0], &i);

                }
                break;
                case IMAGE_ACTION_START:
                {
                    uint8_t numberActionRectangles = bytes[i];
                    i += 1;
                    for (uint8_t n = 0; n < numberActionRectangles; n++) {
                        ImageActionPosition* actionPos = readImageActionPosition((char*)&bytes[0], &i);
                        actions.push_back(actionPos);
                    }
                }
                break;
                case IMAGE_USE_URL:
                {
                    useURL = true;
                }
                    
                break;
                case IMAGE_BEGIN_DATA:
                {
                    if (dataLength > 0) {
                        imageData = {bytes.begin() + i, bytes.begin() + i + dataLength};
                        i += dataLength;
                    } else {
                        // ? dataLength not set yet.
                        // TODO error
                    }
                }
                    
                break;
            }
        }

        if (useURL) {
            std::string streamURL(imageData.begin(), imageData.end());
            cpr::Response res = downloadFileData(streamURL.c_str());
            std::string data = res.text;
            imageData.assign(data.begin(), data.end());
            dataLength = imageData.size();
        }

        // switch between image type and load the right type
        wxMemoryInputStream stream(imageData.data(), dataLength);
        std::string mimeType = mimeTypeFromImageType(imageType);
        if (!mimeType.empty()) {
            if (!image.LoadFile(stream, mimeType))
                return;
        }
	}
}

std::string cImageView::mimeTypeFromImageType(uint8_t imageType) {
    switch (imageType) {
        case IMAGE_TYPE_IC_IMAGE:
        return "application/icimage";
        case IMAGE_TYPE_PNG:
        return "image/png";
        case IMAGE_TYPE_JPEG:
        return "image/jpeg";
        case IMAGE_TYPE_GIF:
        return "image/gif";
    }
    return "";
}
//void cNativeContent::addAction() {}

BEGIN_EVENT_TABLE(cImageView, wxPanel)

EVT_LEFT_UP(cImageView::mouseReleased)

// catch paint events
EVT_PAINT(cImageView::paintEvent)
//Size event
EVT_SIZE(cImageView::OnSize)
END_EVENT_TABLE()

void cImageView::mouseReleased(wxMouseEvent& event) {
    // handle actions
    for (uint8_t n = 0; n < actions.size(); n++) {
        ImageActionPosition* actionPos = actions[n];
        if (pointInActionPos(actionPos, event.GetX(), event.GetY())) {
            WindowManager::ExecuteActionID(actionPos->actionID);
            return;
        }
    }
}

bool cImageView::pointInActionPos(ImageActionPosition* pos, int mouseX, int mouseY) {
    float normX = (float)pos->x / 100.f;
    float normY = (float)pos->y / 100.f;
    float normWidth = (float)pos->w / 100.f;
    float normHeight = (float)pos->h / 100.f;

    int winX = normX * imageWidth;
    int winY = normY * imageHeight;
    int winWeight = normWidth * imageWidth;
    int winHeight = normHeight * imageHeight;

    return (mouseX >= winX && mouseX <= (winX + winWeight)) && (mouseY >= winY && mouseY <= (winY + winHeight));
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void cImageView::paintEvent(wxPaintEvent& evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void cImageView::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void cImageView::render(wxDC& dc)
{
    if (image.IsOk()) {
        int neww, newh;
        dc.GetSize(&neww, &newh);

        if (neww != imageWidth || newh != imageHeight)
        {
            resized = wxBitmap(image.Scale(neww, newh /*, wxIMAGE_QUALITY_HIGH*/));
            imageWidth = neww;
            imageHeight = newh;
            dc.DrawBitmap(resized, 0, 0, false);
        }
        else {
            dc.DrawBitmap(resized, 0, 0, false);
        }
    }
}

/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void cImageView::OnSize(wxSizeEvent& event) {
    Refresh();
    //skip the event.
    event.Skip();
}


cImageView::~cImageView() {
    // TODO delete actionPos
}