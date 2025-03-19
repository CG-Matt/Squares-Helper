#include <stdlib.h>
#include "multithreading.h"
#include "CGMString.h"
#include "string.h"
#include "config.h"
#include "webAgent.h"

// --- Platform dependent includes
#ifndef _WIN32
    #include <pthread.h>
#else
    #include <windows.h>
#endif

// --- LOCAL VARIABLES ---

static ThreadContext contexts[THREAD_COUNT];
static FILE* outfile;
static u8 max_word_length;
static usize url_buffer_length;

#ifndef _WIN32
static pthread_t threads[THREAD_COUNT];
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#else
static HANDLE threads[THREAD_COUNT];
static HANDLE lock;
#endif

// --- THREAD FUNCTION ---

#ifndef _WIN32
void* website_check(void* arg)
#else
DWORD WINAPI website_check(void* arg)
#endif
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

    StringInit(&word);
    StringReserve(&word, max_word_length + 1);  // Reserve memory as to not resize
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
        u8 s = sprintf(url + URL_BASE_LENGTH, "%zu", word.size);
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
#ifndef _WIN32
            pthread_mutex_lock(&lock);
#else
            WaitForSingleObject(lock, INFINITE);
#endif
            StringAppend(&word, '\n');
            fputs(StringCStr(&word), outfile);
            fflush(outfile);
#ifndef _WIN32
            pthread_mutex_unlock(&lock);
#else
            ReleaseMutex(lock);
#endif
        }

        StringClear(&word);
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

#ifdef _WIN32
    // Initialise mutex
    lock = CreateMutex(NULL, FALSE, NULL);

    if(!lock)
    {
        fprintf(stderr, "Err: Could not initialise mutex: ");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
#endif

    printf("threads initialised.\n");
}

void ThreadsAddWord(const char* s)
{
    static u16 ti = 0;
    fputs(s, contexts[ti].input);
    fputc('\n', contexts[ti].input);
    ti = (ti + 1) % THREAD_COUNT;
}

void ThreadsResetFiles(void)
{
    for(u16 i = 0; i < THREAD_COUNT; i++)
        rewind(contexts[i].input);
}

void ThreadsBegin(void)
{
    printf("Creating threads: ");
    for(u16 i = 0; i < THREAD_COUNT; i++)
    {
#ifndef _WIN32
        pthread_create(&threads[i], NULL, website_check, &contexts[i]);
#else
        threads[i] = CreateThread(NULL, 0, website_check, &contexts[i], 0, NULL);
        if(!threads[i])
        {
            fprintf(stderr, "Failed to create thread %hu.\n", i);
            exit(EXIT_FAILURE);
        }
#endif
    }
    printf("threads started sucessfully.\n");
}

void ThreadsJoin(void)
{
    printf("Awaiting completion...\n");
#ifndef _WIN32
    for(u16 i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
#else
    WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);
#endif
    printf("All threads finished.\n");
}

void ThreadsSetOutFile(FILE* f){ outfile = f; }

void ThreadsCleanup(void)
{
#ifdef _WIN32
    CloseHandle(lock);
#endif

    for(u16 i = 0; i < THREAD_COUNT; i++)
    {
#ifdef _WIN32
        CloseHandle(threads[i]);
#endif
        fclose(contexts[i].input);
    }
}
