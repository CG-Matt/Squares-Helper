#pragma once

#include <stdio.h>
#include "types.h"

// --- STRUCTS ---

typedef struct {
    FILE* input;
    u16 thread_id;
} ThreadContext;

// --- FUNCTIONS ---

/**
 * Initialises thread contexts.
 * Exits if any errors occur.
 * @param max_word_length Maximum length of a word to check
 */
void ThreadsInit(u8 max_word_length);

/**
 * Add a word to be processed to a thread
 */
void ThreadsAddWord(const char* str);

/**
 * Rewind the seek position to the beginning
 * of the files in all the threads.
 */
void ThreadsResetFiles();

/**
 * Set the output file for all the threads.
 */
void ThreadsSetOutFile(FILE* file);

/**
 * Start running all threads with the
 * previously provided function.
 */
void ThreadsBegin();

/**
 * Await for all threads to finish.
 */
void ThreadsJoin();

/**
 * Free all resources allocated during threads initialisation.
 */
void ThreadsCleanup();