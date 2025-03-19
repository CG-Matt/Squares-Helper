#pragma once

#include "types.h"

// --- STRUCTS ---

typedef struct {
    char* buffer;
    usize size;
    usize capacity;
} String_t;

// --- FUNCTIONS ---

/**
 * Initialise a string with the requested buffer size
 */
void StringInit(String_t* str);

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

void StringPop(String_t* str);

void StringClear(String_t* str);

void StringReserve(String_t* str, usize size);