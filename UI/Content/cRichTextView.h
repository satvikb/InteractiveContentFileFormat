#pragma once
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>


class cContainer;

class cRichTextView : public wxRichTextCtrl
{
public:
	cRichTextView(cContainer* parent);

	~cRichTextView();

private:
	void clickHandler(wxRichTextEvent& event);
	wxDECLARE_EVENT_TABLE();
};

