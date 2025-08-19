#include <windows.h>
#include <tchar.h>
#include <math.h>
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")

// ��������� ��� ����
#define IDM_SAVE_METAFILE 1001
#define IDM_VIEW_IMAGE    1002

// ���������� ����������
HINSTANCE hInst;  // ���������� ���������� ����������
TCHAR szClassName[] = _T("GraphicOutputApp");  // ��� ������ �������� ����
TCHAR szChildClassName[] = _T("ChildWindow");  // ��� ������ ��������� ����

// ��������� �������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // ��������� �������� ����
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);  // ��������� ��������� ����
void DrawContent(HDC hdc, int x, int y);  // ������� ��������� �����������
void SaveAsMetafile(HWND hWnd);  // ������� ���������� ��� ���������
void DisplayImage(HWND hWnd);  // ������� ����������� �����������

// ����� ����� � ����������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;  // ��������� ���������� ����������

    // ��������� ��������� ������ ����
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);  // ������ ���������
    wc.style = CS_HREDRAW | CS_VREDRAW;  // ����� ���� - ����������� ��� ��������� �������
    wc.lpfnWndProc = WndProc;  // ��������� �� ��������� ����
    wc.hInstance = hInstance;  // ���������� ����������
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // ��������� ������
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // ��� ����
    wc.lpszClassName = szClassName;  // ��� ������

    // ������������ ����� ����
    if (!RegisterClassEx(&wc)) return 0;

    // ������� ������� ����
    HWND hWnd = CreateWindow(szClassName, _T("����������� ����� - ������� 3"),
        WS_OVERLAPPEDWINDOW,  // ����� ����
        CW_USEDEFAULT, CW_USEDEFAULT,  // �������
        800, 600,  // ������
        NULL, NULL, hInstance, NULL);
    if (!hWnd) return 0;  // ���� �� ������� ������� ����, �������

    // ���������� � ��������� ����
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // ���� ��������� ���������
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
        // ������� ����
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, IDM_SAVE_METAFILE, _T("��������� ��� ��������"));
        AppendMenu(hFileMenu, MF_STRING, IDM_VIEW_IMAGE, _T("����������� ��������"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, _T("����"));
        SetMenu(hWnd, hMenu);
        break;
    }
    case WM_COMMAND: {
        // ������������ ������� ����
        switch (LOWORD(wParam)) {
        case IDM_SAVE_METAFILE:
            SaveAsMetafile(hWnd);  // ��������� ��� ��������
            break;
        case IDM_VIEW_IMAGE:
            DisplayImage(hWnd);  // ���������� ��������
            break;
        }
        break;
    }
    case WM_PAINT: {
        // ������������ ��������� � ������������� �����������
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // �������� �������� ����������

        // ������ ����������
        DrawContent(hdc, 50, 50);

        EndPaint(hWnd, &ps);  // ��������� �����������
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);  // ��������� ����������
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
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);  // ��������� ������ �����

    // ������������� ���� ������ (�������)
    SetTextColor(hdc, RGB(0, 255, 0));

    // ������� ����� (�������� �� ���� ���)
    TextOut(hdc, x, y, _T("�������� ������ ����������"), 29);

    // ��������� ������
    int A = 60;  // ������� ��������
    int R = A / 2;  // ������ ��������� (30)

    // ������� ����� ��������� ������ ���� ������
    int figureY = y + 60;

    // ������� ���� ��� ������� (�����, ������� 2)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);  // ��������� ������ ����

    // ������ ������� � ������������ ���������� (����� 45 � 135 ��������)
    HBRUSH hSquareBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 255, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hSquareBrush);  // ��������� ������ �����

    // ������ �������
    Rectangle(hdc, x, figureY, x + A, figureY + A);

    // ������ �������� � �������������� ����������
    HBRUSH hSemiCircleBrush = CreateHatchBrush(HS_HORIZONTAL, RGB(0, 255, 0));
    SelectObject(hdc, hSemiCircleBrush);  // �������� ����� ��� ���������

    // ������� ������ ��� ��������� (������� �������� �������)
    HRGN hSemiCircleRgn = CreateEllipticRgn(x, figureY - R, x + 2 * R, figureY + R);

    // ������� ������������� ������ ��� ������� ������� ����� �������
    HRGN hRectRgn = CreateRectRgn(x, figureY - R, x + 2 * R, figureY);

    // �������� ������, �������� ������ ������� �������� (��������)
    CombineRgn(hSemiCircleRgn, hSemiCircleRgn, hRectRgn, RGN_AND);

    // ����������� ������ ��������� �������������� ����������
    FillRgn(hdc, hSemiCircleRgn, hSemiCircleBrush);

    // ������ ������ ��������� (����)
    Arc(hdc, x, figureY - R, x + 2 * R, figureY + R,
        x + 2 * R, figureY, x, figureY);

    // ��������������� ������������ ������� GDI
    SelectObject(hdc, hOldFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // ������� ��������� ������� GDI
    DeleteObject(hFont);
    DeleteObject(hPen);
    DeleteObject(hSquareBrush);
    DeleteObject(hSemiCircleBrush);
    DeleteObject(hSemiCircleRgn);
    DeleteObject(hRectRgn);
}

// ������� ���������� ��� ���������
void SaveAsMetafile(HWND hWnd) {
    // ������� �������� ���������
    HDC hMetaDC = CreateEnhMetaFile(NULL, _T("output.emf"), NULL, _T("Graphic Output"));

    if (hMetaDC) {
        // ������ ���������� � ��������
        DrawContent(hMetaDC, 50, 50);

        // ��������� �������� ���������
        HENHMETAFILE hemf = CloseEnhMetaFile(hMetaDC);

        if (hemf) {
            MessageBox(hWnd, _T("�������� ������� �������� ��� output.emf"), _T("�����"), MB_OK);
            DeleteEnhMetaFile(hemf);  // ������� �������� �� ������
        }
        else {
            MessageBox(hWnd, _T("������ ��� ���������� ���������"), _T("������"), MB_OK | MB_ICONERROR);
        }
    }
}

// ������� ����������� ���������
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
    HWND hChild = CreateWindow(szChildClassName, _T("��������"),
        WS_OVERLAPPEDWINDOW | WS_CHILD,  // ����� ���� - ��������
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300, hWnd, NULL, hInst, NULL);
    if (hChild) {
        ShowWindow(hChild, SW_SHOW);
        UpdateWindow(hChild);
    }
}

// ��������� ��������� ���� ��� ����������� ���������
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // �������� �������� ����������

        // ��������� ��������
        HENHMETAFILE hemf = GetEnhMetaFile(_T("output.emf"));

        if (hemf) {
            // �������� ������� ���������� �������
            RECT rect;
            GetClientRect(hWnd, &rect);

            // ������������� ��������
            PlayEnhMetaFile(hdc, hemf, &rect);

            // ������� ��������
            DeleteEnhMetaFile(hemf);
        }
        else {
            // ���� �������� �� ��������, ������� ���������
            TextOut(hdc, 10, 10, _T("�������� output.emf �� ������"), 26);
        }

        EndPaint(hWnd, &ps);  // ��������� �����������
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);  // ��������� ����
        break;
    case WM_DESTROY:
        PostQuitMessage(0);  // ��������� �����
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}