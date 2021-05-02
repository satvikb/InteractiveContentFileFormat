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

cRichTextView::~cRichTextView() {

}

wxBEGIN_EVENT_TABLE(cRichTextView, wxRichTextCtrl)
EVT_RICHTEXT_LEFT_CLICK(wxID_ANY, cRichTextView::clickHandler)
wxEND_EVENT_TABLE()
