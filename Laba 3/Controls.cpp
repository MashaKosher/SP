#include "Controls.h"
#include "globals.h"
#include <Richedit.h>
#include "algorithms.h"

#include <algorithm>
#include <iostream>
#include <set>

// Загрузка библиотеки
HINSTANCE hRichEditLib = LoadLibrary(TEXT("Msftedit.dll"));

void MainWndAddWidgets(HWND hwnd)
{
    int dispX, dispY;
    
    gl::table = new Table(hwnd);

    dispX = 20;
    dispY = gl::table->GetTableHeight() + 10;
    gl::hButtonFontSettings = CreateWindowA("button", "Font settings", WS_VISIBLE | WS_CHILD | ES_CENTER | BS_DEFPUSHBUTTON, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);

	dispX = gl::winWidth - gl::btnWidth * 2 - 20;
	gl::hButtonScrUp = CreateWindowA("button", "Scr Up", WS_VISIBLE | WS_CHILD | ES_CENTER | BS_DEFPUSHBUTTON, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);

	dispX = gl::winWidth - gl::btnWidth - 10;
	gl::hButtonScrDown = CreateWindowA("button", "Scr Down", WS_VISIBLE | WS_CHILD | ES_CENTER, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
}

void CreateFontSettingsWindow(HWND hwnd)
{
    EnableWindow(hwnd, FALSE);

    const wchar_t CLASS_NAME[] = L"Font Settings Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = FontSettingsWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Размеры окна настроек шрифтов
    int width = 500;
    int height = 300;

    // Получение размеров главного окна
    RECT parentRect;
    GetClientRect(hwnd, &parentRect);

    // Вычисление координат для центрирования
    int x = (parentRect.right - width) / 2;
    int y = (parentRect.bottom - height) / 2;

    HWND hwndChild = CreateWindowEx(
        0, CLASS_NAME, L"Font settings", WS_POPUP | WS_VISIBLE | WS_CAPTION,
        x, y, width, height,
        hwnd, nullptr, wc.hInstance, nullptr
    );

    int dispX = 60, dispY = 10;
    int comboboxWidth = 120;
    CreateWindowA("static", "Title", WS_VISIBLE | WS_CHILD, dispX, dispY, comboboxWidth, 20, hwndChild, NULL, NULL, NULL);

    dispX += comboboxWidth + 10;
    CreateWindowA("static", "File info", WS_VISIBLE | WS_CHILD, dispX, dispY, comboboxWidth, 20, hwndChild, NULL, NULL, NULL);

    dispX += comboboxWidth + 10;
    CreateWindowA("static", "Sorting info:", WS_VISIBLE | WS_CHILD, dispX, dispY, comboboxWidth, 20, hwndChild, NULL, NULL, NULL);

    dispX = 10, dispY = 30;
    CreateWindowA("static", "Type:", WS_VISIBLE | WS_CHILD, dispX, dispY, 50, 20, hwndChild, NULL, NULL, NULL);

    dispY += 50;
    CreateWindowA("static", "Size:", WS_VISIBLE | WS_CHILD, dispX, dispY, 50, 20, hwndChild, NULL, NULL, NULL);

    dispY += 50;
    CreateWindowA("static", "Color:", WS_VISIBLE | WS_CHILD, dispX, dispY, 50, 20, hwndChild, NULL, NULL, NULL);

    // Создание комбобоксов
    for (int i = 0, dispX = 60; i < 3; i++, dispX += comboboxWidth + 10) // i = 0 - заголовок, 1 - первый и третий столбцы таблицы, 2 - второй и четвертый столбцы таблицы
    {
        dispY = 30;
        gl::hComboboxFontName[i] = CreateWindowA("COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
            dispX, dispY, comboboxWidth, 25 * gl::fontName.size(), hwndChild, nullptr, nullptr, nullptr);
        for(const std::wstring& name: gl::fontName)
            SendMessage(gl::hComboboxFontName[i], CB_ADDSTRING, 0, (LPARAM)name.c_str());
        SendMessage(gl::hComboboxFontName[i], CB_SETCURSEL, gl::table->GetFontNameIndices()[i], 0); // Устанавливаем текущее имя шрифта, которое используется в таблице, в комбобоксе

        dispY += 50;
        gl::hComboboxFontSize[i] = CreateWindowA("COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
            dispX, dispY, comboboxWidth, 25 * gl::fontSize.size(), hwndChild, nullptr, nullptr, nullptr);
        for (const int& size : gl::fontSize)
            SendMessage(gl::hComboboxFontSize[i], CB_ADDSTRING, 0, (LPARAM)std::to_wstring(size).c_str());
        SendMessage(gl::hComboboxFontSize[i], CB_SETCURSEL, gl::table->GetFontSizeIndices()[i], 0); // Устанавливаем текущий размер шрифта

        dispY += 50;
        gl::hComboboxFontColor[i] = CreateWindowA("COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
            dispX, dispY, comboboxWidth, 25 * gl::fontColor.size(), hwndChild, nullptr, nullptr, nullptr);
        for (const gl::ColoredText& coloredText: gl::fontColor)
            SendMessage(gl::hComboboxFontColor[i], CB_ADDSTRING, 0, (LPARAM)coloredText.text.c_str());
        SendMessage(gl::hComboboxFontColor[i], CB_SETCURSEL, gl::table->GetFontColorIndices()[i], 0); // Устанавливаем текущий цвет шрифта
    }

    dispY += 50;
    dispX = 60;
    // Создаем эдит для ввода пути к файлу ttf и кнопку для загрузки шрифта из этого файла
    gl::hEditFontPath = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_BORDER, dispX, dispY, comboboxWidth * 3, 20, hwndChild, NULL, NULL, NULL);
    dispX += comboboxWidth * 3 + 5;
    CreateWindowA("BUTTON", "Load", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        dispX, dispY, 50, 20, hwndChild, (HMENU)3, nullptr, nullptr);

    // Создание кнопок "ОК" и "Отмена"
    dispY = 30 + 50 * 3 + 40;
    dispX = 100;
    CreateWindowA("BUTTON", "OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        dispX, dispY, 100, 30, hwndChild, (HMENU)1, nullptr, nullptr);
    CreateWindowA("BUTTON", "Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        width - dispX - 100, dispY, 100, 30, hwndChild, (HMENU)2, nullptr, nullptr);
}

LRESULT CALLBACK FontSettingsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1: // ID кнопки "OK"
        {   // Получение выбранных настроек шрифта
            int fontNameIndex[3];
            int fontSizeIndex[3];
            int fontColorIndex[3];
            for (int i = 0; i < 3; i++)
            {
                fontNameIndex[i] = SendMessage(gl::hComboboxFontName[i], CB_GETCURSEL, 0, 0);
                fontSizeIndex[i] = SendMessage(gl::hComboboxFontSize[i], CB_GETCURSEL, 0, 0);
                fontColorIndex[i] = SendMessage(gl::hComboboxFontColor[i], CB_GETCURSEL, 0, 0);
            }

            bool isChanged = false; // false, если настройки шрифта не были изм., иначе - true
            for (int i = 0; i < 3; i++)
            {
                if (!isChanged && (gl::table->GetFontNameIndices()[i] != fontNameIndex[i]
                    || gl::table->GetFontSizeIndices()[i] != fontSizeIndex[i]
                    || gl::table->GetFontColorIndices()[i] != fontColorIndex[i]))
                    isChanged = !isChanged;
                gl::table->GetFontNameIndices()[i] = fontNameIndex[i];
                gl::table->GetFontSizeIndices()[i] = fontSizeIndex[i];
                gl::table->GetFontColorIndices()[i] = fontColorIndex[i];
            }
            if (isChanged)
                gl::table->UpdateTitleFont();
                gl::table->RequestRedraw();
            DestroyWindow(hwnd); // Закрываем окно после обработки
            break;
        }
        case 2: // ID кнопки "Cancel"
            DestroyWindow(hwnd);
            break;
        case 3: // ID кнопки "Load"
        {
            // Получаем текст из элемента управления Edit
            wchar_t filePath[260]; // Буфер для хранения пути к файлу .ttf
            GetWindowText(gl::hEditFontPath, filePath, sizeof(filePath) / sizeof(wchar_t));

            // Если файл не существует или его расширение не ttf, отображаем ошибку
            if (!FontFileExists(filePath))
            {
                MessageBox(hwnd, L"File doesn't exist or its extension isn't ttf", L"Error", MB_OK);
                break;
            }
            
            std::set<std::wstring> before, after;

            ListFonts(before);
            LoadCustomFont(filePath);
            ListFonts(after);

            // Множество для хранения разности
            std::set<std::wstring> difference;

            // Находим разницу до загрузки файла ttf и после
            std::set_difference(after.begin(), after.end(),
                before.begin(), before.end(),
                std::inserter(difference, difference.begin()));

            if (!difference.size())
            {
                MessageBox(hwnd, L"Failed to upload file", L"Error", MB_OK);
                break;
            }
            std::wstring loadedFonts;
            for (auto it = difference.begin(); it != difference.end(); ++it) {
                for (int i = 0; i < 3; i++)
                    SendMessage(gl::hComboboxFontName[i], CB_ADDSTRING, 0, (LPARAM)(*it).c_str());
                gl::fontName.push_back((*it));

                loadedFonts += *it; // Добавляем текущий элемент

                // Если это не последний элемент, добавляем запятую
                if (std::next(it) != difference.end()) {
                    loadedFonts += L", ";
                }
            }
            MessageBox(hwnd, (L"Successfully uploaded: " + loadedFonts).c_str(), L"Notification", MB_OK);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    case WM_DESTROY:
        //PostQuitMessage(0);
        EnableWindow(GetParent(hwnd), TRUE);
        SetForegroundWindow(GetParent(hwnd));
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}