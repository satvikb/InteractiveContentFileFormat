#pragma once
#include <wx/wx.h>
#include "cContainer.h"
#include "cNativeContent.h"
#include <wx/mstream.h>
// content is async, this entire class can exist without loading in content data
class cImageView : public cNativeContent, public wxPanel
{
public:
	cImageView(cContainer* parent);
	void applyComponentStyle(struct Style* style);
	~cImageView();
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);
	DECLARE_EVENT_TABLE()

private:
	wxImage image;
	wxBitmap resized;
	int w, h;
	void interpretContent() override;
};
