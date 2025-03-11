#pragma once

#include "types.h"

// --- STRUCTS ---

typedef struct SetNode SetNode;

typedef struct {
    SetNode** buckets;
    usize size;
    usize count;
} Set;

// --- FUNCTIONS ---

/**
 * Function used to hash the string for storing in a set
 */
u64 hashString(const char* str);

/**
 * Initialises required memory for the provided set
 */
void SetInit(Set* set, usize size);

/**
 * Frees the resources used by the set
 */
void SetCleanup(Set* set);

/**
 * Removes all elements from the set and frees their memory,
 * while keeping the set intact
 */
void SetClear(Set* set);

/**
 * Insert the provided element into the set
 */
void SetInsert(Set* set, u64 key);

/**
 * Check if the provided set contains the requested key
 */
int SetContains(Set* set, u64 key);