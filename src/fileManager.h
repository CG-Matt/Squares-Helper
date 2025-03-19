#pragma once

#include <stdio.h>
#include "types.h"

// --- FUNCTIONS ---

/**
 * Open a file for reading.
 * Prints error message and exits
 * if it fails to open the file.
 * @param path The path to the file to open
 * @return The opened file ptr
 */
FILE* ropen(const char* path);

/**
 * Open a file for writing.
 * Prints error message and exits
 * if it fails to open the file.
 * @param path The path to the file to open
 * @return The opened file ptr
 */
FILE* wopen(const char* path);

/**
 * Opens the grid file,
 * reads its contents into the dest buffer,
 * and closes the file.
 * Prints error message and exits
 * if it fails to open the file,
 * or the input is invalid.
 * @param dest Destination for grid data
 */
void readGrid(char dest[16], char repeat_dest[9]);

/**
 * Opens the config file,
 * reads its contents, returns them,
 * and closes the file.
 * Prints error message and exits
 * if it fails to open the file,
 * or the input is invalid
 */
void readConfig(u8* max_word_length);

/**
 * Remove a file
 * Prints error message and exits
 * if it fails to remove the file.
 * @param filename Name of the file to remove
 */
void fremove(const char* filename);
