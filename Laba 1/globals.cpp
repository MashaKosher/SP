#include "globals.h"

namespace gl {

	const int winWidth = GetSystemMetrics(SM_CXSCREEN);
	const int winHeight = GetSystemMetrics(SM_CYSCREEN);

	const int inputWidth = winWidth / 4 - 3;
	const int inputHeight = 20;

	const int btnWidth = 100;
	const int btnHeight = 40;

	HWND  hDirectoryEdit;

	Table* table;

	HWND  hButtonScrUp;
	HWND  hButtonScrDown;

	WNDPROC OriginalEditProc;
}