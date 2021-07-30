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
	void ApplyComponentStyle(struct Style* style) override;
	~cBitmap();
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);
	DECLARE_EVENT_TABLE()

private:
	int w, h;
	void interpretContent() override;
	void drawContent(wxDC& dc);

	void readAndDrawLine(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index);
	void readAndDrawRectangle(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index);
	void readAndDrawCircle(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index);
};
