#include <string.h>
#include <ctype.h>
#include "fileManager.h"
#include "CGMString.h"
#include "types.h"

FILE* ropen(const char* path)
{
    FILE* f = fopen(path, "r");

    if(!f)
    {
        fprintf(stderr, "Err: Unable to open file '%s' for reading: ", path);
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    return f;
}

FILE* wopen(const char* path)
{
    FILE* f = fopen(path, "w");

    if(!f)
    {
        fprintf(stderr, "Err: Unable to open file '%s' for writing: ", path);
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    return f;
}

void readGrid(char dest[16], char repeat_dest[9])
{
    FILE* grid = ropen("GRID");             // Open input grid file
    char occurance_counter[16 + 1];
    memset(occurance_counter, '\0', sizeof(occurance_counter));
    u8 count = 0;
    u8 rci = 0;
    u8 oci = 0;
    while(count < 16)
    {
        int c = fgetc(grid);        // Get new character
        if(c == EOF) break;         // Check if end of file
        if(isspace(c)) continue;    // Check if whitespace
        if(!isalpha(c))             // Check if the character is valid
        {
            fprintf(stderr, "Err: Invalid character '%c' in input grid.\n", c);
            exit(EXIT_FAILURE);
        }
        if(isupper(c)) c += 0x20;   // Convert to lowercase

        if(strchr(occurance_counter, c))
        {
            if(!strchr(repeat_dest, c))
                repeat_dest[rci++] = c;
        }
        else
            occurance_counter[oci++] = c;

        dest[count++] = (char)c;
    }

    if(count != 16)
    {
        fprintf(stderr, "Err: GRID file too small, read: %u characters, expected 16.\n", count);
        exit(EXIT_FAILURE);
    }
    fclose(grid);                           // Close grid file as it is no longer needed
}

void readConfig(u8* mwl)
{
    FILE* config = ropen("settings.conf"); // Open the config file

    *mwl = 0;

    String_t word;
    StringInit(&word, 32);

    void* dest;
    int c;

    while((c = fgetc(config)) != EOF)
    {
        if(c == '=')
        {
            if(!strcmp(StringCStr(&word), "MAX_WORD_LENGTH"))
            {
                dest = mwl;
            }
            word.index = 0;
            continue;
        }
        
        if(c == '\n')
        {
            if(dest == mwl)
            {
                *(u8*)dest = (u8)atoi(StringCStr(&word));
            }
            word.index = 0;
        }
        
        if(isspace(c)) continue;    // Check if whitespace

        StringAppend(&word, c);     // Else append character to string
    }

    if(*mwl > 16)
    {
        fprintf(stderr, "Err: MAX_WORD_LENGTH must be <= 16.\n");
        exit(EXIT_FAILURE);
    }

    StringFree(&word);                      // Free resources from the string    
    fclose(config);                         // Close grid file as it is no longer needed
}

void fremove(const char* filename)
{
    if(remove(filename))
    {
        fprintf(stderr, "Err: Unable to remove file '%s': ", filename);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}