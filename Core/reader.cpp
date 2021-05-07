#include "reader.h"
#include <iostream> 
// https://stackoverflow.com/questions/21737906/how-to-read-write-utf8-text-files-in-c/21745211

// class to read file format
struct InteractiveContent* readFile(const char* filename){
    printf("reading %s ", filename);

    FILE * pFile;
    size_t lSize = 0;
    char * buffer;
    size_t result;
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
        unsigned char chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
        unsigned short id = (((buffer[i]) & 0x1F) << 8) | (unsigned char)buffer[i+1]; // get 5 rightmost from first byte and add to second byte
        char d = buffer[i];
        printf("\nAt byte %d. Reading chunk of type: %d. ID: %d\n", i, chunkType, id);
        // struct Header *header = malloc (sizeof (struct Header));
        switch (chunkType)
        {
        case 0x1:
        {
            // container
            std::pair<uint16_t, struct Container*> newContainer = readContainer(buffer, &i);
            ic->containers.insert(newContainer);
        }
            break;
        case 0x2: {
            // content
            std::pair<uint16_t, struct Content*> newContent = readContent(buffer, &i);
            ic->content.insert(newContent);
        }
            break;
        case 0x3: {
            // layout
            std::pair<uint16_t, struct Layout*> newLayout = readLayout(buffer, &i);
            ic->layouts.insert(newLayout);
        }
            break;
        default:
            break;
        }
        printf("Finished reading chunk. Now at byte: %d/%ld\n", i, lSize);
    }

    return ic;
}

// attempts to read the layout from the given starting position
std::pair<uint16_t, struct Layout*> readLayout(char* buffer, int *index){
    int i = *index;
    
    uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    uint16_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at numElements
    uint8_t numberElements = buffer[i];
    i += 1;

    struct Layout* layout = new Layout;
    layout->chunkType = chunkType;
    layout->chunkID = ID;

    printf("Reading layout. Total ID: %d Elements: %d\n", layout->chunkID, numberElements);
    // layout->positions = (struct elementPosition*) malloc(sizeof(struct elementPosition)*numberElements);

    int eleI;
    for(eleI = 0; eleI < numberElements; eleI++){
        struct elementPosition* pos = new elementPosition;
        pos->x = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
        pos->y = (buffer[i+2] << 8) | (unsigned char)buffer[(i)+3];
        pos->w = (buffer[i+4] << 8) | (unsigned char)buffer[(i)+5];
        pos->h = (buffer[i+6] << 8) | (unsigned char)buffer[(i)+7];
        printf("Read position %d %d %d %d %d\n", i, pos->x, pos->y, pos->w, pos->h);
        i += 8;
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

            infPos->startx      = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
            infPos->starty      = (buffer[i+2] << 8) | (unsigned char)buffer[(i)+3];
            infPos->w           = (buffer[i+4] << 8) | (unsigned char)buffer[(i)+5];
            infPos->h           = (buffer[i+6] << 8) | (unsigned char)buffer[(i)+7];
            infPos->padding     = (buffer[i+8] << 8) | (unsigned char)buffer[(i)+9];

            i += 10;
            printf("Read infinite position %d %d %d %d %d\n", i, infPos->startx, infPos->starty, infPos->w, infPos->h);
            pos->infPos = infPos;
        }else{
            pos->infPos = nullptr; // TODO needed?
        }
        layout->positions.push_back(pos);
    }


    *index = i;
    return std::make_pair(ID, layout);
}

std::pair<uint16_t, struct Container*> readContainer(char* buffer, int *index){
    int i = *index;
    
    struct Container* container = new Container;

    uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    uint16_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at layout ID
    uint16_t layoutID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];

    container->chunkType = chunkType;
    container->chunkID = ID;
    container->layoutID = layoutID;

    i += 2; // now i is at the first byte of the ID or end code

    printf("Reading container. Byte: %d. Container ID: %d. Layout ID: %d.\n", i, ID, layoutID);
    // i must always point to a byte that is either the start of an ID, or a End code
    while(buffer[i] != 0x0){ // 0x0 is byte code for end of container chunk
        // init array to hold one
        std::vector<uint16_t> subelements; // length of this is 1, unless its in an infinite container
        while((unsigned char)buffer[i] != 0xFF){
            uint16_t eleID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
            subelements.push_back(eleID);
            i += 2;
        }
        // end container hit, skip that byte
        i += 1;

        container->elementIDs.push_back(subelements);
    }
    // end chunk hit, skip that byte
    i += 1;

    *index = i;
    return std::make_pair(ID, container);
}

std::pair<uint16_t, struct Content*> readContent(char* buffer, int* index) {
    int i = *index;

    struct Content* content = new Content;

    uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    uint16_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at content type

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

    content->chunkType = chunkType;
    content->chunkID = ID;
    content->length = contentLength;
    content->type = contentType;

    *index = i;
    return std::make_pair(ID, content);
}