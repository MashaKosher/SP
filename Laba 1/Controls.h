#pragma once
#include <windows.h>

void MainWndAddWidgets(HWND hwnd); // ������� ��� �������� �������� ����
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // ��������� ��������� ���� ��� ���� ����� ����������