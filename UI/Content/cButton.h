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

	DECLARE_EVENT_TABLE()

private:
	int w, h;
	struct Action* action;
	struct Style* defaultStyle;
	struct Style* hoverStyle;
	struct Style* clickStyle;

	void interpretContent() override;
	void OnSize(wxSizeEvent& event) override;
	void mouseDown(wxMouseEvent& evt);
	void mouseReleased(wxMouseEvent& evt);
	void windowEnter(wxMouseEvent& evt);
	void windowLeave(wxMouseEvent& evt);

};
