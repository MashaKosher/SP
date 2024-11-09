#pragma once
#include <windows.h>
#include <vector>
#include <string>

#define TITLE 0
#define FILE_INFO 1
#define SORTING_INFO 2

class Table {
    HWND hWnd; // ���������� ����
    std::vector<std::pair<std::wstring, FILETIME>> input, output; // input - ������ � ����������. �������, output - c ����������������
    int tableWidth, tableHeight; // ������, ������ �������
    const int titleHeight = 40; // ������ ��������� �������
    int recordingCapacity = 45; // ������������ ���������� ������� � �������, ������� ������������ �� ������
    int scrollPos = 0; // ������ ������ ��� ����������� ������ ������ � ������� �� �������� input � output (������������ ��� ������ �������)
    const int columnCount = 4; // ���������� �������� � �������

    // ������ �������� � �������� ���� - ������� ���������� �������, ������ - ������� ���������� �� ������, ������ - ������� ���������� �� ����������(����� ������)
    // ������� ���� ������� � ���������� ������� gl::fontName
    std::vector<int> fontName = { 0, 0, 0 };
    // ������� �������� ������� � ���������� ������� gl::fontSize
    std::vector<int> fontSize = { 2, 1 ,1 };
    // ������� ������ ������� � ���������� ������� gl::fontColor
    std::vector<int> fontColor = { 0, 0, 0 };

    bool isRedrawRequired = false; // true, ���� ���������� ������������ ��� �������, ����� - false

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ���������� ����
    void DrawLayout(); // ��������� �������������� ����� ������� + ���������
    void DrawContent(); // ��������� ���� ������ �� �������� input � output
    void Redraw(); // ����������� ���� �������
    void ClearRecords() { input.clear(); output.clear(); scrollPos = 0; } // ����� ������� (������� �������� input, output � ����� scrollPos)
    std::wstring TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc); // �������� ������ �� ������ �����, ���� ����� ������ ��������� ������ ������� � �������

public:
    Table(HWND parent);
    void AddRecord(const std::pair<std::wstring, FILETIME> record); // ���������� ����� � ������ input, output
    HWND GetHandle() const { return hWnd; } // ��������� ����������� ����
    int GetTableHeight() { return tableHeight; } // ��������� ������ �������
    int GetRowHeight() { return tableHeight / (recordingCapacity + 1); } // ��������� ������ ������ �������
    std::vector<int>& GetFontNameIndices(){ return fontName; } // ��������� ������� � ��������� �������� �������
    std::vector<int>& GetFontSizeIndices(){ return fontSize; } // ��������� ������� � ��������� ������� �������
    std::vector<int>& GetFontColorIndices(){ return fontColor; } // ��������� ������� � ��������� ����� �������
    void FillOutTable(const std::wstring& directoryPath); // ���������� ������� �������, ������� ��������� � ���������� ����������
    int GetTableRecordsCount() { return (int)output.size(); } // ���������� ������� ������ ������� output
    void RequestRedraw() { isRedrawRequired = true; InvalidateRect(hWnd, NULL, TRUE);} // ������ ����������� ���� �������
};