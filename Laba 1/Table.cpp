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
        // ���������� ��������� �� ��������� ������ Table, ����� ���������� � ������� � ��������� ������� ������ �����������
        listView = (Table*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)listView); // ��������� ��������� �� ����������� ������ Table � ���������������� ������ ���� ��� ����������� ������� � �������
    }

    switch (uMsg) {
    case WM_PAINT:  // ��������� �������
        if (listView) {
            listView->DrawLayout(); // ��������� ����������� ����� �������
            listView->DrawContent(); // ��������� ������ ������ � �������
        }
        return 0;

    case WM_MOUSEWHEEL: // ��������� ���������
        if (listView) {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            int newScrollPos = listView->scrollPos;
            newScrollPos += (delta > 0) ? -listView->recordingCapacity : listView->recordingCapacity; // ��������� ����� ��� ����

            // �������� �� ����� �� ������� �������
            newScrollPos = newScrollPos < (int)listView->input.size() - listView->recordingCapacity ? newScrollPos : (int)listView->input.size() - listView->recordingCapacity;
            newScrollPos = 0 > newScrollPos ? 0 : newScrollPos;

            if (newScrollPos != listView->scrollPos)
            {
                listView->scrollPos = newScrollPos;
                InvalidateRect(listView->hWnd, NULL, TRUE); // �����������
            }
        }
        return 0;
    case WM_DESTROY: // ���������� ����
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);  // ���������� ���������� Windows
}

// �������������� TIME � ������
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


void Table::DrawLayout()  // ��������� ����������� �����
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
    rect1.left = 1;  // X ��������� �����
    rect1.top = 0;   // Y ��������� �����
    // X �������� �����
    rect1.bottom = 26; // Y �������� �����
    rect2.top = gl::winHeight - 50;
    rect2.bottom = 50;
    // ����������� �������������
    FillRect(hdc, &rect2, hBrush);

    // ������������� ���� ����
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    //FillRect(hdc, &rect1, hBrush);

    const int columnWidth = tableWidth / columnCount; // ������ �������
    const int rowHeight = tableHeight / (recordingCapacity + 1); // ������ ������
    const int leftDisp = 3; // ����� ������ ������ � �������

    // ��������� ������������ ����� ����� ���������
    for (size_t j = 1; j < columnCount; ++j) {
        int x = j * columnWidth;
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, tableHeight);
    }

    // ��������� �������������� ����� ����� ��������
    MoveToEx(hdc, 0, rowHeight, NULL);
    LineTo(hdc, tableWidth, rowHeight);
    const int wW = GetSystemMetrics(SM_CXSCREEN);
    const int wH = GetSystemMetrics(SM_CYSCREEN);

    MoveToEx(hdc, 0, 0, NULL);
    LineTo(hdc, wW, 0);
    //MoveToEx(hdc, wW-1, 1, NULL);
    //LineTo(hdc, wW-1, wH - 1);






    // ������ ������
    const int textHeight = 20;

    // �������� ������ ��� ���������� � ����������� �������
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

    // ��������� ������ �����
    HFONT hOldFont = (HFONT)SelectObject(hdc, hHeaderFont);

    SetBkMode(hdc, TRANSPARENT);

    // ��������� ���������� ��������
    std::wstring headerText = TrimTextToWidth(L"����� ���������� �������", columnWidth, hdc);
    TextOut(hdc, leftDisp + columnWidth, (rowHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"��������� ����������", columnWidth, hdc);
    TextOut(hdc, leftDisp + 2 * columnWidth, (rowHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"����� ���������� �������", columnWidth, hdc);
    TextOut(hdc, leftDisp + 3 * columnWidth, (rowHeight - textHeight) / 2, headerText.c_str(), headerText.length());

    // ��������������� ������ �����
    SelectObject(hdc, hOldFont);
    DeleteObject(hHeaderFont); // ������� ��������� �����

    EndPaint(hWnd, &ps); // ����� ��������� ���������
}

void Table::DrawContent()  // ��������� ������������ �����
{
    if (input.size() == 0)
        return;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    const int columnWidth = tableWidth / columnCount; // ������ �������
    const int rowHeight = tableHeight / (recordingCapacity + 1); // ������ ������
    const int leftDisp = 3; // ����� ������ ������ � �������
    
    // ������� ��������
    for (int i = 0; i < columnCount; i++)
    {
        RECT fillRect =
        {
            ps.rcPaint.left + columnWidth * i + 1,
            ps.rcPaint.top + rowHeight + 1,
            columnWidth * (i + 1) - 1,
            ps.rcPaint.bottom - 1
        };

        // ������������� ���� ����
        FillRect(hdc, &fillRect, (HBRUSH)(COLOR_WINDOW + 1));
    }

    // ������ ������
    const int textHeight = 18;

    // �������� ������ ��� ���������� � ����������� �������
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

    // ��������� ������ �����
    HFONT hOldFont = (HFONT)SelectObject(hdc, hContentFont);
    SetBkMode(hdc, TRANSPARENT);

    const int remainingRecordsCount = ((int)input.size() - scrollPos) < (recordingCapacity) ? ((int)input.size() - scrollPos) : recordingCapacity; // ���������� ������� �� ����� � �������


    // ��������� ����������� �������
    for (size_t j = 0; j < remainingRecordsCount; ++j) {
        std::wstring text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + input[scrollPos + j].first, columnWidth, hdc); // ����� ��������� ������ � �����������
        TextOut(hdc, leftDisp, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());

        //text = TrimTextToWidth(RemoveFileExtension(input[scrollPos + j]), columnWidth, hdc); // ����� ���� ��������� ������ 
        text = fileTimeToWstring(input[scrollPos + j].second); // ����� ���� ��������� ������ 
        //text = text == L"" ? L"*������*" : text;
        TextOut(hdc, leftDisp + columnWidth, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + output[scrollPos + j].first, columnWidth, hdc); // ����� ��������������� ������ � �����������
        TextOut(hdc, leftDisp + 2 * columnWidth, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());

        text = fileTimeToWstring(output[scrollPos + j].second); // ����� ���� ��������������� ������
        //text = text == L"" ? L"*������*" : text;
        TextOut(hdc, leftDisp + 3 * columnWidth, rowHeight + (j * rowHeight) + (rowHeight - textHeight) / 2, text.c_str(), text.length());
    }

    // ��������������� ������ �����
    SelectObject(hdc, hOldFont);
    DeleteObject(hContentFont);

    EndPaint(hWnd, &ps);
}



std::wstring Table::TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc)
{
    std::wstring res = text;
    // ���������� ������������ ������ ������ ��� �������
    SIZE textSize;
    GetTextExtentPoint32(hdc, res.c_str(), res.length(), &textSize);

    // ���� ����� ���� �������, �������� ���
    if (textSize.cx > (columnWidth - 10)) {
        // ������� ����� �� ���������� �����
        while (res.length() > 0 && textSize.cx > (columnWidth - 10)) {
            res.pop_back(); // ������� ��������� ������
            GetTextExtentPoint32(hdc, res.c_str(), res.length(), &textSize);
        }
        // ��������� ����������, ���� ����� ��� �������
        if (res.length() > 0) {
            res += L"...";
        }
    }
    return res;
}

Table::Table(HWND parent) {
    // ������������ ��������� ����� �������
    const wchar_t CLASS_NAME[] = L"Custom Table";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;  // ��������� ����
    wc.hInstance = GetModuleHandle(NULL); // ���������� ����������
    wc.lpszClassName = CLASS_NAME; // ������ ������������� ������ ����
    RegisterClass(&wc); // ����������� ���� � ��

    tableWidth = gl::winWidth;
    tableHeight = gl::winHeight - gl::btnHeight - 20;

    // ������� ����
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
    case 1: // ����� ������ ����� (�� ����� ����������)
        InvalidateRect(this->hWnd, NULL, TRUE);
        break;
    default: // ���������� ��������� ������ � �������
        SortFiles(output, 0, output.size() - 1);
        InvalidateRect(this->hWnd, NULL, TRUE);
        break;
    }
}