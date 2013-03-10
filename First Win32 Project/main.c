#include <windows.h>

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO 9002

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
    wndClass.lpszMenuName = NULL;
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
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
        {
            HMENU hMenu, hSubMenu;
            HICON hIcon, hIconSm;

            hMenu = CreateMenu();
            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, TEXT("E&xit"));
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&File"));

            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, TEXT("G&o"));
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Stuff"));

            SetMenu(hwnd, hMenu);

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
        }
    case WM_LBUTTONDOWN:
        {
            TCHAR szFileName[MAX_PATH];
            HINSTANCE hInstance = GetModuleHandle(NULL);
            GetModuleFileName(hInstance, szFileName, MAX_PATH);
            MessageBox(hwnd, szFileName, TEXT("This program is:"), MB_OK | MB_ICONINFORMATION);
        }
        return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}