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

void cBitmap::render(wxDC& dc)
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
void cBitmap::OnSize(wxSizeEvent& event) {
    Refresh();
    //skip the event.
    event.Skip();
}


cBitmap::~cBitmap() {

}