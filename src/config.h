#pragma once

// --- PROGRAM CONFIGURATION ---

#define MIN_FREQUENCY           1.3         // UNUSED
#define MAX_FREQUENCY           2.5         // UNSUED
#define THREAD_COUNT            32

// Uncomment if you would like to use a custom python executable path
// #define PYTHON_PATH             "/usr/bin/python3"

// --- ADDITIONAL CONSTANTS - DO NOT EDIT ---

#define URL_START               "https://"
#define URL_HOST                "wordfinder.yourdictionary.com"
#define URL_PATH                "/letter-words/"
#define URL_MIDDLE              "-starts-"
#define URL_SUFFIX              "/?dictionary=WWF"
#define FILTER_SCRIPT           "filter.py"
#define URL_BASE_LENGTH         (sizeof(URL_START URL_HOST URL_PATH) - 1)