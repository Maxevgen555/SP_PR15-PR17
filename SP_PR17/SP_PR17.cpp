#include <windows.h>
#include <tchar.h>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")

// ���������
#define IDM_SAVE_METAFILE 1001
#define IDM_VIEW_IMAGE    1002

// ���������� ����������
HINSTANCE hInst;
TCHAR szClassName[] = _T("GraphicOutputApp");
TCHAR szChildClassName[] = _T("ChildWindow");

// ��������� �������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
void DrawContent(HDC hdc, int x, int y);
void SaveAsMetafile(HWND hWnd);
void DisplayImage(HWND hWnd);

// ����� �����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szClassName;

    if (!RegisterClassEx(&wc)) return 0;

    HWND hWnd = CreateWindow(szClassName, _T("����������� ����� - ������� 3"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600, NULL, NULL, hInstance, NULL);
    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// ��������� �������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, IDM_SAVE_METAFILE, _T("��������� ��� ��������"));
        AppendMenu(hFileMenu, MF_STRING, IDM_VIEW_IMAGE, _T("����������� �����������"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, _T("����"));
        SetMenu(hWnd, hMenu);
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case IDM_SAVE_METAFILE:
            SaveAsMetafile(hWnd);
            break;
        case IDM_VIEW_IMAGE:
            DisplayImage(hWnd);
            break;
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ������ ����������
        DrawContent(hdc, 50, 50);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ������� ��������� �����������
void DrawContent(HDC hdc, int x, int y) {
    // ������� � �������� �����
    HFONT hFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, _T("Times New Roman"));
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // ������������� ���� ������ (�������)
    SetTextColor(hdc, RGB(0, 255, 0));

    // ������� ����� (�������� �� ���� ���)
    TextOut(hdc, x, y, _T("������ ���� ��������"), 19);

    // ������ ������ 2
    int A = 60;
    int R = A / 2; // 30
    int B = 60;

    // ������� ���� ��� ������� (�����, ������� 2)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // ������� ����� ��� ������� (�������)
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // ������ ��� ���������� � ����������� �����
    int x1 = x;
    int y1 = y + 50;

    // ����� ����������
    Ellipse(hdc, x1, y1, x1 + 2 * R, y1 + 2 * R);

    // ������ ����������
    int x2 = x1 + A + 2 * R;
    Ellipse(hdc, x2, y1, x2 + 2 * R, y1 + 2 * R);

    // ����������� �����
    MoveToEx(hdc, x1 + R, y1 + R, NULL);
    LineTo(hdc, x2 + R, y1 + R);

    // ��������������� ��������
    SelectObject(hdc, hOldFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // ������� GDI �������
    DeleteObject(hFont);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

// ������� ���������� ��� ���������
void SaveAsMetafile(HWND hWnd) {
    HDC hMetaDC = CreateEnhMetaFile(NULL, _T("output.emf"), NULL, _T("Graphic Output"));

    if (hMetaDC) {
        DrawContent(hMetaDC, 50, 50);
        HENHMETAFILE hemf = CloseEnhMetaFile(hMetaDC);

        if (hemf) {
            MessageBox(hWnd, _T("�������� ������� �������� ��� output.emf"), _T("�����"), MB_OK);
            DeleteEnhMetaFile(hemf);
        }
        else {
            MessageBox(hWnd, _T("������ ��� ���������� ���������"), _T("������"), MB_OK | MB_ICONERROR);
        }
    }
}

// ������� ����������� �����������
void DisplayImage(HWND hWnd) {
    // ������������ ����� ��������� ����
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ChildWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szChildClassName;

    RegisterClassEx(&wc);

    // ������� �������� ����
    HWND hChild = CreateWindow(szChildClassName, _T("�����������"),
        WS_OVERLAPPEDWINDOW | WS_CHILD,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300, hWnd, NULL, hInst, NULL);
    if (hChild) {
        ShowWindow(hChild, SW_SHOW);
        UpdateWindow(hChild);
    }
}

// ��������� ��������� ���� ��� ����������� �����������
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ��������� ����������� (�������� �� ���� � ������ �����������)
        HBITMAP hBmp = (HBITMAP)LoadImage(NULL, _T("image.bmp"),
            IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBmp) {
            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, hBmp);

            BITMAP bmp;
            GetObject(hBmp, sizeof(BITMAP), &bmp);

            // ������� �����������
            BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight,
                hMemDC, 0, 0, SRCCOPY);

            DeleteDC(hMemDC);
            DeleteObject(hBmp);
        }
        else {
            // ���� ����������� �� ���������, ������� ���������
            TextOut(hdc, 10, 10, _T("����������� �� �������"), 20);
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}