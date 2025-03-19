#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "types.h"
#include "fileManager.h"
#include "CGMString.h"
#include "multithreading.h"
#include "hashSet.h"
#include "config.h"

// --- CUSTOM DEFINES ---

#define BIT(n)                  (1U << (n))
#define SET_BIT(var, n)         ((var) |= BIT(n))
#define CLEAR_BIT(var, n)       ((var) &= ~BIT(n))

#ifdef PYTHON_PATH
    #define SCRIPT_EXEC PYTHON_PATH " " FILTER_SCRIPT
#else
    #define SCRIPT_EXEC "python3 " FILTER_SCRIPT
#endif

// --- VARIABLES ---

static char input[16];                          // 4x4 matrix
static u8 max_word_length;                      // Maximum word length
static String_t word;                           // String to store words as they are processed
static u16 visit_mask = 0;                      // 16 bits for 16 characters
static FILE* raw_words;                         // File to store all of the unfiltered words
static u32 word_count = 0;                      // Number of words found
static Set hash_set;                            // Set to store word hashes for duplicate checking
static char repeat_letters[8 + 1];              // Keeps track of letters that are duplicates

// --- PROCESS DESCRIPTION ---

// 1. Create a char array from the input matrix.
// 2. Create every possible word from the matrix.
// 3. Check for duplicates and remove.
// 4. Use python script to check word frequency
// 5. Split filtered words between all threads
// 6. Perform web requests to check if the words are real

// --- STATIC ASSERTS ---
static_assert(THREAD_COUNT <= (u16)-1U, "THREAD_COUNT too large.");

// --- PROGRAM CODE ---

/**
 * Write the word in the string buffer to the raw words file.
 */
void renderWord()
{
    u64 hash = hashString(StringCStr(&word));       // Generate hahs for the word
    if(SetContains(&hash_set, hash)) return;        // If the hash already exists, return
    SetInsert(&hash_set, hash);                     // Else, add hash to set and continue

    word_count++;
    fputs(StringCStr(&word), raw_words);
    fputc('\n', raw_words);
}

/**
 * Split filtered words between separate files for multi-threading
 */
void splitWords(FILE* src)
{
    printf("Splitting words: ");
    int ch;

    while((ch = fgetc(src)) != EOF)
    {
        if(ch == '\n')
        {
            ThreadsAddWord(StringCStr(&word));
            word.index = 0;
        }
        else
            StringAppend(&word, ch);
    }
    printf("split words successfully.\n");
}

void iterate(u8 x, u8 y, u8 len)
{
    SET_BIT(visit_mask, y * 4 + x);         // Mark current location as visited
    StringAppend(&word, input[y * 4 + x]);  // Add latest character to the word buffer

    if(len == 1) renderWord();
    else
    {
        for(i8 dy = y - 1; dy < y + 2; dy++)
        {
            for(i8 dx = x - 1; dx < x + 2; dx++)
            {
                if(dx == x && dy == y)              continue;
                if(dx < 0 || dx > 3)                continue;
                if(dy < 0 || dy > 3)                continue;
                if(visit_mask & BIT(dy * 4 + dx))   continue;

                iterate(dx, dy, len - 1);
            }
        }

    }

    CLEAR_BIT(visit_mask, y * 4 + x);
    word.index--;
}

void generateWords(void)
{
    printf("Generating words: ");
    for(u8 len = 4; len <= max_word_length; len++)
    {
        for(u8 y = 0; y < 4; y++)
        {
            for(u8 x = 0; x < 4; x++)
            {
                iterate(x, y, len);
                visit_mask = 0;                 // Mark all locations as not visited
                word.index = 0;                 // Clear output string
            }
        }

        SetClear(&hash_set);
    }
    printf("found %u words.\n", word_count);
}

int main(void)
{
    readGrid(input, repeat_letters);            // Read and parse grid into input array
    readConfig(&max_word_length);               // Read in the config for the program
    
    StringInit(&word, max_word_length);         // Initialise the global string buffer
    memset(repeat_letters, '\0', sizeof(repeat_letters));
    SetInit(&hash_set, 4096);


    raw_words = wopen("raw_words");             // Open raw words file
    generateWords();                            // Generate all words
    fclose(raw_words);                          // Close the raw words file

    printf("Running %s: ", SCRIPT_EXEC);
    fflush(stdout);
    int exit_code = system(SCRIPT_EXEC);        // Call the python script to perform filtering
    printf("script exited with code %i.\n", exit_code);
    fremove("raw_words");                       // Cleanup raw words file

    if(exit_code) return EXIT_FAILURE;

    ThreadsInit(max_word_length);               // Initialise thread contexts for multi-threading
    
    FILE* words = ropen("filtered_words");      // Open the filter words file generated by the python script
    splitWords(words);                          // Split words between check files
    fclose(words);                              // Close filtered words file
    fremove("filtered_words");                  // Cleanup filtered words file

    FILE* outfile = wopen("output.txt");        // Open the output file
    ThreadsSetOutFile(outfile);                 // Set the output file for the threads

    ThreadsResetFiles();                        // Reset all file streams to the beginning
    ThreadsBegin();                             // Create all the threads
    ThreadsJoin();                              // Await completion of all threads

    ThreadsCleanup();                           // Clean up thread contexts
    fclose(outfile);                            // Close output file
    StringFree(&word);                          // Free word buffer resources
    SetCleanup(&hash_set);                      // Clean up set resources
}
