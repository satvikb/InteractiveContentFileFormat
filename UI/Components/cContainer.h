#pragma once

#include <wx/wx.h>
#include <any>
#include <memory>
#include "FileManager.h"
#include "WindowManager.h"
#include "cRichTextView.h"
#include "cImageView.h"
#include "cBitmap.h"
#include "cButton.h"
#include "cWebView.h"

class cContainer : public wxPanel
{
public:
	cContainer(bool mainContainer, wxWindow* parent);
	cContainer(cContainer* parent) : cContainer(false, parent) {}

	void CreateContainerUI(std::pair<struct Container*, struct Layout*> dataPair);
	void CreateContainerUI(struct Container* container, struct Layout* layout);
	void ReplaceContainerData(bool mainContainer, struct Container* container, struct Layout* layout);
	void ReplaceElementAtIndexWithContent(uint8_t index, uint16_t contentID);
	void SetContainerStyle(struct Style* style);
	bool getIsMainContainer();
	wxWindow* getParentWindow();
	Container* getContainer();


	~cContainer();

	static void applyParentConstraints(wxWindow* applyTo, wxWindow* parent, struct wxConstraintPosition constraints);

private:
	bool isMainContainer = false;
	//int numberElements = 0;

	struct Container* container;
	struct Layout* layout;

	wxWindow* parentWindow;
	// imagine vector for now
	std::vector<wxWindow*> children;

	void CreateContent(struct Content* content, struct elementPosition* pos, uint8_t index = -1);
	void CreateSubContainer(uint32_t containerID, struct elementPosition* pos, uint8_t index = -1);
};