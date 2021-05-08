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

// Define constants
#define CHUNK_CONTAINER 0x1
#define CHUNK_CONTENT 0x2
#define CHUNK_LAYOUT 0x3
#define CHUNK_ACTION 0x4
#define CHUNK_STYLING 0x5
#define CHUNK_EXTENDED 0x6

#define ACTION_LINK 0x1
#define ACTION_REPLACEMENT 0x2

#define STYLE_COMPONENT_BORDER_WIDTH 0x01
#define STYLE_COMPONENT_BORDER_COLOR 0x02
#define STYLE_COMPONENT_BACKGROUND_COLOR 0x03
#define STYLE_TEXT_FONT_NAME 0x10
#define STYLE_TEXT_FONT_FAMILY 0x11
#define STYLE_TEXT_COLOR 0x12
#define STYLE_TEXT_BOLD 0x13
#define STYLE_TEXT_ITALICS 0x14
#define STYLE_TEXT_UNDERLINE 0x15
#define STYLE_TEXT_STRIKETHROUGH 0x16
#define STYLE_TEXT_SUPERSCRIPT 0x17
#define STYLE_TEXT_SUBSCRIPT 0x18

struct Header;
struct Layout;
struct Container;
struct Action;
struct Content;

struct InteractiveContent {
    struct Header* header;
    // Maps ID (with both Chunk Type and the actual ID to the respective pointer type
    std::map<uint16_t, struct Layout*> layouts;
    std::map<uint16_t, struct Container*> containers;
    std::map<uint16_t, struct Action*> actions;
    std::map<uint16_t, struct Content*> content;
    std::map<uint16_t, struct Style*> styles;
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
    unsigned short version;
    uint16_t startContainer;
    std::string author;
};

// the first 3 bits are 011 for layout, the last 13 bits is the actual id
struct Chunk {
    uint8_t chunkType;
    uint16_t chunkID;
};

struct Layout : Chunk {
    // the number of elements in this layout
    uint16_t elementCount;
    // store the positions of each of the elements
    std::vector<struct elementPosition*> positions;
};

struct Container : Chunk {
    // the first 3 bits are 011 for layout, the last 13 bits is the actual layout id
    uint16_t layoutID;
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
    std::vector<std::vector<uint16_t>> elementIDs;
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
    uint16_t containerID;
    // last 3 bits define how many levels of the containers should be shows (if containers embed containers, etc.)
    uint8_t display;
};

struct Replacement : Action {
    // The element to replace. Must currently be visible.
    uint16_t replaceID;
    // This element will replace the above ID
    uint16_t replaceWithID;
};

struct Style : Chunk {
    // TODO uint16 for extended key range
    std::map<uint8_t, std::any> styles;
};

// TODO: pos struct and inherit from it?
struct elementPosition {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t styleID;
    uint8_t inf;
    struct infiniteElementPosition* infPos;
};

struct infiniteElementPosition {
    uint16_t startx;
    uint16_t starty;
    uint16_t w;
    uint16_t h;
    uint16_t padding;
};

struct InteractiveContent* readFile(const char* filename);
std::pair<uint16_t, struct Layout*> readLayout(char* buffer, int *index);
std::pair<uint16_t, struct Container*> readContainer(char* buffer, int *index);
std::pair<uint16_t, struct Content*> readContent(char* buffer, int* index);
std::pair<uint16_t, struct Action*> readAction(char* buffer, int* index);
std::pair<uint16_t, struct Style*> readStyle(char* buffer, int* index);
