#pragma once

#pragma warning( disable : 4996)

#include "framework.h"
#include "Resource.h"
#include "MyFileReader.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <Mmsystem.h>
#include <mciapi.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#define ID_TEST_BTN                     300
#define ID_START_BTN                    301
#define ID_STATUS_STATIC                302
#define ID_LIST_ORDER_STATIC            303
#define ID_CLEAN_ALL_BTN                304


class initUI
{
public:
	initUI(HINSTANCE hInst, HWND hWnd);
	~initUI();

	void eventHandler(HWND hWnd, int wmId);
	static LRESULT CALLBACK skCB(BOOL);
	static char* CALLBACK msgHandler(char*);

private:
	HINSTANCE hInst;
	static HWND hWnd;
	static HWND hWndListOrderTxt;

	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static char* caffeConfig;
	static char* notificationSound;

	char* getIPAddress();

	static char* appendChar(char*,char*, const char*);

	static LPWSTR fromCharPointer(char*);
};


