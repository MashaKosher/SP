#include "Controls.h"
#include "globals.h"
#include <Richedit.h>
#include <shlwapi.h> // ��� PathFileExists

// �������� ����������
HINSTANCE hRichEditLib = LoadLibrary(TEXT("Msftedit.dll"));


void MainWndAddWidgets(HWND hwnd)
{
    int textHeight = 22; // �������� ������ Times New Roman
    
    HFONT hFont = CreateFont(
        textHeight,          // Height
        0,                   // Width
        0,                   // Escapement
        0,                   // Orientation
        FW_NORMAL,           // Weight
        FALSE,               // Italic
        FALSE,               // Underline
        FALSE,               // StrikeOut
        DEFAULT_CHARSET,     // CharSet
        OUT_DEFAULT_PRECIS,  // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,     // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        L"Times New Roman"          // Font Name
    );

    int dispX = 3, dispY = 5;
    // ������� ���� Edit Control
    gl::hDirectoryEdit = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, dispX, dispY, gl::inputWidth, gl::inputHeight, hwnd, NULL, NULL, NULL);

    // ��������� ������ ��� Edit Control
    SendMessage(gl::hDirectoryEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // ������ ����� ����� ��� ��������� ����, ����� �������� ��������� ������� ������� Enter
    gl::OriginalEditProc = (WNDPROC)SetWindowLongPtr(gl::hDirectoryEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    
    // ��������� �������
    gl::table = new Table(hwnd);


	dispX = gl::winWidth/2 - gl::btnWidth  - 1;  // ���������� X ����� ������
	dispY = gl::table->GetTableHeight() + 10;    // ���������� Y ����� � ������ ������

    // ������� ����� ������
	gl::hButtonScrUp = CreateWindowA("button", "�����", WS_VISIBLE | WS_CHILD | ES_CENTER | BS_DEFPUSHBUTTON, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
  
    dispX = gl::winWidth/2 +3; // ���������� X ������ ������
    // ������� ������ ������
	gl::hButtonScrDown = CreateWindowA("button", "����", WS_VISIBLE | WS_CHILD | ES_CENTER, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
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
                std::wstring res = L"Total count - " + std::to_wstring(gl::table->GetTableRecordsCount()); // ��������� � ����
                MessageBox(hwnd, (LPCWSTR)res.c_str(), L"Result", MB_OK); // �������� ������������� ����
            }
            else {
                MessageBox(hwnd, L"Directory does not exist!", L"Error", MB_OK); // �������� ������������� ����
            }
        }
        break;
    
    }

    // �������� ������������ ��������� edit control ��� ��������� �� ���������
    return CallWindowProc(gl::OriginalEditProc, hwnd, uMsg, wParam, lParam);
}