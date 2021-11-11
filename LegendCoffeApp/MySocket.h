#pragma once
#undef UNICODE

#pragma warning( disable : 4996)
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "SocketState.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8888"


class MySocket
{
public:
	bool StartSocket(MyStruct*);

private:
	HANDLE hThrd;

	DWORD socketThreadID;

	static DWORD WINAPI socketThreadBcg(LPVOID lpParam);

	static DWORD WINAPI socketClientHandler(LPVOID lpParam);

};

