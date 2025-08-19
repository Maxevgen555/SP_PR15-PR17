#include <windows.h>
#include "SP_PR15.h"
#include <commdlg.h>
#include <ctime>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#define HANDLE_MSG(hwnd, message, fn) case (message): return (fn)((hwnd), (wParam), (lParam))

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                    LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)),
                    LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1),
                    MAKEINTRESOURCE(IDR_MENU1), L"MainWndClass" };

    RegisterClass(&wc);

    HWND hWnd = CreateWindow(L"MainWndClass", L"SP_PR5",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600, nullptr, nullptr, hInst, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_FILE_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_VIEW_TEXT:
            MessageBox(hWnd, L"Выбрана команда Текст", L"Меню", MB_OK);
            // DialogBox(...);
            break;
        case IDM_VIEW_CTL:
            MessageBox(hWnd, L"Выбрана команда Элементы управления", L"Меню", MB_OK);
            // DialogBox(...);
            break;
        case IDM_HELP_ABOUT:
            DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
            break;
        }
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // Инициализация при создании окна
    return 0;
}
LRESULT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // Пример вывода текста в центре окна
    RECT rect;
    GetClientRect(hWnd, &rect);
    DrawText(hdc, L"SP_PR5 — Win32 API лабораторная работа", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    EndPaint(hWnd, &ps);
    return 0;
}
LRESULT OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    PostQuitMessage(0);
    return 0;
}
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        wchar_t buffer[64];
        time_t now = time(nullptr);
        tm local;
        localtime_s(&local, &now);
        wcsftime(buffer, 64, L"%d.%m.%Y %H:%M:%S", &local);
        SetDlgItemText(hDlg, IDC_TIME_STATIC, buffer);
        return TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        break;
    }
    return FALSE;
}