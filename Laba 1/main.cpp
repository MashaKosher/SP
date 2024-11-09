#include <windows.h>
#pragma comment(lib, "Shlwapi.lib")
#include "Controls.h"
#include "globals.h"
#include "algorithms.h"
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // ������������ ����� ����
    const wchar_t CLASS_NAME[] = L"Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc; // ��������� ����
    wc.hInstance = hInstance;   // ���������� ����������
    wc.lpszClassName = CLASS_NAME; // ������ ������������� ������ ����

    RegisterClass(&wc); // ����������� ���� � ��

    
    // {{{ ������� ����
    RECT rc = { 0, 0, gl::winWidth, gl::winHeight};
    AdjustWindowRect(&rc, WS_POPUP, FALSE);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, NULL,
        WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }
    // ����� �������� ���� }}}


    SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);  // �������� ��������� ����
    ShowWindow(hwnd, nCmdShow);  // ���������� ����

    // {{{ �������� ���� ��������� ���������

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)  // ��������� ���������
    {
        TranslateMessage(&msg); // ���������� ��������� � ���� ������
        DispatchMessage(&msg);  // ���������� ��������� �� ������� � WindowProc
    }

    return 0;
    // ����� ��������� ����� ��������� ��������� }}}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:  // �������� ����
        MainWndAddWidgets(hwnd);
        break;
    case WM_COMMAND:  // ���������������� ��������
        if (LOWORD(wParam) == BN_CLICKED) {
            if ((HWND)lParam == gl::hButtonScrUp) {
                // �������� ������� ��������� �����
                SendMessage(gl::table->GetHandle(), WM_MOUSEWHEEL, MAKEWPARAM(0, WHEEL_DELTA), 0);
            }
            else if ((HWND)lParam == gl::hButtonScrDown) {
                // �������� ������� ��������� ����
                SendMessage(gl::table->GetHandle(), WM_MOUSEWHEEL, MAKEWPARAM(0, -WHEEL_DELTA), 0);
            }
        }
        break;
    case WM_DESTROY: // ���������� ����
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:  // �������� ����
        DestroyWindow(hwnd);
    case WM_PAINT:  // ������ ������� ����
    {
        // ��������� �����
        RECT tempRect;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        RECT rect2;
        GetClientRect(hwnd, &rect2);
        HBRUSH hBrush = CreateSolidBrush(RGB(204, 255, 204));
        rect2.top = gl::winHeight - 60;
        rect2.bottom = gl::winHeight;
        // ����������� �������������
        //FillRect(hdc, &rect2, hBrush);
        //EndPaint(hwnd, &ps);
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);  // ���� Case �� �������� �� ������ ��������� � ������� � Windows ��� ������������
}