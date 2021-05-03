#pragma once
#include <map>

#include "reader.h"
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
	static std::pair<struct Container*, struct Layout*> getContainerAndLayoutByID(uint16_t containerID);
};