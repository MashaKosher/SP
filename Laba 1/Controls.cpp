#include "Controls.h"
#include "globals.h"
#include <Richedit.h>
#include <shlwapi.h> // Для PathFileExists

// Загрузка библиотеки
HINSTANCE hRichEditLib = LoadLibrary(TEXT("Msftedit.dll"));


void MainWndAddWidgets(HWND hwnd)
{
    int textHeight = 22; // Создание шрифта Times New Roman
    
    HFONT hFont = CreateFont(
        textHeight,          // Height
        0,                   // Width
        0,                   // Escapement
        0,                   // Orientation
        FW_NORMAL,           // Weight
        FALSE,               // Italic
        FALSE,               // Underline
        FALSE,               // StrikeOut
        DEFAULT_CHARSET,     // CharSet
        OUT_DEFAULT_PRECIS,  // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,     // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        L"Times New Roman"          // Font Name
    );

    int dispX = 3, dispY = 5;
    // создаем окно Edit Control
    gl::hDirectoryEdit = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, dispX, dispY, gl::inputWidth, gl::inputHeight, hwnd, NULL, NULL, NULL);

    // Установка шрифта для Edit Control
    SendMessage(gl::hDirectoryEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Задаем новый адрес для процедуры окна, чтобы добавить обработку нажатия клавиши Enter
    gl::OriginalEditProc = (WNDPROC)SetWindowLongPtr(gl::hDirectoryEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    
    // добавляем Таблицу
    gl::table = new Table(hwnd);


	dispX = gl::winWidth/2 - gl::btnWidth  - 1;  // Коордианта X левой кнокпи
	dispY = gl::table->GetTableHeight() + 10;    // Координата Y левой и правой кнопки

    // создаем левую кнопку
	gl::hButtonScrUp = CreateWindowA("button", "Вверх", WS_VISIBLE | WS_CHILD | ES_CENTER | BS_DEFPUSHBUTTON, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
  
    dispX = gl::winWidth/2 +3; // Коордианта X правой кнокпи
    // Создаем правую кнопку
	gl::hButtonScrDown = CreateWindowA("button", "Вниз", WS_VISIBLE | WS_CHILD | ES_CENTER, dispX, dispY, gl::btnWidth, gl::btnHeight, hwnd, NULL, NULL, NULL);
}

// Кастомная процедура окна для gl::hDirectoryEdit
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN: 
        if (wParam == VK_RETURN) {
            // Получаем текст из элемента управления Edit
            wchar_t directoryPath[260]; // Буфер для хранения пути
            GetWindowText(gl::hDirectoryEdit, directoryPath, sizeof(directoryPath) / sizeof(wchar_t));

            // Проверяем, существует ли директория
            if (PathFileExists(directoryPath)) {
                // Здесь можно вызвать вашу функцию для обработки директории, если нужно
                gl::table->FillOutTable(directoryPath);
                std::wstring res = L"Total count - " + std::to_wstring(gl::table->GetTableRecordsCount()); // сообщение в окне
                MessageBox(hwnd, (LPCWSTR)res.c_str(), L"Result", MB_OK); // создание высплывающего окна
            }
            else {
                MessageBox(hwnd, L"Directory does not exist!", L"Error", MB_OK); // создание высплывающего окна
            }
        }
        break;
    
    }

    // Вызываем оригинальную процедуру edit control для обработки по умолчанию
    return CallWindowProc(gl::OriginalEditProc, hwnd, uMsg, wParam, lParam);
}