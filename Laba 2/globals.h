#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Table.h"

namespace gl {
	extern const int winWidth; // Ширина, высота основного окна
	extern const int winHeight;

	extern const int inputWidth; // Ширина, высота поля ввода директории
	extern const int inputHeight;

	extern const int btnWidth; // Ширина, высота кнопок скрола
	extern const int btnHeight;

	extern HWND  hDirectoryEdit; // Дескриптор окна(поле для ввода директории)
	extern HFONT hEditFont;

	extern Table *table;

	extern std::vector<int> fontSize; // Вектор размеров шрифтов
	extern std::vector<std::wstring> fontName; // Вектор типов шрифтов
	struct ColoredText {
		std::wstring text;  // Текст
		COLORREF color;     // Цвет текста

		ColoredText(const std::wstring& t, COLORREF c) : text(t), color(c) {}
	};
	extern const std::vector<ColoredText> fontColor; // Вектор структур, которые содержат RGB цвет и его название

	extern HWND hComboboxFontSize[3]; // Дескрипторы окон (комбобоксы для выбора размера шрифта)
	extern HWND hComboboxFontColor[3]; // Дескрипторы окон (комбобоксы для выбора цвета шрифта)
	extern HWND hComboboxFontName[3]; // Дескрипторы окон (комбобоксы для выбора типа/имени шрифта)

	extern HWND hEditFontPath; // Дескриптор окна(поле для ввода пути к файлу .ttf)

	extern std::vector<std::wstring> fontPaths; // Вектор путей к кастомным шрифтам, которые загрузил пользователь
	extern HWND  hButtonFontSettings; // Дескриптор окна(кнопка настроек шрифта)
	extern HWND  hButtonScrUp; // Дескриптор окна(кнопка скрол вверх)
	extern HWND  hButtonScrDown; // Дескриптор окна(кнопка скрол вниз)

	extern WNDPROC OriginalEditProc; // Указатель на оригинальную функцию оконной процедуры для hDirectoryEdit
}