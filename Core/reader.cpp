#include "reader.h"
#include <cpr/cpr.h>
#include <curl/curl.h>

#include <iostream> 
#define CPPHTTPLIB_OPENSSL_SUPPORT
// #define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING
// 
// class to read file format
bool readFile(struct InteractiveContent* ic, const char* filename) {
	printf("reading %s ", filename);

	FILE* pFile;
	size_t lSize = 0;
	char* buffer;

	fopen_s(&pFile, filename, "rb");
	if (pFile == NULL) {
		fputs("File error", stderr);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	buffer = (char*)malloc(lSize * sizeof(char)); // Enough memory for the file
	fread(buffer, lSize, 1, pFile); // Read in the entire file
	fclose(pFile); // Close the file

	printf("Size: %ld BYTES\n", lSize);

	return readFileData(ic, buffer, lSize);;
}

cpr::Response downloadFileData(const char* url) {
	cpr::SslOptions sslOpts;
	//sslOpts.verify_host = false;
	//sslOpts.verify_peer = false;
	//sslOpts.verify_status = false;
	cpr::Response r = cpr::Get(cpr::Url{url});
	//r.status_code;                  // 200
	//r.header["content-type"];       // application/json; charset=utf-8
	//r.text;
	return r;
}

bool streamFile(struct InteractiveContent* ic, const char* url) {
	cpr::Response res = downloadFileData(url);
	std::string data = res.text;
	double bytes = res.downloaded_bytes;
	return readFileData(ic, &data[0], bytes);
}

bool readFileData(struct InteractiveContent* ic, char* buffer, size_t numberBytes) {
	int i = 0;

	const unsigned char magicNum[] = {0x73, 0x61, 0x6D, 0x61, 0x6C, 0x69, 0x74, 0x6C, 0x6E, 0x76, 0x75, 0x61};

	// confirm magic num
	if (memcmp(magicNum, buffer, MAGIC_NUMBER_LENGTH) != 0) {
		return false;
	}
	i += MAGIC_NUMBER_LENGTH;

	while (i < numberBytes) {
		int throwawayI = i;
		std::pair<uint8_t, uint32_t> chunkData = readChunkTypeAndID(buffer, &throwawayI);
		//i -= 2;
		unsigned char chunkType = chunkData.first;
		//unsigned uint32_t id = (((buffer[i]) & 0x1F) << 8) | (unsigned char)buffer[i+1]; // get 5 rightmost from first byte and add to second byte
		char d = buffer[i];
		//printf("\nAt byte %d. Reading chunk of type: %d. ID: %d\n", i, chunkType, id);
		// struct Header *header = malloc (sizeof (struct Header));
		switch (chunkType){
			case CHUNK_HEADER:
			{
				// header, header ID is not used/thrown away (waste of 4 bytes)
				struct Header* header = readHeader(buffer, &i);
				ic->header = header;
			}
			break;
			case CHUNK_CONTAINER:
			{
				// container
				std::pair<uint32_t, struct Container*> newContainer = readContainer(buffer, &i);
				ic->containers.insert(newContainer);
			}
			break;
			case CHUNK_CONTENT:
			{
				// content (2)
				std::pair<uint32_t, struct Content*> newContent = readContent(buffer, &i);
				ic->content.insert(newContent);
			}
			break;
			case CHUNK_LAYOUT:
			{
				// layout
				std::pair<uint32_t, struct Layout*> newLayout = readLayout(buffer, &i);
				ic->layouts.insert(newLayout);
			}
			break;
			case CHUNK_ACTION:
			{
				// action
				std::pair<uint32_t, struct Action*> newAction = readAction(buffer, &i);
				ic->actions.insert(newAction);
			}
			break;
			case CHUNK_STYLING:
			{
				std::pair<uint32_t, struct Style*> newStyle = readStyle(buffer, &i);
				ic->styles.insert(newStyle);
			}
			break;
			default:
			// unknown chunk type, skip the chunk bytes
			// every chunk type in the extended range must have length bytes
			readChunkTypeAndID(buffer, &i); // skip the chunk type and id
			uint32_t len = read32BitInt(buffer, &i);
			i += len;
			break;
		}
	}

	return true;
}

std::pair<uint8_t, uint32_t> readChunkTypeAndID(char* buffer, int* index) {
	int i = *index;

	uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
	uint32_t ID = 0;

	if (chunkType == CHUNK_EXTENDED) {
		// 110 - use extended range
		chunkType = buffer[i] & 0x1F; // 0x1F == 00011111

		// bytes i+1 to i+3 have the 24 bit ID
		ID = (unsigned char)(buffer[i + 1] << 16) | (unsigned char)(buffer[i + 2] << 8) | (unsigned char)buffer[i + 3];
		i += 4;

	} else {
		ID = (unsigned char)((buffer[i] && 0x1F) << 8) | (unsigned char)buffer[i + 1];
		i += 2;
	}
	// now i is after ID bytes

	*index = i;
	return std::make_pair(chunkType, ID);
}

uint32_t read32BitInt(char* buffer, int* index) {
	int i = *index;
	uint32_t number = uint32_t((unsigned char)(buffer[i]) << 24 |
				(unsigned char)(buffer[i + 1]) << 16 |
				(unsigned char)(buffer[i + 2]) << 8 |
				(unsigned char)(buffer[i + 3]));
	i += 4;
	*index = i;
	return number;
}

struct Header* readHeader(char* buffer, int* index) {
	int i = *index;
	i += 2; // skip the header chunk type and ID

	struct Header* header = new Header;
	header->fileVersion = 0;
	header->windowAspectRatio = 0.f;

	// read specification version
	unsigned short specificationVersionMajor = buffer[i] << 8 | (buffer[i + 1]);
	header->specificationVersionMajor = specificationVersionMajor;
	i += 2;
	unsigned char specificationVersionMinor = (unsigned char)buffer[i];
	header->specificationVersionMinor = specificationVersionMinor;
	unsigned char specificationVersionPatch = (unsigned char)buffer[i+1];
	header->specificationVersionPatch = specificationVersionPatch;
	i += 2;

	uint32_t startContainerId = readChunkTypeAndID(buffer, &i).second;
	header->startContainer = startContainerId;

	// begin string:string map
	std::map<std::string, std::string> metadata;

	while (buffer[i] != 0x0) {
		//  read the next key value pair

		std::string key = readString(buffer, &i);
		if (key.compare(HEADER_ATTRIBUTE_AUTO_UPDATE_CURRENT_VERSION) == 0) {
			uint32_t updateVersion = read32BitInt(buffer, &i);
			header->fileVersion = updateVersion;
		} else if (key.compare(HEADER_ATTRIBUTE_AUTO_UPDATE_VERSION_URL) == 0){
			header->updateVersionURL = readString(buffer, &i);
		} else if (key.compare(HEADER_ATTRIBUTE_AUTO_UPDATE_FILE_URL) == 0) {
			header->updateFileURL = readString(buffer, &i);
		} else if (key.compare(HEADER_ATTRIBUTE_WINDOW_ASPECT_RATIO) == 0) {
			uint8_t widthRatio = (unsigned char)buffer[i];
			uint8_t heightRatio = (unsigned char)buffer[i+1];
			i += 2;

			float aspectRatio = (float)widthRatio / (float)heightRatio;
			header->windowAspectRatio = aspectRatio;
		} else {
			metadata[key] = readString(buffer, &i);
		}
	}
	i += 1; // skip header end byte

	header->metadata = metadata;
	*index = i;
	return header;
}

// attempts to read the layout from the given starting position
std::pair<uint32_t, struct Layout*> readLayout(char* buffer, int* index) {
	int i = *index;

	std::pair<uint8_t, uint32_t> layoutIDs = readChunkTypeAndID(buffer, &i);
	struct Layout* layout = new Layout;
	layout->chunkType = layoutIDs.first;
	layout->chunkID = layoutIDs.second;
	uint8_t numberElements = (unsigned char)buffer[i];
	layout->elementCount = numberElements;
	i += 1;

	printf("Reading layout. Total ID: %d Elements: %d\n", layout->chunkID, numberElements);
	// layout->positions = (struct elementPosition*) malloc(sizeof(struct elementPosition)*numberElements);

	int eleI;
	for (eleI = 0; eleI < numberElements; eleI++) {
		struct elementPosition* pos = new elementPosition;
		pos->x = (unsigned char)buffer[(i)];
		pos->y = (unsigned char)buffer[(i)+1];
		pos->w = (unsigned char)buffer[(i)+2];
		pos->h = (unsigned char)buffer[(i)+3];
		printf("Read position %d %d %d %d %d\n", i, pos->x, pos->y, pos->w, pos->h);
		i += 4;
		// now handle style ID
		std::pair<uint8_t, uint32_t> styleIDs = readChunkTypeAndID(buffer, &i);
		pos->styleID = styleIDs.second;
		layout->positions.push_back(pos);
	}

	*index = i;
	return std::make_pair(layout->chunkID, layout);
}

std::pair<uint32_t, struct Container*> readContainer(char* buffer, int* index) {
	int i = *index;

	struct Container* container = new Container;

	std::pair<uint8_t, uint32_t> containerIDs = readChunkTypeAndID(buffer, &i);
	container->chunkType = containerIDs.first;
	container->chunkID = containerIDs.second;

	std::pair<uint8_t, uint32_t> layoutIDs = readChunkTypeAndID(buffer, &i);
	container->layoutID = layoutIDs.second;

	// now i is at the first byte of the ID or end code

	printf("Reading container. Byte: %d. Container ID: %d. Layout ID: %d.\n", i, containerIDs.second, layoutIDs.second);
	// i must always point to a byte that is either the start of an ID, or a End code
	while (buffer[i] != 0x0) { // 0x0 is byte code for end of container chunk
		std::pair<uint8_t, uint32_t> chunkData = readChunkTypeAndID(buffer, &i);
		struct Chunk chunk = {chunkData.first, chunkData.second};
		container->elementIDs.push_back(chunk);
	}
	// end chunk hit
	i += 1;
	*index = i;
	return std::make_pair(container->chunkID, container);
}

std::pair<uint32_t, struct Content*> readContent(char* buffer, int* index) {
	int i = *index;

	struct Content* content = new Content;

	std::pair<uint8_t, uint32_t> contentIDs = readChunkTypeAndID(buffer, &i);
	content->chunkType = contentIDs.first;
	content->chunkID = contentIDs.second;

	uint8_t contentType = (unsigned char)buffer[i];
	i += 1; // now i is at content length

	uint32_t contentLength = read32BitInt(buffer, &i);
	content->data = std::vector<uint8_t>(contentLength, 0x0);
	// i is now at the first byte of the content
	uint32_t byteI = 0;
	uint32_t byteContentEnd = i + contentLength;
	while (byteI < contentLength) {
		content->data[byteI] = buffer[i];
		i += 1;
		byteI += 1;
	}

	content->length = contentLength;
	content->type = contentType;

	*index = i;
	return std::make_pair(content->chunkID, content);
}

std::pair<uint32_t, struct Action*> readAction(char* buffer, int* index) {
	int i = *index;

	std::pair<uint8_t, uint32_t> actionIDs = readChunkTypeAndID(buffer, &i);

	uint8_t actionType = (unsigned char)buffer[i];
	i += 1;

	struct Action* action;

	switch (actionType) {
		case ACTION_LINK:
		{
			// 3 more bytes
			// 2 bytes - container ID
			// 1 byte - display byte
			uint32_t containerID = readChunkTypeAndID(buffer, &i).second;
			uint8_t displayByte = (unsigned char)buffer[i];
			i += 1;
			struct Link* newLink = new Link;
			newLink->chunkType = actionIDs.first;
			newLink->chunkID = actionIDs.second;
			newLink->actionType = actionType;
			newLink->containerID = containerID;
			newLink->display = displayByte;
			action = newLink;
		}
		break;
		case ACTION_SWAP:
		{
			// 4 bytes
			// 2 bytes - replace this container/content ID (must already be visible)
			// 2 bytes - new container/content ID bytes

			//uint8_t oldChunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
			uint32_t oldID = readChunkTypeAndID(buffer, &i).second;
			//uint8_t newChunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
			uint32_t newID = readChunkTypeAndID(buffer, &i).second;
			struct Swap* newSwap = new Swap;
			newSwap->chunkType = actionIDs.first;
			newSwap->chunkID = actionIDs.second;
			newSwap->actionType = actionType;
			newSwap->replaceID = oldID;
			newSwap->replaceWithID = newID;
			action = newSwap;
		}
		break;
		case ACTION_REPLACE_WITH_ELEMENT:
		{
			// 5 bytes total
			// 2 bytes - container id
			// 1 byte - index
			// 2 bytes - content id
			uint32_t containerID = readChunkTypeAndID(buffer, &i).second;
			uint8_t index = (unsigned char)buffer[i];
			i += 1;
			uint32_t contentID = readChunkTypeAndID(buffer, &i).second;
			struct ReplaceWithElement* replace = new ReplaceWithElement;
			replace->chunkType = actionIDs.first;
			replace->chunkID = actionIDs.second;
			replace->actionType = actionType;
			replace->containerID = containerID;
			replace->index = index;
			replace->replaceWithContentID = contentID;
			action = replace;
		}
		break;
		case ACTION_DOWNLOAD_CHUNKS:
		{
			std::string url;
			for (; buffer[i] != '\0'; i++) {
				url += buffer[i];
			}
			i += 1;
			struct DownloadChunks* downloadAction = new DownloadChunks;
			downloadAction->chunkType = actionIDs.first;
			downloadAction->chunkID = actionIDs.second;
			downloadAction->url = url;
			downloadAction->actionType = actionType;
			action = downloadAction;
		}
		break;
		case ACTION_EXECUTE_COMPOSITE:
		{
			struct ExecuteComposite* executeComposite = new ExecuteComposite;
			executeComposite->chunkType = actionIDs.first;
			executeComposite->chunkID = actionIDs.second;
			executeComposite->actionType = actionType;
			while (buffer[i] != 0x0) {
				std::pair<uint8_t, uint32_t> actionID = readChunkTypeAndID(buffer, &i);
				if (actionID.first == CHUNK_ACTION) { // sanity check
					executeComposite->actionsToExecute.push_back(actionID.second);
				}
			}
			i += 1;

			action = executeComposite;
		}
		break;
		default:
		{

		}
		break;
	}


	*index = i;
	return std::make_pair(action->chunkID, action);
}


std::pair<uint32_t, struct Style*> readStyle(char* buffer, int* index) {
	int i = *index;
	std::pair<uint8_t, uint32_t> styleIDs = readChunkTypeAndID(buffer, &i);

	//uint8_t styleType = (unsigned char)buffer[i];
	//i += 1;

	std::map<uint8_t, std::any> styles;

	uint16_t key = (unsigned char)buffer[i];
	if ((key & 0xF0) == 0xF0) {
		// the first 4 bits in the key is 1111, meaning two bytes are used for the style key
		// & with 0x7F to only get the 7 bits from the first byte (since the 1 to signify two bytes is not included in key)
		key = (unsigned char)((buffer[i] & 0x0F) << 8) | (unsigned char)buffer[(i)+1];
		i += 1;
	}
	i += 1;
	// i is now at the value
	while (key != 0x00) {
		switch (key) {
			// read one unsigned byte
			case STYLE_TEXT_WINDOW_DIVIDER: case STYLE_TEXT_SCALE_MODE: case STYLE_TEXT_SIZE:
			{
				uint8_t byteValue = (unsigned char)(buffer[i]);
				styles[key] = (uint8_t)byteValue;
				i += 1;
			}
			break;
			// read two unsigned bytes
			case STYLE_COMPONENT_BORDER_WIDTH:
			{
				uint16_t doubleByteValue = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
				styles[key] = doubleByteValue;
				i += 2;
			}
			break;
			// read colors
			case STYLE_TEXT_COLOR: case STYLE_COMPONENT_BORDER_COLOR: case STYLE_COMPONENT_BACKGROUND_COLOR:
			{
				unsigned char r = (unsigned char)buffer[i];
				unsigned char g = (unsigned char)buffer[i + 1];
				unsigned char b = (unsigned char)buffer[i + 2];
				styles[key] = wxColour(r, g, b);
				i += 3;
			}
			break;
			// read strings
			case STYLE_TEXT_FONT_NAME: case STYLE_TEXT_FONT_FAMILY:
			{
				//TODO read string, possibly without another while loop?
			}
			break;
			// check if it simply exists
			case STYLE_TEXT_BOLD: case STYLE_TEXT_ITALICS: case STYLE_TEXT_UNDERLINE: case STYLE_TEXT_STRIKETHROUGH: case STYLE_TEXT_SUPERSCRIPT: case STYLE_TEXT_SUBSCRIPT:
			{
				styles[key] = true;
			}
			break;
		}
		// reread the next key like above. TODO: refractor
		key = (unsigned char)buffer[i];
		if ((key & 0x80) > 0x0) {
			key = (unsigned char)((buffer[i] & 0x7F) << 8) | (unsigned char)buffer[(i)+1];
			i += 1;
		}
		i += 1;
	}

	struct Style* style = new Style;
	style->chunkType = styleIDs.first;
	style->chunkID = styleIDs.second;
	style->styles = styles;

	*index = i;
	return std::make_pair(style->chunkID, style);
}

struct ImageActionPosition* readImageActionPosition(char* buffer, int* index) {
	int i = *index;

	struct ImageActionPosition* pos = new ImageActionPosition;
	pos->x = (unsigned char)buffer[(i)];
	pos->y = (unsigned char)buffer[(i)+1];
	pos->w = (unsigned char)buffer[(i)+2];
	pos->h = (unsigned char)buffer[(i)+3];

	i += 4;
	std::pair<uint8_t, uint32_t> actionID = readChunkTypeAndID(buffer, &i);
	pos->actionID = actionID.second;
	*index = i;
	return pos;
}

std::string readString(char* buffer, int* index) {
	int i = *index;

	std::string returnString;

	for (; buffer[i] != '\0'; i++) {
		returnString += buffer[i];
	}
	i += 1; // skip null byte for value

	*index = i;
	return returnString;
}