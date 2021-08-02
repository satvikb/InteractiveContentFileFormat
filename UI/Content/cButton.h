#pragma once
#include <wx/wx.h>
#include "cContainer.h"
#include "cNativeContent.h"
#include <wx/mstream.h>

class cButton : public cNativeContent, public wxPanel
{
public:
	cButton(cContainer* parent);
	void ApplyComponentStyle(struct Style* style) override;
	~cButton();
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);
	DECLARE_EVENT_TABLE()

private:
	int w, h;
	struct Action* action;
	struct Style* defaultStyle;
	struct Style* hoverStyle;
	struct Style* clickStyle;

	void interpretContent() override;
	void drawContent(wxDC& dc);

};
