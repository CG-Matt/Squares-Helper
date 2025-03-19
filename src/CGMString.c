#include <stdlib.h>
#include "CGMString.h"

void StringInit(String_t* str)
{
    str->size = 0;
    str->capacity = 16;
    str->buffer = malloc(sizeof(char) * (str->capacity + 1));
    str->buffer[str->capacity] = '\0';
}

char* StringCStr(String_t* str)
{
    str->buffer[str->size] = '\0';
    return str->buffer;
}

void StringAppend(String_t* str, char c)
{
    if(str->size >= str->capacity)
    {
        str->capacity *= 2;
        str->buffer = realloc(str->buffer, sizeof(char) * str->capacity);
    }
    str->buffer[str->size++] = c;
}

void StringFree(String_t* str)
{
    free(str->buffer);
}

void StringClear(String_t* str) { str->size = 0; }

void StringPop(String_t* str) { str->size--; }

void StringReserve(String_t* str, usize size)
{
    if(size > str->capacity)
    {
        str->capacity = size;
        str->buffer = realloc(str->buffer, sizeof(char) * str->capacity);
    }
}