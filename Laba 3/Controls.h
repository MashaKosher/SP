#pragma once
#include <windows.h>

void MainWndAddWidgets(HWND hwnd); // ������� ��� �������� �������� ����
void CreateFontSettingsWindow(HWND hwnd);
LRESULT CALLBACK FontSettingsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);