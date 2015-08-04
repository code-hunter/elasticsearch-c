#include <windows.h>
#include <stdio.h>
#include <process.h>

// Q1.cpp : Defines the entry point for the console application.
// Download by http://www.codefans.net

#include "client.h"

const unsigned int cuStackSize = 128 * 1024;

class CThreadInfo
{
protected:
	HANDLE m_hInitialized;
	HANDLE m_hTerminate; 
	

public:
	Client client;
	Client client1;
	CThreadInfo();
	~CThreadInfo();
	DWORD WaitForInitialized(DWORD dwWaitTime = 5000);
	DWORD WaitForTerminate(DWORD dwWaitTime = 5000);
	void SetInitialized();
	void SetTerminate();
};



CThreadInfo::CThreadInfo()
{
	// create two events
	//		no name, autoreset,
	//		not initially owned, default security

	m_hInitialized = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hInitialized != NULL)
	{
		m_hTerminate = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hTerminate == NULL)
		{
			CloseHandle(m_hInitialized);
			m_hInitialized = NULL;
		}
	}

	client.conn("localhost", "9200");

	client1.conn("localhost", "9200");

	// an error? throw!
	if (m_hInitialized == NULL || m_hTerminate == NULL)
	{
		printf("ERROR: Couldn't create thread handles!\n");
		throw 1;
	}
}

CThreadInfo::~CThreadInfo()
{
	// if they exist, close them
	client.close();
	client1.close();
	if (m_hInitialized == NULL)
		CloseHandle(m_hInitialized);
	if (m_hTerminate == NULL)
		CloseHandle(m_hTerminate);
}

DWORD CThreadInfo::WaitForInitialized(DWORD dwWaitTime /* = 5000 */)
{
	DWORD dwResult = WaitForSingleObject(m_hInitialized, dwWaitTime);

	if (dwWaitTime != 0 && dwResult == WAIT_TIMEOUT)
		throw 2;

	return dwResult;
}

DWORD CThreadInfo::WaitForTerminate(DWORD dwWaitTime /* = 5000 */)
{
	DWORD dwResult = WaitForSingleObject(m_hTerminate, dwWaitTime);

	if (dwWaitTime != 0 && dwResult == WAIT_TIMEOUT)
		throw 2;

	return dwResult;
}

void CThreadInfo::SetInitialized()
{
	SetEvent(m_hInitialized);
}

void CThreadInfo::SetTerminate()
{
	SetEvent(m_hTerminate);
}

void insertDoc1(Client client){
	map<string, string> testMap;
	testMap.insert(make_pair("name", "testname"));
	testMap.insert(make_pair("age", "11"));
	Response *resp = client.createDoc("testworld", "tweet", testMap);
	printf("code = %d\n", resp->statusCode);
	printf("body = %s\n", resp->body);
}

void insertDoc2(Client client){
	map<string, string> testMap;
	testMap.insert(make_pair("name", "testname"));
	testMap.insert(make_pair("age", "101"));
	Response *resp = client.createDoc("testworld", "tweet",  testMap);
	printf("code = %d\n", resp->statusCode);
	printf("body = %s\n", resp->body);
}

void getIndex1(Client client){
	Response *resp = client.getIndex("testworld");
	printf("code = %d\n", resp->statusCode);
	printf("body = %s\n", resp->body);
}

void getIndex2(Client client){
	Response *resp = client.getIndex("testworld");
	printf("code = %d\n", resp->statusCode);
	printf("body = %s\n", resp->body);
}

unsigned __stdcall ProcOne(void* pThreadInfoObject)
{
	CThreadInfo* pInfo = (CThreadInfo*) pThreadInfoObject;

	try
	{
		// let our creator know we're initialized
		pInfo->SetInitialized();

		for(int i=0; i<10; i++){
			getIndex1(pInfo->client);
			Sleep(1000);
		}

		// wait until we're told to terminate
		pInfo->WaitForTerminate();
	}
	catch (int n)
	{
		printf("First thread ERROR: ");
		if (n == 2)
			printf("wait timeout!\n");
		else
			printf("some other error\n");
	}

	return 0;
}

unsigned __stdcall ProcTwo(void* pThreadInfoObject)
{
	CThreadInfo* pInfo = (CThreadInfo*) pThreadInfoObject;

	try
	{
		// let our creator know we're initialized
		pInfo->SetInitialized();

		MSG msg;
		int nIdleCount = 0;

		for(int i=50; i<100; i++){
			getIndex2(pInfo->client1);
			Sleep(1000);
		}

		pInfo->WaitForTerminate();
	}
	catch (int n)
	{
		printf("Second thread ERROR: ");
		if (n == 2)
			printf("wait timeout!\n");
		else
			printf("some other error\n");
	}

	return 0;
}



int main_test(int argc, char* argv[])
{
	CThreadInfo* pInfo = NULL;

	try
	{
		global_init();

		// create a thread info objet to communicate with our threads...		
		pInfo = new CThreadInfo;

		// data to identify threads
		HANDLE hFirstThread;
		DWORD dwFirstThread;
		HANDLE hSecondThread;
		DWORD dwSecondThread;

		// --- create the first thread
		hFirstThread = (HANDLE)	_beginthreadex(NULL, cuStackSize,
				ProcOne, pInfo, 0, (unsigned int*) &dwFirstThread);

		// wait for it to be initialized
		pInfo->WaitForInitialized();

		// try to post a message
		BOOL bResult = PostThreadMessage(dwFirstThread, WM_NULL, 0, 0);

		// print the results
		printf("First thread: PostThreadMessage() returns %d\n", bResult);

		// tell it to terminate
		//pInfo->SetTerminate();

		// --- create the second thread
		hSecondThread = (HANDLE) _beginthreadex(NULL, cuStackSize,
				ProcTwo, pInfo, 0, (unsigned int*) &dwSecondThread);

		// wait for it to be initialized
		pInfo->WaitForInitialized();

		// try to post a message
		bResult = PostThreadMessage(dwSecondThread, WM_NULL, 0, 0);

		// print the results
		printf("Second thread: PostThreadMessage() returns %d\n", bResult);

		// let it run for a second
		Sleep(10000);

		// tell it to terminate
		pInfo->SetTerminate();
		system("Pause");

		global_deinit();
	}
	catch (int n)
	{
		if (n == 1)
			printf("ERROR: couldn't construct thread info object!\n");
		else if (n == 2)
			printf("ERROR: wait timeout!\n");
		else
			printf("ERROR: some other integer exception!\n");
	}
	catch (...)
	{
		printf("ERROR: Something is really bad!\n");
	}

	return 0;
}