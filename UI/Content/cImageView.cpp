#include "cImageView.h"

cImageView::cImageView(cContainer* parent) : wxPanel((wxWindow*) parent, wxID_ANY) {
	//wxPanel::Create((wxWindow*)parent, wxID_ANY);

    // load the file... ideally add a check to see if loading was successful
    w = -1;
    h = -1;
}

void cImageView::interpretContent() {
	if (content->data.size() > 0) {
        // read in the header
        //int i = 0;
        //// check start metadata byte
        //if (content->data[i] == 0x1) {
        //    i += 1;
        //    uint16_t width = (content->data[i] << 8) | content->data[i+1];
        //    i += 2;
        //    uint16_t height = (content->data[i] << 8) | content->data[i + 1];
        //    i += 2;
        //    uint8_t alphaFlag = content->data[i];
        //    i += 1;
        //    // should be at end metadata byte
        //    if (content->data[i] == 0x2) {
        //        i += 1;
        //        // start action table byte
        //        if (content->data[i] == 0x3) {
        //            // TODO loop through all actions
        //            i += 1; // todo remove this

        //            // end action table
        //            if (content->data[i] == 0x4) {
        //                i += 1;
        //                // now at the start of pixel data
        //                std::vector<uint8_t> pixelData(content->data.begin() + i, content->data.end());
        //                int numPixelBytes = content->length - i;//width * height * (alphaFlag == 0x1 ? 4 : 3); // simply would be wrong with alphas

        //                std::cout << "Read file" << std::endl;
        //            }
        //        }
        //    }

        //}
        //image.LoadFile(file, format);

        wxMemoryInputStream stream(content->data.data(), content->length);
        if (!image.LoadFile(stream, "application/icimage"))
            return;
	}
}
//void cNativeContent::addAction() {}

BEGIN_EVENT_TABLE(cImageView, wxPanel)
// some useful events
/*
 EVT_MOTION(wxImagePanel::mouseMoved)
 EVT_LEFT_DOWN(wxImagePanel::mouseDown)
 EVT_LEFT_UP(wxImagePanel::mouseReleased)
 EVT_RIGHT_DOWN(wxImagePanel::rightClick)
 EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
 EVT_KEY_DOWN(wxImagePanel::keyPressed)
 EVT_KEY_UP(wxImagePanel::keyReleased)
 EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
 */

 // catch paint events
    EVT_PAINT(cImageView::paintEvent)
    //Size event
    EVT_SIZE(cImageView::OnSize)
    END_EVENT_TABLE()


    // some useful events
    /*
     void wxImagePanel::mouseMoved(wxMouseEvent& event) {}
     void wxImagePanel::mouseDown(wxMouseEvent& event) {}
     void wxImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
     void wxImagePanel::mouseReleased(wxMouseEvent& event) {}
     void wxImagePanel::rightClick(wxMouseEvent& event) {}
     void wxImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
     void wxImagePanel::keyPressed(wxKeyEvent& event) {}
     void wxImagePanel::keyReleased(wxKeyEvent& event) {}
     */

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

}