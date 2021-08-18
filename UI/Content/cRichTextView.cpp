#include "cRichTextView.h"

cRichTextView::cRichTextView(cContainer* parent, struct Content* content, struct Style* style) : cStyledPanel((wxWindow*)parent, style, wxID_ANY) {
	wxRichTextCtrl::Create((wxWindow*)parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);
	SetContent(content);
	readyToResize = true;
	//SetTransparent(0);
	// TODO can we use the Event Table for this?
	wxRichTextCtrl::Bind(wxEVT_TEXT_URL, &cRichTextView::URLclickHandler, this);
	// TODO will this conflict with the parent event table size event?
	wxRichTextCtrl::Bind(wxEVT_SIZE, &cRichTextView::OnSize, this);
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

		cStyledPanel::Freeze();
		Clear();
		// SetFontScale(5.5f);
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
		cStyledPanel::Thaw();
	}
}

void cRichTextView::insertStringFromIndexes(int start, int end) {
	std::string stringToIns(content->data.begin() + start,
		content->data.begin() + end);
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
			case TEXT_START_STYLE:
			{
				std::vector<uint8_t> data = content->data;
				std::pair<uint8_t, uint32_t> styleID = readChunkTypeAndID((char*)&data[0], &i);

				styleIDs.push(styleID.second);
				interpretTextStyle(FileManager::getStyleByID(styleID.second), false);
			} break;
			case TEXT_END_STYLE:
			{
				interpretTextStyle(FileManager::getStyleByID(styleIDs.top()), true);
				styleIDs.pop();
			} break;
			case TEXT_START_ACTION:
			{
				std::vector<uint8_t> data = content->data;
				std::pair<uint8_t, uint32_t> actionID = readChunkTypeAndID((char*)&data[0], &i);

				wxTextPos pos = GetLastPosition();

				actions[pos] = actionID.second;
				BeginURL(wxT("-")); // TODO what to put here? cant be empty
			} break;
			case TEXT_END_ACTION:
			{
				EndURL();
			} break;
			case TEXT_START_NUMBERED_LIST:
			{
				uint8_t startLevel = (unsigned char)content->data[i];
				currentListLevel = startLevel;
				i++;
				// TODO check if there is an override to the left subindent for this level
				Newline();
				BeginNumberedBullet(currentBulletNumber, currentLeftIndent, CalculateLeftSubIndent());
				auto listSettings = std::make_tuple(true, currentListLevel, currentBulletNumber);
				lists.push(listSettings);
			} break;
			case TEXT_START_UNORDERED_LIST:
			{
				uint8_t startLevel = (unsigned char)content->data[i];
				currentListLevel = startLevel;
				i++;
				Newline();
				BeginSymbolBullet('*', currentLeftIndent, CalculateLeftSubIndent());
				auto listSettings = std::make_tuple(false, currentListLevel, currentBulletNumber);
				lists.push(listSettings);
			} break;
			case TEXT_END_LIST:
			{
				if (lists.size() > 0) {
					auto settingsToRestore = lists.top();
					bool isNumbered = std::get<0>(settingsToRestore);
					int oldLevel = std::get<1>(settingsToRestore);
					int oldBulletNumber = std::get<2>(settingsToRestore);

					currentBulletNumber = oldBulletNumber;
					currentListLevel = oldLevel;

					if (isNumbered) {
						EndNumberedBullet();
					} else {
						EndSymbolBullet();
					}

					lists.pop();
				} else {
					currentBulletNumber = 1;
				}
			} break;
			case TEXT_NEW_BULLET:
			{
				auto currentSettings = lists.top();
				bool isNumbered = std::get<0>(currentSettings);
				if (isNumbered) {
					EndNumberedBullet();
					Newline();
					currentBulletNumber += 1;
					BeginNumberedBullet(currentBulletNumber, currentLeftIndent, CalculateLeftSubIndent());
				} else {
					EndSymbolBullet();
					Newline();
					BeginSymbolBullet('*', currentLeftIndent, CalculateLeftSubIndent());
				}
			} break;
			case TEXT_SET_BULLET_NUMBER:
			{
				uint8_t newBulletNumber = (unsigned char)content->data[i];
				currentBulletNumber = newBulletNumber;
				i++;
			} break;
			case TEXT_SET_LIST_LEVEL:
			{
				uint8_t newLevel = (unsigned char)content->data[i];
				currentListLevel = newLevel;
				i++;
			} break;
			case TEXT_LIST_LEFT_SUBINDENT:
			{
				uint8_t level = (unsigned char)content->data[i];
				i++;
				uint16_t subIndent = (unsigned char)(content->data[i] << 8) | (unsigned char)content->data[(i)+1];
				i += 2;
				subIndentOverrides[level] = subIndent;
			} break;
			case TEXT_LEFT_SUBINDENT_LEVEL_MULTIPLIER:
			{
				uint16_t multiplier = (unsigned char)(content->data[i] << 8) | (unsigned char)content->data[(i)+1];
				leftSubIndentMultiplier = multiplier;
				i += 2;
			} break;
			default:
			// custom control point not used/doesnt mean anything
			break;
		}
	} else {
		// ERROR, not in right place
		i += 1;
	}

	*index = i;
}

int cRichTextView::CalculateLeftSubIndent() {
	if (subIndentOverrides.count(currentListLevel) > 0) {
		return subIndentOverrides[currentListLevel];
	}
	return currentListLevel * leftSubIndentMultiplier;
}

void cRichTextView::ApplyContentStyle(struct Style* style) {
}

void cRichTextView::RenderContent(wxDC& dc) {
	// cRichTextView handles the background rendering itself, so clear whatever cStyledPanel did
	dc.Clear();
}
void cRichTextView::interpretTextStyle(struct Style* style, bool removeStyle) {
	// loop through styles

	if (style != nullptr) {
		std::map<uint8_t, std::any> styles = style->styles;
		uint8_t windowDivider = std::any_cast<uint8_t>(getStyleKeyWithDefaultValue(styles, (uint8_t)STYLE_TEXT_WINDOW_DIVIDER, (uint8_t)4));
		if (windowDivider == 0) {
			windowDivider = 4;
		}
		uint8_t textScaleMode = std::any_cast<uint8_t>(getStyleKeyWithDefaultValue(styles, (uint8_t)STYLE_TEXT_SCALE_MODE, (uint8_t)0));

		for (auto& [key, val] : styles) {
			switch (key) {
				case STYLE_TEXT_BOLD:
				if (removeStyle) {
					EndBold();
				} else {
					BeginBold();
				}
				break;
				case STYLE_TEXT_ITALICS:
				if (removeStyle) {
					EndItalic();
				} else {
					BeginItalic();
				}
				break;
				case STYLE_TEXT_UNDERLINE:
				if (removeStyle) {
					EndUnderline();
				} else {
					BeginUnderline();
				}
				break;
				case STYLE_TEXT_COLOR:
				if (removeStyle) {
					EndTextColour();
				} else {
					wxColour col = std::any_cast<wxColour>(val);
					BeginTextColour(col);
				}
				break;
				case STYLE_TEXT_SIZE:
				if (removeStyle) {
					EndFontSize();
				} else {
					uint8_t rawTextSize = std::any_cast<uint8_t>(val);

					switch (textScaleMode) {
						case TEXT_SCALE_MODE_WINDOW_HEIGHT:
						case TEXT_SCALE_MODE_WINDOW_WIDTH:
						{
							wxSize windowSize = WindowManager::GetWindowSize();
							bool scaleWithWidth = textScaleMode == TEXT_SCALE_MODE_WINDOW_WIDTH;
							int scaledTextSize = WindowManager::GetScaledTextSize(windowSize, scaleWithWidth, windowDivider, rawTextSize);
							BeginFontSize(scaledTextSize);
						} break;
						case TEXT_SCALE_MODE_PARENT_HEIGHT:
						case TEXT_SCALE_MODE_PARENT_WIDTH:
						{
							wxSize parentSize = wxRichTextCtrl::GetSize();
							bool scaleWithWidth = textScaleMode == TEXT_SCALE_MODE_PARENT_WIDTH;
							int scaledTextSize = WindowManager::GetScaledTextSize(parentSize, scaleWithWidth, windowDivider, rawTextSize);
							BeginFontSize(scaledTextSize);
						} break;
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

void cRichTextView::PaintBackground(wxDC& dc) {
	// TODO not sure how to properly cast dc to make the gc, but this works
	gc = wxGraphicsContext::Create((wxWindowDC&)dc);
	dc.GetSize(&contextWidth, &contextHeight);
	RenderBackgroundStyle(dc);
}

void cRichTextView::OnSize(wxSizeEvent& event) {

	Logger::logToFile(std::to_string(contextWidth) + " " + std::to_string(contextHeight));

	if (readyToResize == true) {
		interpretContent();
		// skip the event.
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

// cRichTextView::addAction() {
//
//}

bool cRichTextView::IsEditable() const {
	return false;
}

cRichTextView::~cRichTextView() {
	content = nullptr;
	delete content;
	// actions.erase();
	// styleIDs.empty();
}