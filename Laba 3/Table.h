#pragma once
#include <windows.h>
#include <vector>
#include <string>

#define TITLE 0
#define FILE_INFO 1
#define SORTING_INFO 2

enum FONT_NAME
{
    FONT_ARIAL,
    FONT_TIMES_NEW_ROMAN,
    FONT_CALIBRI,
    FONT_GEORGIA,
    FONT_TAHOMA
};
enum FONT_SIZE {
    SIZE_SMALL,      // 14
    SIZE_MEDIUM,     // 18
    SIZE_LARGE,      // 22
    SIZE_XLARGE,     // 26
    SIZE_XXLARGE     // 30
};
enum FONT_COLOR
{
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_GREY
};

struct Font {
    FONT_NAME name;
    FONT_COLOR color;
    FONT_SIZE size;

    Font(const FONT_NAME n, const FONT_COLOR c, const FONT_SIZE s) : name(n), color(c), size(s) {}
};

class Table {
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Обработчик окна
    void DrawLayout(HDC &hdc); // Отрисовка ограничивающих линий таблицы + заголовок
    void DrawContent(HDC &hdc, PAINTSTRUCT& ps); // Отрисовка имен файлов из векторов input и output
    void Redraw(HDC &hdc, PAINTSTRUCT& ps); // Перерисовка всей таблицы

    class Title
    {
        friend void Table::DrawLayout(HDC &hdc); // Отрисовка ограничивающих линий таблицы + заголовок
        friend void Table::Redraw(HDC &hdc, PAINTSTRUCT& ps); // Перерисовка всей таблицы

        HWND hWnd; // Дескриптор окна
        const int height = 50;
        int width;
        const int columnCount = 4; // Количество столбцов в заголовке
        static HWND  hDirectoryEdit;
        static HFONT hFont;
        int fontColor = 0, fontSize = 2, fontName = 0;

        static LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Кастомная процедура окна для поля ввода директории
        void Draw(HDC &hdc); // Отрисовка заголовка
    public:
        Title(HWND &table, int width);
        HWND GetDirectoryHandle() { return hDirectoryEdit; }
        HFONT GetFont() { return hFont; }
        int& GetFontNameIndex() { return fontName; } // Получение индекса названия шрифта
        int& GetFontSizeIndex() { return fontSize; } // Получение индекса размера шрифта
        int& GetFontColorIndex() { return fontColor; } // Получение индекса цвета шрифта
        int GetHeight() { return height; } // Получение высоты заголовка
    };

    static HWND hWnd; // Дескриптор окна
    static Title* title; // Заголовок таблицы
    std::vector<std::wstring> input, output; // input - вектор с неотсортир. файлами, output - c отсортированными
    int tableWidth, tableHeight; // Ширина, высота таблицы
    int recordingCapacity = 45; // Максимальное количество записей в таблице, которое отображается на экране
    int scrollPos = 0; // Хранит индекс для правильного вывода файлов в таблицу из векторов input и output (используется при скроле таблицы)
    const int columnCount = 4; // Количество столбцов в таблице

    // первые элементы в массивах ниже - индексы заголовков таблицы, вторые - индексы информации по файлам, третьи - индексы информации по сортировке(имена файлов)
    // Индексы имен шрифтов в глобальном массиве gl::fontName
    std::vector<int> fontName = {0, 0, 0 };
    // Индексы размеров шрифтов в глобальном массиве gl::fontSize
    std::vector<int> fontSize = {3, 1 ,1 };
    // Индексы цветов шрифтов в глобальном массиве gl::fontColor
    std::vector<int> fontColor = {0, 0, 0 };

    bool isRedrawRequired = false; // true, если необходимо перерисовать всю таблицу, иначе - false

    void ClearRecords() { input.clear(); output.clear(); scrollPos = 0; } // Сброс таблицы (очистка векторов input, output и сброс scrollPos)
    std::wstring TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc); // Усечение текста до нужной длины, если длина текста превышает ширину столбца в таблице
    bool IsTitle(const int y) { return y <= title->GetHeight(); }
    int GetRowNumber(const int y) { int res = (y - title->GetHeight()) / ((tableHeight - title->GetHeight()) / recordingCapacity) + scrollPos + 1; res = res > recordingCapacity + scrollPos? recordingCapacity + scrollPos : res; return res; }
    int GetColumnNumber(const int x) { int res = x / (tableWidth / columnCount) + 1; return res; }

public:
    Table(HWND parent, const struct Font fontTitle = Font(FONT_NAME::FONT_ARIAL, FONT_COLOR::COLOR_BLACK, FONT_SIZE::SIZE_XLARGE),
                       const struct Font fontFileInfo = Font(FONT_NAME::FONT_ARIAL, FONT_COLOR::COLOR_BLACK, FONT_SIZE::SIZE_MEDIUM),
                       const struct Font fontSortingInfo = Font(FONT_NAME::FONT_ARIAL, FONT_COLOR::COLOR_BLACK, FONT_SIZE::SIZE_MEDIUM));
    void AddRecord(const std::wstring record); // Добавление файла в вектор input, output
    HWND GetHandle() const { return hWnd; } // Получение дескриптора окна
    int GetTableHeight() { return tableHeight; } // Получение высоты таблицы
    int GetRowHeight() { return tableHeight / (recordingCapacity + 1); } // Получение высоты строки таблицы
    std::vector<int>& GetFontNameIndices(){ return fontName; } // Получение массива с индексами названий шрифтов
    std::vector<int>& GetFontSizeIndices(){ return fontSize; } // Получение массива с индексами размера шрифтов
    std::vector<int>& GetFontColorIndices(){ return fontColor; } // Получение массива с индексами цвета шрифтов
    void UpdateTitleFont(); // Обновление шрифта заголовка
    void FillOutTable(const std::wstring& directoryPath); // Заполнение таблицы файлами, которые находятся в переданной директории
    int GetTableRecordsCount() { return (int)output.size(); } // Возвращает текущий размер вектора output
    void RequestRedraw() { isRedrawRequired = true; InvalidateRect(hWnd, NULL, TRUE);} // Запрос перерисовки всей таблицы
};