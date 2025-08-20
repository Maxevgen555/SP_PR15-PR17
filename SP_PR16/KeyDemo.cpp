// Обработка ввода мыши и клавиатуры 
// Справочная информация
// Platform SDK/User Interface Services/User Input/Keyboard Input/
// Keyboard Input Reference/Keyboard Input Messages

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <vector> // Добавляем для использования std::vector

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")

// Глобальные переменные
int     nPosX = 10;
int     nPosY = 30;
LPCTSTR lpszAppName = TEXT("Демонстрация обработки ввода мыши и клавиатуры");
LPCTSTR lpszClassName = TEXT("Key And Mause Demo Class");
HWND    hMainWnd;

// Добавляем глобальные переменные для хранения точек следа мыши
std::vector<POINT> g_mouseTrail; // Вектор для хранения точек следа
const int MAX_TRAIL_POINTS = 1000; // Максимальное количество точек следа

// Глобальные переменные для точного подсчета
struct KeyState {
    UINT vkCode;
    int repeatCount;
    DWORD lastPressTime;
    BOOL isPressed;
};
KeyState g_keyStates[256] = { 0 }; // Для всех возможных виртуальных клавиш
//KeyState g_systemKeys[256] = { 0 };
//========Предварительное объявление функций=========================
/* Процедура главного окна */
LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
/* Регистрация класса главного окна */
BOOL Register(HINSTANCE);
/* Создание главного окна */
HWND Create(HINSTANCE, int);

/* Обработчики сообщений */
/*--- WM_DESTROY -----------------------------------------------------*/
void  km_OnDestroy(HWND hwnd);
/*--- WM_CHAR --------------------------------------------------------*/
void  km_OnChar(HWND hwnd, UINT ch, int cRepeat);
/*--- WM_KEYUP,WM_KEYDOWN --------------------------------------------*/
void  km_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
/*--- WM_SYSKEYDOWN, WM_SYSKEYUP  ------------------------------------*/
void  km_OnSysKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
/*--- WM_LBUTTONDOWN, WM_LBUTTONDBLCLK -------------------------------*/
void  km_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
/*--- WM_LBUTTONUP ---------------------------------------------------*/
void  km_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
/*--- WM_MOUSEMOVE ---------------------------------------------------*/
void  km_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
/*--- WM_PAINT -------------------------------------------------------*/
void  km_OnPaint(HWND hwnd);
/*--------------------------------------------------------------------*/
//====================================================================//


// Точка входа в программу ===========================================//
int WINAPI wWinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    PWSTR lpCmdLine,
    int nCmdShow)
{
    MSG Msg;
    BOOL ok;

    ok = Register(hInst);
    if (!ok) return FALSE;

    hMainWnd = Create(hInst, nCmdShow);
    if (!hMainWnd) return FALSE;
    while (GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return (int)Msg.wParam;
}

//== Функция для регистрации класса окна =============================//
BOOL Register(HINSTANCE hinst)
{
    WNDCLASSEX WndClass;
    BOOL fRes;

    memset(&WndClass, 0, sizeof(WNDCLASSEX));
    WndClass.cbSize = sizeof(WNDCLASSEX);
    WndClass.lpszClassName = lpszClassName;
    WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    WndClass.lpfnWndProc = WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hinst;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WndClass.lpszMenuName = NULL;

    fRes = (BOOL)RegisterClassEx(&WndClass);
    return fRes;
}

//== Функция создания окна ===========================================//
HWND Create(HINSTANCE hInstance, int nCmdShow)
{
    HWND hwnd = CreateWindowEx(0,
        lpszClassName, lpszAppName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) return hwnd;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return hwnd;
}

//== Оконная процедура главного окна =================================//
LRESULT WINAPI
WndProc(HWND hwnd, UINT MesId, WPARAM wParam, LPARAM lParam)
{
    switch (MesId)
    {
        HANDLE_MSG(hwnd, WM_DESTROY, km_OnDestroy);
        HANDLE_MSG(hwnd, WM_CHAR, km_OnChar);
        HANDLE_MSG(hwnd, WM_KEYDOWN, km_OnKey);
        HANDLE_MSG(hwnd, WM_KEYUP, km_OnKey);
        HANDLE_MSG(hwnd, WM_MOUSEMOVE, km_OnMouseMove);
        HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK, km_OnLButtonDown);
        HANDLE_MSG(hwnd, WM_LBUTTONDOWN, km_OnLButtonDown);
        HANDLE_MSG(hwnd, WM_LBUTTONUP, km_OnLButtonUp);
        HANDLE_MSG(hwnd, WM_PAINT, km_OnPaint);
        HANDLE_MSG(hwnd, WM_SYSKEYUP, km_OnSysKey);
        HANDLE_MSG(hwnd, WM_SYSKEYDOWN, km_OnSysKey);
    default:
        return DefWindowProc(hwnd, MesId, wParam, lParam);
    }
}

//====================================================================//
//====================================================================//

//=== Обработчик сообщения WM_DESTROY ================================//
void km_OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}

void km_OnChar(HWND hwnd, UINT ch, int cRepeat)
{
    TCHAR S[100];
    HDC DC = GetDC(hwnd);

    // Ищем соответствующую виртуальную клавишу для этого символа
    int totalRepeats = cRepeat;
    for (int i = 0; i < 256; i++) {
        if (g_keyStates[i].isPressed) {
            totalRepeats = g_keyStates[i].repeatCount;
            break;
        }
    }

    wsprintf(S, TEXT("Символ:   %c    (0x%X)    Повторов:    %d"),
        ch, ch, totalRepeats);

    SetBkColor(DC, GetSysColor(COLOR_WINDOW));
    TextOut(DC, nPosX, nPosY + 20, S, lstrlen(S));
    ReleaseDC(hwnd, DC);
}

//=== Обработчик сообщения WM_KEYDOWN,WM_KEYUP =======================//
void km_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    TCHAR S[100];
    HDC DC = GetDC(hwnd);

    DWORD currentTime = GetTickCount();

    if (fDown) {
        // Первое нажатие или повтор?
        if (g_keyStates[vk].isPressed) {
            // Автоповтор
            g_keyStates[vk].repeatCount += cRepeat;
        }
        else {
            // Первое нажатие
            g_keyStates[vk].vkCode = vk;
            g_keyStates[vk].repeatCount = cRepeat;
            g_keyStates[vk].isPressed = TRUE;
        }
        g_keyStates[vk].lastPressTime = currentTime;

        wsprintf(S, TEXT("Клавиша   %d:    нажата    %d    раз"), vk, g_keyStates[vk].repeatCount);
    }
    else {
        g_keyStates[vk].isPressed = FALSE;
        wsprintf(S, TEXT("Клавиша %d: отпущена"), vk);
    }

    SetBkColor(DC, GetSysColor(COLOR_WINDOW));
    TextOut(DC, nPosX, nPosY + 40, S, lstrlen(S));
    ReleaseDC(hwnd, DC);
}
//=== Обработчик сообщения WM_LBUTTONDOWN, WM_LBUTTONDBLCLK ==========//
void km_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x,
    int y, UINT keyFlags)
{
    TCHAR S[100];//Буфер для формирования выводимой строки(100 байт)
    HDC DC = GetDC(hwnd);//Получаем контекст устройства графического вывода

    if (fDoubleClick) //Двойной щелчек
        wsprintf(S, TEXT("WM_LBUTTONDBLCLK ==> Двойной = %d X = %d Y = %d Флаги = %d "),
            fDoubleClick, x, y, keyFlags);
    else // Одиночный щелчек 
        wsprintf(S, TEXT("WM_LBUTTONDOWN ==> Двойной = %d X = %d Y = %d Флаги = %d "),
            fDoubleClick, x, y, keyFlags);

    SetBkColor(DC, GetSysColor(COLOR_WINDOW));//Задаем цвет фона
    TextOut(DC, nPosX, nPosY + 100, S, lstrlen(S));//Выводим строку
    ReleaseDC(hwnd, DC);//Освобождаем контекст
}

//=== Обработчик сообщения WM_LBUTTONUP ==============================//
void km_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    TCHAR S[100];//Буфер для формирования выводимой строки(100 байт)
    HDC  DC = GetDC(hwnd);//Получаем контекст устройства графического вывода

    wsprintf(S, TEXT("WM_LBUTTONUP ==> X = %d Y = %d Флаги = %d   "),
        x, y, keyFlags);

    SetBkColor(DC, GetSysColor(COLOR_WINDOW));
    TextOut(DC, nPosX, nPosY + 120, S, lstrlen(S));
    ReleaseDC(hwnd, DC);//Освобождаем контекст
}

//=== Обработчик сообщения WM_MOUSEMOVE ==============================//
void km_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    TCHAR S[100];//Буфер для формирования выводимой строки(100 байт)
    HDC DC = GetDC(hwnd);//Получаем контекст устройства графического вывода

    wsprintf(S, TEXT("WM_MOUSEMOVE ==> X = %d Y = %d Флаги = %d    "),
        x, y, keyFlags);
    //Задаем цвет в зависимости от нажатых клавиш мыши и клавиатуры 
    if ((keyFlags & MK_CONTROL) == MK_CONTROL) SetTextColor(DC, RGB(0, 0, 255));
    if ((keyFlags & MK_LBUTTON) == MK_LBUTTON) SetTextColor(DC, RGB(0, 255, 0));
    if ((keyFlags & MK_RBUTTON) == MK_RBUTTON) SetTextColor(DC, RGB(255, 0, 0));
    if ((keyFlags & MK_SHIFT) == MK_SHIFT)   SetTextColor(DC, RGB(255, 0, 255));

    SetBkColor(DC, GetSysColor(COLOR_WINDOW)); //Устанавливаем цвет фона
    TextOut(DC, nPosX, nPosY + 80, S, lstrlen(S));// Выводим строку текста

    // ДОПОЛНЕНИЕ 1: Вывод координат в левом верхнем углу
    if (x < 100 && y < 100) {
        TCHAR coordStr[50];
        wsprintf(coordStr, TEXT("Координаты: %d, %d"), x, y);
        SetTextColor(DC, RGB(0, 0, 0)); // Черный цвет для координат
        TextOut(DC, 10, 10, coordStr, lstrlen(coordStr));
    }

    // ДОПОЛНЕНИЕ 2: Сохранение точек для следа мыши при нажатой левой кнопке
    if (keyFlags & MK_LBUTTON) {
        POINT pt = { x, y };
        g_mouseTrail.push_back(pt);

        // Ограничиваем размер вектора, чтобы избежать утечки памяти
        if (g_mouseTrail.size() > MAX_TRAIL_POINTS) {
            g_mouseTrail.erase(g_mouseTrail.begin());
        }

        // Перерисовываем окно для отображения следа
        InvalidateRect(hwnd, NULL, TRUE);
    }

    ReleaseDC(hwnd, DC);//Освобождаем контекст
}

//=== Обработчик сообщений WM_SYSKEYDOWN и WM_SYSKEYUP ===============//
//=== Обработчик сообщений WM_SYSKEYDOWN и WM_SYSKEYUP ===============//
void km_OnSysKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    TCHAR S[100];
    HDC DC = GetDC(hwnd);
    SetBkColor(DC, GetSysColor(COLOR_WINDOW));

    if (fDown)
    {
        // SYSKEYDOWN - системное нажатие (обычно с ALT)
        wsprintf(S, TEXT("СИСТЕМНАЯ КЛАВИША НАЖАТА: %d"), vk);
        TextOut(DC, nPosX, nPosY + 60, S, lstrlen(S));

        // Показываем дополнительную информацию
        TCHAR info[80];
        wsprintf(info, TEXT("Повторов: %d, Флаги: %d"), cRepeat, flags);
        TextOut(DC, nPosX + 300, nPosY + 60, info, lstrlen(info));

        // Определяем тип системной клавиши
        TCHAR keyName[50];
        switch (vk) {
        case VK_MENU: wcscpy_s(keyName, TEXT("ALT")); break;
        case VK_F10: wcscpy_s(keyName, TEXT("F10")); break;
        case VK_TAB: wcscpy_s(keyName, TEXT("TAB")); break;
        default: wsprintf(keyName, TEXT("Код %d"), vk);
        }
        TextOut(DC, nPosX, nPosY + 100, keyName, lstrlen(keyName));
    }
    else
    {
        // SYSKEYUP - системное отпускание
        wsprintf(S, TEXT("СИСТЕМНАЯ КЛАВИША ОТПУЩЕНА: %d\n"), vk);
        TextOut(DC, nPosX, nPosY + 60, S, lstrlen(S));
    }

    ReleaseDC(hwnd, DC);

    // Пересылаем сообщение стандартной обработке
    if (fDown) {
        FORWARD_WM_SYSKEYDOWN(hwnd, vk, cRepeat, flags, DefWindowProc);
    }
    else {
        FORWARD_WM_SYSKEYUP(hwnd, vk, cRepeat, flags, DefWindowProc);
    }
}
//=== Обработчик сообщения WM_PAINT ==================================//
void km_OnPaint(HWND hwnd)
{
    PAINTSTRUCT PaintStruct;
    RECT Rect;
    //Строки 1 и 2 вверху экрана
    static const TCHAR lpszTitle1[] = TEXT("ДЕМОНСТРАЦИЯ ПЕРЕХВАТА ВВОДА МЫШИ И КЛАВИАТУРЫ");
    static const TCHAR lpszTitle2[] = TEXT("Поэкспериментируйте с мышью и клавиатурой");

    //Массив указателей на строки инициализации экрана 
    static const TCHAR* SList[] =
    {
        TEXT("WM_CHAR     "),
        TEXT("WM_KEY      "),
        TEXT("WM_SYSKEY   "),
        TEXT("WM_MOUSEMOVE"),
        TEXT("WM_MOUSEDOWN"),
        TEXT("WM_MOUSEUP  ")
    };
    TCHAR S[200], S1[200]; //Буферы для формирования строки инициализации экрана

    //Заполняем массив S1 100 пробелами и закрываем нулем
    TCHAR c = ' ';
    for (int i = 0; i < 100; i++) S1[i] = c;
    S1[100] = '\0';

    HDC PaintDC = BeginPaint(hwnd, &PaintStruct);//Получаем контекст
    SetBkColor(PaintDC, GetSysColor(COLOR_WINDOW));//Задаем цвет
    GetClientRect(hwnd, &Rect);//Получаем координаты клиентной области
    DrawText(PaintDC, lpszTitle1, -1, &Rect, DT_CENTER);//Выводим строку 1

    Rect.top = 20; //Задаем координату Х верха полосы вывода
    Rect.bottom = 40; //Задаем координату Х низа полосы вывода
    DrawText(PaintDC, lpszTitle2, -1, &Rect, DT_CENTER); //Выводим строку 2 

    for (int i = 0; i < 6; i++)
    {// Выводим 6 строк с указателями из SList и дополненных 100 пробелами
        lstrcpy(&S[0], SList[i]);//Копируем в S строку с указателем из SList[i]
        lstrcat(S, S1);// Объединяем строку S со строкой S1    
        TextOut(PaintDC, nPosX, nPosY + (20 * (i + 1)), S, lstrlen(S));
    }

    // ДОПОЛНЕНИЕ 3: Рисование следа мыши
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); // Красное перо
    HPEN hOldPen = (HPEN)SelectObject(PaintDC, hPen);

    for (size_t i = 1; i < g_mouseTrail.size(); i++) {
        MoveToEx(PaintDC, g_mouseTrail[i - 1].x, g_mouseTrail[i - 1].y, NULL);
        LineTo(PaintDC, g_mouseTrail[i].x, g_mouseTrail[i].y);
    }

    SelectObject(PaintDC, hOldPen);
    DeleteObject(hPen);

    EndPaint(hwnd, &PaintStruct);//Освобождаем контекст устройства
}

//====================================================================//