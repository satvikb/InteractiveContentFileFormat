#pragma once
#include <wx/wx.h>
#include <wx/webview.h>
#include "cContainer.h"
#include "cNativeContent.h"

class cWebView : public cNativeContent, public wxWindow
{
public:
	//virtual bool Create(wxWindow* parent, wxWindowID id, const wxString& url = wxWebViewDefaultURLStr, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxWebViewNameStr);
	cWebView(cContainer* parent);
	//void applyComponentStyle(struct Style* style);
	~cWebView();

private:
	void interpretContent() override;
	wxWebView* browser;
	std::string url;
	//std::string currentLoadedURL;
};

