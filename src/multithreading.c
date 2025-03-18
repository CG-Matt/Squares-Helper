#include <stdlib.h>
#include <threads.h>
#include "multithreading.h"
#include "CGMString.h"
#include "string.h"
#include "config.h"
#include "webAgent.h"

// --- LOCAL VARIABLES ---

static ThreadContext contexts[THREAD_COUNT];
static thrd_t threads[THREAD_COUNT];
static mtx_t lock;
static FILE* outfile;
static u8 max_word_length;
static usize url_buffer_length;

// --- THREAD FUNCTION ---

int website_check(void* arg)
{
    ThreadContext* context = arg;
    WebAgent* web_agent = WebAgentCreate();

    if(!web_agent)
    {
        fprintf(stderr, "Thread %u: Unable to create web agent.\n", context->thread_id);
        return 0;
    }

    String_t word;

    char* url = malloc(sizeof(char) * url_buffer_length);
    int ch;

    StringInit(&word, max_word_length);
    strcpy(url, URL_START URL_HOST URL_PATH);

    while(1)
    {
        while((ch = fgetc(context->input)) != EOF)
        {
            if(ch == '\n') break;
            StringAppend(&word, ch);
        }

        if(ch == EOF) break;

        // Construct URL from word and length
        u8 s = sprintf(url + URL_BASE_LENGTH, "%u", word.index);
        url[URL_BASE_LENGTH + s] = '\0';
        strcat(url, URL_MIDDLE);
        strcat(url, StringCStr(&word));
        strcat(url, URL_SUFFIX);

        WebAgentRequest(web_agent, url);

        if(web_agent->err)
        {
            fprintf(stderr, "Thread %u: Error performing web request, word: %s\n", context->thread_id, word.buffer);
            WebAgentCleanup(web_agent);
            StringFree(&word);
            free(url);
            return 0;
        }

        if(web_agent->httpCode != 200 && web_agent->httpCode != 404)
            printf("Thread %u: Code: %u, Word: %s\n", context->thread_id, web_agent->httpCode, word.buffer);
        
        if(web_agent->httpCode == 200)
        {
            mtx_lock(&lock);
            StringAppend(&word, '\n');
            fputs(StringCStr(&word), outfile);
            fflush(outfile);
            mtx_unlock(&lock);
        }

        word.index = 0;
    }

    WebAgentCleanup(web_agent);
    StringFree(&word);
    free(url);
    return 0;
}

// --- FUNCTION DEFINITIONS ---

void ThreadsInit(u8 mwl)
{
    printf("Initialising thread contexts: ");

    // Run global web agent init
    WebAgentGlobalInit();

    max_word_length = mwl;
    url_buffer_length = URL_BASE_LENGTH + sizeof(URL_MIDDLE) - 1 + sizeof(URL_SUFFIX) - 1 + max_word_length + 2;

    for(u16 i = 0; i < THREAD_COUNT; i++)
    {
        contexts[i].input = tmpfile();
        if(!contexts[i].input)
        {
            fprintf(stderr, "Err: Could not open check file for thread %i.\n", i);
            exit(EXIT_FAILURE);
        }
        contexts[i].thread_id = i;
    }

    // Initialise mutex
    int success = mtx_init(&lock, mtx_plain);

    if(success != thrd_success)
    {
        fprintf(stderr, "Err: Could not initialise mutex: ");
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    printf("threads initialised.\n");
}

void ThreadsAddWord(const char* s)
{
    static u16 ti = 0;
    fputs(s, contexts[ti].input);
    fputc('\n', contexts[ti].input);
    ti = (ti + 1) % THREAD_COUNT;
}

void ThreadsResetFiles()
{
    for(u16 i = 0; i < THREAD_COUNT; i++)
        rewind(contexts[i].input);
}

void ThreadsBegin()
{
    printf("Creating threads: ");
    for(u16 i = 0; i < THREAD_COUNT; i++)
        thrd_create(&threads[i], website_check, &contexts[i]);
    printf("threads started sucessfully.\n");
}

void ThreadsJoin()
{
    printf("Awaiting completion...\n");
    for(u16 i = 0; i < THREAD_COUNT; i++)
        thrd_join(threads[i], NULL);
    printf("All threads finished.\n");
}

void ThreadsSetOutFile(FILE* f){ outfile = f; }

void ThreadsCleanup()
{
    mtx_destroy(&lock);

    for(u16 i = 0; i < THREAD_COUNT; i++)
        fclose(contexts[i].input);
}