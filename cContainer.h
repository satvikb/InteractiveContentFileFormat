#pragma once

#include <wx/wx.h>
#include "FileManager.h"
#include "cRichTextView.h"

class cContainer : public wxPanel
{
public:
	cContainer(bool mainContainer, wxWindow* parent);
	cContainer(cContainer* parent) : cContainer(false, parent) {}

	void CreateContainerUI(std::pair<struct Container*, struct Layout*> dataPair);
	void CreateContainerUI(struct Container* container, struct Layout* layout);

	~cContainer();

private:
	bool isMainContainer = false;
	//int numberElements = 0;
	std::vector<wxWindow*> children;
};