#include "cWebView.h"

cWebView::cWebView(cContainer* parent){
	// , wxWebViewDefaultURLStr, wxDefaultPosition, wxDefaultSize, 0, wxWebViewNameStr
	wxWindow::Create((wxWindow *)parent, wxID_ANY);
	browser = wxWebView::New(this, wxID_ANY);

	//wxWebView::Create((wxWindow*)parent, wxID_ANY);
	wxConstraintPosition wxPos = FileManager::convertLayoutPositionToConstraint(0, 0, 100, 100);
	cContainer::applyParentConstraints(browser, this, wxPos);
	Layout();
}

void cWebView::interpretContent() {
	if (content->data.size() > 0) {
		if (content->data[0] == 0x1) {
			std::string url;
			for (int i = 1; content->data[i] != '\0'; i++) {
				//header->author[authorI] = buffer[i];
				url += (content->data[i]);
				//printf("%c\n", buffer[i]);
			}
			
			this->url = url;
			browser->LoadURL(wxString(url));
		}
		
	}
}

cWebView::~cWebView() {

}