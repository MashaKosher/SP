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
	extern HFONT hEditFont;

	extern Table *table;

	extern std::vector<int> fontSize; // ������ �������� �������
	extern std::vector<std::wstring> fontName; // ������ ����� �������
	struct ColoredText {
		std::wstring text;  // �����
		COLORREF color;     // ���� ������

		ColoredText(const std::wstring& t, COLORREF c) : text(t), color(c) {}
	};
	extern const std::vector<ColoredText> fontColor; // ������ ��������, ������� �������� RGB ���� � ��� ��������

	extern HWND hComboboxFontSize[3]; // ����������� ���� (���������� ��� ������ ������� ������)
	extern HWND hComboboxFontColor[3]; // ����������� ���� (���������� ��� ������ ����� ������)
	extern HWND hComboboxFontName[3]; // ����������� ���� (���������� ��� ������ ����/����� ������)

	extern HWND hEditFontPath; // ���������� ����(���� ��� ����� ���� � ����� .ttf)

	extern std::vector<std::wstring> fontPaths; // ������ ����� � ��������� �������, ������� �������� ������������
	extern HWND  hButtonFontSettings; // ���������� ����(������ �������� ������)
	extern HWND  hButtonScrUp; // ���������� ����(������ ����� �����)
	extern HWND  hButtonScrDown; // ���������� ����(������ ����� ����)

	extern WNDPROC OriginalEditProc; // ��������� �� ������������ ������� ������� ��������� ��� hDirectoryEdit
}