#pragma once
// ќбеспечивает однократное включение файла в процессе компил€ции, чтобы избежать ошибок повторного определени€.

// —тандартные элементы
#ifndef IDC_STATIC
#define IDC_STATIC                      -1
#endif
// ≈сли не определЄн IDC_STATIC, определить его как -1. 
// IDC_STATIC - стандартный идентификатор дл€ статических контролов без обработки сообщений.

// »конки
#define IDI_ICON1                       101
#define IDI_SPPR15                      107
#define IDI_SMALL                       108
// »дентификаторы иконок, используемых в приложении. 
// ќбычно св€зываютс€ с ресурсами-иконками (файлы .ico).

// ћеню и акселераторы
#define IDR_MAIN_MENU                   101
#define IDR_ACCELERATOR                 102
// IDR_MAIN_MENU - идентификатор главного меню приложени€.
// IDR_ACCELERATOR - идентификатор таблицы акселераторов (гор€чих клавиш).

// ƒиалоговые окна
#define IDD_ABOUTBOX                    103
#define IDD_TEXTVIEWER                  130
#define IDD_CONTROLS                    131
#define IDD_MODELESS                    132
// »дентификаторы диалоговых окон. 
// IDD_ABOUTBOX Ч диалог "ќ программе".
// ќстальные Ч авторские диалоги дл€ текста, элементов управлени€ и модального окна.

//  оманды меню (основные)
#define IDM_FILE_NEW                    40001
#define IDM_FILE_OPEN                   40002
#define IDM_FILE_EXIT                   40003
#define IDM_EDIT_SELECT                 40004
#define IDM_EDIT_CUT                    40005
#define IDM_EDIT_COPY                   40006
#define IDM_EDIT_PASTE                  40007
#define IDM_HELP_HELP                   40008
#define IDM_HELP_ABOUT                  40009
#define IDM_FILE_CLOSE                  40010
// »дентификаторы команд меню с префиксом IDM_.
// ќни св€заны с обработчиками команд меню (new, open, exit и т.д.).

// Ќовые команды меню дл€ лабораторной работы 15
#define IDM_VIEW_TEXT                   40011
#define IDM_VIEW_CTL                    40012
#define IDM_VIEW_MODELESS               40013
// ƒополнительные команды меню, добавленные дл€ конкретной лабораторной работы.
// ќбычно используютс€ дл€ переключени€ представлений или открыти€ окон.

// Ёлементы управлени€ в диалогах
#define IDC_APP_ICON                    5001
#define IDC_CURRENT_TIME                5002
#define IDC_EDIT_TEXT                   5003
#define IDC_LIST_ITEMS                  5004
#define IDC_READ_BUTTON                 5005
#define IDC_ADD_BUTTON                  5006
#define IDC_LOAD_BUTTON                 5007
#define IDC_EDIT_CONTENT                5008
#define IDS_APP_TITLE                   101
// »дентификаторы контролов внутри диалогов: кнопок, списков, полей ввода и т.п.
// IDS_APP_TITLE Ч идентификатор строкового ресурса с названием приложени€.

// ƒл€ генератора ресурсов
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        133
#define _APS_NEXT_COMMAND_VALUE         40014
#define _APS_NEXT_CONTROL_VALUE         5009
#define _APS_NEXT_SYMED_VALUE           110
#endif
#endif
// Ёти макросы используютс€ Visual Studio (App Studio) дл€ автоматического 
// присвоени€ новых значений при добавлении ресурсов Ч предотвращают конфликт ID.
// _APS_NEXT_RESOURCE_VALUE Ч следующий ID ресурса.
// _APS_NEXT_COMMAND_VALUE Ч следующий ID команды (например, дл€ меню).
// _APS_NEXT_CONTROL_VALUE Ч следующий ID элемента управлени€.
// _APS_NEXT_SYMED_VALUE Ч следующий ID дл€ символов (SYMED) ресурса (например, дл€ иконок и др.).

