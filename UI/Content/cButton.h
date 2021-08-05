#pragma once
#include <wx/wx.h>
#include "cContainer.h"
#include "cNativeContent.h"
#include <wx/mstream.h>

class cButton : public cStyledPanel, public cNativeContent
{
public:
	cButton(cContainer* parent, struct Content* content, struct Style* style);
	void ApplyContentStyle(struct Style* style) override;
	void RenderContent(wxDC& dc) override;
	~cButton();

private:
	int w, h;
	uint32_t actionID;
	struct Style* defaultStyle;
	struct Style* hoverStyle;
	struct Style* clickStyle;

	void interpretContent() override;
	void OnSize(wxSizeEvent& event) override;
	void mouseDown(wxMouseEvent& event) override;
	void mouseReleased(wxMouseEvent& event) override;
	void windowEnter(wxMouseEvent& event) override;
	void windowLeave(wxMouseEvent& event) override;

};
