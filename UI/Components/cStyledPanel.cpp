#include "cStyledPanel.h"

cStyledPanel::cStyledPanel() {

}

BEGIN_EVENT_TABLE(cStyledPanel, wxPanel)

// catch paint events
EVT_PAINT(cStyledPanel::paintEvent)
//Size event
EVT_SIZE(cStyledPanel::OnSize)
// mouse events
EVT_LEFT_UP(cStyledPanel::mouseReleased)
// window events
EVT_ENTER_WINDOW(cStyledPanel::windowEnter)
EVT_LEAVE_WINDOW(cStyledPanel::windowLeave)

END_EVENT_TABLE()

cStyledPanel::cStyledPanel(wxWindow* aParent, struct Style* componentStyle, wxWindowID winID, const wxPoint& pos, const wxSize& size, long style, const wxString& name){
    Create(aParent, componentStyle, winID, pos, size, style, name);
}

bool cStyledPanel::Create(wxWindow* parent, struct Style* componentStyle, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) {
    bool success = wxPanel::Create(parent, id, pos, size, style, name);
	contextWidth = -1;
	contextWidth = -1;
	ApplyComponentStyle(componentStyle);
    return success;
}

cStyledPanel::~cStyledPanel() {

}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void cStyledPanel::paintEvent(wxPaintEvent& evt) {
    // depending on your system you may need to look at double-buffered dcs
	wxBufferedPaintDC dc(this);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	gc = wxGraphicsContext::Create(dc);
    RenderComponent(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void cStyledPanel::paintNow() {
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
	gc = wxGraphicsContext::Create(dc);
    RenderComponent(dc);
}

void cStyledPanel::ApplyComponentStyle(struct Style* style) {
    this->componentStyle = style;
	paintNow();
}

void cStyledPanel::RenderBackgroundStyle(wxDC& dc) {
	// save brush and pen state
	wxColour currentBrushColor = brush.GetColour();
	wxColour currentPenColor = pen.GetColour();

	if (this->componentStyle != nullptr && gc) {
		// c++17 style loop
		for (auto [key, val] : this->componentStyle->styles) {
			switch (key) {
				case STYLE_COMPONENT_BACKGROUND_COLOR:
				{
					brush.SetColour(std::any_cast<wxColour>(val));
				} 
				break;
				case STYLE_COMPONENT_BORDER_COLOR:
				{
					pen.SetColour(std::any_cast<wxColour>(val));
				}
				break;
				case STYLE_COMPONENT_BORDER_WIDTH:
				{
					// TODO percent or pixel
				}
				break;
			}
		}
	}

	gc->SetBrush(brush);
	gc->SetPen(pen);

	// TODO conditionally draw only if needed
	dc.DrawRectangle(0, 0, contextWidth, contextHeight);

	// revert brush and pen state
	brush.SetColour(currentBrushColor);
	pen.SetColour(currentPenColor);
}

// this is where the "layers" are applied.
// first backgrounds, then the content, then overlays like borders.
void cStyledPanel::RenderComponent(wxDC& dc) {
	dc.wxDC::GetSize(&contextWidth, &contextHeight);
	wxSize cs = GetClientSize();
	contextWidth = cs.GetWidth();
	contextHeight = cs.GetHeight();
	Logger::logToFile("2: "+std::to_string(cs.GetWidth()) + " " + std::to_string(cs.GetHeight()));

	brush = wxBrush(wxTransparentColour);
	pen = wxPen(wxTransparentColour);

	RenderBackgroundStyle(dc);
	RenderContent(dc);
}