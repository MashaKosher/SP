#include "Table.h"
#include "globals.h"
#include "algorithms.h"
#include <shlwapi.h> // Для PathFileExists
#pragma comment(lib, "winmm.lib")
#include "utils.h"


// Определение статической переменной hFont
HFONT Table::Title::hFont = NULL;

// Определение статической переменной hDirectoryEdit
HWND Table::Title::hDirectoryEdit = NULL;

// Определение статической переменной title
Table::Title* Table::title = NULL;

// Определение статической переменной hWnd
HWND Table::hWnd = NULL;

#define WM_CELL_LBUTTONCLCK WM_USER + 10
#define WM_CELL_DLBUTTONCLCK WM_USER + 11
#define WM_CELL_TLBUTTONCLCK WM_USER + 12
#define WM_CELL_RBUTTONCLCK WM_USER + 13
#define WM_CELL_DRBUTTONCLCK WM_USER + 14
#define WM_CELL_TRBUTTONCLCK WM_USER + 15

static int clickCount = 0;
static const int clickTime = 500; // Время, которое дается пользователю для клика мышью
static DWORD startTime = 0, elapsedTime = 0;
static POINT currentPoint, previousPoint;
static int rowNum, columnNum;
static DWORD startTest = timeGetTime();
static bool oneRightClick = false;
static bool twoRightClick = false;

// Кастомная процедура окна для gl::hDirectoryEdit
LRESULT CALLBACK Table::Title::EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN:
        if (wParam == VK_RETURN) {
            // Получаем текст из элемента управления Edit
            wchar_t directoryPath[260]; // Буфер для хранения пути
            GetWindowText(hDirectoryEdit, directoryPath, sizeof(directoryPath) / sizeof(wchar_t));

            // Проверяем, существует ли директория
            if (PathFileExists(directoryPath)) {
                // Здесь можно вызвать вашу функцию для обработки директории, если нужно
                gl::table->FillOutTable(directoryPath);
                std::wstring res = L"Sorting done. Total count - " + std::to_wstring(gl::table->GetTableRecordsCount());
                MessageBox(hwnd, (LPCWSTR)res.c_str(), L"Notification", MB_OK);
            }
            else {
                MessageBox(hwnd, L"Directory does not exist!", L"Error", MB_OK);
            }
        }
        break;
    case WM_DESTROY: {
        // Освобождение ресурсов
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    }
    }

    // Вызываем оригинальную процедуру edit control для обработки по умолчанию
    return CallWindowProc(gl::OriginalEditProc, hwnd, uMsg, wParam, lParam);
}

void Table::Title::Draw(HDC &hdc)
{
    // Устанавливаем цвет фона
    RECT fillRect = { 0, 0, width, height }; // Прямоугольник для заливки
    FillRect(hdc, &fillRect, (HBRUSH)(COLOR_WINDOW + 1));

    DeleteObject(hFont);
    hFont = CreateFont(
        gl::fontSize[fontSize],    // Height
        0,                 // Width
        0,                 // Escapement
        0,                 // Orientation
        FW_NORMAL,        // Weight
        FALSE,            // Italic
        FALSE,            // Underline
        FALSE,            // StrikeOut
        DEFAULT_CHARSET,  // CharSet
        OUT_DEFAULT_PRECIS, // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,  // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        gl::fontName[fontName].c_str()    // Font Name
    );

    // Сохраняем старый шрифт
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor].color);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm); // Получаем метрики текста
    const int textHeight = tm.tmHeight;

    const int columnWidth = width / columnCount; // Ширина столбца
    const int leftDisp = 3; // Левый отступ текста в столбце

    // Рисование вертикальных линий между столбцами
    for (size_t j = 1; j < columnCount; ++j) {
        int x = j * columnWidth;
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, height);
    }

    // Рисование горизонтальных линий между строками
    MoveToEx(hdc, 0, height, NULL);
    LineTo(hdc, width, height);

    SendMessage(hDirectoryEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SetWindowPos(hDirectoryEdit, NULL, leftDisp, (height - textHeight) / 2, columnWidth - 10, textHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
    InvalidateRect(hDirectoryEdit, NULL, TRUE);
    
    std::wstring headerText = L"Имя файла";
    TextOut(hdc, leftDisp + columnWidth, (height - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = L"Результат сортировки";
    TextOut(hdc, leftDisp + 2 * columnWidth, (height - textHeight) / 2, headerText.c_str(), headerText.length());
    headerText = L"Имя файла";
    TextOut(hdc, leftDisp + 3 * columnWidth, (height - textHeight) / 2, headerText.c_str(), headerText.length());

    // Восстанавливаем старый шрифт
    SelectObject(hdc, hOldFont);
}

Table::Title::Title(HWND &table, int width) {
    this->width = width;
    int dispX = 0, dispY = 0;
    hDirectoryEdit = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, dispX, dispY, width / columnCount, height, table, NULL, NULL, NULL);

    // Задаем новый адрес для процедуры окна, чтобы добавить обработку нажатия клавиши Enter
    gl::OriginalEditProc = (WNDPROC)SetWindowLongPtr(hDirectoryEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

    hWnd = table;
}


LRESULT CALLBACK Table::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Table* listView = (Table*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (uMsg == WM_NCCREATE) {
        // Извлечение указателя на экземпляр класса Table, чтобы обращаться к методам и свойствам объекта внутри обработчика
        listView = (Table*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)listView); // Сохраняем указатель на создаваемый объект Table в пользовательских данных окна для дальнейшего доступа к объекту
    }

    switch (uMsg) {
    case WM_PAINT:
        if (listView) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(listView->GetHandle(), &ps);
            if (listView->isRedrawRequired)
                listView->Redraw(hdc, ps);
            else if (listView->input.size() == 0)
                listView->DrawLayout(hdc); // Отрисовка статической части таблицы
            else
                listView->DrawContent(hdc, ps); // Отрисовка списка файлов в таблице
            EndPaint(listView->GetHandle(), &ps);
        }
        return 0;
    case WM_MOUSEWHEEL: {
        if (listView) {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            int newScrollPos = listView->scrollPos;
            newScrollPos += (delta > 0) ? -listView->recordingCapacity : listView->recordingCapacity; // Прокрутка вверх или вниз

            // Проверка на выход за пределы вектора
            newScrollPos = newScrollPos < (int)listView->input.size() - listView->recordingCapacity ? newScrollPos : (int)listView->input.size() - listView->recordingCapacity;
            newScrollPos = 0 > newScrollPos ? 0 : newScrollPos;

            if (newScrollPos != listView->scrollPos)
            {
                listView->scrollPos = newScrollPos;
                InvalidateRect(listView->hWnd, NULL, TRUE); // Перерисовка
            }
        }
        return 0;
    }
    case WM_SETCURSOR: {
        // Проверяем, что курсор находится над окном
        if (LOWORD(lParam) == HTCLIENT) {
            // Устанавливаем курсор на стрелку
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE; // Сообщаем, что курсор установлен
        }
        break; // Иначе продолжаем обработку
    }

    case WM_CELL_LBUTTONCLCK:
    {

        PrintMessage(hwnd, L"WM_CELL_LBUTTONCLCK", columnNum, rowNum, elapsedTime);
        return 0;
    }
    case WM_CELL_DLBUTTONCLCK:
    {
        PrintMessage(hwnd, L"WM_CELL_DLBUTTONCLCK", columnNum, rowNum, elapsedTime);
        return 0;
    }
    case WM_CELL_TLBUTTONCLCK:
    {
        PrintMessage(hwnd, L"WM_CELL_TLBUTTONCLCK", columnNum, rowNum, elapsedTime);
        return 0;
    }
    case WM_CELL_RBUTTONCLCK:
    {
        PrintMessage(hwnd, L"WM_CELL_RBUTTONCLCK", columnNum, rowNum, elapsedTime);
        return 0;
    }
    case WM_CELL_DRBUTTONCLCK:
    {
        PrintMessage(hwnd, L"WM_CELL_DRBUTTONCLCK", columnNum, rowNum, elapsedTime);
        return 0;
    }

    case WM_CELL_TRBUTTONCLCK:
    {
        PrintMessage(hwnd, L"WM_CELL_TRBUTTONCLCK", columnNum, rowNum, elapsedTime);
        return 0;
    }

    case WM_RBUTTONDOWN: {

        currentPoint.x = LOWORD(lParam);
        currentPoint.y = HIWORD(lParam);
        if (twoRightClick) {
            KillTimer(hwnd, WM_CELL_DRBUTTONCLCK);
            startTime = GetTickCount();
            SetTimer(hwnd, WM_CELL_TRBUTTONCLCK, clickTime, NULL);
            startTime = GetTickCount();
            previousPoint = currentPoint;
            twoRightClick = false;
            break;

        }
        startTime = GetTickCount();
        SetTimer(hwnd, WM_CELL_RBUTTONCLCK, clickTime, NULL);
        startTime = GetTickCount();
        previousPoint = currentPoint;
        oneRightClick = true;
        break;
    }

    case WM_RBUTTONDBLCLK:
    {
        if (oneRightClick) {
            KillTimer(hwnd, WM_CELL_RBUTTONCLCK);
        }
        oneRightClick = false;

        currentPoint.x = LOWORD(lParam);
        currentPoint.y = HIWORD(lParam);
        startTime = GetTickCount();
        SetTimer(hwnd, WM_CELL_DRBUTTONCLCK, clickTime, NULL);
        previousPoint = currentPoint;
        twoRightClick = true;
        break;
        
    }



    //case WM_RBUTTONDOWN:
    //{
    //    // Получаем текущие координаты курсора
    //    currentPoint.x = LOWORD(lParam);
    //    currentPoint.y = HIWORD(lParam);

    //    if (clickCount > 0 && (currentPoint.x != previousPoint.x || currentPoint.y != previousPoint.y) || clickCount > 1)
    //    {
    //        KillTimer(hwnd, WM_CELL_RBUTTONCLCK);
    //        clickCount = 0;
    //        return 0;
    //    }

    //    switch (clickCount) {
    //    case 0:
    //        SetTimer(hwnd, WM_CELL_RBUTTONCLCK, clickTime, NULL);
    //        startTime = GetTickCount();
    //        elapsedTime = 0;
    //        break;
    //    case 1:
    //        elapsedTime += GetTickCount() - startTime;
    //        KillTimer(hwnd, WM_CELL_RBUTTONCLCK);
    //        SetTimer(hwnd, WM_CELL_DRBUTTONCLCK, clickTime, NULL);
    //        startTime = GetTickCount();
    //        break;
    //    }
    //    // Сохраняем текущие координаты как предыдущие
    //    previousPoint = currentPoint;
    //    clickCount++; // Увеличиваем счетчик кликов
    //    return 0;
    //}
    case WM_LBUTTONDOWN:
    {
        // Получаем текущие координаты курсора
        currentPoint.x = LOWORD(lParam);
        currentPoint.y = HIWORD(lParam);

        if (clickCount > 0 && (currentPoint.x != previousPoint.x || currentPoint.y != previousPoint.y) || clickCount > 2)
        {
            KillTimer(hwnd, WM_CELL_LBUTTONCLCK + clickCount - 1);
            clickCount = 0;
            return 0;
        }

        switch (clickCount) {
        case 0:
            SetTimer(hwnd, WM_CELL_LBUTTONCLCK, clickTime, NULL);
            startTime = GetTickCount();
            elapsedTime = 0;
            break;
        case 1:
        case 2:
            elapsedTime += GetTickCount() - startTime;
            KillTimer(hwnd, WM_CELL_LBUTTONCLCK + clickCount - 1);
            SetTimer(hwnd, WM_CELL_LBUTTONCLCK + clickCount, clickTime, NULL);
            startTime = GetTickCount();
            break;
        }
        // Сохраняем текущие координаты как предыдущие
        previousPoint = currentPoint;
        clickCount++; // Увеличиваем счетчик кликов
        return 0;
    }

    case WM_TIMER:
    {
        //elapsedTime += GetTickCount() - startTime;
        elapsedTime = timeGetTime() - startTest;
        KillTimer(hwnd, wParam);
        if (!listView->IsTitle(currentPoint.y))
        {
            columnNum = listView->GetColumnNumber(currentPoint.x);
            rowNum = listView->GetRowNumber(currentPoint.y);
            PostMessage(hwnd, wParam, 0, 0);
        }
        else {
            columnNum = listView->GetColumnNumber(currentPoint.x);
            rowNum = -1;
            PostMessage(hwnd, wParam, 0, 0);

        }
        clickCount = 0;
        return 0;
    }


    case WM_NCPAINT: {
        HDC hdc = GetWindowDC(hwnd);
        RECT rect;
        GetWindowRect(hwnd, &rect);
        OffsetRect(&rect, -rect.left, -rect.top);

        // Рисуем красную рамку
        HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0)); // Красный цвет
        HGDIOBJ oldBrush = SelectObject(hdc, brush);
        // Рисуем рамку
        FrameRect(hdc, &rect, brush);
        SelectObject(hdc, oldBrush);
        DeleteObject(brush);
        ReleaseDC(hwnd, hdc);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Table::DrawLayout(HDC& hdc)
{
    if (title) {
        title->Draw(hdc);
    }

    // Устанавливаем цвет фона
    RECT fillRect = { 0, title->GetHeight()+1, tableWidth, tableHeight}; // Прямоугольник для заливки
    FillRect(hdc, &fillRect, (HBRUSH)(COLOR_WINDOW + 1));

    const int columnWidth = tableWidth / columnCount; // Ширина столбца

    // Рисование вертикальных линий между столбцами
    for (size_t j = 1; j < columnCount; ++j) {
        int x = j * columnWidth;
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, tableHeight);
    }
}

void Table::DrawContent(HDC &hdc, PAINTSTRUCT& ps)
{
    // Сохраняем старый шрифт
    HFONT hFileContentFont = CreateFont(
        gl::fontSize[fontSize[FILE_INFO]],    // Height
        0,                 // Width
        0,                 // Escapement
        0,                 // Orientation
        FW_NORMAL,        // Weight
        FALSE,            // Italic
        FALSE,            // Underline
        FALSE,            // StrikeOut
        DEFAULT_CHARSET,  // CharSet
        OUT_DEFAULT_PRECIS, // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,  // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        gl::fontName[fontName[FILE_INFO]].c_str()    // Font Name
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFileContentFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[FILE_INFO]].color);

    TEXTMETRIC tm1, tm2;
    GetTextMetrics(hdc, &tm1); // Получаем метрики текста
    const int textHeightFileContent = tm1.tmHeight;


    HFONT hSortedContentFont = CreateFont(
        gl::fontSize[fontSize[SORTING_INFO]],    // Height
        0,                 // Width
        0,                 // Escapement
        0,                 // Orientation
        FW_NORMAL,        // Weight
        FALSE,            // Italic
        FALSE,            // Underline
        FALSE,            // StrikeOut
        DEFAULT_CHARSET,  // CharSet
        OUT_DEFAULT_PRECIS, // OutPrecision
        CLIP_DEFAULT_PRECIS, // ClipPrecision
        DEFAULT_QUALITY,  // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        gl::fontName[fontName[SORTING_INFO]].c_str()    // Font Name
    );
    SelectObject(hdc, hSortedContentFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, gl::fontColor[fontColor[SORTING_INFO]].color);

    GetTextMetrics(hdc, &tm2); // Получаем метрики текста
    const int textHeightSortedContent = tm2.tmHeight;

    // Вычисление вместимости таблицы
    int textHeight = textHeightSortedContent > textHeightFileContent ? textHeightSortedContent : textHeightFileContent;
    int rowHeight = textHeight + 4;
    const int contentHeight = tableHeight - title->GetHeight();
    recordingCapacity = contentHeight / rowHeight;

    while (contentHeight - recordingCapacity * rowHeight > 6)
    {
        recordingCapacity--;
        rowHeight = contentHeight / recordingCapacity;
    }

    const int columnWidth = tableWidth / columnCount; // Ширина столбца
    const int leftDisp = 3; // Левый отступ текста в столбце

    for (int i = 0; i < columnCount; i++)
    {
        RECT fillRect =
        {
            ps.rcPaint.left + columnWidth * i + 1,
            ps.rcPaint.top + title->GetHeight() + 1,
            columnWidth * (i + 1) - 1,
            ps.rcPaint.bottom - 1
        };

        // Устанавливаем цвет фона
        FillRect(hdc, &fillRect, (HBRUSH)(COLOR_WINDOW + 1));
    }

    const int remainingRecordsCount = ((int)input.size() - scrollPos) < (recordingCapacity) ? ((int)input.size() - scrollPos) : recordingCapacity; // Количество записей на вывод в таблицу

    // Рисование содержимого таблицы
    for (size_t j = 0; j < remainingRecordsCount; ++j) {

        (HFONT)SelectObject(hdc, hFileContentFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gl::fontColor[fontColor[FILE_INFO]].color);

        std::wstring text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + input[scrollPos + j], columnWidth, hdc); // Вывод найденных файлов с расширением
        TextOut(hdc, leftDisp, title->GetHeight() + (j * rowHeight) + (rowHeight - textHeightFileContent) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(std::to_wstring(scrollPos + j + 1) + L")" + output[scrollPos + j], columnWidth, hdc); // Вывод отсортированных файлов с расширением
        TextOut(hdc, leftDisp + 2 * columnWidth, title->GetHeight() + (j * rowHeight) + (rowHeight - textHeightFileContent) / 2, text.c_str(), text.length());

        SelectObject(hdc, hSortedContentFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gl::fontColor[fontColor[SORTING_INFO]].color);

        text = TrimTextToWidth(RemoveFileExtension(input[scrollPos + j]), columnWidth, hdc); // Вывод имен найденных файлов 
        //text = text == L"" ? L"*пустое*" : text;
        TextOut(hdc, leftDisp + columnWidth, title->GetHeight() + (j * rowHeight) + (rowHeight - textHeightSortedContent) / 2, text.c_str(), text.length());

        text = TrimTextToWidth(RemoveFileExtension(output[scrollPos + j]), columnWidth, hdc); // Вывод имен отсортированных файлов
        //text = text == L"" ? L"*пустое*" : text;
        TextOut(hdc, leftDisp + 3 * columnWidth, title->GetHeight() + (j * rowHeight) + (rowHeight - textHeightSortedContent) / 2, text.c_str(), text.length());
    }

    // Рисование горизонтальных линий между строками
    for (size_t i = 1; i < recordingCapacity; ++i) {
        int y = title->GetHeight() + i * (contentHeight / recordingCapacity);
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, tableWidth, y);
    }

    // Восстанавливаем старый шрифт
    SelectObject(hdc, hOldFont);
}

void Table::Redraw(HDC &hdc, PAINTSTRUCT& ps)
{
    if (title) {
        title->Draw(hdc);
    }

    DrawLayout(hdc);
    DrawContent(hdc, ps);

    isRedrawRequired = false; // Сбрасываем флаг в false, чтобы без изменения шрифта таблица отрисовывалась с помощью функций DrawLayout и DrawContent
}

std::wstring Table::TrimTextToWidth(const std::wstring& text, const int columnWidth, HDC& hdc)
{
    std::wstring res = text;
    // Определяем максимальную ширину текста для столбца
    SIZE textSize;
    GetTextExtentPoint32(hdc, res.c_str(), res.length(), &textSize);

    // Если текст шире столбца, обрезать его
    if (textSize.cx > (columnWidth - 2)) {
        // Урезаем текст до подходящей длины
        while (res.length() > 0 && textSize.cx > (columnWidth - 2)) {
            res.pop_back(); // Удаляем последний символ
            GetTextExtentPoint32(hdc, (res + L"...").c_str(), (res + L"...").length(), &textSize);
        }
        // Добавляем многоточие, если текст был обрезан
        if (res.length() > 0) {
            res += L"...";
        }
    }
    return res;
}

Table::Table(HWND parent, const struct Font fontTitle, const struct Font fontFileInfo, const struct Font fontSortingInfo) {
    // Регистрируем кастомный класс таблицы
    const wchar_t CLASS_NAME[] = L"Custom Table";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    wc.style = CS_DBLCLKS;
    RegisterClass(&wc);

    fontName[TITLE] = fontTitle.name;
    fontColor[TITLE] = fontTitle.color;
    fontSize[TITLE] = fontTitle.size;

    fontName[FILE_INFO] = fontFileInfo.name;
    fontColor[FILE_INFO] = fontFileInfo.color;
    fontSize[FILE_INFO] = fontFileInfo.size;

    fontName[SORTING_INFO] = fontSortingInfo.name;
    fontColor[SORTING_INFO] = fontSortingInfo.color;
    fontSize[SORTING_INFO] = fontSortingInfo.size;

    int disp = 0;
    tableWidth = gl::winWidth - disp * 1;
    tableHeight = gl::winHeight - gl::btnHeight - 20;

    this->title = new Title(parent, tableWidth);

    UpdateTitleFont();

    // Создаем окно
    hWnd = CreateWindowEx(
        0,
        CLASS_NAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        disp, disp, tableWidth, tableHeight,
        parent, NULL, NULL, this
    );
}

void Table::AddRecord(const std::wstring record)
{
    input.push_back(record);
    output.push_back(record);
}

void Table::FillOutTable(const std::wstring& directoryPath)
{
    gl::table->ClearRecords();

    ListFiles((std::wstring)directoryPath);

    switch (output.size())
    {
    case 1: // Вывод одного файла (не нужна сортировка)
        InvalidateRect(this->hWnd, NULL, TRUE);
        break;
    default: // Сортировка множества файлов с выводом
        SortFiles(output, 0, output.size() - 1, Comparator);
        InvalidateRect(this->hWnd, NULL, TRUE);
        break;
    }
}

void Table::UpdateTitleFont()
{
    title->GetFontNameIndex() = fontName[TITLE];
    title->GetFontSizeIndex() = fontSize[TITLE];
    title->GetFontColorIndex() = fontColor[TITLE];
}