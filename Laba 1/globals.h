#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Table.h"

namespace gl {
	extern const int winWidth; // Ўирина, высота основного окна
	extern const int winHeight;

	extern const int inputWidth; // Ўирина, высота пол€ ввода директории
	extern const int inputHeight;

	extern const int btnWidth; // Ўирина, высота кнопок скрола
	extern const int btnHeight;

	extern HWND  hDirectoryEdit; // ƒескриптор окна(поле дл€ ввода директории)

	extern Table *table;

	extern HWND  hButtonScrUp; // ƒескриптор окна(кнопка скрол вверх)
	extern HWND  hButtonScrDown; // ƒескриптор окна(кнопка скрол вниз)

	extern WNDPROC OriginalEditProc; // ”казатель на оригинальную функцию оконной процедуры дл€ hDirectoryEdit
}