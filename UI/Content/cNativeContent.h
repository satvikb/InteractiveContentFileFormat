#pragma once
#include <map>
#include "reader.h"
#include "FileManager.h"

// content is async, this entire class can exist without loading in content data
class cNativeContent
{
public:
	virtual void SetContent(struct Content* content);
protected:
	bool didReadContent; // if content is ready to display
	struct Content* content;
	virtual void interpretContent();
	uint16_t getIDFromBytes(uint8_t left, uint8_t right);
	//virtual void addAction();
};

