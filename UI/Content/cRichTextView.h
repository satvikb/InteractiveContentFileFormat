#pragma once
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <map>
#include <string>
#include "cNativeContent.h"

class cContainer;

struct Font {
	std::string fontName;
	std::string fontFamily;
};

class cRichTextView : public cNativeContent, public wxRichTextCtrl
{
public:
	cRichTextView(cContainer* parent);

	~cRichTextView();

private:
	// map to convert start position of URLs in text to a Action ID
	std::map<uint16_t, uint16_t> actions;
	void clickHandler(wxRichTextEvent& event);
	//void clickHandler(wxTextUrlEvent& event);

	void interpretContent() override;

	wxDECLARE_EVENT_TABLE();
};

