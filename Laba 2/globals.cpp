#include "globals.h"

namespace gl {
	/*const int winWidth = 1600;
	const int winHeight = 900;*/

	const int winWidth = GetSystemMetrics(SM_CXSCREEN);
	const int winHeight = GetSystemMetrics(SM_CYSCREEN);

	const int inputWidth = winWidth / 4 - 3;
	const int inputHeight = 20;

	const int btnWidth = 100;
	const int btnHeight = 40;

	HWND  hDirectoryEdit;
	HFONT hEditFont = NULL;

	Table* table;

	//const std::vector<int> fontSize = { 20, 20, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17, 18,19, 21, 22,23, 24, 25, 26, 27, 28, 29, 30 };
	std::vector<int> fontSize = {10, 18, 22, 26, 30};
	std::vector<std::wstring> fontName = { L"Arial", L"Times New Roman", L"Calibri", L"Georgia", L"Tahoma" };

	const std::vector<ColoredText> fontColor = { ColoredText(L"Black", RGB(0, 0, 0)),
												  ColoredText(L"Red", RGB(255, 0, 0)),
												  ColoredText(L"Green", RGB(0, 255, 0)),
												  ColoredText(L"Yellow", RGB(255, 255, 0)),
												  ColoredText(L"Grey", RGB(128, 128, 128))};
	HWND hComboboxFontSize[3];
	HWND hComboboxFontColor[3];
	HWND hComboboxFontName[3];

	HWND hEditFontPath;

	std::vector<std::wstring> fontPaths;

	HWND hButtonFontSettings;
	HWND  hButtonScrUp;
	HWND  hButtonScrDown;

	WNDPROC OriginalEditProc;
}