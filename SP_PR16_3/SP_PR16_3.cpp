#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <vector>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
// ���������� ����������
int xFrame = 30;      // ��������� ������� X �����
int yFrame = 130;     // ��������� ������� Y �����
const int cxFrame = 150; // ������ �����
const int cyFrame = 300; // ������ �����
int step = 1;         // ��� �����������

bool drawingMode = false; // ����� ��������� �����
int counter = 0;      // ������� ��������� ����
const int n = 5;      // �������� ������ 5-� ������

COLORREF markerColor = RGB(0, 0, 255); // ����� ���� �������

std::vector<POINT> markers; // ������ ��� �������� ��������

HWND hMainWnd;        // handle �������� ����
HWND hFrameWnd;       // handle ����-�����

// ��������� �������
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK FrameWndProc(HWND, UINT, WPARAM, LPARAM);
void UpdateFramePosition();

// ����� �����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ����������� ������ �������� ����
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("MainWindowClass");

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("������ ����������� �������� ����!"), _T("������"), MB_ICONERROR);
        return 1;
    }

    // ����������� ������ ����-�����
    WNDCLASSEX wcFrame = {};
    wcFrame.cbSize = sizeof(WNDCLASSEX);
    wcFrame.style = CS_HREDRAW | CS_VREDRAW;
    wcFrame.lpfnWndProc = FrameWndProc;
    wcFrame.hInstance = hInstance;
    wcFrame.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcFrame.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcFrame.lpszClassName = _T("FrameWindowClass");

    if (!RegisterClassEx(&wcFrame)) {
        MessageBox(NULL, _T("������ ����������� ����-�����!"), _T("������"), MB_ICONERROR);
        return 1;
    }

    // �������� �������� ����
    hMainWnd = CreateWindow(_T("MainWindowClass"), _T("���������� ������"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600, NULL, NULL, hInstance, NULL);

    if (!hMainWnd) {
        MessageBox(NULL, _T("������ �������� �������� ����!"), _T("������"), MB_ICONERROR);
        return 1;
    }

    // �������� ����-�����
    hFrameWnd = CreateWindow(_T("FrameWindowClass"), _T(""),
        WS_CHILD | WS_BORDER | WS_VISIBLE,
        xFrame, yFrame, cxFrame, cyFrame,
        hMainWnd, NULL, hInstance, NULL);

    if (!hFrameWnd) {
        MessageBox(NULL, _T("������ �������� ����-�����!"), _T("������"), MB_ICONERROR);
        return 1;
    }

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    // ���� ���������
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// ��������� �������� ����
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_KEYDOWN:
        switch (wParam) {
        case 'D': // �����
            xFrame -= step;
            break;
        case 'F': // ������
            xFrame += step;
            break;
        case 'C': // �����
            yFrame -= step;
            break;
        case 'V': // ����
            yFrame += step;
            break;
        case VK_F2: // �������� ����� ���������
            drawingMode = true;
            break;
        case VK_F3: // ��������� ����� ���������
            drawingMode = false;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        // ��������� ������� �����
        UpdateFramePosition();
        InvalidateRect(hWnd, NULL, TRUE); // �������������� ������� ����
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ����� ��������� ����� � ������ ����� ����
        RECT rect;
        GetClientRect(hWnd, &rect);

        TCHAR text[100];
        _stprintf_s(text, _T("���������� �����: X=%d, Y=%d"), xFrame, yFrame);

        SetTextColor(hdc, markerColor);
        TextOut(hdc, 10, rect.bottom - 30, text, _tcslen(text));

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

// ��������� ����-�����
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_MOUSEMOVE: {
        if (drawingMode && (wParam & MK_LBUTTON)) {
            counter++;

            // ������ ������ ������ ��� ������� n-�� ���������
            if (counter % n == 0) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);

                // ��������� ���������� �������
                markers.push_back({ x, y });

                // �������������� ����-�����
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ������ ��� �������
        HPEN hPen = CreatePen(PS_SOLID, 2, markerColor);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        for (const auto& point : markers) {
            // ������ ������������� ������ �����
            Rectangle(hdc, point.x - 5, point.y - 5, point.x + 5, point.y + 5);
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        EndPaint(hWnd, &ps);
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ������� ���������� ������� ����-�����
void UpdateFramePosition() {
    SetWindowPos(hFrameWnd, NULL, xFrame, yFrame, cxFrame, cyFrame, SWP_NOZORDER);
}