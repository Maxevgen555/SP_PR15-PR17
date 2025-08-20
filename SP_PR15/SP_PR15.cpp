#include <windows.h>          // Основные WinAPI функции и типы
#include <windowsx.h>         // Макросы-обработчики сообщений (например, HANDLE_MSG)
#include <tchar.h>            // Поддержка TCHAR для Unicode/ANSI
#include <winuser.h>          // Пользовательские функции WinAPI (доп. к windows.h)
#include <commdlg.h>          // Общие диалоговые окна (например, открытия файла)
#include <string>             // STL string (std::wstring)
#include <vector>             // STL vector
#include "resource.h"         // Идентификаторы ресурсов из вашего проекта

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
// Указывает линкеру использовать оконный подсистему, точку входа wWinMainCRTStartup
// Это позволяет использовать Unicode версию WinMain (_tWinMain)


// Прототипы функций обработки оконных сообщений и команд
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify);
void OnDestroy(HWND hWnd);
void OnPaint(HWND hWnd);
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, UINT flags);

// Прототипы процедур обработки диалогов
INT_PTR CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK TextViewerProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ControlsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ModelessProc(HWND, UINT, WPARAM, LPARAM);


// Глобальные переменные
LPCTSTR g_lpszApplicationTitle = TEXT("SP_PR15 - Диалоговые окна");
LPCTSTR g_lpszClassName = TEXT("sp_pr15_class");
HINSTANCE g_hInst;

static HMENU g_hEditMenu = NULL;    // Меню "Правка"
static HMENU g_hFileMenu = NULL;    // Меню "Файл"

// Для немодального диалога
HWND g_hModelessDlg = NULL;

// Глобальный список строк (для хранения элементов списка)
std::vector<std::wstring> g_itemsList;

// Буфер для текста (стандартный, 10000 символов)
TCHAR g_szBuffer[10000] = L"Тест 2";


// Точка входа программы (Unicode версия)
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPTSTR lpszCmdLine, int nCmdShow)
{
    g_hInst = hInstance;  // Сохраняем дескриптор экземпляра приложения

    WNDCLASSEX wc = { 0 };
    MSG msg;
    HWND hWnd;

    memset(&wc, 0, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = g_lpszClassName;      // Имя класса окна
    wc.lpfnWndProc = WndProc;                 // Функция-обработчик окна
    wc.style = CS_VREDRAW | CS_HREDRAW;      // Перерисовывать при изменении размера (гор и вет)
    wc.hInstance = hInstance;                 // Экземпляр приложения
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));    // Иконка окна
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1),
        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);                      // Маленькая иконка
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);                     // Стандартный курсор
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));      // Фон окна (светло-серый)
    wc.lpszMenuName = NULL;                 // Меню устанавливаем позже через CreateWindowEx
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    // Регистрируем класс окна ОС
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
            TEXT("Ошибка"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Загружаем главное меню из ресурсов
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));

    // Создаем основное окно
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
        MessageBox(NULL, TEXT("Окно не создано!"),
            TEXT("Ошибка"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Загружаем акселераторы (горячие клавиши) из ресурсов
    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    ShowWindow(hWnd, nCmdShow);     // Показываем окно
    UpdateWindow(hWnd);             // Обновляем окно, вызывая WM_PAINT

    // Главный цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // Если есть немодальный диалог и сообщение подходит для диалога,
        // то отдадим ему первым. Иначе — обработка обычным образом
        if (g_hModelessDlg == NULL || !IsDialogMessage(g_hModelessDlg, &msg))
        {
            if (!TranslateAccelerator(hWnd, hAccel, &msg))  // Обработка ускорителей
            {
                TranslateMessage(&msg);  // Перевод сообщений клавиатуры
                DispatchMessage(&msg);   // Отправка сообщения в окно (WndProc)
            }
        }
    }

    return (int)msg.wParam; // Возврат кода завершения
}

// Основная оконная функция - обработчик сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);                 // При создании окна
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);               // При выполнении команды (меню, кнопки)
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);               // При уничтожении окна
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);                   // Перерисовка окна
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);       // Левый клик мыши
        HANDLE_MSG(hWnd, WM_RBUTTONDOWN, OnRButtonDown);       // Правый клик мыши

    case WM_MENUSELECT:
        // Выделение пункта меню — отдельная обработка
        OnMenuSelect(hWnd, (HMENU)lParam, LOWORD(wParam), HIWORD(wParam));
        return 0;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);       // Все остальные сообщения - дефолтная обработка
    }
    return 0;
}

// Обработчик события создания окна
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    // Создание кнопки "Выход" в окне
    HWND hBtExit = CreateWindowEx(0, TEXT("Button"), TEXT("Выход"),
        WS_CHILD | WS_VISIBLE,
        50, 150, 100, 30,
        hWnd, (HMENU)(IDCANCEL), g_hInst, NULL);
    if (!hBtExit) return -1;

    // Получаем главное меню и подменю для дальнейшего управления
    HMENU hMainMenu = GetMenu(hWnd);
    g_hFileMenu = GetSubMenu(hMainMenu, 0);  // Первое подменю - Файл
    g_hEditMenu = GetSubMenu(hMainMenu, 1);  // Второе - Правка

    // Добавляем пункт "Закрыть документ" в меню Файл на позицию 3 (четвертый пункт)
    InsertMenu(g_hFileMenu, 3, MF_BYPOSITION | MF_STRING, IDM_FILE_CLOSE, TEXT("Закрыть документ"));
    // Изначально пункт недоступен (серый)
    EnableMenuItem(g_hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED);

    // Изначально все пункты меню "Правка" делаем недоступными (серые)
    for (int i = 0; i < 4; i++) {
        EnableMenuItem(g_hEditMenu, i, MF_BYPOSITION | MF_GRAYED);
    }

    // Инициализируем буфер тестовой строкой
    wcscpy_s(g_szBuffer, L"Тест2");

    // Заполняем список демонстрационными строками
    g_itemsList.push_back(L"Пример строки");
    //g_itemsList.push_back(L"Пример строки 2");
    //g_itemsList.push_back(L"Пример строки 3");

    return TRUE;
}

// Обработка команд меню, кнопок и т.п.
void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDCANCEL:
    case IDM_FILE_EXIT:
        DestroyWindow(hWnd);     // Закрытие главного окна => завершение программы
        break;

    case IDM_FILE_NEW:
        MessageBox(hWnd, TEXT("Создан новый документ"), TEXT("Меню Файл"), MB_OK);
        // Разблокируем пункты меню
        EnableMenuItem(g_hEditMenu, IDM_EDIT_SELECT, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(g_hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED);
        break;

    case IDM_FILE_OPEN:
        MessageBox(hWnd, TEXT("Выбрана команда 'Открыть'"), TEXT("Меню Файл"), MB_OK);
        break;

    case IDM_EDIT_SELECT:
        MessageBox(hWnd, TEXT("Текст выделен"), TEXT("Меню Правка"), MB_OK);
        EnableMenuItem(g_hEditMenu, IDM_EDIT_COPY, MF_BYCOMMAND | MF_ENABLED);
        break;

    case IDM_EDIT_COPY:
        MessageBox(hWnd, TEXT("Текст скопирован"), TEXT("Меню Правка"), MB_OK);
        break;

    case IDM_FILE_CLOSE:
        MessageBox(hWnd, TEXT("Документ закрыт"), TEXT("Меню Файл"), MB_OK);
        // Блокируем пункты меню "Правка" и "Закрыть документ"
        for (int i = 0; i < 4; i++) {
            EnableMenuItem(g_hEditMenu, i, MF_BYPOSITION | MF_GRAYED);
        }
        EnableMenuItem(g_hFileMenu, IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED);
        break;

    case IDM_HELP_HELP:
        MessageBox(hWnd, TEXT("Выбрана команда 'Помощь'"), TEXT("Меню Справка"), MB_OK);
        break;

    case IDM_HELP_ABOUT:
        // Показываем модальный диалог "О программе"
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutProc);
        break;

    case IDM_VIEW_TEXT:
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TEXTVIEWER), hWnd, TextViewerProc);
        break;

    case IDM_VIEW_CTL:
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONTROLS), hWnd, ControlsProc);
        InvalidateRect(hWnd, NULL, TRUE); // Перерисовать окно
        break;

    case IDM_VIEW_MODELESS:
        if (g_hModelessDlg == NULL)
        {
            // Создаем немодальный диалог, если еще не создан
            g_hModelessDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_MODELESS), hWnd, ModelessProc);
            if (g_hModelessDlg == NULL)
            {
                MessageBox(hWnd, TEXT("Не удалось создать немодальный диалог"), TEXT("Ошибка"), MB_OK);
            }
        }
        else
        {
            // Если открыт - показываем и ставим на передний план
            SetForegroundWindow(g_hModelessDlg);
        }
        break;
    }
}

// Обработка разрушения главного окна
void OnDestroy(HWND hWnd)
{
    PostQuitMessage(0);   // Выход из цикла сообщений и завершение приложения
}

// Обработка перерисовки окна
void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT rect;
    GetClientRect(hWnd, &rect);

    // Выводим список строк из глобального массива с интервалом 20 по вертикали
    int y = 50;
    for (const auto& item : g_itemsList)
    {
        TextOut(hdc, 20, y, item.c_str(), (int)item.length());
        y += 20;
    }

    // Выводим информацию о программе сверху окна
    LPCTSTR infoText = TEXT("Лабораторная работа 15: Диалоговые окна");
    TextOut(hdc, 20, 10, infoText, lstrlen(infoText));

    EndPaint(hWnd, &ps);
}

// Клик левой кнопкой по окну
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    HDC hdc = GetDC(hWnd);
    RECT rect = { x, y, 0, 0 };

    LPCTSTR clickText = TEXT("Щелчок левой кнопкой мыши");
    DrawText(hdc, clickText, lstrlen(clickText), &rect, DT_LEFT);
    ReleaseDC(hWnd, hdc);
}

// Клик правой кнопкой — создание и показ контекстного меню
void OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    HMENU hContextMenu = CreatePopupMenu();
    AppendMenu(hContextMenu, MF_STRING, IDM_EDIT_SELECT, TEXT("Выделить"));
    AppendMenu(hContextMenu, MF_STRING, IDM_EDIT_COPY, TEXT("Копировать"));
    AppendMenu(hContextMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hContextMenu, MF_STRING, IDM_VIEW_TEXT, TEXT("Просмотр текста"));
    AppendMenu(hContextMenu, MF_STRING, IDM_VIEW_CTL, TEXT("Элементы управления"));

    // Получаем текущее состояние пунктов из главного меню, чтобы синхронизировать доступность
    UINT selectState = GetMenuState(g_hEditMenu, IDM_EDIT_SELECT, MF_BYCOMMAND);
    UINT copyState = GetMenuState(g_hEditMenu, IDM_EDIT_COPY, MF_BYCOMMAND);

    // Соответственно устанавливаем разрешение или отключение пунктов в контекстном меню
    EnableMenuItem(hContextMenu, IDM_EDIT_SELECT,
        (selectState & MF_GRAYED) ? MF_BYCOMMAND | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED);

    EnableMenuItem(hContextMenu, IDM_EDIT_COPY,
        (copyState & MF_GRAYED) ? MF_BYCOMMAND | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED);

    POINT pt = { x, y };
    ClientToScreen(hWnd, &pt);   // Конвертируем координаты клиентской области в экранные

    // Отображаем контекстное меню, привязанное к позиции клика
    TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, hWnd, NULL);

    DestroyMenu(hContextMenu);
}

// Обработка выделения пункта меню — используется для вывода информации/подсказок
void OnMenuSelect(HWND hWnd, HMENU hMenu, int item, UINT flags)
{
    HDC hdc = GetDC(hWnd);
    RECT rc;
    GetClientRect(hWnd, &rc);

    // Очищаем область подсказки
    LPCTSTR clearStr = TEXT("                                        ");
    TextOut(hdc, 10, rc.bottom - 30, clearStr, lstrlen(clearStr));

    // Если выбран обычный пункт меню (не разделитель и не выпадающее меню)
    if (item > 0 && !(flags & (MF_SEPARATOR | MF_POPUP)))
    {
        TCHAR helpText[256];
        // Загружаем строку-подсказку из ресурсов по ID пункта меню
        if (LoadString(g_hInst, item, helpText, 256) > 0)
        {
            TextOut(hdc, 10, rc.bottom - 30, helpText, lstrlen(helpText));
        }
    }
    ReleaseDC(hWnd, hdc);
}

// Обработчик модального диалога "О программе"
INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // Загружаем и устанавливаем иконку диалога
        HICON hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
        SendDlgItemMessage(hDlg, IDC_APP_ICON, STM_SETICON, (WPARAM)hIcon, 0);

        // Отображаем текущее системное время в контроле IDC_CURRENT_TIME
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
            EndDialog(hDlg, LOWORD(wParam)); // Закрытие диалога
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Обработчик диалога просмотра текста
INT_PTR CALLBACK TextViewerProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_EDIT_CONTENT, TEXT("Начало работы")); // Инициализация текста
        SetFocus(GetDlgItem(hDlg, IDC_LOAD_BUTTON)); // Фокус на кнопку загрузки
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_LOAD_BUTTON)
        {
            // Открываем диалог выбора файла
            OPENFILENAME ofn;
            TCHAR szFile[260] = { 0 };

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = TEXT("Текстовые файлы\0*.txt\0Все файлы\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                // Открываем выбранный файл для чтения
                HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD dwBytesRead;
                    // Читаем содержимое файла в буфер
                    ReadFile(hFile, g_szBuffer, sizeof(g_szBuffer) - sizeof(TCHAR), &dwBytesRead, NULL);
                    g_szBuffer[dwBytesRead / sizeof(TCHAR)] = 0;  // Завершаем строку нулём
                    SetDlgItemText(hDlg, IDC_EDIT_CONTENT, g_szBuffer); // Выводим содержимое в контрол
                    CloseHandle(hFile);
                }
            }
        }
        else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));  // Закрытие диалога
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Обработчик диалога с элементами управления
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
            // Читаем текст из поля ввода
            GetDlgItemText(hDlg, IDC_EDIT_TEXT, szText, 256);
            // Копируем в глобальный буфер
            wcscpy_s(g_szBuffer, szText);
        }
        break;
        case IDC_ADD_BUTTON:
        {
            TCHAR szText[256];
            // Читаем текст из поля ввода
            GetDlgItemText(hDlg, IDC_EDIT_TEXT, szText, 256);
            // Добавляем эту строку в список элементов ListBox
            SendDlgItemMessage(hDlg, IDC_LIST_ITEMS, LB_ADDSTRING, 0, (LPARAM)szText);
        }
        break;
        case IDOK:
        {
            // При нажатии ОК сохраняем все элементы из ListBox в глобальный вектор
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

// Обработчик немодального диалога
INT_PTR CALLBACK ModelessProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hDlg);        // Уничтожаем окно диалога при закрытии
            g_hModelessDlg = NULL;      // Очищаем глобальный хендл
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
