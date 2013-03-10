#include <windows.h>
#include "resource.h"

typedef struct 
{
    int width;
    int height;
    int x;
    int y;
    int dx;
    int dy;
}BALLINFO;

const int BALL_MOVE_DELTA = 2;
BALLINFO g_ballInfo;
const int ID_TIMER = 1;

HBITMAP g_hBmp = NULL;
HBITMAP g_hBmpMask = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
    static TCHAR appName[] = TEXT("HelloWin");
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndClass;

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = 0;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
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
        MessageBox(hwnd, TEXT("Damn it!"), TEXT("Uh oh..."), MB_OK | MB_ICONEXCLAMATION);
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
            BITMAP bm;

            g_hBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BALL));
            if (g_hBmp == NULL)
                MessageBox(hwnd, TEXT("Cannot load the bitmap image"), TEXT("Error"), MB_OK|MB_ICONERROR);
            g_hBmpMask = CreateBitmapMask(g_hBmp, RGB(0, 0, 0));
            if (g_hBmpMask == NULL)
                MessageBox(hwnd, TEXT("Cannot create the bitmap mask"), TEXT("Error"), MB_OK|MB_ICONERROR);

            GetObject(g_hBmp, sizeof(bm), &bm);
            ZeroMemory(&g_ballInfo, sizeof(BALLINFO));
            g_ballInfo.width = bm.bmWidth;
            g_ballInfo.height = bm.bmHeight;
            g_ballInfo.dx = g_ballInfo.dy = BALL_MOVE_DELTA;

            UINT ret = SetTimer(hwnd, ID_TIMER, 50, NULL);
            if (ret == 0)
                MessageBox(hwnd, TEXT("Cannot create the timer"), TEXT("Error"), MB_OK | MB_ICONERROR);
        }
        return 0;
    case WM_TIMER:
        {
            RECT rcClient;
            HDC hdc = GetDC(hwnd);
            GetClientRect(hwnd, &rcClient);
            
            // update ball
            g_ballInfo.x += g_ballInfo.dx;
            g_ballInfo.y += g_ballInfo.dy;

            if (g_ballInfo.x + g_ballInfo.width > rcClient.right || g_ballInfo.x < 0)
            {
                g_ballInfo.dx = -g_ballInfo.dx;
                g_ballInfo.x += g_ballInfo.dx;
            }
            if (g_ballInfo.y < 0 || g_ballInfo.y + g_ballInfo.height > rcClient.bottom)
            {
                g_ballInfo.dy = -g_ballInfo.dy;
                g_ballInfo.y += g_ballInfo.dy;
            }

            // draw ball
            // MSDN: This function creates a memory device context (DC) compatible with the specified device.
            HDC hdcMem = CreateCompatibleDC(hdc);
            HDC hdcBuffer = CreateCompatibleDC(hdc);
            HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
            HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
            HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, g_hBmp);
            // do the painting
            FillRect(hdcBuffer, &rcClient, (HBRUSH)GetStockObject(WHITE_BRUSH));

            SelectObject(hdcMem, g_hBmpMask);
            BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCAND);
            SelectObject(hdcMem, g_hBmp);
            BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCPAINT);
            BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcBuffer, 0, 0, SRCCOPY);

            // do the cleaning
            // restore the memory DC
            SelectObject(hdcBuffer, hbmOldBuffer);
            DeleteDC(hdcBuffer);
            SelectObject(hdcMem, hBmpOld);
            DeleteDC(hdcMem);
            DeleteObject(hbmBuffer);
            ReleaseDC(hwnd, hdc);
        }
        return 0;
    case WM_PAINT:
        {
            BITMAP bmp;
            PAINTSTRUCT ps;
            HDC hdcWindow, hdcMem;

            hdcWindow = BeginPaint(hwnd, &ps);
            // MSDN: This function creates a memory device context (DC) compatible with the specified device.
            hdcMem = CreateCompatibleDC(hdcWindow);
            HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, g_hBmp);
            // do the painting
            // get the bitmap information to variable bmp
            GetObject(g_hBmp, sizeof(bmp), &bmp);
            BitBlt(hdcWindow, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, g_hBmpMask);
            BitBlt(hdcWindow, 0, bmp.bmHeight, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCAND);
            SelectObject(hdcMem, g_hBmp);
            BitBlt(hdcWindow, 0, bmp.bmHeight, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCPAINT);

            // do the cleaning
            // restore the memory DC
            SelectObject(hdcMem, hBmpOld);
            DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
        }
        return 0;
    case WM_COMMAND:
        return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY: 
        KillTimer(hwnd, ID_TIMER);
        DeleteObject(g_hBmp);
        DeleteObject(g_hBmpMask);
        PostQuitMessage(0); 
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMemColor, hdcMemMask;
    HBITMAP hbmMask;
    BITMAP bm;

    GetObject(g_hBmp, sizeof(bm), &bm);

    hdcMemColor = CreateCompatibleDC(NULL);
    hdcMemMask = CreateCompatibleDC(NULL);

    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    SelectObject(hdcMemColor, hbmColour);
    SelectObject(hdcMemMask, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMemColor, crTransparent);
    BitBlt(hdcMemMask, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemColor, 0, 0, SRCCOPY);
    // now we had the mask
    // if the background color in the original image isn't black, we must set it to black to make the
    // transparent effect work right
    BitBlt(hdcMemColor, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemMask, 0, 0, SRCINVERT);

    DeleteDC(hdcMemColor);
    DeleteDC(hdcMemMask);
    
    return hbmMask;
}
