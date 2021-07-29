#include "cImageView.h"

cImageView::cImageView(cContainer* parent) : wxPanel((wxWindow*) parent, wxID_ANY) {
	//wxPanel::Create((wxWindow*)parent, wxID_ANY);

    // load the file... ideally add a check to see if loading was successful
    w = -1;
    h = -1;
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
                    imageType = bytes[i];
                    i += 1;
                break;
                case IMAGE_DATA_LENGTH:
                    dataLength = read32BitInt((char*)&bytes[0], &i);
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
                    useURL = true;
                    i += 1;
                break;
                case IMAGE_BEGIN_DATA:
                    if (dataLength > 0) {
                        imageData = {bytes.begin() + i, bytes.begin() + i + dataLength };
                        i += dataLength;
                    } else {
                        // ? dataLength not set yet.
                        // TODO error
                    }
                break;
            }

        }

        // switch between image type and load the right type
        wxMemoryInputStream stream(imageData.data(), dataLength);
        if (!image.LoadFile(stream, "application/icimage"))
            return;
        
	}
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

        if (neww != w || newh != h)
        {
            resized = wxBitmap(image.Scale(neww, newh /*, wxIMAGE_QUALITY_HIGH*/));
            w = neww;
            h = newh;
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