#pragma once

#include <winsock2.h>

typedef LRESULT(CALLBACK* SOCKET_START)(BOOL);
typedef char*(CALLBACK* SOCKET_MSG)(char*);

struct MyStruct
{
	SOCKET ClientSocket;
	SOCKET_START StatusCB;
	SOCKET_MSG MSGHandler;
};