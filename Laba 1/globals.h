#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Table.h"

namespace gl {
	extern const int winWidth; // ������, ������ ��������� ����
	extern const int winHeight;

	extern const int inputWidth; // ������, ������ ���� ����� ����������
	extern const int inputHeight;

	extern const int btnWidth; // ������, ������ ������ ������
	extern const int btnHeight;

	extern HWND  hDirectoryEdit; // ���������� ����(���� ��� ����� ����������)

	extern Table *table;

	extern HWND  hButtonScrUp; // ���������� ����(������ ����� �����)
	extern HWND  hButtonScrDown; // ���������� ����(������ ����� ����)

	extern WNDPROC OriginalEditProc; // ��������� �� ������������ ������� ������� ��������� ��� hDirectoryEdit
}