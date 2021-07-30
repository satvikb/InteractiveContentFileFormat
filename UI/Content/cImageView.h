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
	void ApplyComponentStyle(struct Style* style) override;
	~cImageView();
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);
	void mouseReleased(wxMouseEvent& event);
	DECLARE_EVENT_TABLE()

private:

	uint8_t imageType;
	int dataLength;
	std::vector<struct ImageActionPosition*> actions;
	bool useURL;
	std::vector<uint8_t> imageData;

	wxImage image;
	wxBitmap resized;
	int imageWidth, imageHeight;
	void interpretContent() override;
	std::string mimeTypeFromImageType(uint8_t imageType);
	bool pointInActionPos(ImageActionPosition* pos, int mouseX, int mouseY);
};
