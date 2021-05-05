#pragma once
#include <map>
#include "reader.h"

// content is async, this entire class can exist without loading in content data
struct cNativeContent
{
public:
	virtual void SetContent(Content* content);

private:
	bool didReadContent; // if content is ready to display
	Content* content;
	virtual void interpretContent();
};

