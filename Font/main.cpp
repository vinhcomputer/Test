#include <windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";
HFONT g_hfFont;
BOOL g_bOpaque = FALSE;
COLORREF g_rgbText = RGB(0, 0, 0);
COLORREF g_rgbBackground = RGB(255, 255, 255);
COLORREF g_rgbCustom[16] = {0};

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawClientSize(HDC hdc, RECT *pRect, HFONT hf);
void DoSelectFont(HWND hwnd);
void DoSelectColor(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Drawing fonts",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
        {
            g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_FORMAT_OPAQUE:
            g_bOpaque = !g_bOpaque;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case ID_FORMAT_DEFAULTGUIFONT:
            DeleteObject(g_hfFont);
            g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case ID_FORMAT_TIMESNEWROMAN:
            {
                HFONT hf;
                HDC hdc;
                long lfHeight;

                hdc = GetDC(NULL);
                lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
                ReleaseDC(hwnd, hdc);

                hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0 , 0, 0, 0, 0, 0, TEXT("Times New Roman"));
                if (hf)
                {
                    DeleteObject(g_hfFont);
                    g_hfFont = hf;
                }
                else
                {
                    MessageBox(hwnd, TEXT("Cannot create font"), TEXT("Error"), MB_OK|MB_ICONERROR);
                }

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            break;
        case ID_FORMAT_FONT:
            DoSelectFont(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case ID_FORMAT_BACKGROUNDCOLOR:
            DoSelectColor(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        break;
    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rc;
            GetClientRect(hwnd, &rc);
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawClientSize(hdc, &rc, g_hfFont);
            EndPaint(hwnd, &ps);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void DrawClientSize(HDC hdc, RECT *pRect, HFONT hf)
{
    HFONT hfOld = (HFONT)SelectObject(hdc, hf);
    SetBkColor(hdc, g_rgbBackground);
    SetTextColor(hdc, g_rgbText);

    if (g_bOpaque)
    {
        SetBkMode(hdc, OPAQUE);
    }
    else
        SetBkMode(hdc, TRANSPARENT);

    DrawText(hdc, TEXT("These are the dimension of your client area"), -1, pRect, DT_WORDBREAK);
    TCHAR szSize[100];
    wsprintf(szSize, "{%d, %d, %d, %d}", pRect->left, pRect->top, pRect->right, pRect->bottom);
    DrawText(hdc, szSize, -1, pRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    
    SelectObject(hdc, hfOld);
}

void DoSelectFont(HWND hwnd)
{
    CHOOSEFONT cf;
    LOGFONT lf;
    GetObject(g_hfFont, sizeof(lf), &lf);
    //ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(CHOOSEFONT);
    cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.rgbColors = g_rgbText;
    if(ChooseFont(&cf))
    {
        HFONT hf = CreateFontIndirect(&lf);
        if(hf)
            g_hfFont = hf;
        else
            MessageBox(hwnd, TEXT("Font creation failed!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        g_rgbText = cf.rgbColors;
    }
}

void DoSelectColor(HWND hwnd)
{
    CHOOSECOLOR cc;
    cc.hwndOwner = hwnd;
    cc.lStructSize = sizeof(cc);
    cc.rgbResult = g_rgbBackground;
    cc.lpCustColors = g_rgbCustom;
    cc.Flags = CC_RGBINIT | CC_ANYCOLOR || CC_FULLOPEN;

    if (ChooseColor(&cc))
    {
        g_rgbBackground = cc.rgbResult;
    }
}