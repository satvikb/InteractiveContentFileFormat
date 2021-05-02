#pragma once

#include <wx/wx.h>
#include "format.h"
#include "cRichTextView.h"

class cContainer : public wxPanel
{
public:
	cContainer(bool mainContainer, wxWindow* parent);
	cContainer(cContainer* parent);

	void SetContainerData(struct Container* data, struct Layout* layout);

	~cContainer();

private:
	bool isMainContainer = false;
	int numberElements = 0;
	wxWindow* children[];
};