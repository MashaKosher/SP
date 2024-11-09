#include "algorithms.h"
#include "globals.h"
#include <cwchar> // Для wcscpy
#include <locale>
#include <codecvt>
#include <iomanip>
#include <sstream>
#include "utils.h"


void ListFiles(const std::wstring& directory)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // Формируем строку для поиска файлов
    std::wstring searchPath = directory + L"\\*";

    // Начинаем поиск файлов
    hFind = FindFirstFile(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return; // Ошибка, если не удалось открыть директорию
    }

    do {
        const std::wstring fileName = findFileData.cFileName;

        // Пропускаем "." и ".."
        if (fileName != L"." && fileName != L"..") {
            // Если это директория, рекурсивно обходим ее
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ListFiles(directory + L"\\" + fileName);
            }
            else {
                // Добавляем файл в таблицу
                gl::table->AddRecord(fileName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind); // Закрываем дескриптор поиска
};

std::wstring CreateLine(const std::wstring &fileName, int count)
{
    std::wstring line = std::to_wstring(count) + L") " + fileName + L"\r\n";
    return line;
};

void SortFiles(std::vector<std::wstring>& files, int left, int right, bool (*comparator)(const std::wstring&, const std::wstring&)) {
    if (left >= right) return;

    // Центральный элемент
    std::wstring mid = files[left + (right - left) / 2];
    int i = left;
    int j = right;

    do {
        // Пропускаем элементы, которые меньше центрального
        while (comparator(files[i], mid)) {
            i++;
        }
        // Пропускаем элементы, которые больше центрального
        while (comparator(mid, files[j])) {
            j--;
        }

        // Меняем элементы местами
        if (i <= j) {
            Swap(files[i], files[j]);
            i++;
            j--;
        }
    } while (i <= j);

    // Рекурсивные вызовы для левой и правой части
    if (left < j) {
        SortFiles(files, left, j, comparator);
    }
    if (i < right) {
        SortFiles(files, i, right, comparator);
    }
}

void Swap(std::wstring& a, std::wstring& b)
{
    std::wstring temp = a;
    a = b;
    b = temp;
};

bool Comparator(const std::wstring& a, const std::wstring& b)
{
    bool res = RemoveFileExtension(a) < RemoveFileExtension(b);
    return res;
}

std::wstring RemoveFileExtension(const std::wstring& filePath) {
    // Поиск первого вхождения '.' в строку
    size_t dotPos = filePath.find_first_of(L'.');

    std::wstring res = filePath;
    
    // Проверка, есть ли расширение
    if (dotPos != std::wstring::npos)
        // Удаляем расширение из строки с именем файла
        res.erase(dotPos);
    return res;
}

bool FontFileExists(const wchar_t* filePath) {
    // Проверяем, существует ли файл
    DWORD fileAttr = GetFileAttributesW(filePath);
    bool res = (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));

    // Если не существует, прекращаем проверку
    if (!res)
        return res;

    std::wstring fontPath(filePath);

    // Проверяем на расширение ttf
    size_t dotPos = fontPath.find_last_of(L'.');
    fontPath = fontPath.substr(dotPos);
    if (fontPath != L".ttf")
        res = false;
    return res;
}

bool LoadCustomFont(const wchar_t* fontFilePath)
{
    DWORD result = AddFontResourceEx(fontFilePath, FR_PRIVATE, 0);
    // result = 0, если загрузить шрифт не удалось либо он уже загружен
    if (result != 0) {
        // Сохраняем путь к кастомному шрифту в глобальный вектор
        gl::fontPaths.push_back(fontFilePath);
        return true;
    }
    return false;
}

int CALLBACK EnumFontFamExProc(const LOGFONTA* lpLogFont, const TEXTMETRICA*, DWORD FontType, LPARAM lParam) {
    // Получаем вектор для сохранения имен шрифтов
    auto* fontNames = reinterpret_cast<std::set<std::wstring>*>(lParam);

    // Проверяем, является ли шрифт TrueType
    if (FontType & TRUETYPE_FONTTYPE) {
        // Конвертация имени шрифта из char в wstring
        std::wstring fontName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(lpLogFont->lfFaceName);
        fontNames->insert(fontName); // Добавляем имя шрифта в множество
    }

    return 1; // Продолжаем перечисление
}

void ListFonts(std::set<std::wstring>& fontNames) {
    // Получаем контекст устройства
    HDC hdc = GetDC(NULL);
    if (hdc) {
        // Перечисляем шрифты (по итогу работы все возможные имена шрифтов будут в fontNames)
        EnumFontFamiliesExA(hdc, NULL, EnumFontFamExProc, reinterpret_cast<LPARAM>(&fontNames), 0);
        ReleaseDC(NULL, hdc);
    }
}

void RemoveCustomFonts()
{
    // проходимся по глобальному вектору, который хранит пути к кастомным шрифтам, что загрузил пользователь
    for(const std::wstring &path : gl::fontPaths)
        RemoveFontResourceEx(path.c_str(), FR_PRIVATE, NULL);
    gl::fontPaths.clear();
}

//std::wstring millisecondsToTimeString(long long milliseconds) {
//    long long hours = milliseconds / 3600000;
//    milliseconds %= 3600000;
//    long long minutes = milliseconds / 60000;
//    milliseconds %= 60000;
//    long long seconds = milliseconds / 1000;
//    long long millis = milliseconds % 1000;
//
//    std::wstringstream result;
//    result << std::setw(2) << std::setfill(L'0') << hours << L':'
//        << std::setw(2) << std::setfill(L'0') << minutes << L':'
//        << std::setw(2) << std::setfill(L'0') << seconds << L':'
//        << std::setw(3) << std::setfill(L'0') << millis;
//
//    return result.str();
//}
