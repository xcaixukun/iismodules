#pragma once
#ifndef __MY_MODULE_H__
#define __MY_MODULE_H__
#include <stdio.h>
#include "pch.h"

#define HEADER_VAL_LEN 256 // ULI: Added
class GlobalModule : public CGlobalModule {};
class IISHttpModule : public CHttpModule
{
public:
	TCHAR m_pszHeaderValue[HEADER_VAL_LEN];
	TCHAR m_pszHeaderName[1024];
	HANDLE m_hEventLog;

	//-----------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------
public:
	IISHttpModule()
	{
		Initialize(_T("X-Forwarded-For"));
	}

	IISHttpModule(TCHAR* szHeaderName)
	{
		Initialize(szHeaderName);
	}

	~IISHttpModule()
	{
		if (NULL != m_hEventLog)
		{
			DebugMessage(_T("<-- Deregistering F5 X-Forwarded-For Http Module..."));
			DeregisterEventSource(m_hEventLog);
			m_hEventLog = NULL;
		}
	}
public:
	void Initialize(TCHAR* szHeaderName)
	{
		SetHeaderName(szHeaderName);

		memset(m_pszHeaderValue, 0x00, HEADER_VAL_LEN); // ULI Added

		m_hEventLog = RegisterEventSource(NULL, _T("IISADMIN"));

		DebugMessage(_T("--> Registering F5 X-Forwarded-For Http Module..."));
	}

	TCHAR* GetHeaderName()
	{
		return m_pszHeaderName;
	}

	void SetHeaderName(TCHAR* szHeaderName)
	{
		if (NULL != szHeaderName)
		{
			_tcsncpy(m_pszHeaderName, szHeaderName, 1023);
		}
	}
public:
	void DebugMessage(TCHAR* szFormat, ...);
	bool WriteFileLogMessage(TCHAR* szMsg);
	bool WriteEventLogMessage(TCHAR* szMsg);

public:
	REQUEST_NOTIFICATION_STATUS
		OnBeginRequest(IN IHttpContext* pHttpContext, IN IHttpEventProvider* pProvider); // ULI Changed

	REQUEST_NOTIFICATION_STATUS
		OnSendResponse(IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider);

};
class IISHttpModuleFactory : public IHttpModuleFactory
{
public:
	TCHAR m_HeaderName[1024];

public:
	IISHttpModuleFactory()
	{
		SetHeaderName(_T("X-Forwarded-For"));
	}

	IISHttpModuleFactory(TCHAR* szHeaderName)
	{
		SetHeaderName(szHeaderName);
	}

public:
	void SetHeaderName(TCHAR* szHeaderName)
	{
		if (NULL != szHeaderName)
		{
			_tcsncpy(m_HeaderName, szHeaderName, 1023);
		}
	}

public:
	virtual HRESULT
		GetHttpModule(OUT CHttpModule** ppModule, IN IModuleAllocator*)
	{
		HRESULT hr = S_OK;
		IISHttpModule* pModule = NULL;

		if (ppModule == NULL)
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
			goto Finished;
		}

		pModule = new IISHttpModule(m_HeaderName);
		if (pModule == NULL)
		{
			hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
			goto Finished;
		}

		*ppModule = pModule;
		pModule = NULL;

	Finished:

		if (pModule != NULL)
		{
			delete pModule;
			pModule = NULL;
		}

		return hr;
	}

	virtual void
		Terminate()
	{
		delete this;
	}
};

BOOL IsSpider(const char* szUa);
std::string HttpRequest(std::string szUrl, std::string  ua, std::string  accept, const char* uc_ua, const char* referer, const char* CilentIP);
#endif
