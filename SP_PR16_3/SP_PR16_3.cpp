#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <vector>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
// Глобальные переменные
int xFrame = 30;      // начальная позиция X рамки
int yFrame = 130;     // начальная позиция Y рамки
const int cxFrame = 150; // ширина рамки
const int cyFrame = 300; // высота рамки
int step = 1;         // шаг перемещения

bool drawingMode = false; // режим рисования следа
int counter = 0;      // счетчик сообщений мыши
const int n = 5;      // рисовать каждый 5-й маркер

COLORREF markerColor = RGB(0, 0, 255); // синий цвет маркера

std::vector<POINT> markers; // вектор для хранения маркеров

HWND hMainWnd;        // handle главного окна
HWND hFrameWnd;       // handle окна-рамки

// Прототипы функций
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK FrameWndProc(HWND, UINT, WPARAM, LPARAM);
void UpdateFramePosition();

// Точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Регистрация класса главного окна
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("MainWindowClass");

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("Ошибка регистрации главного окна!"), _T("Ошибка"), MB_ICONERROR);
        return 1;
    }

    // Регистрация класса окна-рамки
    WNDCLASSEX wcFrame = {};
    wcFrame.cbSize = sizeof(WNDCLASSEX);
    wcFrame.style = CS_HREDRAW | CS_VREDRAW;
    wcFrame.lpfnWndProc = FrameWndProc;
    wcFrame.hInstance = hInstance;
    wcFrame.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcFrame.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcFrame.lpszClassName = _T("FrameWindowClass");

    if (!RegisterClassEx(&wcFrame)) {
        MessageBox(NULL, _T("Ошибка регистрации окна-рамки!"), _T("Ошибка"), MB_ICONERROR);
        return 1;
    }

    // Создание главного окна
    hMainWnd = CreateWindow(_T("MainWindowClass"), _T("Управление рамкой"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600, NULL, NULL, hInstance, NULL);

    if (!hMainWnd) {
        MessageBox(NULL, _T("Ошибка создания главного окна!"), _T("Ошибка"), MB_ICONERROR);
        return 1;
    }

    // Создание окна-рамки
    hFrameWnd = CreateWindow(_T("FrameWindowClass"), _T(""),
        WS_CHILD | WS_BORDER | WS_VISIBLE,
        xFrame, yFrame, cxFrame, cyFrame,
        hMainWnd, NULL, hInstance, NULL);

    if (!hFrameWnd) {
        MessageBox(NULL, _T("Ошибка создания окна-рамки!"), _T("Ошибка"), MB_ICONERROR);
        return 1;
    }

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Процедура главного окна
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_KEYDOWN:
        switch (wParam) {
        case 'D': // Влево
            xFrame -= step;
            break;
        case 'F': // Вправо
            xFrame += step;
            break;
        case 'C': // Вверх
            yFrame -= step;
            break;
        case 'V': // Вниз
            yFrame += step;
            break;
        case VK_F2: // Включить режим рисования
            drawingMode = true;
            break;
        case VK_F3: // Выключить режим рисования
            drawingMode = false;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        // Обновляем позицию рамки
        UpdateFramePosition();
        InvalidateRect(hWnd, NULL, TRUE); // Перерисовываем главное окно
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Вывод координат рамки в нижней части окна
        RECT rect;
        GetClientRect(hWnd, &rect);

        TCHAR text[100];
        _stprintf_s(text, _T("Координаты рамки: X=%d, Y=%d"), xFrame, yFrame);

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

// Процедура окна-рамки
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_MOUSEMOVE: {
        if (drawingMode && (wParam & MK_LBUTTON)) {
            counter++;

            // Рисуем маркер только для каждого n-го сообщения
            if (counter % n == 0) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);

                // Сохраняем координаты маркера
                markers.push_back({ x, y });

                // Перерисовываем окно-рамку
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Рисуем все маркеры
        HPEN hPen = CreatePen(PS_SOLID, 2, markerColor);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        for (const auto& point : markers) {
            // Рисуем прямоугольник вокруг точки
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

// Функция обновления позиции окна-рамки
void UpdateFramePosition() {
    SetWindowPos(hFrameWnd, NULL, xFrame, yFrame, cxFrame, cyFrame, SWP_NOZORDER);
}