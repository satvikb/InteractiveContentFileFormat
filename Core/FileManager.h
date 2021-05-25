#pragma once
#include <map>

#include "reader.h"

// used for constraints, int is a integer percent from 0-100
struct wxConstraintPosition {
	uint8_t x;
	uint8_t y;
	uint8_t width;
	uint8_t height;
};
// Static class to handle the current file that is read in
class FileManager
{
private:
	static struct InteractiveContent* ic;
	FileManager(){}

public:
	static bool loadFile(const char* filename);
	static uint16_t getStartContainerID();
	static struct Layout* getLayoutByID(uint16_t layoutID);
	static struct Container* getContainerByID(uint16_t containerID);
	static struct Content* getContentByID(uint16_t contentID);
	static struct Action* getActionByID(uint16_t actionID);
	static struct Style* getStyleByID(uint16_t styleID);
	static std::pair<struct Container*, struct Layout*> getContainerAndLayoutByID(uint16_t containerID);
	static struct wxConstraintPosition convertLayoutPositionToConstraint(struct elementPosition* pos);
	static struct wxConstraintPosition convertLayoutPositionToConstraint(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
};