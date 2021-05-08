#pragma once

#include <wx/wx.h>
#include "FileManager.h"
#include "WindowManager.h"
#include "cRichTextView.h"

class cContainer : public wxPanel
{
public:
	cContainer(bool mainContainer, wxWindow* parent);
	cContainer(cContainer* parent) : cContainer(false, parent) {}

	void CreateContainerUI(std::pair<struct Container*, struct Layout*> dataPair);
	void CreateContainerUI(struct Container* container, struct Layout* layout);
	void ReplaceContainerData(bool mainContainer, struct Container* container, struct Layout* layout);
	void SetContainerStyle(struct Style* style);
	bool getIsMainContainer();
	wxWindow* getParentWindow();
	Container* getContainer();


	~cContainer();

	static void applyParentContraints(wxWindow* applyTo, wxWindow* parent, struct wxConstraintPosition constraints);

private:
	bool isMainContainer = false;
	//int numberElements = 0;

	struct Container* container;
	struct Layout* layout;

	wxWindow* parentWindow;
	std::vector<wxWindow*> children;
};