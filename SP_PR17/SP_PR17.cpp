#include <windows.h>
#include <tchar.h>
#include <math.h>
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")

// Константы для меню
#define IDM_SAVE_METAFILE 1001
#define IDM_VIEW_IMAGE    1002

// Глобальные переменные
HINSTANCE hInst;  // Дескриптор экземпляра приложения
TCHAR szClassName[] = _T("GraphicOutputApp");  // Имя класса главного окна
TCHAR szChildClassName[] = _T("ChildWindow");  // Имя класса дочернего окна

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // Процедура главного окна
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);  // Процедура дочернего окна
void DrawContent(HDC hdc, int x, int y);  // Функция рисования содержимого
void SaveAsMetafile(HWND hWnd);  // Функция сохранения как метафайла
void DisplayImage(HWND hWnd);  // Функция отображения изображения

// Точка входа в приложение
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;  // Сохраняем дескриптор экземпляра

    // Заполняем структуру класса окна
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);  // Размер структуры
    wc.style = CS_HREDRAW | CS_VREDRAW;  // Стиль окна - перерисовка при изменении размера
    wc.lpfnWndProc = WndProc;  // Указатель на процедуру окна
    wc.hInstance = hInstance;  // Дескриптор экземпляра
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Загружаем курсор
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // Фон окна
    wc.lpszClassName = szClassName;  // Имя класса

    // Регистрируем класс окна
    if (!RegisterClassEx(&wc)) return 0;

    // Создаем главное окно
    HWND hWnd = CreateWindow(szClassName, _T("Графический вывод - Вариант 3"),
        WS_OVERLAPPEDWINDOW,  // Стиль окна
        CW_USEDEFAULT, CW_USEDEFAULT,  // Позиция
        800, 600,  // Размер
        NULL, NULL, hInstance, NULL);
    if (!hWnd) return 0;  // Если не удалось создать окно, выходим

    // Показываем и обновляем окно
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Цикл обработки сообщений
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
        // Создаем меню
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, IDM_SAVE_METAFILE, _T("Сохранить как метафайл"));
        AppendMenu(hFileMenu, MF_STRING, IDM_VIEW_IMAGE, _T("Просмотреть метафайл"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, _T("Файл"));
        SetMenu(hWnd, hMenu);
        break;
    }
    case WM_COMMAND: {
        // Обрабатываем команды меню
        switch (LOWORD(wParam)) {
        case IDM_SAVE_METAFILE:
            SaveAsMetafile(hWnd);  // Сохраняем как метафайл
            break;
        case IDM_VIEW_IMAGE:
            DisplayImage(hWnd);  // Показываем метафайл
            break;
        }
        break;
    }
    case WM_PAINT: {
        // Обрабатываем сообщение о необходимости перерисовки
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // Получаем контекст устройства

        // Рисуем содержимое
        DrawContent(hdc, 50, 50);

        EndPaint(hWnd, &ps);  // Завершаем перерисовку
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);  // Завершаем приложение
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
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);  // Сохраняем старый шрифт

    // Устанавливаем цвет текста (зеленый)
    SetTextColor(hdc, RGB(0, 255, 0));

    // Выводим текст (замените на ваши ФИО)
    TextOut(hdc, x, y, _T("Гончаров Максим Евгеньевич"), 29);

    // Параметры фигуры
    int A = 60;  // Сторона квадрата
    int R = A / 2;  // Радиус полукруга (30)

    // Смещаем точку рисования фигуры ниже текста
    int figureY = y + 60;

    // Создаем перо для контура (синий, толщина 2)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);  // Сохраняем старое перо

    // Рисуем квадрат с диагональной штриховкой (сетка 45 и 135 градусов)
    HBRUSH hSquareBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 255, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hSquareBrush);  // Сохраняем старую кисть

    // Рисуем квадрат
    Rectangle(hdc, x, figureY, x + A, figureY + A);

    // Рисуем полукруг с горизонтальной штриховкой
    HBRUSH hSemiCircleBrush = CreateHatchBrush(HS_HORIZONTAL, RGB(0, 255, 0));
    SelectObject(hdc, hSemiCircleBrush);  // Выбираем кисть для полукруга

    // Создаем регион для полукруга (верхняя половина эллипса)
    HRGN hSemiCircleRgn = CreateEllipticRgn(x, figureY - R, x + 2 * R, figureY + R);

    // Создаем прямоугольный регион для обрезки верхней части эллипса
    HRGN hRectRgn = CreateRectRgn(x, figureY - R, x + 2 * R, figureY);

    // Обрезаем эллипс, оставляя только верхнюю половину (полукруг)
    CombineRgn(hSemiCircleRgn, hSemiCircleRgn, hRectRgn, RGN_AND);

    // Закрашиваем регион полукруга горизонтальной штриховкой
    FillRgn(hdc, hSemiCircleRgn, hSemiCircleBrush);

    // Рисуем контур полукруга (дугу)
    Arc(hdc, x, figureY - R, x + 2 * R, figureY + R,
        x + 2 * R, figureY, x, figureY);

    // Восстанавливаем оригинальные объекты GDI
    SelectObject(hdc, hOldFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // Удаляем созданные объекты GDI
    DeleteObject(hFont);
    DeleteObject(hPen);
    DeleteObject(hSquareBrush);
    DeleteObject(hSemiCircleBrush);
    DeleteObject(hSemiCircleRgn);
    DeleteObject(hRectRgn);
}

// Функция сохранения как метафайла
void SaveAsMetafile(HWND hWnd) {
    // Создаем контекст метафайла
    HDC hMetaDC = CreateEnhMetaFile(NULL, _T("output.emf"), NULL, _T("Graphic Output"));

    if (hMetaDC) {
        // Рисуем содержимое в метафайл
        DrawContent(hMetaDC, 50, 50);

        // Завершаем создание метафайла
        HENHMETAFILE hemf = CloseEnhMetaFile(hMetaDC);

        if (hemf) {
            MessageBox(hWnd, _T("Метафайл успешно сохранен как output.emf"), _T("Успех"), MB_OK);
            DeleteEnhMetaFile(hemf);  // Удаляем метафайл из памяти
        }
        else {
            MessageBox(hWnd, _T("Ошибка при сохранении метафайла"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        }
    }
}

// Функция отображения метафайла
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
    HWND hChild = CreateWindow(szChildClassName, _T("Метафайл"),
        WS_OVERLAPPEDWINDOW | WS_CHILD,  // Стиль окна - дочернее
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300, hWnd, NULL, hInst, NULL);
    if (hChild) {
        ShowWindow(hChild, SW_SHOW);
        UpdateWindow(hChild);
    }
}

// Процедура дочернего окна для отображения метафайла
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // Получаем контекст устройства

        // Загружаем метафайл
        HENHMETAFILE hemf = GetEnhMetaFile(_T("output.emf"));

        if (hemf) {
            // Получаем размеры клиентской области
            RECT rect;
            GetClientRect(hWnd, &rect);

            // Воспроизводим метафайл
            PlayEnhMetaFile(hdc, hemf, &rect);

            // Удаляем метафайл
            DeleteEnhMetaFile(hemf);
        }
        else {
            // Если метафайл не загружен, выводим сообщение
            TextOut(hdc, 10, 10, _T("Метафайл output.emf не найден"), 26);
        }

        EndPaint(hWnd, &ps);  // Завершаем перерисовку
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);  // Закрываем окно
        break;
    case WM_DESTROY:
        PostQuitMessage(0);  // Завершаем поток
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}