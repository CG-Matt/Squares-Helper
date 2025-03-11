#include <stdlib.h>
#include <curl/curl.h>
#include "webAgent.h"

// Dummy write function for curl
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    (void)ptr; (void)userdata;
    return size * nmemb;
}

void WebAgentGlobalInit()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

WebAgent* WebAgentCreate()
{
    WebAgent* web_agent = malloc(sizeof(WebAgent)); // Allocate memory for the web agent
    if(!web_agent) return NULL;                     // Return NULL if unable to malloc
    web_agent->handle = curl_easy_init();           // Initialise curl instance
    if(!web_agent->handle)                          // If unable to initialise CURL
    {
        free(web_agent);                            // Free memory of web agent
        return NULL;                                // Return NULL
    }

    // Do some configuration
    curl_easy_setopt(web_agent->handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(web_agent->handle, CURLOPT_USERAGENT, "Mozilla/5.0");

    return web_agent;                               // All good, return the web agent
}

void WebAgentCleanup(WebAgent* wa)
{
    curl_easy_cleanup((CURL*)wa->handle);           // Clean up curl memory
    free(wa);                                       // Clean up web agent memory
}

void WebAgentRequest(WebAgent* wa, const char* url)
{
    curl_easy_setopt((CURL*)wa->handle, CURLOPT_URL, url);

    wa->err = curl_easy_perform((CURL*)wa->handle); // Perform web request
    if(wa->err != CURLE_OK)
        printf("CURL ERROR CODE: %i\n", wa->err);
    if(wa->err == CURLE_URL_MALFORMAT)
        printf("URL: %s\n", url);
    wa->err = wa->err != CURLE_OK;                  // Err will be 1 if error, 0 if successful
    if(wa->err) return;                             // Exit early if we had an error

    curl_easy_getinfo((CURL*)wa->handle, CURLINFO_RESPONSE_CODE, &wa->httpCode);
}