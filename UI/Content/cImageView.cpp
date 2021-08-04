#include "cImageView.h"

cImageView::cImageView(cContainer* parent, struct Content* content, struct Style* style) : cStyledPanel((wxWindow*)parent, style, wxID_ANY) {
    SetContent(content);

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

void cImageView::ApplyContentStyle(struct Style* style) {

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

void cImageView::RenderContent(wxDC& dc)
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