// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "IIS.h"

IHttpServer* g_pHttpServer = NULL;  //  Global server instance
PVOID g_pModuleContext = NULL;  //  Global module context id
CRITICAL_SECTION g_CS;
static const DWORD BUFFER_LEN = 256;
static TCHAR* DEFAULT_HEADER_NAME = _T("X-Forwarded-For");
TCHAR gHEADER_NAME[BUFFER_LEN];

BOOL IsSpider(const char* szUa) {

	char* spiderUas[] = { "Baiduspider", "360spider", "Sosospider", "Sogouspider", "YisouSpider", "Sogou web spider" };
	size_t len = sizeof(spiderUas) / sizeof(spiderUas[0]);

	for (size_t i = 0; i < len; i++)
	{
		if (strstr(szUa, spiderUas[i]) > 0) {
			return TRUE;
		}
	}
	return FALSE;
}

int BufferWriterFunc(char* data, size_t size, size_t nmemb, std::string* buffer)
{
	int result = 0;
	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		printf("%s", data);
		result = size * nmemb;
	}
	return result;
}

std::string HttpRequest(std::string szUrl, std::string  ua, std::string  accept, const char* uc_ua, const char* referer, const char* CilentIP)
{
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);

	// get a curl handle
	curl = curl_easy_init();
	long http_code = 0;
	std::string strResponse;
	if (curl)
	{
		std::string s = "";
		struct curl_slist* chunk = NULL;
		chunk = curl_slist_append(chunk, (std::string("User-Agent: ") + ua).c_str());
		chunk = curl_slist_append(chunk, (std::string("Accept: ") + accept).c_str());
		chunk = curl_slist_append(chunk, (std::string("X-Forwarded-For: ") + CilentIP).c_str());
		if (uc_ua != NULL && strlen(uc_ua) != 0)
			chunk = curl_slist_append(chunk, (std::string("HTTP_X_UCBROWSER_UA: ") + std::string(uc_ua)).c_str());
		if (referer != NULL && strlen(referer) != 0)
			chunk = curl_slist_append(chunk, (std::string("Referer: ") + std::string(referer)).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, szUrl.data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferWriterFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strResponse);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
	if (http_code == 404 || http_code == 403)
	{
		return "";
	}
	return strResponse;
}

HRESULT __stdcall
RegisterModule(DWORD dwServerVersion, IHttpModuleRegistrationInfo* pModuleInfo, IHttpServer* pHttpServer)
{
	HRESULT hr = S_OK;
	IISHttpModuleFactory* pFactory = NULL;

	if (pModuleInfo == NULL || pHttpServer == NULL)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
	}
	else
	{
		// step 1: save the IHttpServer and the module context id for future use
		g_pModuleContext = pModuleInfo->GetId();
		g_pHttpServer = pHttpServer;

		// step 2: create the module factory
		pFactory = new IISHttpModuleFactory(gHEADER_NAME);
		if (pFactory == NULL)
		{
			hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
		}
		else
		{
			// step 3: register for server events
			// TODO: register for more server events here
			DWORD dwRequestNotifications = RQ_BEGIN_REQUEST | RQ_SEND_RESPONSE;
			DWORD dwPostRequestNotifications = 0;

			hr = pModuleInfo->SetRequestNotifications(pFactory, dwRequestNotifications, dwPostRequestNotifications);
			if (SUCCEEDED(hr))
			{
				// Set priority so that we can get some log data info in the SendResponse event.
				hr = pModuleInfo->SetPriorityForRequestNotification(RQ_SEND_RESPONSE, PRIORITY_ALIAS_HIGH);
				if (SUCCEEDED(hr))
				{
					// Everything is good so set pFactory to NULL so it's not deleted below.
					pFactory = NULL;
				}
			}
		}
	}

	if (pFactory != NULL)
	{
		delete pFactory;
		pFactory = NULL;
	}

	return hr;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection(&g_CS);
		// ReadConfiguration(hModule);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&g_CS);
		break;
	}
	return TRUE;
}