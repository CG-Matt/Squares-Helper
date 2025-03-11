#pragma once

#include "types.h"

// --- STRUCTS ---

typedef struct {
    char* buffer;
    u8 index;
} String_t;

// --- FUNCTIONS ---

/**
 * Initialise a string with the requested buffer size
 */
void StringInit(String_t* str, u8 buffer_size);

/**
 * Free the resources used by the string
 */
void StringFree(String_t* str);

/**
 * Adds a `\0` terminator to the buffer
 * and returns a pointer to the internal buffer.
 */
char* StringCStr(String_t* str);

/**
 * Append `c` to the string and advanced
 * the internal buffer index.
 */
void StringAppend(String_t* str, char c);