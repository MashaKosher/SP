#pragma once

// преобразование миллисек. в формат ч:м:с:млс
std::wstring millisecondsToTimeString(long long milliseconds); 

// Появляение комбобокса с информацией о кликах при нажатии на ячейкм
void PrintMessage(HWND hwnd, std::wstring event, int columnNum, int rowNum, DWORD ellapsedTime); 


