#include <windows.h>
#include <tchar.h>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")

// Константы
#define IDM_SAVE_METAFILE 1001
#define IDM_VIEW_IMAGE    1002

// Глобальные переменные
HINSTANCE hInst;
TCHAR szClassName[] = _T("GraphicOutputApp");
TCHAR szChildClassName[] = _T("ChildWindow");

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
void DrawContent(HDC hdc, int x, int y);
void SaveAsMetafile(HWND hWnd);
void DisplayImage(HWND hWnd);

// Точка входа
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

    HWND hWnd = CreateWindow(szClassName, _T("Графический вывод - Вариант 3"),
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

// Процедура главного окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, IDM_SAVE_METAFILE, _T("Сохранить как метафайл"));
        AppendMenu(hFileMenu, MF_STRING, IDM_VIEW_IMAGE, _T("Просмотреть изображение"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, _T("Файл"));
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

        // Рисуем содержимое
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

// Функция рисования содержимого
void DrawContent(HDC hdc, int x, int y) {
    // Создаем и выбираем шрифт
    HFONT hFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, _T("Times New Roman"));
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Устанавливаем цвет текста (зеленый)
    SetTextColor(hdc, RGB(0, 255, 0));

    // Выводим текст (замените на ваши ФИО)
    TextOut(hdc, x, y, _T("Иванов Иван Иванович"), 19);

    // Рисуем фигуру 2
    int A = 60;
    int R = A / 2; // 30
    int B = 60;

    // Создаем перо для контура (синий, толщина 2)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Создаем кисть для заливки (зеленая)
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // Рисуем две окружности и соединяющую линию
    int x1 = x;
    int y1 = y + 50;

    // Левая окружность
    Ellipse(hdc, x1, y1, x1 + 2 * R, y1 + 2 * R);

    // Правая окружность
    int x2 = x1 + A + 2 * R;
    Ellipse(hdc, x2, y1, x2 + 2 * R, y1 + 2 * R);

    // Соединяющая линия
    MoveToEx(hdc, x1 + R, y1 + R, NULL);
    LineTo(hdc, x2 + R, y1 + R);

    // Восстанавливаем контекст
    SelectObject(hdc, hOldFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // Удаляем GDI объекты
    DeleteObject(hFont);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

// Функция сохранения как метафайла
void SaveAsMetafile(HWND hWnd) {
    HDC hMetaDC = CreateEnhMetaFile(NULL, _T("output.emf"), NULL, _T("Graphic Output"));

    if (hMetaDC) {
        DrawContent(hMetaDC, 50, 50);
        HENHMETAFILE hemf = CloseEnhMetaFile(hMetaDC);

        if (hemf) {
            MessageBox(hWnd, _T("Метафайл успешно сохранен как output.emf"), _T("Успех"), MB_OK);
            DeleteEnhMetaFile(hemf);
        }
        else {
            MessageBox(hWnd, _T("Ошибка при сохранении метафайла"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        }
    }
}

// Функция отображения изображения
void DisplayImage(HWND hWnd) {
    // Регистрируем класс дочернего окна
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ChildWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szChildClassName;

    RegisterClassEx(&wc);

    // Создаем дочернее окно
    HWND hChild = CreateWindow(szChildClassName, _T("Изображение"),
        WS_OVERLAPPEDWINDOW | WS_CHILD,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300, hWnd, NULL, hInst, NULL);
    if (hChild) {
        ShowWindow(hChild, SW_SHOW);
        UpdateWindow(hChild);
    }
}

// Процедура дочернего окна для отображения изображения
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Загружаем изображение (замените на путь к вашему изображению)
        HBITMAP hBmp = (HBITMAP)LoadImage(NULL, _T("image.bmp"),
            IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBmp) {
            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, hBmp);

            BITMAP bmp;
            GetObject(hBmp, sizeof(BITMAP), &bmp);

            // Выводим изображение
            BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight,
                hMemDC, 0, 0, SRCCOPY);

            DeleteDC(hMemDC);
            DeleteObject(hBmp);
        }
        else {
            // Если изображение не загружено, выводим сообщение
            TextOut(hdc, 10, 10, _T("Изображение не найдено"), 20);
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