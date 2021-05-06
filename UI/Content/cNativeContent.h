#pragma once
#include <map>
#include "reader.h"

// content is async, this entire class can exist without loading in content data
class cNativeContent
{
public:
	virtual void SetContent(Content* content);

protected:
	bool didReadContent; // if content is ready to display
	Content* content;
	virtual void interpretContent();
	uint16_t getActionIDFromBytes(uint8_t left, uint8_t right);
	//virtual void addAction();
};

