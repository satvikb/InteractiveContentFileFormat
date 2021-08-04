#pragma once
#include <wx/wx.h>
#include "cContainer.h"
#include "cNativeContent.h"
#include <wx/mstream.h>
// content is async, this entire class can exist without loading in content data
class cImageView : public cStyledPanel, public cNativeContent
{
public:
	cImageView(cContainer* parent, struct Content* content, struct Style* style);
	void ApplyContentStyle(struct Style* style) override;
	~cImageView();

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

	void OnSize(wxSizeEvent& event) override;
	void RenderContent(wxDC& dc) override;
};
