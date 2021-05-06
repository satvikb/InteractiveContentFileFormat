#include "cRichTextView.h"

cRichTextView::cRichTextView(cContainer* parent) {
	Create((wxWindow *)parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(200, 200), wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);

	//BeginURL(wxT("testURL"));
	//WriteText(wxT("Welcome to wxRichTextCtrl, a wxWidgets control for editing and presenting styled text and images"));
	//EndURL();

}

void cRichTextView::clickHandler(wxRichTextEvent& event) {
	////BeginURL(wxT("ss"));
	//WriteText(wxT("clic"));
	////EndURL();
}

void cRichTextView::interpretContent() {
	if (content->data.size() > 0) {
		int strStart = 0;
		int i = 0;
		for (; i < content->data.size(); i++) {
			if (content->data[i] == 0xEE) {
				// control byte start
				insertStringFromIndexes(strStart, i);
				interpretControlBytes(&i);
				strStart = i;
			}
		}

		insertStringFromIndexes(strStart, i);
		// strStart = 0;
		// i = 0
		// loop byte by byte
			// test if byte is control point start(? maybe have to test all 3 bytes at once?) - 11101110 10000000 10000000 is code point U+E000. First byte 11101110 is EE in hex. 
				// insertString(data[strStart] -> data[i])
				// interpretControlCharacters()
				// strStart = i
			// else
				// i += 1

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

	// next byte MUST be BC by definition. Sanity check:
	if (content->data[i + 1] == 0xBC) {

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

cRichTextView::~cRichTextView() {

}

wxBEGIN_EVENT_TABLE(cRichTextView, wxRichTextCtrl)
EVT_RICHTEXT_LEFT_CLICK(wxID_ANY, cRichTextView::clickHandler)
wxEND_EVENT_TABLE()
