#include "cNativeContent.h"

void cNativeContent::SetContent(Content* content) {
	this->content = content;
	interpretContent();
}
void cNativeContent::ApplyComponentStyle(Style* style) {
	this->componentStyle = style;
}
uint16_t cNativeContent::getIDFromBytes(uint8_t left, uint8_t right) {
	return (left << 8) | right;
}

void cNativeContent::interpretContent() {}
//void cNativeContent::addAction() {}