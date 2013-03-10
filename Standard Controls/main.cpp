#include <windows.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DlgProc);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hWnd, IDC_EDIT_TEXT, TEXT("This is a string"));
        SetDlgItemInt(hWnd, IDC_EDIT_TIMES, 5, FALSE);
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_ADD:
            {
                //get the number n the user entered, then add the string n times
                BOOL bSuccess;
                int times = GetDlgItemInt(hWnd, IDC_EDIT_TIMES, &bSuccess, FALSE);
                if (bSuccess)
                {
                    // get the string they entered
                    // first we must know the string's length
                    int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_EDIT_TEXT));
                    if (len > 0)
                    {
                        TCHAR *buf;
                        buf = (TCHAR*)GlobalAlloc(GPTR, len + 1);
                        GetDlgItemText(hWnd, IDC_EDIT_TEXT, buf, len + 1);
                        // now we can add the string n times to the listbox
                        for (int i = 0; i<times; i++)
                        {
                            int index = SendDlgItemMessage(hWnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)buf);
                            // associate some data to each item just for...fun
                            SendDlgItemMessage(hWnd, IDC_LIST, LB_SETITEMDATA, (WPARAM)index, (LPARAM)times);
                        }
                        // finally free the buffer
                        GlobalFree((HGLOBAL)buf);
                    }
                    else
                        MessageBox(hWnd, TEXT("This is not a text"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
                }
                else
                    MessageBox(hWnd, TEXT("You must enter a positive number"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
            }
            break;
        case IDC_REMOVE:
            // first get the number of the selected items
            {
                HWND hList = GetDlgItem(hWnd, IDC_LIST);
                int count = SendMessage(hWnd, LB_GETSELCOUNT, 0, 0);
                if (count != LB_ERR)
                {
                    if (count > 0)
                    {
                        int *buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
                        SendMessage(hWnd, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);
                        // now we remove the items
                        // loop BACKWARD
                        for (int i = count-1; i>=0; i--)
                        {
                            SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
                        }
                        // free the buffer
                        GlobalFree((HGLOBAL)buf);
                    }
                    else
                        MessageBox(hWnd, TEXT("No item selected"), TEXT("Error"), MB_OK|MB_ICONEXCLAMATION);
                }
                else
                {
                    MessageBox(hWnd, TEXT("Error counting the items"), TEXT("Error"), MB_OK|MB_ICONEXCLAMATION);
                }
            }
            break;
        case IDC_CLEAR:
            SendDlgItemMessage(hWnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
            break;
        case IDC_LIST:
            switch(HIWORD(wParam))
            {
            case LBN_SELCHANGE:
                {
                    // get the number of items selected
                    int count = SendDlgItemMessage(hWnd, IDC_LIST, LB_GETSELCOUNT, 0, 0);
                    if (count == 1)
                    {
                        int index;
                        SendDlgItemMessage(hWnd, IDC_LIST, LB_GETSELITEMS, (WPARAM)1, (LPARAM)&index);
                        int data = SendDlgItemMessage(hWnd, IDC_LIST, LB_GETITEMDATA, (WPARAM)index, 0);
                        SetDlgItemInt(hWnd, IDC_SHOWTIME, data, FALSE);
                    }
                    else
                        SetDlgItemText(hWnd, IDC_SHOWTIME, TEXT("-"));
                }
                
            }
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hWnd, 0); break;
    default: return FALSE;
    }
    return TRUE;
}