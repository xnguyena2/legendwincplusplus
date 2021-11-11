#include "MySocket.h"

bool MySocket::StartSocket(MyStruct* skCB) {

    hThrd = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)socketThreadBcg, skCB, 0, &socketThreadID);

    return hThrd != NULL;
}

DWORD WINAPI MySocket::socketThreadBcg(LPVOID lpParam) {

    MyStruct* statusCB = (MyStruct*)lpParam;

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        statusCB->StatusCB(FALSE);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        statusCB->StatusCB(FALSE);
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        statusCB->StatusCB(FALSE);
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        statusCB->StatusCB(FALSE);
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        statusCB->StatusCB(FALSE);
        return 1;
    }
    statusCB->StatusCB(TRUE);
    while (true) {
        SOCKET ClientSocket = INVALID_SOCKET;
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            break;
        }

        statusCB->ClientSocket = ClientSocket;
        DWORD clientSocketHandlerID;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)socketClientHandler, statusCB, 0, &clientSocketHandlerID);

    }

    // No longer need server socket
    closesocket(ListenSocket);
    WSACleanup();

    return 0;

}


DWORD WINAPI MySocket::socketClientHandler(LPVOID lpParam) {

    int iResult;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    MyStruct* statusCB = (MyStruct*)lpParam;
    SOCKET ClientSocket = statusCB->ClientSocket;

    // Receive until the peer shuts down the connection

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);

        char* reciveData = new char[iResult+1]();
        for (int i = 0; i < iResult;i++) {
            reciveData[i] = recvbuf[i];
        }

        char* response = statusCB->MSGHandler(reciveData);
        delete[] reciveData;
        // Echo the buffer back to the sender
        iSendResult = send(ClientSocket, response, strlen(response), 0);
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
        printf("Bytes sent: %d\n", iSendResult);
    }
    else if (iResult == 0)
        printf("Connection closing...\n");
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
}