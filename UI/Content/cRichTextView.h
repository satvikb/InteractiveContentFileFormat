#pragma once

#include <wx/richtext/richtextctrl.h>
#include <map>
#include <string>
#include <stack>

#include "WindowManager.h"
#include "cNativeContent.h"
#include "cStyledPanel.h"

class cContainer;

struct Font {
	std::string fontName;
	std::string fontFamily;
};

class cRichTextView : public wxRichTextCtrl, public cStyledPanel, public cNativeContent
{
public:
	cRichTextView(cContainer* parent, struct Content* content, struct Style* style);
	void ApplyContentStyle(struct Style* style) override;
	~cRichTextView();
private:
	// map to convert start position of URLs in text to a Action ID
	std::map<wxTextPos, uint32_t> actions;
	std::stack<uint32_t> styleIDs;
	bool readyToResize = false;

	void URLclickHandler(wxTextUrlEvent& event);
	void OnSize(wxSizeEvent& event) override;
	//void clickHandler(wxTextUrlEvent& event);
	void RenderContent(wxDC& dc) override;

	void interpretContent() override;
	void interpretControlBytes(int* index);
	void insertStringFromIndexes(int start, int end);

	void PaintBackground(wxDC& dc) override;

	void interpretTextStyle(struct Style* style, bool removeStyle);
	std::any getStyleKeyWithDefaultValue(std::map<uint8_t, std::any> styles, uint8_t key, std::any defaultValue);

	virtual bool IsEditable() const;
	//DECLARE_EVENT_TABLE()
	//void addAction() override;
};

