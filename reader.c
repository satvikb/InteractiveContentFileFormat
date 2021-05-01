#include "format.h"
#include "reader.h"

// https://stackoverflow.com/questions/21737906/how-to-read-write-utf8-text-files-in-c/21745211

// class to read file format
struct InteractiveContent readFile(const char* filename){
    printf("reading %s ", filename);

    FILE * pFile;
    size_t lSize = 0;
    char * buffer;
    size_t result;
    int i;
    
    fopen_s(&pFile, filename , "rb" );
    if (pFile==NULL) {
        fputs ("File error",stderr); 
        return;
    }

    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    
    buffer = (char *)malloc(lSize * sizeof(char)); // Enough memory for the file
    fread(buffer, lSize, 1, pFile); // Read in the entire file
    fclose(pFile); // Close the file

    printf("Size: %ld BYTES\n", lSize);

    i = 0;

    struct Header *header = (struct Header *)malloc (sizeof (struct Header));
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

    printf("Author: ");
    int authorI = 0;
    for(; buffer[i] != '\0' && authorI < 255; i++,authorI++){
        header->author[authorI] = buffer[i];
        printf("%c", buffer[i]);
    }
    header->author[authorI+1] = '\0';
    printf("\n");
    // i = the byte number of the null of the author
    // from here on out, its all just chunks
    i += 1;

    while(i < lSize){
        unsigned char chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
        unsigned short id = (((buffer[i]) & 0x1F) << 8) | (unsigned char)buffer[i+1]; // get 5 rightmost from first byte and add to second byte

        printf("\nAt byte %d. Reading chunk of type: %d. ID: %d\n", i, chunkType, id);
        // struct Header *header = malloc (sizeof (struct Header));
        switch (chunkType)
        {
        case 1:
            // container
            readContainer(buffer, &i);
            break;
        case 2:
            // content
            break;
        case 3:
            // layout
            readLayout(buffer, &i);
            break;
        default:
            break;
        }
        printf("Finished reading chunk. Now at byte: %d/%ld\n", i, lSize);
    }
}

// attempts to read the layout from the given starting position
struct Layout* readLayout(char* buffer, int *index){
    int i = *index;
    
    uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    uint16_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at numElements
    uint8_t numberElements = buffer[i];
    i += 1;

    struct Layout *layout = (struct Layout *)malloc (sizeof (struct Layout)+numberElements*sizeof(struct elementPosition*));
    layout->chunk.type = chunkType;
    layout->chunk.ID = ID;

    printf("Reading layout. Total ID: %d Elements: %d\n", layout->chunk.ID, numberElements);
    // layout->positions = (struct elementPosition*) malloc(sizeof(struct elementPosition)*numberElements);

    int eleI;
    for(eleI = 0; eleI < numberElements; eleI++){
        struct elementPosition *pos = (struct elementPosition *)malloc (sizeof (struct elementPosition));
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
            i += 1;
        }
        layout->positions[eleI] = pos;
    }


    *index = i;
    return layout;
}

struct Container* readContainer(char* buffer, int *index){
    int i = *index;
    
    uint8_t chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    uint16_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at layout ID
    uint16_t layoutID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at the first byte of the ID or end code
    int numElementsInChunk = 0;
    struct elementID** elementIDs = (struct elementID**)calloc(0, sizeof(struct elementID*));

    printf("Reading container. Byte: %d. Container ID: %d. Layout ID: %d.\n", i, ID, layoutID);
    // i must always point to a byte that is either the start of an ID, or a End code
    while(buffer[i] != 0){
        // init array to hold one
        uint16_t* containerElementIDs = (uint16_t*)calloc(0, sizeof(uint16_t));
        uint16_t numberElements = 0;
        while((unsigned char)buffer[i] != 0xFF){
            uint16_t eleID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];

            numberElements += 1;

            containerElementIDs = (uint16_t*)realloc(containerElementIDs, numberElements*sizeof(uint16_t));
            containerElementIDs[numberElements-1] = eleID;
            i += 2;
        }
        // end container hit, skip that byte
        i += 1;

        struct elementID *currentElementIDs = (struct elementID *)malloc (sizeof (struct elementID)+numberElements*sizeof(uint16_t));
        currentElementIDs->numberElements = numberElements;
        // copy the IDs over?
        for(int j = 0; j < numberElements; j++){
            currentElementIDs->IDs[j] = containerElementIDs[j];
            printf("ID: %d\n", containerElementIDs[j]);
        }

        numElementsInChunk += 1;
        elementIDs = (struct elementID**)realloc(elementIDs, numElementsInChunk*sizeof(struct elementID*));
        elementIDs[numElementsInChunk-1] = currentElementIDs;
    }
    // end chunk hit, skip that byte
    i += 1;
    struct Container *container = (struct Container *)malloc (sizeof (struct Container)+numElementsInChunk*sizeof(struct elementID*));
    container->chunk.type = chunkType;
    container->chunk.ID = ID;
    for(int j = 0; j < numElementsInChunk; j++){
        container->elementIDs[j] = elementIDs[j];
        printf("Layout container done. %d\n", j);
    }

    *index = i;
    return container;
}