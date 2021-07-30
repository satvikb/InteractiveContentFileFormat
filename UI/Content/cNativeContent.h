#pragma once
#include <map>
#include "reader.h"
#include "FileManager.h"

// content is async, this entire class can exist without loading in content data
class cNativeContent
{
public:
	virtual void SetContent(struct Content* content);
	virtual void ApplyComponentStyle(struct Style* style);
protected:
	bool didReadContent; // if content is ready to display
	struct Content* content;
	struct Style* componentStyle;
	virtual void interpretContent();
	uint16_t getIDFromBytes(uint8_t left, uint8_t right);
	//virtual void addAction();
};

