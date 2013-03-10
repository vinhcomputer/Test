#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK About2DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hToolbar = NULL;

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
    static TCHAR appName[] = TEXT("HelloWin");
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndClass;

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = 0;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wndClass.lpszClassName = appName;

    if (!RegisterClassEx(&wndClass))
    {
        return 1;
    }
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, appName, TEXT("Hello Windows 7"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 240, 120, NULL, NULL, hInstance,
        NULL);
    if (hwnd == NULL)
    {
        MessageBox(hwnd, TEXT("Damn it!"), TEXT("Holy shit!"), MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }
    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, hwnd, 0, 0) > 0)
    {
        if (!IsDialogMessage(g_hToolbar, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    switch(message)
    {
    case WM_CREATE:
        {
            // create modeless dialog box
            g_hToolbar = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT2), hwnd, About2DlgProc);
            if (g_hToolbar != NULL)
                ShowWindow(g_hToolbar, SW_SHOW);
            else
                MessageBox(hwnd, TEXT("CreateDialog returned NULL"), TEXT("Error happened"), MB_OK | MB_ICONERROR);

        
            //HMENU hMenu, hSubMenu;
            HICON hIcon, hIconSm;

            //hMenu = CreateMenu();
            //hSubMenu = CreatePopupMenu();
            //AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, TEXT("E&xit"));
            //AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&File"));

            //hSubMenu = CreatePopupMenu();
            //AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, TEXT("G&o"));
            //AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Stuff"));
            //
            //hSubMenu = CreatePopupMenu();
            //AppendMenu(hSubMenu, MF_STRING, ID_ABOUT_MODAL, TEXT("&Modal"));
            //AppendMenu(hSubMenu, MF_STRING, ID_ABOUT_MODELESS_SHOW, TEXT("&Show Modeless dialog"));
            //AppendMenu(hSubMenu, MF_STRING, ID_ABOUT_MODELESS_HIDE, TEXT("&Hide Modeless dialog"));
            //AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&About"));

            //SetMenu(hwnd, hMenu);

            hIcon = (HICON)LoadImage(NULL, TEXT("bulbasaur.ico"), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            if (hIcon)
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            else
                MessageBox(hwnd, TEXT("Could not load icon image"), TEXT("Error occurred"), MB_OK | MB_ICONERROR);

            hIconSm = (HICON)LoadImage(NULL, TEXT("bulbasaur.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            if (hIconSm)
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
            else
                MessageBox(hwnd, TEXT("Could not load icon image"), TEXT("Error occurred"), MB_OK | MB_ICONERROR);
        }
        return 0;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_FILE_EXIT: PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_STUFF_GO: 
            PostMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
            break;
        case ID_ABOUT_MODAL:
            {
                int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
                if (ret == IDOK)
                {
                    MessageBox(hwnd, TEXT("Dialog exited with IDOK"), TEXT("Notice"), MB_OK);
                }
                else if (ret == IDCANCEL)
                {
                    MessageBox(hwnd, TEXT("Dialog exited with IDCANCEL"), TEXT("Notice"), MB_OK);
                }
            }
            break;
        case ID_ABOUT_SHOW:
            ShowWindow(g_hToolbar, SW_SHOW); break;
        case ID_ABOUT_HIDE:
            ShowWindow(g_hToolbar, SW_HIDE); break;
        }
        return 0;
    case WM_LBUTTONDOWN:
        {
            TCHAR szFileName[MAX_PATH];
            HINSTANCE hInstance = GetModuleHandle(NULL);
            GetModuleFileName(hInstance, szFileName, MAX_PATH);
            MessageBox(hwnd, szFileName, TEXT("This program is:"), MB_OK | MB_ICONINFORMATION);
        }
        return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        DestroyWindow(g_hToolbar);
        PostQuitMessage(0); 
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_INITDIALOG: break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hWnd, IDOK); 
            break;
        case IDCANCEL: 
            EndDialog(hWnd, IDCANCEL);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK About2DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_PRESS: MessageBox(hWnd, TEXT("Hi!"), TEXT("This is a message"), MB_OK); break;
        case IDC_OTHER: MessageBox(hWnd, TEXT("Bye! ^_^"), TEXT("This is also a message"), MB_OK); break;
        }
        break;
    default: return FALSE;
    }
    return TRUE;
}