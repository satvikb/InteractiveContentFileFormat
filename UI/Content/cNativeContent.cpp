#include "cNativeContent.h"

void cNativeContent::SetContent(Content* content) {
	this->content = content;
	interpretContent();
}

void cNativeContent::interpretContent() {
}