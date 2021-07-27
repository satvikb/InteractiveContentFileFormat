#pragma once
#include <wx/wx.h>
#include "cContainer.h"
#include "cNativeContent.h"
#include <wx/mstream.h>
// content is async, this entire class can exist without loading in content data
class cBitmap : public cNativeContent, public wxPanel
{
public:
	cBitmap(cContainer* parent);
	void applyComponentStyle(struct Style* style);
	~cBitmap();
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);
	DECLARE_EVENT_TABLE()

private:
	int w, h;
	void interpretContent() override;
};
