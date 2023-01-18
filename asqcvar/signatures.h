#pragma once

#include <signatures_template.h>

#ifdef _WIN32

#define curl_easy_init_Signature "curl_easy_init"

#define LIBCURL_DLL_NAME "libcurl.dll"

#else

#define curl_easy_init_Signature "curl_easy_init"

#define LIBCURL_DLL_NAME "libcurl.so.4"

#endif
