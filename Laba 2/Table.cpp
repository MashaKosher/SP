#include "Table.h"
#include "globals.h"
#include "algorithms.h"
#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <windows.h>
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
    case WM_PAINT:
        if (listView) {
            if (listView->isRedrawRequired) // ��������� � �����������
                listView->Redraw();
            else if (listView->input.size() == 0)
                listView->DrawLayout(); // ��������� ����������� ����� �������
            else
                listView->DrawContent(); // ��������� ������ ������ � �������
        }
        return 0;
    case WM_MOUSEWHEEL: { // ��������� ���������
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
    }
    case WM_DESTROY: // ���������� ����
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam); // ���������� ���������� Windows
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
    return ws.str();
}

void Table::DrawLayout()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // ������������� ���� ����
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    // ��������� ������ �����
    HFONT hTitleFont = CreateFont(
        gl::fontSize[fontSize[TITLE]],    // Height
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
        gl::fontName[fontName[TITLE]].c_str()    // Font Name
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[TITLE]].color);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm); // �������� ������� ������
    const int textHeight = tm.tmHeight;

    const int columnWidth = tableWidth / columnCount; // ������ �������
    //titleHeight = textHeight + 4; // ������ ���������
    const int leftDisp = 3; // ����� ������ ������ � �������

    // ��������� ������������ ����� ����� ���������
    for (size_t j = 1; j < columnCount; ++j) {
        int x = j * columnWidth;
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, tableHeight);
    }

    // ��������� �������������� ����� ����� ��������
    MoveToEx(hdc, 0, titleHeight, NULL);
    LineTo(hdc, tableWidth, titleHeight);

    // ��������� ���������� ��������
    DeleteObject(gl::hEditFont);
    gl::hEditFont = CreateFont(
        gl::fontSize[fontSize[TITLE]],    // Height
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
        gl::fontName[fontName[TITLE]].c_str()    // Font Name
    );
    SendMessage(gl::hDirectoryEdit, WM_SETFONT, (WPARAM)gl::hEditFont, TRUE);
    SetWindowPos(gl::hDirectoryEdit, NULL, leftDisp, (titleHeight - textHeight) / 2 + 2, columnWidth - 10, textHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
    InvalidateRect(gl::hDirectoryEdit, NULL, TRUE);

    std::wstring headerText = TrimTextToWidth(L"��� �����", columnWidth, hdc);
    TextOut(hdc, leftDisp + columnWidth, (titleHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"��������� ����������", columnWidth, hdc);
    TextOut(hdc, leftDisp + 2 * columnWidth, (titleHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"��� �����", columnWidth, hdc);
    TextOut(hdc, leftDisp + 3 * columnWidth, (titleHeight - textHeight) / 2, headerText.c_str(), headerText.length());

    // ��������������� ������ �����
    SelectObject(hdc, hOldFont);

    // ������� ����� ����� ����� �������������� �������
    DeleteObject(hTitleFont);

    EndPaint(hWnd, &ps);
}

void Table::DrawContent()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // ��������� ������ �����
    HFONT hFileContentFont = CreateFont(
        gl::fontSize[fontSize[FILE_INFO]],    // Height
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
        gl::fontName[fontName[FILE_INFO]].c_str()    // Font Name
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFileContentFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[FILE_INFO]].color);

    TEXTMETRIC tm1, tm2;
    GetTextMetrics(hdc, &tm1); // �������� ������� ������
    const int textHeightFileContent = tm1.tmHeight;


    HFONT hSortedContentFont = CreateFont(
        gl::fontSize[fontSize[SORTING_INFO]],    // Height
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
        gl::fontName[fontName[SORTING_INFO]].c_str()    // Font Name
    );
    SelectObject(hdc, hSortedContentFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[SORTING_INFO]].color);

    GetTextMetrics(hdc, &tm2); // �������� ������� ������
    const int textHeightSortedContent = tm2.tmHeight;

    // ���������� ����������� �������
    int textHeight = textHeightSortedContent > textHeightFileContent ? textHeightSortedContent : textHeightFileContent;
    int rowHeight = textHeight + 4;
    const int contentHeight = tableHeight - titleHeight;
    recordingCapacity = contentHeight / rowHeight;

    while (contentHeight - recordingCapacity * rowHeight > 6)
    {
        recordingCapacity--;
        rowHeight = contentHeight / recordingCapacity;
    }

    const int columnWidth = tableWidth / columnCount; // ������ �������
    const int leftDisp = 3; // ����� ������ ������ � �������



    for (int i = 0; i < columnCount; i++)
    {
        RECT fillRect =
        {
            ps.rcPaint.left + columnWidth * i + 1,
            ps.rcPaint.top + titleHeight + 1,
            columnWidth * (i + 1) - 1,
            ps.rcPaint.bottom - 1
        };

        // ������������� ���� ����
        FillRect(hdc, &fillRect, (HBRUSH)(COLOR_WINDOW + 1));
    }

    const int remainingRecordsCount = ((int)input.size() - scrollPos) < (recordingCapacity) ? ((int)input.size() - scrollPos) : recordingCapacity; // ���������� ������� �� ����� � �������

    // ��������� ����������� �������
    for (size_t j = 0; j < remainingRecordsCount; ++j) {

        (HFONT)SelectObject(hdc, hFileContentFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gl::fontColor[fontColor[FILE_INFO]].color);

        std::wstring text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + input[scrollPos + j].first, columnWidth, hdc); // ����� ��������� ������ � �����������
        TextOut(hdc, leftDisp, titleHeight + (j * rowHeight) + (rowHeight - textHeightFileContent) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + output[scrollPos + j].first, columnWidth, hdc); // ����� ���� ��������� ������ 
        TextOut(hdc, leftDisp + 2 * columnWidth, titleHeight + (j * rowHeight) + (rowHeight - textHeightFileContent) / 2, text.c_str(), text.length());

        SelectObject(hdc, hSortedContentFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gl::fontColor[fontColor[SORTING_INFO]].color);

        text = TrimTextToWidth(fileTimeToWstring(input[scrollPos + j].second), columnWidth, hdc); // ����� ��������������� ������ � ��������
        TextOut(hdc, leftDisp + columnWidth, titleHeight + (j * rowHeight) + (rowHeight - textHeightSortedContent) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(fileTimeToWstring(output[scrollPos + j].second), columnWidth, hdc); // ����� ���� ��������������� ������
        //text = text == L"" ? L"*������*" : text;
        TextOut(hdc, leftDisp + 3 * columnWidth, titleHeight + (j * rowHeight) + (rowHeight - textHeightSortedContent) / 2, text.c_str(), text.length());
    }

    // ��������������� ������ �����
    SelectObject(hdc, hOldFont);

    EndPaint(hWnd, &ps);
}

void Table::Redraw()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // ������������� ���� ����
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    // ������� ����� ��� ��������� �������
    HFONT hTitleFont = CreateFont(
        gl::fontSize[fontSize[TITLE]], // Height
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
        gl::fontName[fontName[TITLE]].c_str()    // Font Name
    );

    // ��������� ������ ����� 
    HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);

    // ������������� ���������� ��� ����� ������� ������
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[TITLE]].color);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm); // �������� ������� ������
    int textHeight = tm.tmHeight;

    const int columnWidth = tableWidth / columnCount; // ������ �������
    //titleHeight = textHeight + 4; // ������ ���������
    const int leftDisp = 3; // ����� ������ ������ � �������

    // ������� ���������� ����� � gl::hDirectoryEdit � ������� �����
    DeleteObject(gl::hEditFont);
    gl::hEditFont = CreateFont(
        gl::fontSize[fontSize[TITLE]],    // Height
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
        gl::fontName[fontName[TITLE]].c_str()    // Font Name
    );


 
    //gl::hEditFont.

    // �������� ����������� ��� gl::hDirectoryEdit (� main.cpp ��������� WM_CTLCOLOREDIT, � ������� �������� ���� ������ ��� gl::hDirectoryEdit!!!)
    SendMessage(gl::hDirectoryEdit, WM_SETFONT, (WPARAM)gl::hEditFont, TRUE);
    SetWindowPos(gl::hDirectoryEdit, NULL, leftDisp, (titleHeight - textHeight) / 2 + 2, columnWidth - 10, textHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
    InvalidateRect(gl::hDirectoryEdit, NULL, TRUE);
    // ��������� ������
    //MoveToEx(hdc, 0, titleHeight+3, NULL);
    //LineTo(hdc, tableWidth, titleHeight+3);


    // ������������ ��������� ����� ��������� �������
    std::wstring headerText = TrimTextToWidth(L"��� �����", columnWidth, hdc);
    TextOut(hdc, leftDisp + columnWidth, (titleHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"��������� ����������", columnWidth, hdc);
    TextOut(hdc, leftDisp + 2 * columnWidth, (titleHeight - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = TrimTextToWidth(L"��� �����", columnWidth, hdc);
    TextOut(hdc, leftDisp + 3 * columnWidth, (titleHeight - textHeight) / 2, headerText.c_str(), headerText.length());

    // ������� ����� ���������
    DeleteObject(hTitleFont);

    // ��������� ������������ ����� ����� ���������
    for (size_t j = 1; j < columnCount; ++j) {
        int x = j * columnWidth;
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, tableHeight);
    }

    // ��������� �������������� ����� ����� ��������
    MoveToEx(hdc, 0, titleHeight, NULL);
    LineTo(hdc, tableWidth, titleHeight);



    // ������� �����, ������� ������������ � 1 � 3 �������� �������
    HFONT hFileContentFont = CreateFont(
        gl::fontSize[fontSize[FILE_INFO]],    // Height
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
        gl::fontName[fontName[FILE_INFO]].c_str()    // Font Name
    );
    SelectObject(hdc, hFileContentFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[FILE_INFO]].color);

    TEXTMETRIC tm1, tm2;
    GetTextMetrics(hdc, &tm1); // �������� ������� ������
    const int textHeightFileContent = tm1.tmHeight;

    // ������� �����, ������� ������������ � 2 � 4 �������� �������
    HFONT hSortedContentFont = CreateFont(
        gl::fontSize[fontSize[SORTING_INFO]],    // Height
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
        gl::fontName[fontName[SORTING_INFO]].c_str()    // Font Name
    );
    SelectObject(hdc, hSortedContentFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[SORTING_INFO]].color);

    GetTextMetrics(hdc, &tm2); // �������� ������� ������
    const int textHeightSortedContent = tm2.tmHeight;

    // ���������� ����������� �������
    textHeight = textHeightSortedContent > textHeightFileContent ? textHeightSortedContent : textHeightFileContent;
    int rowHeight = textHeight + 4;
    const int contentHeight = tableHeight - titleHeight;
    recordingCapacity = contentHeight / rowHeight; // ������������ �����������

    // ����������� ����������� ��� ���������(������� ����� ������� ������� ��� ��������� � ��������� ������� ������� ������� ������ ���� �� ����� 6)
    while (contentHeight - recordingCapacity * rowHeight > 6)
    {
        recordingCapacity--;
        rowHeight = contentHeight / recordingCapacity;
    }

    const int remainingRecordsCount = ((int)input.size() - scrollPos) < (recordingCapacity) ? ((int)input.size() - scrollPos) : recordingCapacity; // ���������� ������� �� ����� � �������

    // ��������� ����������� �������
    for (size_t j = 0; j < remainingRecordsCount; ++j) {

        (HFONT)SelectObject(hdc, hFileContentFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gl::fontColor[fontColor[FILE_INFO]].color);

        std::wstring text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + input[scrollPos + j].first, columnWidth, hdc); // ����� ��������� ������ � �����������
        TextOut(hdc, leftDisp, titleHeight + (j * rowHeight) + (rowHeight - textHeightFileContent) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + output[scrollPos + j].first, columnWidth, hdc); // ����� ���� ��������� ������ 
 // ����� ���� ��������� ������ 
        TextOut(hdc, leftDisp + 2 * columnWidth, titleHeight + (j * rowHeight) + (rowHeight - textHeightFileContent) / 2, text.c_str(), text.length());

        SelectObject(hdc, hSortedContentFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gl::fontColor[fontColor[SORTING_INFO]].color);

        text = fileTimeToWstring(input[scrollPos + j].second); // ����� ��������������� ������ � ��������
        //text = text == L"" ? L"*������*" : text;
        TextOut(hdc, leftDisp + columnWidth, titleHeight + (j * rowHeight) + (rowHeight - textHeightSortedContent) / 2, text.c_str(), text.length());

        text = fileTimeToWstring(output[scrollPos + j].second); // ����� ���� ��������������� ������
        //text = text == L"" ? L"*������*" : text;
        TextOut(hdc, leftDisp + 3 * columnWidth, titleHeight + (j * rowHeight) + (rowHeight - textHeightSortedContent) / 2, text.c_str(), text.length());
    }

    // ��������������� ������ �����
    SelectObject(hdc, hOldFont);

    isRedrawRequired = false; // ���������� ���� � false, ����� ��� ��������� ������ ������� �������������� � ������� ������� DrawLayout � DrawContent

    EndPaint(hWnd, &ps);
}

std::wstring Table::TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc)
{
    std::wstring res = text;
    // ���������� ������������ ������ ������ ��� �������
    SIZE textSize;
    GetTextExtentPoint32(hdc, res.c_str(), res.length(), &textSize);

    // ���� ����� ���� �������, �������� ���
    if (textSize.cx > (columnWidth - 2)) {
        // ������� ����� �� ���������� �����
        while (res.length() > 0 && textSize.cx > (columnWidth - 2)) {
            res.pop_back(); // ������� ��������� ������
            GetTextExtentPoint32(hdc, (res + L"...").c_str(), (res + L"...").length(), &textSize);
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
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    int disp = 0;
    tableWidth = gl::winWidth - disp * 2;
    tableHeight = gl::winHeight - gl::btnHeight - 20;

    // ������� ����
    hWnd = CreateWindowEx(
        0,
        CLASS_NAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        disp, disp, tableWidth, tableHeight,
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