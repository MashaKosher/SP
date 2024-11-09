#pragma once
#include <windows.h>

void MainWndAddWidgets(HWND hwnd); // Функция для создания дочерних окон
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Кастомная процедура окна для поля ввода директории
void CreateFontSettingsWindow(HWND hwnd);
LRESULT CALLBACK FontSettingsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);