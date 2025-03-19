#pragma once

// --- STRUCTS ---

typedef struct {
    void* handle;
    unsigned int err;
    unsigned int httpCode;
} WebAgent;

// --- FUNCTIONS ---

/**
 * Must be called once at the beginning of the program
 * before any other WebAgent functions are used
 */
void WebAgentGlobalInit(void);

/**
 * Create a new web agent instance
 */
WebAgent* WebAgentCreate(void);

/**
 * Clean up memory for web agent
 */
void WebAgentCleanup(WebAgent* web_agent);

/**
 * Execute a web request with the provided web_agent
 * to the provided url.
 */
void WebAgentRequest(WebAgent* web_agent, const char* url);
