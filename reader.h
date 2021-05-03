#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>

struct Header;
struct Layout;
struct Container;
struct Link;
struct Content;

struct InteractiveContent {
    struct Header* header;
    // Maps ID (with both Chunk Type and the actual ID to the respective pointer type
    std::map<uint16_t, struct Layout*> layouts;
    std::map<uint16_t, struct Container*> containers;
    std::map<uint16_t, struct Link*> links;
    std::map<uint16_t, struct Content*> content;
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

struct Chunk {
    uint8_t type;
    uint16_t ID;
};

struct Layout {
    // the first 3 bits are 011 for layout, the last 13 bits is the actual layout id
    struct Chunk chunk;
    // the number of elements in this layout
    uint16_t elementCount;
    // store the positions of each of the elements
    std::vector<struct elementPosition*> positions;
};

struct Container {
    // the first 3 bits are 001 for container, the last 13 bits is the actual id
    struct Chunk chunk;
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

struct Content {
    struct Chunk chunk;
    // type of content (text, img, etc)
    uint8_t type;
    uint32_t length;
    std::vector<uint8_t> data; // of size = length
};

struct Link {
    struct Chunk chunk;
    uint16_t containerID;
    // last 3 bits define how many levels of the containers should be shows (if containers embed containers, etc.)
    uint8_t display;
};

// TODO, do we even need this struct?
//struct elementID {
//    uint16_t numberElements;
//    // each elementID has at least 1 value here: 111 11111 signifying end of array
//    uint16_t IDs[];
//};

struct elementPosition {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
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
std::pair<int, struct Layout*> readLayout(char* buffer, int *index);
std::pair<int, struct Container*> readContainer(char* buffer, int *index);
std::pair<int, struct Content*> readContent(char* buffer, int* index);
