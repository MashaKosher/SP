#include <windows.h>
#pragma comment(lib, "Shlwapi.lib")
#include "Controls.h"
#include "globals.h"
#include "algorithms.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Регистрируем класс окна
    const wchar_t CLASS_NAME[] = L"Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc; // процедура окна
    wc.hInstance = hInstance;   // дескриптор приложения
    wc.lpszClassName = CLASS_NAME; // строка идентификатор класса окна

    RegisterClass(&wc); // регистриуем окно в ОС

    // {{{ Создаем окно
    RECT rc = { 0, 0, gl::winWidth, gl::winHeight};
    AdjustWindowRect(&rc, WS_POPUP, FALSE);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, NULL,
        WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }
    // конец создания окна }}}

    SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); // изменяем положение окна
    ShowWindow(hwnd, nCmdShow);  // показываем окно

    // {{{ Основной цикл обработки сообщений
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) // извлекаем сообщение
    {
        TranslateMessage(&msg); // Переводимм сообщение в норм формат
        DispatchMessage(&msg); // Отправляет сообщение из очереди в WindowProc
    }

    return 0;
    // Конец основного цикла обработки сообщений }}}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: // создание окна
        MainWndAddWidgets(hwnd);
        break;
    case WM_COMMAND:  // пользовательское действие
        if (LOWORD(wParam) == BN_CLICKED) {
            if ((HWND)lParam == gl::hButtonScrUp) {
                // Отправка события прокрутки вверх
                SendMessage(gl::table->GetHandle(), WM_MOUSEWHEEL, MAKEWPARAM(0, WHEEL_DELTA), 0);
            }
            else if ((HWND)lParam == gl::hButtonScrDown) {
                // Отправка события прокрутки вниз
                SendMessage(gl::table->GetHandle(), WM_MOUSEWHEEL, MAKEWPARAM(0, -WHEEL_DELTA), 0);
            }
            else if ((HWND)lParam == gl::hButtonFontSettings) {
                // Создание окна с настройками шрфита
                CreateFontSettingsWindow(hwnd);
            }
        }
        break;
    case WM_DESTROY: // Разрушение окна
        RemoveCustomFonts();
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE: // закрытие окна
        DestroyWindow(hwnd);

    case WM_PAINT:// красим главное окно
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    case WM_CTLCOLOREDIT: {  // изменение цвета
        HDC hdcEdit = (HDC)wParam;

        SetTextColor(hdcEdit, gl::fontColor[gl::table->GetFontColorIndices()[TITLE]].color); // Цвет текста
        SetBkMode(hdcEdit, TRANSPARENT); // Прозрачный фон

        // Установка кисти для фона
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // Белый фон
        return (LRESULT)hBrush; // Возвращаем кисть для фона
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); // если Case не подходит то кидает сообщение в функцию и Windows сам обрабатывает
}