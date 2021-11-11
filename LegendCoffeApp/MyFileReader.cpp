#include "MyFileReader.h"


DWORD MyFileReader::g_BytesTransferred = 0;

char* MyFileReader::ReadConfig(const char* fileName) {

    HANDLE hFile;
    DWORD  dwBytesRead = 0;
    char   ReadBuffer[BUFFERSIZE] = { 0 };
    OVERLAPPED ol = { 0 };

    printf("\n");

    hFile = CreateFile(fileName,               // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
        NULL);                 // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    // Read one character less than the buffer size to save room for
    // the terminating NULL character. 

    if (FALSE == ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine))
    {
        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }
    SleepEx(5000, TRUE);
    dwBytesRead = g_BytesTransferred;
    // This is the section of code that assumes the file is ANSI text. 
    // Modify this block for other data types if needed.

    if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
    {
        ReadBuffer[dwBytesRead] = '\0'; // NULL character

        printf("%s\n", ReadBuffer);
    }
    else if (dwBytesRead == 0)
    {
    }
    else
    {
        printf("\n ** Unexpected value for dwBytesRead ** \n");
    }

    // It is always good practice to close the open file handles even though
    // the app will exit here and clean up open handles anyway.

    CloseHandle(hFile);

    int newLength = strlen(ReadBuffer);
    char* newChar = new char[newLength];

    strcpy(newChar, ReadBuffer);

    return newChar;
}


VOID CALLBACK MyFileReader::FileIOCompletionRoutine(
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped)
{
    g_BytesTransferred = dwNumberOfBytesTransfered;
}