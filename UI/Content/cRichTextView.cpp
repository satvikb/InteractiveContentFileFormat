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
}

void cRichTextView::insertStringFromIndexes(int start, int end) {
	std::string stringToIns(content->data.begin() + start, content->data.begin() + end);
	//wxString wxStr((u8"\xF0\x9D\x8C\x86"));
	//wxString wxStr2(("\xF0\x9D\x8C\x86"));

	//wxString wxStrUTF = wxStr.ToUTF8();
	//WriteText(wxStrUTF);
	//WriteText(wxStr);
	//WriteText(wxStr2);
	//WriteText(wxStr2.ToUTF8());
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
		case 0x82: {
			uint8_t left = content->data[i];
			uint8_t right = content->data[i+1];
			i += 2; // skip the action ID bytes
			uint16_t actionID = getActionIDFromBytes(left, right);
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