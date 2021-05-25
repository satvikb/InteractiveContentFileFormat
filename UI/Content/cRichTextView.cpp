#include "cRichTextView.h"

cRichTextView::cRichTextView(cContainer* parent) {
	wxRichTextCtrl::Create((wxWindow *)parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(200, 200), wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);

	wxRichTextCtrl::Bind(wxEVT_TEXT_URL, &cRichTextView::URLclickHandler, this);
}

void cRichTextView::URLclickHandler(wxTextUrlEvent& event) {
	wxTextPos startPos = event.GetURLStart();
	uint16_t actionID = actions[startPos];
	wxTextPos endPos = event.GetURLEnd();

	// execute action
	WindowManager::ExecuteActionID(actionID);
}

void cRichTextView::interpretContent() {
	Freeze();
	//SetFontScale(5.5f);
	if (content->data.size() > 0) {
		int strStart = 0;
		int i = 0;
		for (; (unsigned int)i < content->data.size(); i++) {
			if (content->data[i] == 0xEE) {
				// control byte start
				insertStringFromIndexes(strStart, i);
				interpretControlBytes(&i);
				strStart = i;
			}
		}

		insertStringFromIndexes(strStart, i);
	}
	Thaw();
}

void cRichTextView::insertStringFromIndexes(int start, int end) {
	std::string stringToIns(content->data.begin() + start, content->data.begin() + end);
	WriteText(wxString::FromUTF8(stringToIns));
}

void cRichTextView::interpretControlBytes(int* index) {
	int i = *index;

	// i is at EE byte
	i += 1;
	// next byte MUST be BC by definition. Sanity check:
	if (content->data[i] == 0xBC) {
		i += 1;
		uint8_t val = content->data[i];

		// now move i into the data bytes for this control byte
		i += 1;

		switch (val) {
		case 0x80: {
			uint8_t left = content->data[i];
			uint8_t right = content->data[i + 1];
			i += 2; // skip the style ID bytes
			uint16_t styleID = getIDFromBytes(left, right);
			styleIDs.push(styleID);
			interpretTextStyle(FileManager::getStyleByID(styleID), false);
		}
		break;
		case 0x81: {
			interpretTextStyle(FileManager::getStyleByID(styleIDs.top()), true);
			styleIDs.pop();
		}
		break;
		case 0x82: {
			uint8_t left = content->data[i];
			uint8_t right = content->data[i+1];
			i += 2; // skip the action ID bytes
			uint16_t actionID = getIDFromBytes(left, right);
			wxTextPos pos = GetLastPosition();

			actions[pos] = actionID;
			BeginURL(wxT("d")); // TODO what to put here? cant be empty
		}
		break;
		case 0x83: {
			EndURL();
		}
		break;
		default:
			// custom control point not used/doesnt mean anything
			break;
		}
	}
	else {
		// ERROR, not in right place
		i += 1;
	}


	*index = i;
}

void cRichTextView::applyComponentStyle(struct Style* style) {
	if (style != nullptr) {
		// c++17 style loop
		for (auto [key, val] : style->styles) {
			switch (key) {
			case STYLE_COMPONENT_BACKGROUND_COLOR: {
				SetBackgroundStyle(wxBG_STYLE_COLOUR);
				SetBackgroundColour(std::any_cast<wxColour>(val));
			}
			break;
			}
		}
	}
}

void cRichTextView::interpretTextStyle(struct Style* style, bool removeStyle) {
	// loop through styles
	// removeStyle == true ? EndApplyStyle() : ApplyStyle()
	if (style != nullptr) {
		for (auto [key, val] : style->styles) {
			switch (key) {
			case STYLE_TEXT_BOLD:
				if (removeStyle) { EndBold(); }
				else { BeginBold(); }
			break;
			case STYLE_TEXT_ITALICS:
				if (removeStyle) { EndItalic(); }
				else { BeginItalic(); }
			break;
			case STYLE_TEXT_UNDERLINE:
				if (removeStyle) { EndUnderline(); }
				else { BeginUnderline(); }
				break;
			default:
			break;
			}
		}
	}
}


// interpretControlCharacters(&i)
/*
	handle font table start/end
	font start/end
	paragraph start/end (?)
	paragraph styles
	text styles (multiple)
*/

//cRichTextView::addAction() {
//
//}

cRichTextView::~cRichTextView() {

}