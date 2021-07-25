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
	bool readyToResize = false;

	void URLclickHandler(wxTextUrlEvent& event);
	void OnSize(wxSizeEvent& event);
	//void clickHandler(wxTextUrlEvent& event);

	void interpretContent() override;
	void interpretControlBytes(int* index);
	void insertStringFromIndexes(int start, int end);

	void interpretTextStyle(struct Style* style, bool removeStyle);
	std::any getStyleKeyWithDefaultValue(std::map<uint8_t, std::any> styles, uint8_t key, std::any defaultValue);

	DECLARE_EVENT_TABLE()
	//void addAction() override;
};

