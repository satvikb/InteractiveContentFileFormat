#include "cRichTextView.h"

cRichTextView::cRichTextView(cContainer* parent) {
	wxRichTextCtrl::Create((wxWindow *)parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(200, 200), wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);
	content = nullptr;
	readyToResize = true;

	// TODO can we use the Event Table for this?
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
	if (content != nullptr) {
		Freeze();
		Clear();
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
}

BEGIN_EVENT_TABLE(cRichTextView, wxRichTextCtrl)
//Size event
EVT_SIZE(cRichTextView::OnSize)
END_EVENT_TABLE()

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
		// Start Style
		case 0x80: {
			std::vector<uint8_t> data = content->data;
			std::pair<uint8_t, uint32_t> styleID = readChunkTypeAndID((char*)&data[0], &i);

			//uint8_t left = content->data[i];
			//uint8_t right = content->data[i + 1];
			//i += 2; // skip the style ID bytes
			//uint16_t styleID = getIDFromBytes(left, right);
			styleIDs.push(styleID.second);
			interpretTextStyle(FileManager::getStyleByID(styleID.second), false);
		}
		break;
		// End Style
		case 0x81: {
			interpretTextStyle(FileManager::getStyleByID(styleIDs.top()), true);
			styleIDs.pop();
		}
		break;
		// Begin Action/URL
		case 0x82: {
			std::vector<uint8_t> data = content->data;
			std::pair<uint8_t, uint32_t> actionID = readChunkTypeAndID((char*)&data[0], &i);

			//uint8_t left = content->data[i];
			//uint8_t right = content->data[i+1];
			//i += 2; // skip the action ID bytes
			//uint16_t actionID = getIDFromBytes(left, right);
			wxTextPos pos = GetLastPosition();

			actions[pos] = actionID.second;
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

	if (style != nullptr) {
		std::map<uint8_t, std::any> styles = style->styles;
		uint8_t windowDivider = std::any_cast<uint8_t>(getStyleKeyWithDefaultValue(styles, (uint8_t)STYLE_TEXT_WINDOW_DIVIDER, (uint8_t)4));
		uint8_t textScaleMode = std::any_cast<uint8_t>(getStyleKeyWithDefaultValue(styles, (uint8_t)STYLE_TEXT_SCALE_MODE, (uint8_t)0));

		for (auto& [key, val] : styles) {
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
			case STYLE_TEXT_COLOR:
				if (removeStyle) { EndTextColour();  }
				else {
					wxColour col = std::any_cast <wxColour>(val);
					BeginTextColour(col);
				}
			break;
			case STYLE_TEXT_SIZE:
				if (removeStyle) { EndFontSize(); }
				else {
					uint8_t rawTextSize = std::any_cast <uint8_t>(val);

					switch (textScaleMode)
					{
					case TEXT_SCALE_MODE_WINDOW_HEIGHT: case TEXT_SCALE_MODE_WINDOW_WIDTH: {
						wxSize windowSize = WindowManager::GetWindowSize();
						int windowDimension = textScaleMode == TEXT_SCALE_MODE_WINDOW_WIDTH ? windowSize.GetWidth() : windowSize.GetHeight();
						float windowMultiplier = (float)windowDimension / (float)windowDivider;
						uint8_t scaledTextSize = ((float)rawTextSize / 100.f) * windowMultiplier;
						BeginFontSize(scaledTextSize);
					}
					break;
					default:
						BeginFontSize(rawTextSize);
						break;
					}
				}
			break;
			default:
			break;
			}
		}
	}
}

void cRichTextView::OnSize(wxSizeEvent& event) {
	if (readyToResize == true){
		interpretContent();
		//skip the event.
		event.Skip();
	}
}

std::any cRichTextView::getStyleKeyWithDefaultValue(std::map<uint8_t, std::any> styles, uint8_t key, std::any defaultValue) {
	auto it = styles.find(key);
	if (it == styles.end()) {
		return defaultValue;
	}
	return it->second;
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
	content = nullptr;
	delete content;
	//actions.erase();
	//styleIDs.empty();
}