#pragma once

#define _CRT_SECURE_NO_WARNINGS // supress the string warnings that started coming up?
#include <wx/colour.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <any>
#include <vector>
#include <cpr/response.h>

// TODO dirty way to include logger everywhere

#include "Logger.h"
// Define constants
#define MAGIC_NUMBER_LENGTH 12
#define CHUNK_CONTAINER 0x1
#define CHUNK_CONTENT 0x2
#define CHUNK_LAYOUT 0x3
#define CHUNK_ACTION 0x4
#define CHUNK_STYLING 0x5
#define CHUNK_HEADER 0x6 // 110
#define CHUNK_EXTENDED 0x7 // 111
// --- Begin Extended Range Chunk Types
//#define CHUNK_FUTURE_TYPE 0x8 // 01000

#define CONTENT_TEXT 0x1
#define CONTENT_IMAGE 0x2
#define CONTENT_BITMAP 0x3
#define CONTENT_BUTTON 0x4
#define CONTENT_WEB 0x23 // 35 in decimal, external content type
#define CONTENT_STREAMED 0x24

#define IMAGE_TYPE 0x0
#define IMAGE_DATA_LENGTH 0x1
#define IMAGE_ACTION_START 0x2
#define IMAGE_USE_URL 0x3
#define IMAGE_BEGIN_DATA 0xFF

#define IMAGE_TYPE_IC_IMAGE 0x1
#define IMAGE_TYPE_PNG 0x2
#define IMAGE_TYPE_JPEG 0x3
#define IMAGE_TYPE_GIF 0x4

#define ACTION_LINK 0x1
#define ACTION_SWAP 0x2
#define ACTION_REPLACE_WITH_ELEMENT 0x3
#define ACTION_DOWNLOAD_CHUNKS 0x4
#define ACTION_EXECUTE_COMPOSITE 0x5

#define STYLE_COMPONENT_BORDER_WIDTH 0x01
#define STYLE_COMPONENT_BORDER_COLOR 0x02
#define STYLE_COMPONENT_BACKGROUND_COLOR 0x03

#define STYLE_TEXT_FONT_NAME 0x3A
#define STYLE_TEXT_FONT_FAMILY 0x3B
#define STYLE_TEXT_COLOR 0x30
#define STYLE_TEXT_BOLD 0x40
#define STYLE_TEXT_ITALICS 0x41
#define STYLE_TEXT_UNDERLINE 0x42
#define STYLE_TEXT_STRIKETHROUGH 0x43
#define STYLE_TEXT_SUPERSCRIPT 0x44
#define STYLE_TEXT_SUBSCRIPT 0x45
#define STYLE_TEXT_WINDOW_DIVIDER 0x50
#define STYLE_TEXT_SCALE_MODE 0x51
#define STYLE_TEXT_SIZE 0x52

#define TEXT_SCALE_MODE_POINTS 0x0
#define TEXT_SCALE_MODE_WINDOW_WIDTH 0x01
#define TEXT_SCALE_MODE_WINDOW_HEIGHT 0x2

#define BITMAP_LINE 0x1
#define BITMAP_RECTANGLE 0x2
#define BITMAP_CIRCLE 0x3
#define BITMAP_CIRCLE_RADIUS_WIDTH 0x1
#define BITMAP_CIRCLE_RADIUS_HEIGHT 0x2

#define BUTTON_DEFAULT_STYLE 0x1
#define BUTTON_HOVER_STYLE 0x2
#define BUTTON_CLICK_STYLE 0x3

#define HEADER_ATTRIBUTE_AUTO_UPDATE_CURRENT_VERSION "file_version"
#define HEADER_ATTRIBUTE_AUTO_UPDATE_VERSION_URL "version_url"
#define HEADER_ATTRIBUTE_AUTO_UPDATE_FILE_URL "update_url"
#define HEADER_FORCE_AUTO_UPDATE_VALUE 0xFFFF

#define HEADER_ATTRIBUTE_WINDOW_ASPECT_RATIO "win_aspect"

struct Header;
struct Layout;
struct Container;
struct Action;
struct Content;

struct InteractiveContent {
    // There can only be one latest header
    struct Header* header;
    // Maps ID (with both Chunk Type and the actual ID to the respective pointer type
    std::map<uint32_t, struct Layout*> layouts;
    std::map<uint32_t, struct Container*> containers;
    std::map<uint32_t, struct Action*> actions;
    std::map<uint32_t, struct Content*> content;
    std::map<uint32_t, struct Style*> styles;
};

//// TODO make substructs to hold array of each of these
//struct InteractiveContent {
//    struct Header* header;
//    // each of these array pointers should be ended with null byte?
//    struct Layout* layouts;
//    struct Container* containers;
//    struct Link* links;
//    struct Content* content;
//};

struct Header {
    unsigned short specificationVersionMajor;
    unsigned char specificationVersionMinor;
    unsigned char specificationVersionPatch;

    uint32_t startContainer;
    std::map<std::string, std::string> metadata;
    // Recognized attributes
    float windowAspectRatio;
    // Auto update settings
    uint32_t fileVersion;
    std::string updateVersionURL;
    std::string updateFileURL;
};

// the first 3 bits are 011 for layout, the last 13 bits is the actual id
struct Chunk {
    uint8_t chunkType;
    uint32_t chunkID;
};

struct Layout : Chunk {
    // the number of elements in this layout
    uint16_t elementCount;
    // store the positions of each of the elements
    std::vector<struct elementPosition*> positions;
};

struct Container : Chunk {
    // the first 3(/8) bits are 011(or extended code) for layout, the last 13 (/24) bits is the actual layout id
    uint32_t layoutID;
    // storage of actual IDs for elements
    /*
        This is a 2D array.
        First dimention is array of pointers, where each pointer leads to a elementID struct.
        Second dimention is the element(s) within this element i
            0 elements: the ID array has just 11111111, meaning to skip this element
            1 element: the ID array has 2 elements, first is the actual container/content, second is 11111111
            n > 1 elements: the ID array has n+1 elements, the first n is list of container/contnts, n+1 is 1111111. ERROR if layout.positions[i].inf == false
    */
    // a vector of pointers to elementIDs
    std::vector<Chunk> elementIDs;
};

struct Content : Chunk{
    // type of content (text, img, etc)
    uint8_t type;
    uint32_t length;
    std::vector<uint8_t> data; // of size = length
};

struct Action : Chunk {
    virtual ~Action() = default; // required for downcasting?
    uint8_t actionType;
};

struct Link : Action {
    // links can only link to containers
    uint32_t containerID;
    // last 3 bits define how many levels of the containers should be shows (if containers embed containers, etc.)
    uint8_t display;
};

struct Swap : Action {
    // The element to replace. Must currently be visible.
    uint32_t replaceID;
    // This element will replace the above ID
    uint32_t replaceWithID;
};

struct ReplaceWithElement : Action {
    // The container to target. Must currently be visible.
    uint32_t containerID;
    // The index of the element inside this container to replace
    uint8_t index;
    // This content will replace the above element at the index
    uint32_t replaceWithContentID;
};

struct DownloadChunks : Action {
    std::string url;
};

struct ExecuteComposite : Action {
    std::vector<uint32_t> actionsToExecute;
};

struct Style : Chunk {
    // TODO uint16 for extended key range
    std::map<uint8_t, std::any> styles;
};

// TODO: pos struct and inherit from it?
struct elementPosition {
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    uint32_t styleID;
};

struct ImageActionPosition {
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    uint32_t actionID;
};

bool readFile(struct InteractiveContent* ic, const char* filename);
cpr::Response downloadFileData(const char* url);
bool streamFile(struct InteractiveContent* ic, const char* url);

bool readFileData(struct InteractiveContent* ic, char* buffer, size_t numberBytes);

struct Header* readHeader(char* buffer, int* index);
std::pair<uint32_t, struct Layout*> readLayout(char* buffer, int *index);
std::pair<uint32_t, struct Container*> readContainer(char* buffer, int *index);
std::pair<uint32_t, struct Content*> readContent(char* buffer, int* index);
std::pair<uint32_t, struct Action*> readAction(char* buffer, int* index);
std::pair<uint32_t, struct Style*> readStyle(char* buffer, int* index);

std::pair<uint8_t, uint32_t> readChunkTypeAndID(char* buffer, int* index);
uint32_t read32BitInt(char* buffer, int* index);
std::string readString(char* buffer, int* index);
struct ImageActionPosition* readImageActionPosition(char* buffer, int* index);