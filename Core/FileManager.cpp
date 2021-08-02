#include "FileManager.h"
#include <fstream>

// TODO for all functions: 
// error handling, check for null ic
// check if IDs exist

// maybe have a pointer here to cMain in order to have a ExecuteAction(id) function? or maybe a completely different ActionHandler class?
InteractiveContent* FileManager::ic;

bool FileManager::loadFile(const char* filename) {
	ic = new InteractiveContent;
	bool success = readFile(ic, filename);

	if (success) {
		std::string updateVersionURL = ic->header->updateVersionURL;
		if (updateVersionURL.size() > 0) {
			// version URL exists, download the version
			// TODO do this async
			cpr::Response versionDataRes = downloadFileData(updateVersionURL.c_str());
			std::string versionData = versionDataRes.text;
			// the downloaded file should only be two bytes containing the updated version
			if (versionDataRes.downloaded_bytes == 2) {
				uint16_t newFileVersion = (unsigned char)(versionData[0] << 8) | (unsigned char)versionData[1];
				if (newFileVersion > ic->header->fileVersion || newFileVersion == HEADER_FORCE_AUTO_UPDATE_VALUE) {
					// update the file
					std::string updateFileURL = ic->header->updateFileURL;

					if (updateFileURL.size() > 0) {
						if (downloadUpdatedFile(filename, updateFileURL.c_str())) {
							// TODO proper deleting of file
							delete ic;
							// reload the file
							return loadFile(filename);
						}
					}
				}
			}
		}
	}

	return success;
}

// download the file and replace filename
bool FileManager::downloadUpdatedFile(const char* filename, const char* updateFileURL) {
	// TODO try catch & async completion
	cpr::Response newFileData = downloadFileData(updateFileURL);
	std::string data = newFileData.text;
	size_t length = newFileData.downloaded_bytes;
	if (length > 0) {
		//outfile.write(&length, sizeof(length));
		std::ofstream outfile(filename, std::ofstream::binary);

		outfile.write(&data[0], length);
		outfile.close();

		return true;
	} else {
		return false;
	}

}

bool FileManager::addChunksFromURL(const char* url) {
	return streamFile(ic, url);
}

uint32_t FileManager::getStartContainerID() {
	uint32_t startID = ic->header->startContainer;
	return startID;
}

std::pair<struct Container*, struct Layout*> FileManager::getContainerAndLayoutByID(uint32_t containerID) {
	struct Container* container = getContainerByID(containerID);
	uint32_t layoutID = (*container).layoutID;
	struct Layout* layout = getLayoutByID(layoutID);
	return std::make_pair(container, layout);
}

struct Header* FileManager::getHeader() {
	return ic->header;
}

struct Layout* FileManager::getLayoutByID(uint32_t layoutID) {
	if (ic->layouts.count(layoutID) > 0) {
		return ic->layouts[layoutID];
	}
	else {
		return nullptr;
	}
}

struct Container* FileManager::getContainerByID(uint32_t containerID) {
	if (ic->containers.count(containerID) > 0) {
		return ic->containers[containerID];
	}
	else {
		return nullptr;
	}
}

struct Content* FileManager::getContentByID(uint32_t contentID) {
	if (ic->content.count(contentID) > 0) {
		return ic->content[contentID];
	}
	else {
		return nullptr;
	}
}

struct Action* FileManager::getActionByID(uint32_t actionID) {
	if (ic->actions.count(actionID) > 0) {
		return ic->actions[actionID];
	}
	else {
		return nullptr;
	}
}

struct Style* FileManager::getStyleByID(uint32_t styleID) {
	if (ic->styles.count(styleID) > 0) {
		return ic->styles[styleID];
	}
	else {
		return nullptr;
	}
}

struct Content* FileManager::getStreamedContent(Content* content) {
	std::vector<uint8_t> streamURLBytes = content->data;
	std::string streamURL(streamURLBytes.begin(), streamURLBytes.end());
	cpr::Response res = downloadFileData(streamURL.c_str());
	std::string data = res.text;
	std::pair<uint32_t, struct Content*> newContentPair = readContent(&data[0], 0);
	Content* newContent = newContentPair.second;
	// the streamed content ID does not matter, keep the original ID
	newContent->chunkID = content->chunkID;
	return newContent;
}

struct wxConstraintPosition FileManager::convertLayoutPositionToConstraint(struct elementPosition* pos) {
	/*int x = (int)((pos->x / 40000.0)*100.0);
	int y = (int)((pos->y / 40000.0) * 100.0);
	int w = (int)((pos->w / 40000.0) * 100.0);
	int h = (int)((pos->h / 40000.0) * 100.0);*/
	struct wxConstraintPosition retPos = {
		pos->x,
		pos->y,
		pos->w,
		pos->h
	};
	return retPos;
}

struct wxConstraintPosition FileManager::convertLayoutPositionToConstraint(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	struct wxConstraintPosition retPos = {
		x, y, w, h
	};
	return retPos;
}