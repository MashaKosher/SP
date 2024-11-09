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
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ���������� ����
    void DrawLayout(HDC &hdc); // ��������� �������������� ����� ������� + ���������
    void DrawContent(HDC &hdc, PAINTSTRUCT& ps); // ��������� ���� ������ �� �������� input � output
    void Redraw(HDC &hdc, PAINTSTRUCT& ps); // ����������� ���� �������

    class Title
    {
        friend void Table::DrawLayout(HDC &hdc); // ��������� �������������� ����� ������� + ���������
        friend void Table::Redraw(HDC &hdc, PAINTSTRUCT& ps); // ����������� ���� �������

        HWND hWnd; // ���������� ����
        const int height = 50;
        int width;
        const int columnCount = 4; // ���������� �������� � ���������
        static HWND  hDirectoryEdit;
        static HFONT hFont;
        int fontColor = 0, fontSize = 2, fontName = 0;

        static LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ��������� ��������� ���� ��� ���� ����� ����������
        void Draw(HDC &hdc); // ��������� ���������
    public:
        Title(HWND &table, int width);
        HWND GetDirectoryHandle() { return hDirectoryEdit; }
        HFONT GetFont() { return hFont; }
        int& GetFontNameIndex() { return fontName; } // ��������� ������� �������� ������
        int& GetFontSizeIndex() { return fontSize; } // ��������� ������� ������� ������
        int& GetFontColorIndex() { return fontColor; } // ��������� ������� ����� ������
        int GetHeight() { return height; } // ��������� ������ ���������
    };

    static HWND hWnd; // ���������� ����
    static Title* title; // ��������� �������
    std::vector<std::wstring> input, output; // input - ������ � ����������. �������, output - c ����������������
    int tableWidth, tableHeight; // ������, ������ �������
    int recordingCapacity = 45; // ������������ ���������� ������� � �������, ������� ������������ �� ������
    int scrollPos = 0; // ������ ������ ��� ����������� ������ ������ � ������� �� �������� input � output (������������ ��� ������ �������)
    const int columnCount = 4; // ���������� �������� � �������

    // ������ �������� � �������� ���� - ������� ���������� �������, ������ - ������� ���������� �� ������, ������ - ������� ���������� �� ����������(����� ������)
    // ������� ���� ������� � ���������� ������� gl::fontName
    std::vector<int> fontName = {0, 0, 0 };
    // ������� �������� ������� � ���������� ������� gl::fontSize
    std::vector<int> fontSize = {3, 1 ,1 };
    // ������� ������ ������� � ���������� ������� gl::fontColor
    std::vector<int> fontColor = {0, 0, 0 };

    bool isRedrawRequired = false; // true, ���� ���������� ������������ ��� �������, ����� - false

    void ClearRecords() { input.clear(); output.clear(); scrollPos = 0; } // ����� ������� (������� �������� input, output � ����� scrollPos)
    std::wstring TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc); // �������� ������ �� ������ �����, ���� ����� ������ ��������� ������ ������� � �������
    bool IsTitle(const int y) { return y <= title->GetHeight(); }
    int GetRowNumber(const int y) { int res = (y - title->GetHeight()) / ((tableHeight - title->GetHeight()) / recordingCapacity) + scrollPos + 1; res = res > recordingCapacity + scrollPos? recordingCapacity + scrollPos : res; return res; }
    int GetColumnNumber(const int x) { int res = x / (tableWidth / columnCount) + 1; return res; }

public:
    Table(HWND parent, const struct Font fontTitle = Font(FONT_NAME::FONT_ARIAL, FONT_COLOR::COLOR_BLACK, FONT_SIZE::SIZE_XLARGE),
                       const struct Font fontFileInfo = Font(FONT_NAME::FONT_ARIAL, FONT_COLOR::COLOR_BLACK, FONT_SIZE::SIZE_MEDIUM),
                       const struct Font fontSortingInfo = Font(FONT_NAME::FONT_ARIAL, FONT_COLOR::COLOR_BLACK, FONT_SIZE::SIZE_MEDIUM));
    void AddRecord(const std::wstring record); // ���������� ����� � ������ input, output
    HWND GetHandle() const { return hWnd; } // ��������� ����������� ����
    int GetTableHeight() { return tableHeight; } // ��������� ������ �������
    int GetRowHeight() { return tableHeight / (recordingCapacity + 1); } // ��������� ������ ������ �������
    std::vector<int>& GetFontNameIndices(){ return fontName; } // ��������� ������� � ��������� �������� �������
    std::vector<int>& GetFontSizeIndices(){ return fontSize; } // ��������� ������� � ��������� ������� �������
    std::vector<int>& GetFontColorIndices(){ return fontColor; } // ��������� ������� � ��������� ����� �������
    void UpdateTitleFont(); // ���������� ������ ���������
    void FillOutTable(const std::wstring& directoryPath); // ���������� ������� �������, ������� ��������� � ���������� ����������
    int GetTableRecordsCount() { return (int)output.size(); } // ���������� ������� ������ ������� output
    void RequestRedraw() { isRedrawRequired = true; InvalidateRect(hWnd, NULL, TRUE);} // ������ ����������� ���� �������
};