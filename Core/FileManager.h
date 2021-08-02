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
	static bool downloadUpdatedFile(const char* filename, const char* updateFileURL);
	static bool addChunksFromURL(const char* url);

	static uint32_t getStartContainerID();
	static struct Header* getHeader();
	static struct Layout* getLayoutByID(uint32_t layoutID);
	static struct Container* getContainerByID(uint32_t containerID);
	static struct Content* getContentByID(uint32_t contentID);
	static struct Action* getActionByID(uint32_t actionID);
	static struct Style* getStyleByID(uint32_t styleID);
	static struct Content* getStreamedContent(Content* content);
	static std::pair<struct Container*, struct Layout*> getContainerAndLayoutByID(uint32_t containerID);
	static struct wxConstraintPosition convertLayoutPositionToConstraint(struct elementPosition* pos);
	static struct wxConstraintPosition convertLayoutPositionToConstraint(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
};