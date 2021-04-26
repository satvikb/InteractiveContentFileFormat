#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct InteractiveContent readFile(char* filename);
struct Layout* readLayout(char* buffer, int *i);