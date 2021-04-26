#include "format.h"
#include "reader.h"

// https://stackoverflow.com/questions/21737906/how-to-read-write-utf8-text-files-in-c/21745211

// class to read file format
struct InteractiveContent readFile(char* filename){
    printf("reading %s ", filename);

    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    int i;
    
    pFile = fopen ( filename , "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    
    buffer = (char *)malloc(lSize * sizeof(char)); // Enough memory for the file
    fread(buffer, lSize, 1, pFile); // Read in the entire file
    fclose(pFile); // Close the file

    printf("Size: %ld %ld\n", lSize, sizeof(buffer));
    // for(i = 0; i < lSize; i++) {
    //    fread(buffer+i, 1, 1, pFile);
    //    printf("%c\n", buffer);
    // }

    for(i = 0; i < lSize; i++){
       printf("%d\n", buffer[i]);
    }

    i = 0;

    struct Header *header = malloc (sizeof (struct Header));
    // read magic num
    i += 4;
    // read file version
    unsigned short version = buffer[i] << 8 | (buffer[i+1]);
    header->version = version;
    printf("Ver: %d %d %d\n", version,buffer[i], buffer[i+1]);
    i += 2;
    short startContainerId = buffer[i] << 8 | (buffer[i+1]);
    header->startContainer = startContainerId;
    i += 2;

    int authorI = 0;
    for(; buffer[i] != '\0' && authorI < 255; i++,authorI++){
        header->author[authorI] = buffer[i];
        printf("(%c)", buffer[i]);
    }
    header->author[authorI+1] = '\0';

    // i = the byte number of the null of the author
    // from here on out, its all just chunks
    i += 1;
    unsigned char chunkType = (buffer[i] >> 5) & 0x7; // get 3 leftmost
    unsigned short id = (((buffer[i]) & 0x1F) << 8) | (unsigned char)buffer[i+1]; // get 5 rightmost from first byte and add to second byte

    printf("\n%d %d %d\n", buffer[i], chunkType, id);
    // struct Header *header = malloc (sizeof (struct Header));
    switch (chunkType)
    {
    case 1:
        // container
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
}

struct Layout* readLayout(char* buffer, int *index){
    int i = *index;
    
    uint16_t ID = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
    i += 2; // now i is at numElements
    uint8_t numberElements = buffer[i];
    i += 1;

    // attempts to read the layout from the given starting position
    struct Layout *layout = malloc (sizeof (struct Layout)+numberElements*sizeof(struct elementPosition*));
    layout->chunk.ID = ID;

    printf("Reading layout. Total ID: %d Elements: %d\n", layout->chunk.ID, numberElements);
    // layout->positions = (struct elementPosition*) malloc(sizeof(struct elementPosition)*numberElements);

    int eleI;
    for(eleI = 0; eleI < numberElements; eleI++){
        struct elementPosition *pos = malloc (sizeof (struct elementPosition));
        pos->x = (buffer[i] << 8) | (unsigned char)buffer[(i)+1];
        pos->y = (buffer[i+2] << 8) | (unsigned char)buffer[(i)+3];
        pos->w = (buffer[i+4] << 8) | (unsigned char)buffer[(i)+5];
        pos->h = (buffer[i+6] << 8) | (unsigned char)buffer[(i)+7];
        i += 4;
        printf("Read position %d %d %d %d\n", pos->x, pos->y, pos->w, pos->h);
        // now handle Infinite byte

        // handle infinite position if infinite element
        layout->positions[eleI] = pos;
    }


    *index = i;
    return layout;
}