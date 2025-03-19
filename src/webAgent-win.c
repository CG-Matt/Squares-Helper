#ifdef _WIN32

#include <stdlib.h>
#include <windows.h>
#include <winhttp.h>
#include "webAgent.h"
#include "config.h"

#pragma comment(lib, "winhttp.lib")

/**
 * Stores the handles to both the agent and the current open connection
 */
typedef struct {
    HINTERNET agent;
    HINTERNET connection;
} HTTPHandles;

/**
 * Mallocs memory, must be free'd afterwards.
 * Converts a narrow string to a wide string.
 */
static wchar_t* to_wide(const char* narrow)
{
    size_t size = strlen(narrow) + 1;
    wchar_t* wide = malloc(sizeof(wchar_t) * size);
    if(!wide) return NULL;
    for(size_t i = 0; i < size; i++)
    {
        wide[i] = narrow[i];
    }
    return wide;
}

/**
 * Get just the path from a url.
 */
static const char* get_path(const char* url)
{
    const char* start = url;
    while(*start && *start != '/') start++;
    start += 2;
    while(*start && *start != '/') start++;
    return start;
}

/**
 * Does nothing on windows but needs to be here
 */
void WebAgentGlobalInit(void){}

WebAgent* WebAgentCreate(void)
{
    WebAgent* web_agent = malloc(sizeof(WebAgent));     // Allocate memory for the web agent
    if(!web_agent) return NULL;                         // Return NULL if unable to malloc

    web_agent->handle = malloc(sizeof(HTTPHandles));    // Allocate memory to store the required handles
    if(!web_agent->handle)                              // Return NULL if unable to malloc
    {
        free(web_agent);
        return NULL;
    }

    HTTPHandles* handles = web_agent->handle;           // Used for convenience

    handles->agent = WinHttpOpen(                       // Create a WinHttp handle
        L"Mozilla/5.0",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    if(!handles->agent)                                 // Return if unable to open handle
    {
        free(web_agent->handle);
        free(web_agent);
        return NULL;
    }
    wchar_t* host = to_wide(URL_HOST);                  // Convert the host name to wide string
    handles->connection = WinHttpConnect(               // Open a connection to the host
        handles->agent,
        host,
        INTERNET_DEFAULT_HTTP_PORT,
        0);
    free(host);                                         // Free host name memory
    if(!handles->connection)                            // If unable to connect return NULL
    {
        WinHttpCloseHandle(handles->agent);
        free(web_agent->handle);
        free(web_agent);
        return NULL;
    }

    return web_agent;                               // All good, return the web agent
}

void WebAgentCleanup(WebAgent* wa)
{
    HTTPHandles* handles = wa->handle;              // For convenience
    WinHttpCloseHandle(handles->connection);        // Close connection handle
    WinHttpCloseHandle(handles->agent);             // Close agent handle
    free(wa);                                       // Clean up web agent memory
}

void WebAgentRequest(WebAgent* wa, const char* url)
{
    wchar_t* path = to_wide(get_path(url));         // Get path in wide character format
    HINTERNET request = WinHttpOpenRequest(         // Open request handle
        ((HTTPHandles*)(wa->handle))->connection,
        L"GET",
        path,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0
    );
    free(path);                                     // Free path memory

    if(!request)                                    // If unable to open request handle
    {
        wa->err = 1;
        return;
    }

    int res = WinHttpSendRequest(request,           // Send the request
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0
    );
    
    if(!res)                                        // If request did not succeed, return
    {
        wa->err = 1;
        WinHttpCloseHandle(request);
        return;
    }

    WinHttpReceiveResponse(request, NULL);          // Receive the response from the request 

    DWORD status_size = sizeof(wa->httpCode);
    WinHttpQueryHeaders(request,                    // Get the http status code of the request
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &wa->httpCode, &status_size,
        WINHTTP_NO_HEADER_INDEX
    );

    WinHttpCloseHandle(request);                    // Close the request handle
    wa->err = 0;                                    // Clear the web agent error
}

#endif
