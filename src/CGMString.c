#include <stdlib.h>
#include "CGMString.h"

void StringInit(String_t* str, u8 size)
{
    str->index = 0;
    str->buffer = malloc(sizeof(char) * (size + 1));
    str->buffer[size] = '\0';
}

char* StringCStr(String_t* str)
{
    str->buffer[str->index] = '\0';
    return str->buffer;
}

void StringAppend(String_t* str, char c)
{
    str->buffer[str->index++] = c;
}

void StringFree(String_t* str)
{
    free(str->buffer);
}