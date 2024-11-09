#pragma once
#include <windows.h>
#include <vector>
#include <string>

#define TITLE 0
#define FILE_INFO 1
#define SORTING_INFO 2

class Table {
    HWND hWnd; // Дескриптор окна
    std::vector<std::pair<std::wstring, FILETIME>> input, output; // input - вектор с неотсортир. файлами, output - c отсортированными
    int tableWidth, tableHeight; // Ширина, высота таблицы
    const int titleHeight = 40; // Высота заголовка таблицы
    int recordingCapacity = 45; // Максимальное количество записей в таблице, которое отображается на экране
    int scrollPos = 0; // Хранит индекс для правильного вывода файлов в таблицу из векторов input и output (используется при скроле таблицы)
    const int columnCount = 4; // Количество столбцов в таблице

    // первые элементы в массивах ниже - индексы заголовков таблицы, вторые - индексы информации по файлам, третьи - индексы информации по сортировке(имена файлов)
    // Индексы имен шрифтов в глобальном массиве gl::fontName
    std::vector<int> fontName = { 0, 0, 0 };
    // Индексы размеров шрифтов в глобальном массиве gl::fontSize
    std::vector<int> fontSize = { 2, 1 ,1 };
    // Индексы цветов шрифтов в глобальном массиве gl::fontColor
    std::vector<int> fontColor = { 0, 0, 0 };

    bool isRedrawRequired = false; // true, если необходимо перерисовать всю таблицу, иначе - false

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Обработчик окна
    void DrawLayout(); // Отрисовка ограничивающих линий таблицы + заголовки
    void DrawContent(); // Отрисовка имен файлов из векторов input и output
    void Redraw(); // Перерисовка всей таблицы
    void ClearRecords() { input.clear(); output.clear(); scrollPos = 0; } // Сброс таблицы (очистка векторов input, output и сброс scrollPos)
    std::wstring TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc); // Усечение текста до нужной длины, если длина текста превышает ширину столбца в таблице

public:
    Table(HWND parent);
    void AddRecord(const std::pair<std::wstring, FILETIME> record); // Добавление файла в вектор input, output
    HWND GetHandle() const { return hWnd; } // Получение дескриптора окна
    int GetTableHeight() { return tableHeight; } // Получение высоты таблицы
    int GetRowHeight() { return tableHeight / (recordingCapacity + 1); } // Получение высоты строки таблицы
    std::vector<int>& GetFontNameIndices(){ return fontName; } // Получение массива с индексами названий шрифтов
    std::vector<int>& GetFontSizeIndices(){ return fontSize; } // Получение массива с индексами размера шрифтов
    std::vector<int>& GetFontColorIndices(){ return fontColor; } // Получение массива с индексами цвета шрифтов
    void FillOutTable(const std::wstring& directoryPath); // Заполнение таблицы файлами, которые находятся в переданной директории
    int GetTableRecordsCount() { return (int)output.size(); } // Возвращает текущий размер вектора output
    void RequestRedraw() { isRedrawRequired = true; InvalidateRect(hWnd, NULL, TRUE);} // Запрос перерисовки всей таблицы
};