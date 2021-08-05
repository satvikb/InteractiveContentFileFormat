#pragma once
#include <wx/wx.h>
#include "cContainer.h"
#include "cStyledPanel.h"
#include "cNativeContent.h"
// content is async, this entire class can exist without loading in content data
class cBitmap : public cStyledPanel, public cNativeContent
{
public:
	cBitmap(cContainer* parent, struct Content* content, struct Style* style);
	void ApplyContentStyle(struct Style* style) override;
	~cBitmap();

private:
	int w, h;

	void RenderContent(wxDC& dc) override;
	void OnSize(wxSizeEvent& event) override;
	void interpretContent() override;

	void readAndDrawLine(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index);
	void readAndDrawRectangle(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index);
	void readAndDrawCircle(wxDC& dc, int w, int h, std::vector<uint8_t> bytes, int* index);
};
