#include "cRichTextView.h"

cRichTextView::cRichTextView(cContainer* parent) {
	Create((wxWindow *)parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(200, 200), wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);

	BeginURL(wxT("testURL"));
	WriteText(wxT("Welcome to wxRichTextCtrl, a wxWidgets control for editing and presenting styled text and images"));
	EndURL();

}

void cRichTextView::clickHandler(wxRichTextEvent& event) {
	BeginURL(wxT("ss"));
	WriteText(wxT("clic"));
	EndURL();
}

void cRichTextView::interpretContent() {
	if (content->data.size() > 0) {
		// strStart = 0;
		// i = 0
		// loop byte by byte
			// test if byte is control point start(? maybe have to test all 3 bytes at once?)
				// insertString(data[strStart] -> data[i])
				// interpretControlCharacters()
				// strStart = i
			// else
				// i += 1
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

cRichTextView::~cRichTextView() {

}

wxBEGIN_EVENT_TABLE(cRichTextView, wxRichTextCtrl)
EVT_RICHTEXT_LEFT_CLICK(wxID_ANY, cRichTextView::clickHandler)
wxEND_EVENT_TABLE()
