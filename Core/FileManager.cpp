#include "FileManager.h"

// TODO for all functions: 
// error handling, check for null ic
// check if IDs exist

InteractiveContent* FileManager::ic;

bool FileManager::loadFile(const char* filename) {
	ic = readFile(filename);
	return true;
}

uint16_t FileManager::getStartContainerID() {
	uint16_t startID = ic->header->startContainer;
	return startID;
}

std::pair<struct Container*, struct Layout*> FileManager::getContainerAndLayoutByID(uint16_t containerID) {
	struct Container* container = getContainerByID(containerID);
	uint16_t layoutID = (*container).layoutID;
	struct Layout* layout = getLayoutByID(layoutID);
	return std::make_pair(container, layout);
}

struct Layout* FileManager::getLayoutByID(uint16_t layoutID) {
	if (ic->layouts.count(layoutID) > 0) {
		return ic->layouts[layoutID];
	}
	else {
		return nullptr;
	}
}

struct Container* FileManager::getContainerByID(uint16_t containerID) {
	if (ic->containers.count(containerID) > 0) {
		return ic->containers[containerID];
	}
	else {
		return nullptr;
	}
}

struct Content* FileManager::getContentByID(uint16_t contentID) {
	if (ic->content.count(contentID) > 0) {
		return ic->content[contentID];
	}
	else {
		return nullptr;
	}
}

struct wxConstraintPosition FileManager::convertLayoutPositionToConstraint(struct elementPosition* pos) {
	int x = (int)((pos->x / 40000.0)*100.0);
	int y = (int)((pos->y / 40000.0) * 100.0);
	int w = (int)((pos->w / 40000.0) * 100.0);
	int h = (int)((pos->h / 40000.0) * 100.0);
	struct wxConstraintPosition retPos = {
		x,
		y,
		w,
		h
	};
	return retPos;
}