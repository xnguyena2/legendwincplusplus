#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN


#pragma warning( disable : 4996)


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFERSIZE 1024

class MyFileReader
{
public:

	static char* ReadConfig(const char*);

	static VOID CALLBACK FileIOCompletionRoutine(
		__in  DWORD dwErrorCode,
		__in  DWORD dwNumberOfBytesTransfered,
		__in  LPOVERLAPPED lpOverlapped
	);


	static DWORD g_BytesTransferred;
};

