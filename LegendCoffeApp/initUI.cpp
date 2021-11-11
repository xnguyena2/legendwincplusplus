#include "initUI.h"


HWND initUI::hWnd = NULL;

HWND initUI::hWndListOrderTxt = NULL;

char* initUI::caffeConfig = NULL;

char* initUI::notificationSound = NULL;

initUI::initUI(HINSTANCE hInst, HWND hWnd)
{
    this->hInst = hInst;
    initUI::hWnd = hWnd;
    HWND hwndButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Get IP",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        10,         // x position 
        10,         // y position 
        100,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_TEST_BTN,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    HWND startSocket = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Start Server",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        10,         // x position 
        50,         // y position 
        100,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_START_BTN,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    HWND hWndServerNotf = CreateWindow(
        L"STATIC",
        L"Server Status!!",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        150,
        55,
        500,
        20,
        hWnd,
        (HMENU)ID_STATUS_STATIC,
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
        NULL);

    hWndListOrderTxt = CreateWindow(
        L"STATIC",
        NULL,
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        10,
        90,
        1000,
        700,
        hWnd,
        (HMENU)ID_LIST_ORDER_STATIC,
        (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
        NULL);

    HFONT hf = CreateFont(23, 0, 0, 0, FW_MEDIUM, 0, 0, 0, VIETNAMESE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Sans Serif");
    SendMessage(hWndListOrderTxt, WM_SETFONT, (WPARAM)hf, 0);


    HWND hwndCleanButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Clean",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        10,         // x position 
        800,         // y position 
        100,        // Button width
        30,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_CLEAN_ALL_BTN,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.
}

initUI::~initUI()
{
}

void initUI::eventHandler(HWND hWnd, int wmId) {
    switch (wmId)
    {
    case ID_TEST_BTN:
    {
        char* listIP = this->getIPAddress();

        char ipPrefix[5] = "IP";
        HWND hWndStatic = CreateWindow(
            L"STATIC",
            fromCharPointer(appendChar(ipPrefix, listIP, ": ")),
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            150,
            15,
            500,
            20,
            hWnd,
            (HMENU)IDC_IP_ADDRESS,
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
            NULL);

        //DialogBox(this->hInst, MAKEINTRESOURCE(IDD_DIALOG_IP_ADDRESS), hWnd, About);


        /*
        HWND diahwnd = CreateDialog(this->hInst, MAKEINTRESOURCE(IDD_DIALOG_IP_ADDRESS), hWnd, About);
        HWND hEdit = GetDlgItem(diahwnd, IDC_IP_ADDRESS);
        int index = GetWindowTextLength(hEdit);

        char ipPrefix[5] = "IP";
        SetWindowText(hEdit, fromCharPointer(appendChar(ipPrefix, listIP, ": ")));

        ShowWindow(diahwnd, SW_SHOW);
        */
    }
    break;

    case ID_START_BTN:
    {
        printf("Start click!!");

        MyFileReader fileReader;

        caffeConfig = fileReader.ReadConfig("config.json");

        if (caffeConfig == NULL) {
            skCB(FALSE);
            return;
        }

        FILE* f = fopen("popding.wav", "rb");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

        notificationSound = new char[(fsize + 1)]();
        fread(notificationSound, 1, fsize, f);
        fclose(f);
        //notificationSound[fsize] = 0;


        if (notificationSound == NULL) {
            skCB(FALSE);
            return;
        }

        MySocket mySocket;
        MyStruct* myState = new MyStruct();
        myState->StatusCB = skCB;
        myState->MSGHandler = msgHandler;
        if (FALSE == mySocket.StartSocket(myState)) {
            skCB(FALSE);
        }
    }
    break;

    case ID_CLEAN_ALL_BTN:
    {
        SetWindowText(hWndListOrderTxt, NULL);
    }
    break;
    }

}

LRESULT CALLBACK initUI::skCB(BOOL status) {
    HWND hEdit = GetDlgItem(initUI::hWnd, ID_STATUS_STATIC);
    char* st = NULL;
    if (status) {
        char success[] = "Start Success!!";
        st = success;
    }
    else {
        char fail[] = "Start Server Fail!!";
        st = fail;
    }
    SetWindowText(hEdit, fromCharPointer(st));
    return 0;
}


char* CALLBACK initUI::msgHandler(char* msg) {
    if (strstr(msg, "PING<EOF>") != NULL) {
        return caffeConfig;
    }
    char* eof = strstr(msg, "<EOF>");
    int length = strlen(msg) - strlen(eof);

    char* data = new char[length + 1]();

    for (int i = 0; i < length; i++) {
        data[i] = msg[i];
    }

    int oldLength = GetWindowTextLength(hWndListOrderTxt);
    int totelLength = length + oldLength + 1;
    wchar_t* txt = new wchar_t[oldLength+1]();
    wchar_t* newIncome = fromCharPointer(data);

    GetWindowText(hWndListOrderTxt, txt, oldLength);

    wchar_t* newChar = new wchar_t[totelLength];

    wcscpy(newChar, newIncome);
    wcscat(newChar, L"\n");
    wcscat(newChar, txt);

    SetWindowText(hWndListOrderTxt, newChar);

    PlaySound(notificationSound, NULL, SND_MEMORY | SND_ASYNC);

    return caffeConfig;
}


// Message handler for about box.
INT_PTR CALLBACK initUI::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LPWSTR initUI::fromCharPointer(char* txt) {

    int length = strlen(txt);

    wchar_t* wtext = NULL;
    wtext = new wchar_t[length + 1];
    mbstowcs(wtext, txt, length + 1);//Plus null
    return wtext;
}

char* initUI::getIPAddress() {

    int i;

    /* Variables used by GetIpAddrTable */
    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IN_ADDR IPAddr;

    char* allIP = NULL;

    /* Variables used to return error message */
    LPVOID lpMsgBuf;

    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(sizeof(MIB_IPADDRTABLE));

    if (pIPAddrTable) {
        // Make an initial call to GetIpAddrTable to get the
        // necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
            ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(dwSize);

        }
        if (pIPAddrTable == NULL) {
            printf("Memory allocation failed for GetIpAddrTable\n");
            exit(1);
        }
    }
    // Make a second call to GetIpAddrTable to get the
    // actual data we want
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
        printf("GetIpAddrTable failed with error %d\n", dwRetVal);
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
            (LPTSTR)&lpMsgBuf, 0, NULL)) {
            printf("\tError: %s", lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        exit(1);
    }

    printf("\tNum Entries: %ld\n", pIPAddrTable->dwNumEntries);
    for (i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
        printf("\n\tInterface Index[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwIndex);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
        char* ipA = inet_ntoa(IPAddr);

        printf("\tIP Address[%d]:     \t%s\n", i, ipA);

        char* newAllIP = appendChar(allIP, ipA, ", ");
        allIP = newAllIP;

        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwMask;
        printf("\tSubnet Mask[%d]:    \t%s\n", i, inet_ntoa(IPAddr));
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwBCastAddr;
        printf("\tBroadCast[%d]:      \t%s (%ld%)\n", i, inet_ntoa(IPAddr), pIPAddrTable->table[i].dwBCastAddr);
        printf("\tReassembly size[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwReasmSize);
        printf("\tType and State[%d]:", i);
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
            printf("\tPrimary IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
            printf("\tDynamic IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
            printf("\tAddress is on disconnected interface");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
            printf("\tAddress is being deleted");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
            printf("\tTransient address");
        printf("\n");
    }

    printf(allIP);

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = NULL;
    }

    return allIP;
}


char* initUI::appendChar(char* old, char* append, const char* sperater) {

    int newLength = 0;
    char* newChar = NULL;
    if (old == NULL)
    {
        newLength = strlen(append);
        newChar = new char[newLength];

        strcpy(newChar, append);
    }
    else {
        newLength = strlen(old) + strlen(append) + strlen(sperater);
        newChar = new char[newLength];

        strcpy(newChar, old);
        strcat(newChar, sperater);
        strcat(newChar, append);
    }

    return newChar;
}