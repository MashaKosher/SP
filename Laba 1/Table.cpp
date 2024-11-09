#include "Table.h"
#include "globals.h"
#include "algorithms.h"
#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <windows.h>
//#include <stdio.h>
//#pragma comment(lib, "user32.lib")
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <shlwapi.h>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <regex>
#include <sstream>;
#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

LRESULT CALLBACK Table::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Table* listView = (Table*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (uMsg == WM_NCCREATE) {
        // Извлечение указателя на экземпляр класса Table, чтобы обращаться к методам и свойствам объекта внутри обработчика
        listView = (Table*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)listView); // Сохраняем указатель на создаваемый объект Table в пользовательских данных окна для дальнейшего доступа к объекту
    }

    switch (uMsg) {
    case WM_PAINT:  // рисование таблицы
        if (listView) {
            listView->DrawLayout(); // Отрисовка статической части таблицы
            listView->DrawContent(); // Отрисовка списка файлов в таблице
        }
        return 0;

    case WM_MOUSEWHEEL: // обработка прокрутки
        if (listView) {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            int newScrollPos = listView->scrollPos;
            newScrollPos += (delta > 0) ? -listView->recordingCapacity : listView->recordingCapacity; // Прокрутка вверх или вниз

            // Проверка на выход за пределы вектора
            newScrollPos = newScrollPos < (int)listView->input.size() - listView->recordingCapacity ? newScrollPos : (int)listView->input.size() - listView->recordingCapacity;
            newScrollPos = 0 > newScrollPos ? 0 : newScrollPos;

            if (newScrollPos != listView->scrollPos)
            {
                listView->scrollPos = newScrollPos;
                InvalidateRect(listView->hWnd, NULL, TRUE); // Перерисовка
            }
        }
        return 0;
    case WM_DESTROY: // разрушеник окна
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);  // встроенный обработчик Windows
}

// Преобразование TIME в строку
std::wstring fileTimeToWstring(const FILETIME& ft) {
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    std::wstringstream ws;
    ws << std::setfill(L'0') << std::setw(3) << st.wMilliseconds
        << std::setfill(L'0') << std::setw(2) << st.wSecond
        << std::setfill(L'0') << std::setw(2) << st.wMinute
        << std::setfill(L'0') << std::setw(2) << st.wHour
        << std::setfill(L'0') << std::setw(2) << st.wDay
        << std::setfill(L'0') << std::setw(2) << st.wMonth
        << st.wYear;

    //ws << std::setfill(L'0') << std::setw(2) << st.wDay << L"/"
    //    << std::setfill(L'0') << std::setw(2) << st.wMonth << L"/"
    //    << st.wYear << L" "
    //    << std::setfill(L'0') << std::setw(2) << st.wHour << L":"
    //    << std::setfill(L'0') << std::setw(2) << st.wMinute << L":"
    //    << std::setfill(L'0') << std::setw(2) << st.wSecond
    //    << std::setfill(L'0') << std::setw(2) << st.wMilliseconds;
    return ws.str();
}


void Table::DrawLayout()  // отрисовка статической части
{
    if (input.size() != 0)
        return;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HBRUSH hBrush = CreateSolidBrush(RGB(204, 255, 204));
    RECT rect1;
    RECT rect2;
    GetClientRect(hWnd, &rect1);
    GetClientRect(hWnd, &rect2);
    rect1.left = 1;  // X начальной точки
    rect1.top = 0;   // Y начальной точки
    // X конечной точки
    rect1.bottom = 26; // Y конечной точки
    rect2.top = gl::winHeight - 50;
    rect2.bottom = 50;
    // Закрашиваем прямоугольник
    FillRect(hdc, &rect2, hBrush);

    // Устанавливаем цвет фона
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    //FillRect(hdc, &rect1, hBrush);

    const int columnWidth = tableWidth / columnCount; // Ширина столбца
    const int rowHeight = tableHeight / (recordingCapacity + 1); // Высота строки
    const int leftDisp = 3; // Левый отступ текста в столбце

    // Рисование вертикальных линий между столбцами
    for (size_t j = 1; j < columnCount; ++j) {
        int x = j * columnWidth;
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, tableHeight);
    }

    // Рисование горизонтальных линий между строками
    MoveToEx(hdc, 0, rowHeight, NULL);
    LineTo(hdc, tableWidth, rowHeight);
    const int wW = GetSystemMetrics(SM_CXSCREEN);
    const int wH = GetSystemMetrics(SM_CYSCREEN);

    MoveToEx(hdc, 0, 0, NULL);
    LineTo(hdc, wW, 0);
    //MoveToEx(hdc, wW-1, 1, NULL);
    //LineTo(hdc, wW-1, wH - 1);






    // высота текста
    const int textHeight = 20;

    // Создание шрифта для заголовков и содержимого таблицы
    HFONT hHeaderFont = CreateFont(
        textHeight,                // Height
        0,                 // Width
        0,                 // Escapement
        0,                 // Orientation
        FW_NORMAL,        // Weight
        FALSE,            // Italic
        FALSE,            // Underline
        FALSE,            // StrikeOut
        DEFAULT_CHARSET,  // CharSet
        OUT_DEFAULT_PRECIS, // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,  // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        L"Times New Roman"          // Font Name
    );

    // Сохраняем старый шрифт
    HFONT hOldFont = (HFONT)SelectObject(hdc, hHeaderFont);

    SetBkMode(hdc, TRANSPARENT);

    // Рисование заголовков столбцов
    std::wstring headerText = TrimTextToWidth(L"Время последнего доступа", columnWidth, hdc);
    TextOut(hdc, leftDisp + columnWidth, (rowHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"Результат сортировки", columnWidth, hdc);
    TextOut(hdc, leftDisp + 2 * columnWidth, (rowHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"Время последнего доступа", columnWidth, hdc);
    TextOut(hdc, leftDisp + 3 * columnWidth, (rowHeight - textHeight) / 2, headerText.c_str(), headerText.length());

    // Восстанавливаем старый шрифт
    SelectObject(hdc, hOldFont);
    DeleteObject(hHeaderFont); // Удаляем созданный шрифт

    EndPaint(hWnd, &ps); // конец процедуры рисования
}

void Table::DrawContent()  // отрисовка динамической части
{
    if (input.size() == 0)
        return;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    const int columnWidth = tableWidth / columnCount; // Ширина столбца
    const int rowHeight = tableHeight / (recordingCapacity + 1); // Высота строки
    const int leftDisp = 3; // Левый отступ текста в столбце
    
    // очистка столбцов
    for (int i = 0; i < columnCount; i++)
    {
        RECT fillRect =
        {
            ps.rcPaint.left + columnWidth * i + 1,
            ps.rcPaint.top + rowHeight + 1,
            columnWidth * (i + 1) - 1,
            ps.rcPaint.bottom - 1
        };

        // Устанавливаем цвет фона
        FillRect(hdc, &fillRect, (HBRUSH)(COLOR_WINDOW + 1));
    }

    // высота текста
    const int textHeight = 18;

    // Создание шрифта для заголовков и содержимого таблицы
    HFONT hContentFont = CreateFont(
        textHeight,                // Height
        0,                 // Width
        0,                 // Escapement
        0,                 // Orientation
        FW_NORMAL,        // Weight
        FALSE,            // Italic
        FALSE,            // Underline
        FALSE,            // StrikeOut
        DEFAULT_CHARSET,  // CharSet
        OUT_DEFAULT_PRECIS, // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,  // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        L"Times New Roman"          // Font Name
    );

    // Сохраняем старый шрифт
    HFONT hOldFont = (HFONT)SelectObject(hdc, hContentFont);
    SetBkMode(hdc, TRANSPARENT);

    const int remainingRecordsCount = ((int)input.size() - scrollPos) < (recordingCapacity) ? ((int)input.size() - scrollPos) : recordingCapacity; // Количество записей на вывод в таблицу


    // Рисование содержимого таблицы
    for (size_t j = 0; j < remainingRecordsCount; ++j) {
        std::wstring text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + input[scrollPos + j].first, columnWidth, hdc); // Вывод найденных файлов с расширением
        TextOut(hdc, leftDisp, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());

        //text = TrimTextToWidth(RemoveFileExtension(input[scrollPos + j]), columnWidth, hdc); // Вывод имен найденных файлов 
        text = fileTimeToWstring(input[scrollPos + j].second); // Вывод имен найденных файлов 
        //text = text == L"" ? L"*пустое*" : text;
        TextOut(hdc, leftDisp + columnWidth, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + output[scrollPos + j].first, columnWidth, hdc); // Вывод отсортированных файлов с расширением
        TextOut(hdc, leftDisp + 2 * columnWidth, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());

        text = fileTimeToWstring(output[scrollPos + j].second); // Вывод имен отсортированных файлов
        //text = text == L"" ? L"*пустое*" : text;
        TextOut(hdc, leftDisp + 3 * columnWidth, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());
    }

    // Восстанавливаем старый шрифт
    SelectObject(hdc, hOldFont);
    DeleteObject(hContentFont);

    EndPaint(hWnd, &ps);
}



std::wstring Table::TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc)
{
    std::wstring res = text;
    // Определяем максимальную ширину текста для столбца
    SIZE textSize;
    GetTextExtentPoint32(hdc, res.c_str(), res.length(), &textSize);

    // Если текст шире столбца, обрезать его
    if (textSize.cx > (columnWidth - 10)) {
        // Урезаем текст до подходящей длины
        while (res.length() > 0 && textSize.cx > (columnWidth - 10)) {
            res.pop_back(); // Удаляем последний символ
            GetTextExtentPoint32(hdc, res.c_str(), res.length(), &textSize);
        }
        // Добавляем многоточие, если текст был обрезан
        if (res.length() > 0) {
            res += L"...";
        }
    }
    return res;
}

Table::Table(HWND parent) {
    // Регистрируем кастомный класс таблицы
    const wchar_t CLASS_NAME[] = L"Custom Table";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;  // процедура окна
    wc.hInstance = GetModuleHandle(NULL); // дескриптор приложения
    wc.lpszClassName = CLASS_NAME; // строка идентификатор класса окна
    RegisterClass(&wc); // регистриуем окно в ОС

    tableWidth = gl::winWidth;
    tableHeight = gl::winHeight - gl::btnHeight - 20;

    // Создаем окно
    hWnd = CreateWindowEx(
        0,
        CLASS_NAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, tableWidth, tableHeight,
        parent, NULL, NULL, this
    );
}

void Table::AddRecord(const std::pair<std::wstring, FILETIME> record)
{
    input.push_back(record);
    output.push_back(record);
}

void Table::FillOutTable(const std::wstring& directoryPath)
{
    gl::table->ClearRecords();

    ListFiles((std::wstring)directoryPath);


    switch (output.size())
    {
    case 1: // Вывод одного файла (не нужна сортировка)
        InvalidateRect(this->hWnd, NULL, TRUE);
        break;
    default: // Сортировка множества файлов с выводом
        SortFiles(output, 0, output.size() - 1);
        InvalidateRect(this->hWnd, NULL, TRUE);
        break;
    }
}