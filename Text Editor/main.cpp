#include <windows.h>
#include <CommCtrl.h>
#include "resource.h"

#define IDC_MAIN_MDI  105
#define IDC_MAIN_EDIT 101
#define IDC_MAIN_TOOL 103
#define IDC_MAIN_STATUS 104
#define IDC_CHILD_EDIT 105
#define ID_MDI_FIRSTCHILD 50000

const TCHAR g_szClassName[] = TEXT("myWindowClass");
const TCHAR g_szChildName[] = TEXT("myChildWindow");

HBRUSH g_hbrBackground = CreateSolidBrush(RGB(25, 25, 100));
HWND g_hMDIClient = NULL;
HWND g_hMainWindow = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// MDI functions
LRESULT CALLBACK MDIWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL SetupMDIChildWindowClass(HINSTANCE hInstace);
HWND CreateNewMDIWindow(HWND hMDIClient);

// common functions
bool LoadFileToEdit(HWND hEdit, TCHAR* fileName);
bool SaveFile(HWND hEdit, TCHAR* fileName);
void DoFileOpen(HWND);
void DoFileSave(HWND);
void DoFileNew(HWND);
void DoExit(HWND);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    InitCommonControls();

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_TORCHIC));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_TORCHIC));

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    if (!SetupMDIChildWindowClass(hInstance))
        return 0;

    hwnd = CreateWindowEx(
        0,
        g_szClassName,
        TEXT("Vinh's first text editor"),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    g_hMainWindow = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        if (!TranslateMDISysAccel(g_hMDIClient, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
    return Msg.wParam;
}

BOOL SetupMDIChildWindowClass(HINSTANCE hInstace)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpfnWndProc = MDIWndProc;
    wc.lpszClassName = g_szChildName;
    wc.lpszMenuName = NULL;
    wc.hInstance = hInstace;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Cannot register the window"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
        // create the edit control
        /*{
            HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 100, 100, hwnd, 
                (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
            if (hEdit == NULL)
                MessageBox(hwnd, TEXT("Cannot create edit window"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
            HFONT hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
        }*/
        // create the toolbar and the buttons
        {
            // first the toolbar
            HWND hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd,
                (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
            if (hTool == NULL)
                MessageBox(hwnd, TEXT("Cannot create the toolbar"), TEXT("Error"), MB_OK | MB_ICONERROR);

            // send the TB_BUTTONSTRUCTSIZE message for backward compatibility
            SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

            // then the buttons and images
            TBBUTTON tbb[3];
            TBADDBITMAP tbab;

            // load the predefined images
            tbab.hInst = HINST_COMMCTRL;
            tbab.nID = IDB_STD_SMALL_COLOR;
            SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

            // then the buttons
            ZeroMemory(tbb, sizeof(tbb));
            tbb[0].iBitmap = STD_FILENEW;
            tbb[0].fsState = TBSTATE_ENABLED;
            tbb[0].fsStyle = TBSTYLE_BUTTON;
            tbb[0].idCommand = ID_FILE_NEW;

            tbb[1].iBitmap = STD_FILEOPEN;
            tbb[1].fsState = TBSTATE_ENABLED;
            tbb[1].fsStyle = TBSTYLE_BUTTON;
            tbb[1].idCommand = ID_FILE_OPEN;

            tbb[2].iBitmap = STD_FILESAVE;
            tbb[2].fsState = TBSTATE_ENABLED;
            tbb[2].fsStyle = TBSTYLE_BUTTON;
            tbb[2].idCommand = ID_FILE_SAVE;

            SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);
        }
        // create the status bar
        {
            HWND hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 
                0, 0, 0, 0, hwnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);
            // divide the status bar into sections
            int statWidth[] = {100, -1};
            SendMessage(hStatus, SB_SETPARTS, sizeof(statWidth)/sizeof(int), (LPARAM)statWidth);
            SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)TEXT("Hi there! :)"));
        }
        // create the MDI client
        {
            CLIENTCREATESTRUCT css;
            css.hWindowMenu = GetSubMenu(GetMenu(hwnd), 2);
            css.idFirstChild = ID_MDI_FIRSTCHILD;
            g_hMDIClient = CreateWindowEx(0, TEXT("mdiclient"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN
                | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                hwnd, (HMENU)IDC_MAIN_MDI, GetModuleHandle(NULL), (LPVOID)&css);
        }
        break;
    case WM_SIZE:
        {
            HWND hTool, hStatus, hMDI;
            RECT rcTool, rcStatus;
            HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
            RECT rcClient;

            hTool = GetDlgItem(hwnd, IDC_MAIN_TOOL);
            hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
            hMDI = GetDlgItem(hwnd, IDC_MAIN_MDI);

            // toolbars will auto position themselves when sent the TB_AUTOSIZE message, and status bars will 
            // do the same if you send them WM_SIZE (the common control libraries are not known for consistency)
            SendMessage(hTool, TB_AUTOSIZE, 0, 0);
            SendMessage(hStatus, WM_SIZE, 0, 0);

            GetClientRect(hTool, &rcTool);
            GetClientRect(hStatus, &rcStatus);

            GetClientRect(hwnd, &rcClient);
            SetWindowPos(hMDI, NULL, 0, rcTool.bottom - rcTool.top, rcClient.right, 
                rcClient.bottom - rcStatus.bottom + rcStatus.top - rcTool.bottom + rcTool.top, SWP_NOZORDER);
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_FILE_NEW:
            CreateNewMDIWindow(g_hMDIClient);
            break;
        case ID_FILE_OPEN:
            {
                HWND hChild = CreateNewMDIWindow(g_hMDIClient);
                if (hChild)
                    DoFileOpen(hChild);
            }
            
            break;
        case ID_FILE_CLOSE:
            {
                HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
                if (hChild)
                    SendMessage(hChild, WM_CLOSE, 0, 0);
            }
            break;
        case ID_FILE_EXIT:
            DoExit(hwnd);
            break;
        case ID_WINDOW_TILE:
            SendMessage(g_hMDIClient, WM_MDITILE, 0, 0);
            break;
        case ID_WINDOW_CASCADE:
            SendMessage(g_hMDIClient, WM_MDICASCADE, 0, 0);
            break;
        case ID_HELP_ABOUT:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, DlgProc);
            /*MessageBox(hwnd, TEXT("The incredibly good software you are using\nis made by Vinh (^_^)"), 
                TEXT("Wow! I'm good"), MB_OK | MB_ICONINFORMATION);*/
            break;
        default:
            {
                if (LOWORD(wParam) >= ID_MDI_FIRSTCHILD)
                {
                    DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);
                }
                else
                {
                    HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE, 0, 0);
                    if (hChild)
                    {
                        SendMessage(hChild, msg, wParam, lParam);
                    }
                }
            }
        }
        break;
    case WM_CLOSE:
        DeleteObject(g_hbrBackground);
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MDIWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
        {
            HFONT hfDefault;
            HWND hEdit;

            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
                WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 150, 100, hwnd, 
                (HMENU)IDC_CHILD_EDIT, GetModuleHandle(NULL), NULL);
            if (hEdit == NULL)
                MessageBox(hwnd, TEXT("Cannot create the edit box"), TEXT("Error"), MB_OK | MB_ICONERROR);

            hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hwnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
        }
        break;
    case WM_MDIACTIVATE:
        {
            UINT enableFlag;
            HMENU hMenu, hFileMenu;

            hMenu = GetMenu(g_hMainWindow);
            if (hwnd == (HWND)lParam)
            {
                // this window is activated, enable the menus
                enableFlag = MF_ENABLED;
            }
            else
            {
                // the windows is deactivated, disable the menus
                enableFlag = MF_DISABLED;
            }

            // 1 is Edit, 2 is Window
            EnableMenuItem(hMenu, 1, MF_BYPOSITION | enableFlag);
            EnableMenuItem(hMenu, 2, MF_BYPOSITION | enableFlag);

            hFileMenu = GetSubMenu(hMenu, 0);
            EnableMenuItem(hFileMenu, ID_FILE_SAVE, enableFlag | MF_BYCOMMAND);
            EnableMenuItem(hFileMenu, ID_FILE_CLOSE, enableFlag | MF_BYCOMMAND);
            // apply the changes to the menu bar
            DrawMenuBar(g_hMainWindow);
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_FILE_OPEN:
            DoFileOpen(hwnd); break;
        case ID_FILE_SAVE:
            DoFileSave(hwnd); break;
        case ID_EDIT_COPY:
            SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_COPY, 0, 0); break;
        case ID_EDIT_CUT:
            SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_CUT, 0, 0); break;
        case ID_EDIT_PASTE:
            SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_PASTE, 0, 0); break;
        }
        break;
    case WM_SIZE:
        {
            HWND hEdit;
            RECT rcClient;

            GetClientRect(hwnd, &rcClient);
            hEdit = GetDlgItem(hwnd, IDC_CHILD_EDIT);
            SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
        }
        return DefMDIChildProc(hwnd, msg, wParam, lParam);
    default:
        return DefMDIChildProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CTLCOLORDLG:
        return (LONG)g_hbrBackground;
    case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LONG)g_hbrBackground;
        }
    case WM_INITDIALOG:
        PostMessage(hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_TORCHIC)));
        PostMessage(hwnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_TORCHIC)));
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hwnd, IDOK);
            break;
        }
        break;
    default: return FALSE;
    }
    return TRUE;
}

HWND CreateNewMDIWindow(HWND hMDIClient)
{
    MDICREATESTRUCT mcs;
    HWND hChild;

    ZeroMemory(&mcs, sizeof(MDICREATESTRUCT));
    mcs.hOwner = GetModuleHandle(NULL);
    mcs.szClass = g_szChildName;
    mcs.szTitle = TEXT("[United]");
    mcs.x = mcs.cx = CW_USEDEFAULT;
    mcs.y = mcs.cy = CW_USEDEFAULT;
    mcs.style = MDIS_ALLCHILDSTYLES;

    hChild = (HWND)SendMessage(hMDIClient, WM_MDICREATE, 0, (LONG)&mcs);
    if (!hChild)
        MessageBox(hMDIClient, TEXT("Cannot create child window"), TEXT("Uh oh..."), MB_OK | MB_ICONERROR);
    return hChild;
}

void DoFileOpen(HWND hWnd)
{
    OPENFILENAME ofn;
    TCHAR buf[MAX_PATH] = TEXT("");
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lpstrDefExt = TEXT("txt");
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = TEXT("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.nMaxFile = MAX_PATH;
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = buf;

    if (GetOpenFileName(&ofn))
    {
        if (LoadFileToEdit(GetDlgItem(hWnd, IDC_CHILD_EDIT), buf))
        {
            SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)TEXT("Opened..."));
            SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)buf);

            SetWindowText(hWnd, buf);
        }
        else
            MessageBox(hWnd, TEXT("Cannot open the file"), TEXT("Error"), MB_OK | MB_ICONERROR);
    }
    
}

void DoFileSave(HWND hWnd)
{
    OPENFILENAME ofn;
    TCHAR buf[MAX_PATH] = TEXT("");
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lpstrDefExt = TEXT("txt");
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = TEXT("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.nMaxFile = MAX_PATH;
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = buf;

    if (GetSaveFileName(&ofn))
    {
        if (SaveFile(GetDlgItem(hWnd, IDC_CHILD_EDIT), buf))
        {
            SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)TEXT("Saved..."));
            SendDlgItemMessage(g_hMainWindow, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)buf);

            SetWindowText(hWnd, buf);
        }
        else
            MessageBox(hWnd, TEXT("Cannot save the file"), TEXT("Error"), MB_OK | MB_ICONERROR);
    }
}

void DoFileNew(HWND hWnd)
{
    SetDlgItemText(hWnd, IDC_MAIN_EDIT, NULL);
}

void DoExit(HWND hWnd)
{
    PostMessage(hWnd, WM_CLOSE, 0, 0);
}

bool SaveFile(HWND hEdit, TCHAR* fileName)
{
    HANDLE hFile;
    bool success = false;

    hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLength;
        dwTextLength = GetWindowTextLength(hEdit);
        if (dwTextLength > 0)
        {
            TCHAR* text = (TCHAR*)GlobalAlloc(GPTR, dwTextLength + 1);
            if (text != NULL)
            {
                if (GetWindowText(hEdit, text, dwTextLength))
                {
                    DWORD dwWritten;
                    if(WriteFile(hFile, text, dwTextLength, &dwWritten, NULL))
                        success = true;
                }
                GlobalFree((HGLOBAL)text);
            }
        }
        CloseHandle(hFile);
    }

    return success;
}

bool LoadFileToEdit(HWND hEdit, TCHAR* fileName)
{
    HANDLE hFile;
    bool success = false;

    hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER nFileSize;
        if(GetFileSizeEx(hFile, &nFileSize))
        {
            TCHAR* fileText = (TCHAR*)GlobalAlloc(GPTR, nFileSize.LowPart + 1);
            if (fileText != NULL)
            {
                DWORD dwRead;
                if(ReadFile(hFile, fileText, nFileSize.LowPart, &dwRead, NULL) > 0)
                {
                    fileText[nFileSize.LowPart] = '\0';
                    if (SetWindowText(hEdit, fileText))
                        success = true;
                }
                GlobalFree((HGLOBAL)fileText);
            }
        }
        CloseHandle(hFile);
    }

    return success;
}