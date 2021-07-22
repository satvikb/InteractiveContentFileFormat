#pragma once
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include "WindowManager.h"
#include "cNativeContent.h"
#include <map>
#include <string>
#include <stack>

class cContainer;

struct Font {
	std::string fontName;
	std::string fontFamily;
};

class cRichTextView : public cNativeContent, public wxRichTextCtrl
{
public:
	cRichTextView(cContainer* parent);
	void applyComponentStyle(struct Style* style);
	~cRichTextView();
private:
	// map to convert start position of URLs in text to a Action ID
	std::map<wxTextPos, uint32_t> actions;
	std::stack<uint32_t> styleIDs;
	void URLclickHandler(wxTextUrlEvent& event);
	//void clickHandler(wxTextUrlEvent& event);

	void interpretContent() override;
	void interpretControlBytes(int* index);
	void insertStringFromIndexes(int start, int end);

	void interpretTextStyle(struct Style* style, bool removeStyle);
	//void addAction() override;
};

