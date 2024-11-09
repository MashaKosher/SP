#include "Controls.h"
#include "globals.h"
#include <Richedit.h>
#include <shlwapi.h> // ��� PathFileExists
#include "algorithms.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

// �������� ����������
HINSTANCE hRichEditLib = LoadLibrary(TEXT("Msftedit.dll"));

void MainWndAddWidgets(HWND hwnd)
{
    int dispX = 3, dispY = 5;
    gl::hDirectoryEdit = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, dispX, dispY, gl::inputWidth, gl::inputHeight, hwnd, NULL, NULL, NULL);

    // ������ ����� ����� ��� ��������� ����, ����� �������� ��������� ������� ������� Enter
    gl::OriginalEditProc = (WNDPROC)SetWindowLongPtr(gl::hDirectoryEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    
    gl::table = new Table(hwnd);

    // {{{ ������� ������ ��� �������������� ������
    dispX = 20;
    dispY = gl::table->GetTableHeight() + 10;
    gl::hButtonFontSettings = CreateWindowA("button", "Font settings", WS_VISIBLE | WS_CHILD | ES_CENTER | BS_DEFPUSHBUTTON, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
    // ������� ������ ��� �������������� ������ }}}


    dispX = gl::winWidth/2 - gl::btnWidth  - 2; // ���������� X ����� ������
    dispY = gl::table->GetTableHeight() + 10;
	gl::hButtonScrUp = CreateWindowA("button", "Scr Up", WS_VISIBLE | WS_CHILD | ES_CENTER | BS_DEFPUSHBUTTON, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);

    dispX = gl::winWidth/2 +5;
	gl::hButtonScrDown = CreateWindowA("button", "Scr Down", WS_VISIBLE | WS_CHILD | ES_CENTER, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
}

// ��������� ��������� ���� ��� gl::hDirectoryEdit
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN:
        if (wParam == VK_RETURN) {
            // �������� ����� �� �������� ���������� Edit
            wchar_t directoryPath[260]; // ����� ��� �������� ����
            GetWindowText(gl::hDirectoryEdit, directoryPath, sizeof(directoryPath) / sizeof(wchar_t));

            // ���������, ���������� �� ����������
            if (PathFileExists(directoryPath)) {
                // ����� ����� ������� ���� ������� ��� ��������� ����������, ���� �����
                gl::table->FillOutTable(directoryPath);
                std::wstring res = L"Sorting done. Total count - " + std::to_wstring(gl::table->GetTableRecordsCount());
                MessageBox(hwnd, (LPCWSTR)res.c_str(), L"Notification", MB_OK);
            }
            else {
                MessageBox(hwnd, L"Directory does not exist!", L"Error", MB_OK);
            }
        }
        break;
    case WM_DESTROY: {
        // ������������ ��������
        DeleteObject(gl::hEditFont);
        PostQuitMessage(0);
        break;
    }
    }

    // �������� ������������ ��������� edit control ��� ��������� �� ���������
    return CallWindowProc(gl::OriginalEditProc, hwnd, uMsg, wParam, lParam);
}

void CreateFontSettingsWindow(HWND hwnd)
{
    EnableWindow(hwnd, FALSE);

    const wchar_t CLASS_NAME[] = L"Font Settings Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = FontSettingsWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // ������� ���� �������� �������
    int width = 500;
    int height = 300;

    // ��������� �������� �������� ����
    RECT parentRect;
    GetClientRect(hwnd, &parentRect);

    // ���������� ��������� ��� �������������
    int x = (parentRect.right - width) / 2;
    int y = (parentRect.bottom - height) / 2;

    HWND hwndChild = CreateWindowEx(
        0, CLASS_NAME, L"Font settings", WS_POPUP | WS_VISIBLE | WS_CAPTION,
        x, y, width, height,
        hwnd, nullptr, wc.hInstance, nullptr
    );

    int dispX = 60, dispY = 10;
    int comboboxWidth = 120;
    CreateWindowA("static", "Title", WS_VISIBLE | WS_CHILD, dispX, dispY, comboboxWidth, 20, hwndChild, NULL, NULL, NULL);

    dispX += comboboxWidth + 10;
    CreateWindowA("static", "File info", WS_VISIBLE | WS_CHILD, dispX, dispY, comboboxWidth, 20, hwndChild, NULL, NULL, NULL);

    dispX += comboboxWidth + 10;
    CreateWindowA("static", "Sorting info:", WS_VISIBLE | WS_CHILD, dispX, dispY, comboboxWidth, 20, hwndChild, NULL, NULL, NULL);

    dispX = 10, dispY = 30;
    CreateWindowA("static", "Type:", WS_VISIBLE | WS_CHILD, dispX, dispY, 50, 20, hwndChild, NULL, NULL, NULL);

    dispY += 50;
    CreateWindowA("static", "Size:", WS_VISIBLE | WS_CHILD, dispX, dispY, 50, 20, hwndChild, NULL, NULL, NULL);

    dispY += 50;
    CreateWindowA("static", "Color:", WS_VISIBLE | WS_CHILD, dispX, dispY, 50, 20, hwndChild, NULL, NULL, NULL);


    int temp = 4;
    // �������� �����������
    for (int i = 0, dispX = 60; i < 3; i++, dispX += comboboxWidth + 10) // i = 0 - ���������, 1 - ������ � ������ ������� �������, 2 - ������ � ��������� ������� �������
    {
        dispY = 30;
        gl::hComboboxFontName[i] = CreateWindowA("COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
            dispX, dispY, comboboxWidth, 25 * gl::fontName.size(), hwndChild, nullptr, nullptr, nullptr);
        for(const std::wstring& name: gl::fontName)
            SendMessage(gl::hComboboxFontName[i], CB_ADDSTRING, 0, (LPARAM)name.c_str());
        SendMessage(gl::hComboboxFontName[i], CB_SETCURSEL, gl::table->GetFontNameIndices()[i], 0); // ������������� ������� ��� ������, ������� ������������ � �������, � ����������

        dispY += 50;
        gl::hComboboxFontSize[i] = CreateWindowA("COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
            dispX, dispY, comboboxWidth, 25 * gl::fontSize.size(), hwndChild, (HMENU)temp, nullptr, nullptr);
        for (const int& size : gl::fontSize){
            SendMessage(gl::hComboboxFontSize[i], CB_ADDSTRING, 0, (LPARAM)std::to_wstring(size).c_str());
        }

        SendMessage(gl::hComboboxFontSize[i], CB_SETCURSEL, gl::table->GetFontSizeIndices()[i], 0); // ������������� ������� ������ ������

        dispY += 50;
        gl::hComboboxFontColor[i] = CreateWindowA("COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
            dispX, dispY, comboboxWidth, 25 * gl::fontColor.size(), hwndChild, nullptr, nullptr, nullptr);
        for (const gl::ColoredText& coloredText: gl::fontColor)
            SendMessage(gl::hComboboxFontColor[i], CB_ADDSTRING, 0, (LPARAM)coloredText.text.c_str());
        SendMessage(gl::hComboboxFontColor[i], CB_SETCURSEL, gl::table->GetFontColorIndices()[i], 0); // ������������� ������� ���� ������
        temp += 1;
    }

    dispY += 50;
    dispX = 60;

    // ������� ���� ��� ����� ���� � ����� ttf � ������ ��� �������� ������ �� ����� �����
    gl::hEditFontPath = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_BORDER, dispX, dispY, comboboxWidth * 3, 20, hwndChild, NULL, NULL, NULL);
    dispX += comboboxWidth * 3 + 5;
    CreateWindowA("BUTTON", "Load", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        dispX, dispY, 50, 20, hwndChild, (HMENU)3, nullptr, nullptr);

    // �������� ������ "��" � "������"
    dispY = 30 + 50 * 3 + 40;
    dispX = 100;
    CreateWindowA("BUTTON", "OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        dispX, dispY, 100, 30, hwndChild, (HMENU)1, nullptr, nullptr);
    CreateWindowA("BUTTON", "Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        width - dispX - 100, dispY, 100, 30, hwndChild, (HMENU)2, nullptr, nullptr);
}





//LRESULT CALLBACK FontSettingsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg) {
//    case WM_COMMAND:
//        switch (LOWORD(wParam))
//        {
//        case 1: // ID ������ "OK"
//        {   // ��������� ��������� �������� ������
//            int fontNameIndex[3];
//            int fontSizeIndex[3];
//            int fontColorIndex[3];
//            for (int i = 0; i < 3; i++)
//            {
//                fontNameIndex[i] = SendMessage(gl::hComboboxFontName[i], CB_GETCURSEL, 0, 0);
//                fontSizeIndex[i] = SendMessage(gl::hComboboxFontSize[i], CB_GETCURSEL, 0, 0);
//                fontColorIndex[i] = SendMessage(gl::hComboboxFontColor[i], CB_GETCURSEL, 0, 0);
//            }
//
//            bool isChanged = false; // false, ���� ��������� ������ �� ���� ���., ����� - true
//            for (int i = 0; i < 3; i++)
//            {
//                if (!isChanged && (gl::table->GetFontNameIndices()[i] != fontNameIndex[i]
//                    || gl::table->GetFontSizeIndices()[i] != fontSizeIndex[i]
//                    || gl::table->GetFontColorIndices()[i] != fontColorIndex[i]))
//                    isChanged = !isChanged;
//                gl::table->GetFontNameIndices()[i] = fontNameIndex[i];
//                gl::table->GetFontSizeIndices()[i] = fontSizeIndex[i];
//                gl::table->GetFontColorIndices()[i] = fontColorIndex[i];
//            }
//            if (isChanged)
//                gl::table->RequestRedraw();
//            DestroyWindow(hwnd); // ��������� ���� ����� ���������
//            break;
//        }
//        case 2: // ID ������ "Cancel"
//            DestroyWindow(hwnd);
//            break;
//        case 3: // ID ������ "Load"
//        {
//            // �������� ����� �� �������� ���������� Edit
//            wchar_t filePath[260]; // ����� ��� �������� ���� � ����� .ttf
//            GetWindowText(gl::hEditFontPath, filePath, sizeof(filePath) / sizeof(wchar_t));
//
//            // ���� ���� �� ���������� ��� ��� ���������� �� ttf, ���������� ������
//            if (!FontFileExists(filePath))
//            {
//                MessageBox(hwnd, L"File doesn't exist or its extension isn't ttf", L"Error", MB_OK);
//                break;
//            }
//            
//            std::set<std::wstring> before, after;
//
//            ListFonts(before);
//            LoadCustomFont(filePath);
//            ListFonts(after);
//
//            // ��������� ��� �������� ��������
//            std::set<std::wstring> difference;
//
//            // ������� ������� �� �������� ����� ttf � �����
//            std::set_difference(after.begin(), after.end(),
//                before.begin(), before.end(),
//                std::inserter(difference, difference.begin()));
//
//            if (!difference.size())
//            {
//                MessageBox(hwnd, L"Failed to upload file", L"Error", MB_OK);
//                break;
//            }
//            std::wstring loadedFonts;
//            for (auto it = difference.begin(); it != difference.end(); ++it) {
//                for (int i = 0; i < 3; i++)
//                    SendMessage(gl::hComboboxFontName[i], CB_ADDSTRING, 0, (LPARAM)(*it).c_str());
//                gl::fontName.push_back((*it));
//
//                loadedFonts += *it; // ��������� ������� �������
//
//                // ���� ��� �� ��������� �������, ��������� �������
//                if (std::next(it) != difference.end()) {
//                    loadedFonts += L", ";
//                }
//            }
//            MessageBox(hwnd, (L"Successfully uploaded: " + loadedFonts).c_str(), L"Notification", MB_OK);
//            break;
//        }
//        default:
//            return DefWindowProc(hwnd, uMsg, wParam, lParam);
//        }
//    case WM_DESTROY:
//        //PostQuitMessage(0);
//        EnableWindow(GetParent(hwnd), TRUE);
//        SetForegroundWindow(GetParent(hwnd));
//        break;
//    }
//
//    // �������� ������������ ��������� edit control ��� ��������� �� ���������
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//
//
//
//
//}



void AddToComboBox(HWND hComboBox, int value) {
    std::wstringstream ss;
    ss << value;
    std::wstring wstr = ss.str();
    SendMessage(hComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(wstr.c_str()));

}
void AddValueToAllComboBoxes(int value) {
    for (HWND hComboBox : gl::hComboboxFontSize) {
        AddToComboBox(hComboBox, value);
    }
}
bool IsValueInVector(const std::vector<int>& vec, int value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}


LRESULT CALLBACK FontSettingsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// ��������� ����������� ��� Font
{
    switch (uMsg) {
    case WM_COMMAND: {
        if (HIWORD(wParam) == CBN_EDITCHANGE) {
            HWND hComboBox = (HWND)lParam;
            wchar_t buffer[256];
            GetWindowText(hComboBox, buffer, 256);

            int newValue = _wtoi(buffer);
            if (newValue > 0 && !IsValueInVector(gl::fontSize, newValue)) {

                gl::fontSize.push_back(newValue);
                AddValueToAllComboBoxes(newValue);
                std::wstringstream ss;
                ss << newValue;
                std::wstring wstr = ss.str();
                int ind = 0;
                //switch (LOWORD(wParam))
                //{
                //case 4:
                //{
                //    ind = 0;
                //    break;
                //}
                //case 5:
                //{
                //    ind = 1;
                //    break;
                //}
                //case 6:
                //{
                //    ind = 2;
                //    break;
                //}
                //default: break;
                //}
                int index = SendMessage(gl::hComboboxFontSize[ind], CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(wstr.c_str()));
                if (index != CB_ERR) {
                    SendMessage(gl::hComboboxFontSize[ind], CB_SETCURSEL, index, 0);
                }
                int x = SendMessage(gl::hComboboxFontSize[ind], CB_GETCURSEL, 0, 0);
                std::cout << std::to_string(x) << std::endl;
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        else switch (LOWORD(wParam))
        {
        case 1:
        {
            int fontNameIndex[3];
            int fontSizeIndex[3];
            int fontColorIndex[3];
            for (int i = 0; i < 3; i++)
            {
                fontNameIndex[i] = SendMessage(gl::hComboboxFontName[i], CB_GETCURSEL, 0, 0);
                int index = SendMessage(gl::hComboboxFontSize[i], CB_GETCURSEL, 0, 0);
                if (index != CB_ERR) {
                    fontSizeIndex[i] = index;
                }
                else {
                    wchar_t buffer[256];
                    GetWindowText(gl::hComboboxFontSize[i], buffer, 256);
                    index = SendMessage(gl::hComboboxFontSize[i], CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(buffer));

                    if (index == CB_ERR) {
                        SendMessage(gl::hComboboxFontSize[i], CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buffer));
                        index = SendMessage(gl::hComboboxFontSize[i], CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(buffer));
                    }
                    SendMessage(gl::hComboboxFontSize[i], CB_SETCURSEL, index, 0);
                    fontSizeIndex[i] = index;
                }
                fontColorIndex[i] = SendMessage(gl::hComboboxFontColor[i], CB_GETCURSEL, 0, 0);
            }

            bool isChanged = false;
            for (int i = 0; i < 3; i++)
            {
                if (!isChanged && (gl::table->GetFontNameIndices()[i] != fontNameIndex[i]
                    || gl::table->GetFontSizeIndices()[i] != fontSizeIndex[i]
                    || gl::table->GetFontColorIndices()[i] != fontColorIndex[i]))
                    isChanged = !isChanged;
                gl::table->GetFontNameIndices()[i] = fontNameIndex[i];
                gl::table->GetFontSizeIndices()[i] = fontSizeIndex[i];
                gl::table->GetFontColorIndices()[i] = fontColorIndex[i];
            }
            if (isChanged)
                gl::table->RequestRedraw();
            DestroyWindow(hwnd);
            break;
        }
        case 2:
            DestroyWindow(hwnd);
            break;
        case 3:
        {
            wchar_t filePath[260];
            GetWindowText(gl::hEditFontPath, filePath, sizeof(filePath) / sizeof(wchar_t));
            if (!FontFileExists(filePath))
            {
                MessageBox(hwnd, L"File doesn't exist or its extension isn't ttf", L"Error", MB_OK);
                break;
            }

            std::set<std::wstring> before, after;
            ListFonts(before);
            LoadCustomFont(filePath);
            ListFonts(after);
            std::set<std::wstring> difference;
            std::set_difference(after.begin(), after.end(),before.begin(), before.end(),std::inserter(difference, difference.begin()));

                if (!difference.size())
                {
                    MessageBox(hwnd, L"Nothing new to upload", L"Error", MB_OK);
                    break;
                }
                std::wstring loadedFonts;
                for (auto it = difference.begin(); it != difference.end(); ++it) {
                    for (int i = 0; i < 3; i++)
                        SendMessage(gl::hComboboxFontName[i], CB_ADDSTRING, 0, (LPARAM)(*it).c_str());
                    gl::fontName.push_back((*it));

                    loadedFonts += *it;

                    if (std::next(it) != difference.end()) {
                        loadedFonts += L", ";
                    }
                }
                MessageBox(hwnd, (L"Uploaded font: " + loadedFonts).c_str(), L"Notification", MB_OK);
                break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
        break;
    }
    case WM_DESTROY:
        EnableWindow(GetParent(hwnd), TRUE);
        SetForegroundWindow(GetParent(hwnd));
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


