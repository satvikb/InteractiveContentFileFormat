#pragma once
#include <wx/wx.h>
#include "reader.h"

/*
	
	All Content types and Container will inherit from cStyledPanel.
	cStyledPanel will handle the entire styling of the Element.
	This includes border color, border width, background colors, and any other styles that apply to the entire Panel.

	Order of application/layer system:
	Lowest - Background color
	Middle - Actual content
	Highest - Borders

*/
class cStyledPanel : public wxPanel {
	public:
	cStyledPanel();
	cStyledPanel(wxWindow* aParent, struct Style* componentStyle, wxWindowID winID = wxID_ANY,
								   const wxPoint& pos = wxDefaultPosition,
								   const wxSize& size = wxDefaultSize,
								   long style = wxTAB_TRAVERSAL,
								   const wxString& name = wxPanelNameStr);
	bool Create(wxWindow* parent, struct Style* componentStyle, wxWindowID id = wxID_ANY,
								  const wxPoint& pos = wxDefaultPosition,
								  const wxSize& size = wxDefaultSize,
								  long style = wxTAB_TRAVERSAL,
								  const wxString& name = wxPanelNameStr);
	~cStyledPanel();

	void ApplyComponentStyle(struct Style* style);
	void RenderComponent(wxDC& dc);

	private:
	// event listeners
	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	// virtual event listeners
	virtual void OnSize(wxSizeEvent& event) {};

	// Content specific render
	virtual void RenderContent(wxDC& dc) {};
	// Content specific style
	virtual void ApplyContentStyle(struct Style* style) { this->componentStyle = style; };

	DECLARE_EVENT_TABLE()

	protected:
	struct Style* componentStyle;

};