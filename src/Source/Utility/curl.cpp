#include <metahook.h>
#include "plugins.h"
#include "curl.h"
#include <wtypes.h>

void* (*curl_easy_init)(void) = nullptr;
CURLcode(*curl_easy_setopt)(void* curl, CURLoption option, ...) = nullptr;
CURLcode (*curl_easy_getinfo)(void* curl, CURLINFO info, ...) = nullptr;
CURLcode(*curl_easy_perform)(void* curl) = nullptr;
void (*curl_easy_cleanup)(void* curl) = nullptr;
struct curl_slist* (*curl_slist_append)(struct curl_slist*, const char*) = nullptr;
void (*curl_slist_free_all)(struct curl_slist* list) = nullptr;

HMODULE g_hLibCurl = NULL;

void LoadLibcurl()
{
	g_hLibCurl = LoadLibrary("libcurl.dll");

	if (!g_hLibCurl)
	{
		SYS_ERROR("Could not load libcurl.dll");
		return;
	}

#define LIBCURL_FUNCTION_DEFINE(name) name = (decltype(name))(GetProcAddress(g_hLibCurl, #name))
	LIBCURL_FUNCTION_DEFINE(curl_easy_init);
	LIBCURL_FUNCTION_DEFINE(curl_slist_append);
	LIBCURL_FUNCTION_DEFINE(curl_easy_setopt);
	LIBCURL_FUNCTION_DEFINE(curl_easy_getinfo);
	LIBCURL_FUNCTION_DEFINE(curl_easy_perform);
	LIBCURL_FUNCTION_DEFINE(curl_slist_free_all);
	LIBCURL_FUNCTION_DEFINE(curl_easy_cleanup);
}

void FreeLibcurl()
{
	if (g_hLibCurl)
	{
		FreeLibrary(g_hLibCurl);
		g_hLibCurl = NULL;
	}
}