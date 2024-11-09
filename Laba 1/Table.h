#pragma once
#include <windows.h>
#include <vector>
#include <string>

class Table {
    HWND hWnd; // ���������� ����
    //std::vector<std::wstring> input, output; // input - ������ � ����������. �������, output - c ����������������
    std::vector<std::pair<std::wstring, FILETIME>> input, output;
    int tableWidth, tableHeight; // ������, ������ �������
    int recordingCapacity = 30; // ������������ ���������� ������� � �������, ������� ������������ �� ������
    int scrollPos = 0; // ������ ������ ��� ����������� ������ ������ � ������� �� �������� input � output (������������ ��� ������ �������)
    const int columnCount = 4; // ���������� �������� � �������

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ���������� ����
    void DrawContent(); // ��������� ���� ������ �� �������� input � output
    void DrawLayout(); // ��������� �������������� ����� ������� + ���������
    void ClearRecords() { input.clear(); output.clear(); scrollPos = 0; } // ����� ������� (������� �������� input, output � ����� scrollPos)
    std::wstring TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc); // �������� ������ �� ������ �����, ���� ����� ������ ��������� ������ ������� � �������

public:
    Table(HWND parent);
    void AddRecord(const std::pair<std::wstring, FILETIME> record); // ���������� ����� � ������ input, output
    HWND GetHandle() const { return hWnd; } // ��������� ����������� ����
    int GetTableHeight() { return tableHeight; } // ��������� ������ �������
    int GetRowHeight() { return tableHeight / (recordingCapacity + 1); } // ��������� ������ ������ �������
    void FillOutTable(const std::wstring& directoryPath); // ���������� ������� �������, ������� ��������� � ���������� ����������
    int GetTableRecordsCount() { return (int)output.size(); } // ���������� ������� ������ ������� output
};