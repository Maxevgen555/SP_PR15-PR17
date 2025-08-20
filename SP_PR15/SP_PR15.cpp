#include <windows.h>          // �������� WinAPI ������� � ����
#include <windowsx.h>         // �������-����������� ��������� (��������, HANDLE_MSG)
#include <tchar.h>            // ��������� TCHAR ��� Unicode/ANSI
#include <winuser.h>          // ���������������� ������� WinAPI (���. � windows.h)
#include <commdlg.h>          // ����� ���������� ���� (��������, �������� �����)
#include <string>             // STL string (std::wstring)
#include <vector>             // STL vector
#include "resource.h"         // �������������� �������� �� ������ �������

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
// ��������� ������� ������������ ������� ����������, ����� ����� wWinMainCRTStartup
// ��� ��������� ������������ Unicode ������ WinMain (_tWinMain)


// ��������� ������� ��������� ������� ��������� � ������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
void OnDestroy(HWND hWnd);
void OnPaint(HWND hWnd);
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, UINT flags);

// ��������� �������� ��������� ��������
INT_PTR CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK TextViewerProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ControlsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ModelessProc(HWND, UINT, WPARAM, LPARAM);


// ���������� ����������
LPCTSTR g_lpszApplicationTitle = TEXT("SP_PR15 - ���������� ����");
LPCTSTR g_lpszClassName = TEXT("sp_pr15_class");
HINSTANCE g_hInst;

static HMENU g_hEditMenu = NULL;    // ���� "������"
static HMENU g_hFileMenu = NULL;    // ���� "����"

// ��� ������������ �������
HWND g_hModelessDlg = NULL;

// ���������� ������ ����� (��� �������� ��������� ������)
std::vector<std::wstring> g_itemsList;

// ����� ��� ������ (�����������, 10000 ��������)
TCHAR g_szBuffer[10000] = L"���� 2";


// ����� ����� ��������� (Unicode ������)
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPTSTR lpszCmdLine, int nCmdShow)
{
    g_hInst = hInstance;  // ��������� ���������� ���������� ����������

    WNDCLASSEX wc = { 0 };
    MSG msg;
    HWND hWnd;

    memset(&wc, 0, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = g_lpszClassName;      // ��� ������ ����
    wc.lpfnWndProc = WndProc;                 // �������-���������� ����
    wc.style = CS_VREDRAW | CS_HREDRAW;      // �������������� ��� ��������� ������� (��� � ���)
    wc.hInstance = hInstance;                 // ��������� ����������
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));    // ������ ����
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1),
        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);                      // ��������� ������
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);                     // ����������� ������
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));      // ��� ���� (������-�����)
    wc.lpszMenuName = NULL;                 // ���� ������������� ����� ����� CreateWindowEx
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    // ������������ ����� ���� ��
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("������ ����������� ������ ����!"),
            TEXT("������"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // ��������� ������� ���� �� ��������
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));

    // ������� �������� ����
    hWnd = CreateWindowEx(
        NULL,
        g_lpszClassName,
        g_lpszApplicationTitle,
        WS_OVERLAPPEDWINDOW,
        0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        hMenu,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL, TEXT("���� �� �������!"),
            TEXT("������"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // ��������� ������������ (������� �������) �� ��������
    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    ShowWindow(hWnd, nCmdShow);     // ���������� ����
    UpdateWindow(hWnd);             // ��������� ����, ������� WM_PAINT

    // ������� ���� ��������� ���������
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // ���� ���� ����������� ������ � ��������� �������� ��� �������,
        // �� ������� ��� ������. ����� � ��������� ������� �������
        if (g_hModelessDlg == NULL || !IsDialogMessage(g_hModelessDlg, &msg))
        {
            if (!TranslateAccelerator(hWnd, hAccel, &msg))  // ��������� �����������
            {
                TranslateMessage(&msg);  // ������� ��������� ����������
                DispatchMessage(&msg);   // �������� ��������� � ���� (WndProc)
            }
        }
    }

    return (int)msg.wParam; // ������� ���� ����������
}

// �������� ������� ������� - ���������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);                 // ��� �������� ����
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);               // ��� ���������� ������� (����, ������)
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);               // ��� ����������� ����
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);                   // ����������� ����
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);       // ����� ���� ����
        HANDLE_MSG(hWnd, WM_RBUTTONDOWN, OnRButtonDown);       // ������ ���� ����

    case WM_MENUSELECT:
        // ��������� ������ ���� � ��������� ���������
        OnMenuSelect(hWnd, (HMENU)lParam, LOWORD(wParam), HIWORD(wParam));
        return 0;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);       // ��� ��������� ��������� - ��������� ���������
    }
    return 0;
}

// ���������� ������� �������� ����
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    // �������� ������ "�����" � ����
    HWND hBtExit = CreateWindowEx(0, TEXT("Button"), TEXT("�����"),
        WS_CHILD | WS_VISIBLE,
        50, 150, 100, 30,
        hWnd, (HMENU)(IDCANCEL), g_hInst, NULL);
    if (!hBtExit) return -1;

    // �������� ������� ���� � ������� ��� ����������� ����������
    HMENU hMainMenu = GetMenu(hWnd);
    g_hFileMenu = GetSubMenu(hMainMenu, 0);  // ������ ������� - ����
    g_hEditMenu = GetSubMenu(hMainMenu, 1);  // ������ - ������

    // ��������� ����� "������� ��������" � ���� ���� �� ������� 3 (��������� �����)
    InsertMenu(g_hFileMenu, 3, MF_BYPOSITION | MF_STRING, IDM_FILE_CLOSE, TEXT("������� ��������"));
    // ���������� ����� ���������� (�����)
    EnableMenuItem(g_hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED);

    // ���������� ��� ������ ���� "������" ������ ������������ (�����)
    for (int i = 0; i < 4; i++) {
        EnableMenuItem(g_hEditMenu, i, MF_BYPOSITION | MF_GRAYED);
    }

    // �������������� ����� �������� �������
    wcscpy_s(g_szBuffer, L"����2");

    // ��������� ������ ����������������� ��������
    g_itemsList.push_back(L"������ ������");
    //g_itemsList.push_back(L"������ ������ 2");
    //g_itemsList.push_back(L"������ ������ 3");

    return TRUE;
}

// ��������� ������ ����, ������ � �.�.
void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDCANCEL:
    case IDM_FILE_EXIT:
        DestroyWindow(hWnd);     // �������� �������� ���� => ���������� ���������
        break;

    case IDM_FILE_NEW:
        MessageBox(hWnd, TEXT("������ ����� ��������"), TEXT("���� ����"), MB_OK);
        // ������������ ������ ����
        EnableMenuItem(g_hEditMenu, IDM_EDIT_SELECT, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(g_hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED);
        break;

    case IDM_FILE_OPEN:
        MessageBox(hWnd, TEXT("������� ������� '�������'"), TEXT("���� ����"), MB_OK);
        break;

    case IDM_EDIT_SELECT:
        MessageBox(hWnd, TEXT("����� �������"), TEXT("���� ������"), MB_OK);
        EnableMenuItem(g_hEditMenu, IDM_EDIT_COPY, MF_BYCOMMAND | MF_ENABLED);
        break;

    case IDM_EDIT_COPY:
        MessageBox(hWnd, TEXT("����� ����������"), TEXT("���� ������"), MB_OK);
        break;

    case IDM_FILE_CLOSE:
        MessageBox(hWnd, TEXT("�������� ������"), TEXT("���� ����"), MB_OK);
        // ��������� ������ ���� "������" � "������� ��������"
        for (int i = 0; i < 4; i++) {
            EnableMenuItem(g_hEditMenu, i, MF_BYPOSITION | MF_GRAYED);
        }
        EnableMenuItem(g_hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED);
        break;

    case IDM_HELP_HELP:
        MessageBox(hWnd, TEXT("������� ������� '������'"), TEXT("���� �������"), MB_OK);
        break;

    case IDM_HELP_ABOUT:
        // ���������� ��������� ������ "� ���������"
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutProc);
        break;

    case IDM_VIEW_TEXT:
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TEXTVIEWER), hWnd, TextViewerProc);
        break;

    case IDM_VIEW_CTL:
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONTROLS), hWnd, ControlsProc);
        InvalidateRect(hWnd, NULL, TRUE); // ������������ ����
        break;

    case IDM_VIEW_MODELESS:
        if (g_hModelessDlg == NULL)
        {
            // ������� ����������� ������, ���� ��� �� ������
            g_hModelessDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_MODELESS), hWnd, ModelessProc);
            if (g_hModelessDlg == NULL)
            {
                MessageBox(hWnd, TEXT("�� ������� ������� ����������� ������"), TEXT("������"), MB_OK);
            }
        }
        else
        {
            // ���� ������ - ���������� � ������ �� �������� ����
            SetForegroundWindow(g_hModelessDlg);
        }
        break;
    }
}

// ��������� ���������� �������� ����
void OnDestroy(HWND hWnd)
{
    PostQuitMessage(0);   // ����� �� ����� ��������� � ���������� ����������
}

// ��������� ����������� ����
void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT rect;
    GetClientRect(hWnd, &rect);

    // ������� ������ ����� �� ����������� ������� � ���������� 20 �� ���������
    int y = 50;
    for (const auto& item : g_itemsList)
    {
        TextOut(hdc, 20, y, item.c_str(), (int)item.length());
        y += 20;
    }

    // ������� ���������� � ��������� ������ ����
    LPCTSTR infoText = TEXT("������������ ������ 15: ���������� ����");
    TextOut(hdc, 20, 10, infoText, lstrlen(infoText));

    EndPaint(hWnd, &ps);
}

// ���� ����� ������� �� ����
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    HDC hdc = GetDC(hWnd);
    RECT rect = { x, y, 0, 0 };

    LPCTSTR clickText = TEXT("������ ����� ������� ����");
    DrawText(hdc, clickText, lstrlen(clickText), &rect, DT_LEFT);
    ReleaseDC(hWnd, hdc);
}

// ���� ������ ������� � �������� � ����� ������������ ����
void OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    HMENU hContextMenu = CreatePopupMenu();
    AppendMenu(hContextMenu, MF_STRING, IDM_EDIT_SELECT, TEXT("��������"));
    AppendMenu(hContextMenu, MF_STRING, IDM_EDIT_COPY, TEXT("����������"));
    AppendMenu(hContextMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hContextMenu, MF_STRING, IDM_VIEW_TEXT, TEXT("�������� ������"));
    AppendMenu(hContextMenu, MF_STRING, IDM_VIEW_CTL, TEXT("�������� ����������"));

    // �������� ������� ��������� ������� �� �������� ����, ����� ���������������� �����������
    UINT selectState = GetMenuState(g_hEditMenu, IDM_EDIT_SELECT, MF_BYCOMMAND);
    UINT copyState = GetMenuState(g_hEditMenu, IDM_EDIT_COPY, MF_BYCOMMAND);

    // �������������� ������������� ���������� ��� ���������� ������� � ����������� ����
    EnableMenuItem(hContextMenu, IDM_EDIT_SELECT,
        (selectState & MF_GRAYED) ? MF_BYCOMMAND | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED);

    EnableMenuItem(hContextMenu, IDM_EDIT_COPY,
        (copyState & MF_GRAYED) ? MF_BYCOMMAND | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED);

    POINT pt = { x, y };
    ClientToScreen(hWnd, &pt);   // ������������ ���������� ���������� ������� � ��������

    // ���������� ����������� ����, ����������� � ������� �����
    TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, hWnd, NULL);

    DestroyMenu(hContextMenu);
}

// ��������� ��������� ������ ���� � ������������ ��� ������ ����������/���������
void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, UINT flags)
{
    HDC hdc = GetDC(hWnd);
    RECT rc;
    GetClientRect(hWnd, &rc);

    // ������� ������� ���������
    LPCTSTR clearStr = TEXT("                                        ");
    TextOut(hdc, 10, rc.bottom - 30, clearStr, lstrlen(clearStr));

    // ���� ������ ������� ����� ���� (�� ����������� � �� ���������� ����)
    if (item > 0 && !(flags & (MF_SEPARATOR | MF_POPUP)))
    {
        TCHAR helpText[256];
        // ��������� ������-��������� �� �������� �� ID ������ ����
        if (LoadString(g_hInst, item, helpText, 256) > 0)
        {
            TextOut(hdc, 10, rc.bottom - 30, helpText, lstrlen(helpText));
        }
    }
    ReleaseDC(hWnd, hdc);
}

// ���������� ���������� ������� "� ���������"
INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // ��������� � ������������� ������ �������
        HICON hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
        SendDlgItemMessage(hDlg, IDC_APP_ICON, STM_SETICON, (WPARAM)hIcon, 0);

        // ���������� ������� ��������� ����� � �������� IDC_CURRENT_TIME
        SYSTEMTIME st;
        GetLocalTime(&st);
        TCHAR timeStr[50];
        wsprintf(timeStr, TEXT("%02d.%02d.%04d %02d:%02d:%02d"),
            st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
        SetDlgItemText(hDlg, IDC_CURRENT_TIME, timeStr);
    }
    return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam)); // �������� �������
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ���������� ������� ��������� ������
INT_PTR CALLBACK TextViewerProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_EDIT_CONTENT, TEXT("������ ������")); // ������������� ������
        SetFocus(GetDlgItem(hDlg, IDC_LOAD_BUTTON)); // ����� �� ������ ��������
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_LOAD_BUTTON)
        {
            // ��������� ������ ������ �����
            OPENFILENAME ofn;
            TCHAR szFile[260] = { 0 };

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = TEXT("��������� �����\0*.txt\0��� �����\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                // ��������� ��������� ���� ��� ������
                HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD dwBytesRead;
                    // ������ ���������� ����� � �����
                    ReadFile(hFile, g_szBuffer, sizeof(g_szBuffer) - sizeof(TCHAR), &dwBytesRead, NULL);
                    g_szBuffer[dwBytesRead / sizeof(TCHAR)] = 0;  // ��������� ������ ����
                    SetDlgItemText(hDlg, IDC_EDIT_CONTENT, g_szBuffer); // ������� ���������� � �������
                    CloseHandle(hFile);
                }
            }
        }
        else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));  // �������� �������
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ���������� ������� � ���������� ����������
INT_PTR CALLBACK ControlsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_READ_BUTTON:
        {
            TCHAR szText[256];
            // ������ ����� �� ���� �����
            GetDlgItemText(hDlg, IDC_EDIT_TEXT, szText, 256);
            // �������� � ���������� �����
            wcscpy_s(g_szBuffer, szText);
        }
        break;
        case IDC_ADD_BUTTON:
        {
            TCHAR szText[256];
            // ������ ����� �� ���� �����
            GetDlgItemText(hDlg, IDC_EDIT_TEXT, szText, 256);
            // ��������� ��� ������ � ������ ��������� ListBox
            SendDlgItemMessage(hDlg, IDC_LIST_ITEMS, LB_ADDSTRING, 0, (LPARAM)szText);
        }
        break;
        case IDOK:
        {
            // ��� ������� �� ��������� ��� �������� �� ListBox � ���������� ������
            g_itemsList.clear();
            int count = SendDlgItemMessage(hDlg, IDC_LIST_ITEMS, LB_GETCOUNT, 0, 0);
            for (int i = 0; i < count; i++)
            {
                TCHAR szItem[256];
                SendDlgItemMessage(hDlg, IDC_LIST_ITEMS, LB_GETTEXT, i, (LPARAM)szItem);
                g_itemsList.push_back(szItem);
            }
            EndDialog(hDlg, IDOK);
        }
        return (INT_PTR)TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ���������� ������������ �������
INT_PTR CALLBACK ModelessProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hDlg);        // ���������� ���� ������� ��� ��������
            g_hModelessDlg = NULL;      // ������� ���������� �����
            return (INT_PTR)TRUE;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        g_hModelessDlg = NULL;
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}
