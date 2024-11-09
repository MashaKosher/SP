#pragma once
#include <windows.h>
#include <vector>
#include <string>

class Table {
    HWND hWnd; // Дескриптор окна
    //std::vector<std::wstring> input, output; // input - вектор с неотсортир. файлами, output - c отсортированными
    std::vector<std::pair<std::wstring, FILETIME>> input, output;
    int tableWidth, tableHeight; // Ширина, высота таблицы
    int recordingCapacity = 30; // Максимальное количество записей в таблице, которое отображается на экране
    int scrollPos = 0; // Хранит индекс для правильного вывода файлов в таблицу из векторов input и output (используется при скроле таблицы)
    const int columnCount = 4; // Количество столбцов в таблице

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Обработчик окна
    void DrawContent(); // Отрисовка имен файлов из векторов input и output
    void DrawLayout(); // Отрисовка ограничивающих линий таблицы + заголовки
    void ClearRecords() { input.clear(); output.clear(); scrollPos = 0; } // Сброс таблицы (очистка векторов input, output и сброс scrollPos)
    std::wstring TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc); // Усечение текста до нужной длины, если длина текста превышает ширину столбца в таблице

public:
    Table(HWND parent);
    void AddRecord(const std::pair<std::wstring, FILETIME> record); // Добавление файла в вектор input, output
    HWND GetHandle() const { return hWnd; } // Получение дескриптора окна
    int GetTableHeight() { return tableHeight; } // Получение высоты таблицы
    int GetRowHeight() { return tableHeight / (recordingCapacity + 1); } // Получение высоты строки таблицы
    void FillOutTable(const std::wstring& directoryPath); // Заполнение таблицы файлами, которые находятся в переданной директории
    int GetTableRecordsCount() { return (int)output.size(); } // Возвращает текущий размер вектора output
};