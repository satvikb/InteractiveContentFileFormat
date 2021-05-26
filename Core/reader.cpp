#include "reader.h"
#include <iostream> 
// https://stackoverflow.com/questions/21737906/how-to-read-write-utf8-text-files-in-c/21745211

// class to read file format
struct InteractiveContent* readFile(const char* filename){
    printf("reading %s ", filename);

    FILE * pFile;
    size_t lSize = 0;
    char * buffer;
    int i;
    
    fopen_s(&pFile, filename , "rb" );
    if (pFile==NULL) {
        fputs ("File error",stderr); 
        return nullptr;
    }

    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    
    buffer = (char *)malloc(lSize * sizeof(char)); // Enough memory for the file
    fread(buffer, lSize, 1, pFile); // Read in the entire file
    fclose(pFile); // Close the file

    printf("Size: %ld BYTES\n", lSize);

    struct InteractiveContent* ic = new InteractiveContent;

    i = 0;

    struct Header *header = new Header;
    // read magic num
    i += 4;
    // read file version
    unsigned short version = buffer[i] << 8 | (buffer[i+1]);
    header->version = version;
    printf("Version: %d\n", version);
    i += 2;
    short startContainerId = buffer[i] << 8 | (buffer[i+1]);
    header->startContainer = startContainerId;
    i += 2;
    std::cout << "READING AUTHOR" << std::endl;
    printf("Author: ");
    //int authorI = 0;
    std::string author;
    for(; buffer[i] != '\0'; i++){
        //header->author[authorI] = buffer[i];
        author += (buffer[i]);
        printf("%c\n", buffer[i]);
    }
    //header->author[authorI+1] = '\0';
    header->author = author;
    //std::string author;
    //const char* authorStart = &buffer[i];
    //strcpy(author, authorStart);
    //printf("\n");
    // i = the byte number of the null of the author
    // from here on out, its all just chunks
    i += 1;

    ic->header = header;

    while(i < lSize){
        std::pair<uint8_t, uint32_t> chunkData = readChunkTypeAndID(buffer, &i);
        i -= 2;
        unsigned char chunkType = chunkData.first;
        //unsigned uint32_t id = (((buffer[i]) & 0x1F) << 8) | (unsigned char)buffer[i+1]; // get 5 rightmost from first byte and add to second byte
        char d = buffer[i];
        //printf("\nAt byte %d. Reading chunk of type: %d. ID: %d\n", i, chunkType, id);
        // struct Header *header = malloc (sizeof (struct Header));
        switch (chunkType)
        {
        case CHUNK_CONTAINER:
        {
            // container
            std::pair<uint16_t, struct Container*> newContainer = readContainer(buffer, &i);
            ic->containers.insert(newContainer);
        }
            break;
        case CHUNK_CONTENT: {
            // content (2)
            std::pair<uint16_t, struct Content*> newContent = readContent(buffer, &i);
            ic->content.insert(newContent);
        }
            break;
        case CHUNK_LAYOUT: {
            // layout
            std::pair<uint16_t, struct Layout*> newLayout = readLayout(buffer, &i);
            ic->layouts.insert(newLayout);
        }
            break;
        case CHUNK_ACTION: {
            // action
            std::pair<uint16_t, struct Action*> newAction = readAction(buffer, &i);
            ic->actions.insert(newAction);
        }
            break;
        case CHUNK_STYLING: {
            std::pair<uint16_t, struct Style*> newStyle = readStyle(buffer, &i);
            ic->styles.insert(newStyle);
        }
            break;
        default:
            break;
        }
        printf("Finished reading chunk. Now at byte: %d/%ld\n", i, lSize);
    }

    return ic;
}

std::pair<uint8_t, uint32_t> readChunkTypeAndID(char* buffer, int* index) {
    int i = *index;

    uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    uint32_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is after bytes

    *index = i;
    return std::make_pair(chunkType, ID);
}

// attempts to read the layout from the given starting position
std::pair<uint32_t, struct Layout*> readLayout(char* buffer, int *index){
    int i = *index;
    
    std::pair<uint8_t, uint32_t> layoutIDs = readChunkTypeAndID(buffer, &i);
    struct Layout* layout = new Layout;
    layout->chunkType = layoutIDs.first;
    layout->chunkID = layoutIDs.second;
    uint8_t numberElements = buffer[i];
    i += 1;

    //layout->chunkType = chunkType;
    //layout->chunkID = ID;

    printf("Reading layout. Total ID: %d Elements: %d\n", layout->chunkID, numberElements);
    // layout->positions = (struct elementPosition*) malloc(sizeof(struct elementPosition)*numberElements);

    int eleI;
    for(eleI = 0; eleI < numberElements; eleI++){
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

        // now handle Infinite byte
        pos->inf = (unsigned char)buffer[i];
        unsigned char infinite = (buffer[i] >> 7) & 1;
        // handle infinite position if infinite element
        if(infinite == 1){
            printf("Infinite. Byte: %d. Value: %d\n", i, infinite);
            // TODO have bool in the elementPosition to keep track of paging, etc. ?
            
            // move onto infpos bytes
            i += 1;

            // handle infinite element position
            struct infiniteElementPosition *infPos = (struct infiniteElementPosition *)malloc (sizeof (struct infiniteElementPosition));

            infPos->startx      = (unsigned char)buffer[(i)];
            infPos->starty      = (unsigned char)buffer[(i)+1];
            infPos->w           = (unsigned char)buffer[(i)+2];
            infPos->h           = (unsigned char)buffer[(i)+3];
            infPos->padding     = (unsigned char)buffer[(i)+4];

            i += 5;
            printf("Read infinite position %d %d %d %d %d\n", i, infPos->startx, infPos->starty, infPos->w, infPos->h);
            pos->infPos = infPos;
        }else{
            i += 1; // skip infinite byte
            pos->infPos = nullptr; // TODO needed?
        }
        layout->positions.push_back(pos);
    }


    *index = i;
    return std::make_pair(layout->chunkID, layout);
}

std::pair<uint32_t, struct Container*> readContainer(char* buffer, int *index){
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
    while(buffer[i] != 0x0){ // 0x0 is byte code for end of container chunk
        // init array to hold one
        std::vector<Chunk> subelements; // length of this is 1, unless its in an infinite container
        while((unsigned char)buffer[i] != 0xFF){
            std::pair<uint8_t, uint32_t> chunkData = readChunkTypeAndID(buffer, &i);
            struct Chunk chunk = { chunkData.first, chunkData.second };
            subelements.push_back(chunk);
        }
        // end container hit, skip that byte
        i += 1;

        container->elementIDs.push_back(subelements);
    }
    // end chunk hit, skip that byte
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

    uint32_t contentLength = uint32_t  ((unsigned char)(buffer[i]) << 24 |
                            (unsigned char)(buffer[i+1]) << 16 |
                            (unsigned char)(buffer[i+2]) << 8 |
                            (unsigned char)(buffer[i+3]));
    i += 4;

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
    case ACTION_LINK: {
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
    case ACTION_SWAP: {
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
    case ACTION_REPLACE_WITH_CONTENT: {
        // 5 bytes total
        // 2 bytes - container id
        // 1 byte - index
        // 2 bytes - content id
        uint16_t containerID = readChunkTypeAndID(buffer, &i).second;
        uint8_t index = (unsigned char)buffer[i];
        i += 1;
        uint16_t contentID = readChunkTypeAndID(buffer, &i).second;
        struct ReplaceWithContent* replace = new ReplaceWithContent;
        replace->chunkType = actionIDs.first;
        replace->chunkID = actionIDs.second;
        replace->actionType = actionType;
        replace->containerID = containerID;
        replace->index = index;
        replace->replaceWithContentID = contentID;
        action = replace;
    }
    break;
    default: {

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

    uint8_t key = (unsigned char)buffer[i];
    i += 1;
    while (key != 0x00) {
        switch (key) {
        case STYLE_COMPONENT_BORDER_WIDTH: {
            uint16_t width = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
            styles[key] = width;
            i += 2;
        }
        break;
        case STYLE_COMPONENT_BORDER_COLOR: case STYLE_COMPONENT_BACKGROUND_COLOR: {
            unsigned char r = (unsigned char)buffer[i];
            unsigned char g = (unsigned char)buffer[i+1];
            unsigned char b = (unsigned char)buffer[i+2];
            styles[key] = wxColour(r, g, b);
            i += 3;
        }
        break;
        case STYLE_TEXT_FONT_NAME: {
            //TODO read string, possibly without another while loop?
        }
        break;
        case STYLE_TEXT_FONT_FAMILY: {
            //TODO read string, possibly without another while loop?
        }
        break;
        case STYLE_TEXT_BOLD: case STYLE_TEXT_ITALICS: case STYLE_TEXT_UNDERLINE: case STYLE_TEXT_STRIKETHROUGH: case STYLE_TEXT_SUPERSCRIPT: case STYLE_TEXT_SUBSCRIPT: {
            styles[key] = true;
        }
        break;
        }
        key = (unsigned char)buffer[i];
        i += 1;
    }

    struct Style* style = new Style;
    style->chunkType = styleIDs.first;
    style->chunkID = styleIDs.second;
    style->styles = styles;
    
    *index = i;
    return std::make_pair(style->chunkID, style);
}