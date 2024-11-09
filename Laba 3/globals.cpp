#include "globals.h"

namespace gl {
	//enum FONT_NAME
	//{
	//	FONT_ARIAL,
	//	FONT_TIMES_NEW_ROMAN,
	//	FONT_CALIBRI,
	//	FONT_GEORGIA,
	//	FONT_TAHOMA
	//};
	//enum FONT_SIZE {
	//	SIZE_SMALL,      // 14
	//	SIZE_MEDIUM,     // 18
	//	SIZE_LARGE,      // 22
	//	SIZE_XLARGE,     // 26
	//	SIZE_XXLARGE     // 30
	//};
	//enum FONT_COLOR
	//{
	//	COLOR_BLACK,
	//	COLOR_RED,
	//	COLOR_GREEN,
	//	COLOR_YELLOW,
	//	COLOR_GREY
	//};

	//struct FontSettings {
	//	FONT_NAME name;
	//	FONT_COLOR color;
	//	FONT_SIZE size;
	//};

	const int winWidth = GetSystemMetrics(SM_CXSCREEN);
	const int winHeight = GetSystemMetrics(SM_CYSCREEN);

	const int btnWidth = 100;
	const int btnHeight = 40;

	Table* table;

	const std::vector<int> fontSize = {14, 18, 22, 26, 30};
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