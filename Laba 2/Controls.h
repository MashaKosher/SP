#pragma once
#include <windows.h>

void MainWndAddWidgets(HWND hwnd); // ������� ��� �������� �������� ����
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ��������� ��������� ���� ��� ���� ����� ����������
void CreateFontSettingsWindow(HWND hwnd);
LRESULT CALLBACK FontSettingsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);