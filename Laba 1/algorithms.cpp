#include "algorithms.h"
#include "globals.h"
#include <iostream>
#include <fstream>

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
                FILETIME lastAccess = findFileData.ftLastAccessTime;
                std::pair<std::wstring, FILETIME> tempPair = { fileName, lastAccess };
                gl::table->AddRecord(tempPair);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind); // Закрываем дескриптор поиска
};


std::wstring CreateLine(const std::wstring &fileName, int count)
{
    // Вывод номера и сущности
    std::wstring line = std::to_wstring(count) + L") " + fileName + L"\r\n";
    return line;
};


void SortFiles(std::vector<std::pair<std::wstring, FILETIME>>& files, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    int i = left;
    int j = right;
    do {
        while (Comparator(files[i].second, files[mid].second)) {
            i++;
        }
        while (Comparator(files[mid].second, files[j].second)) {
            j--;
        }
        if (i <= j) {
            Swap(files[i], files[j]);
            i++;
            j--;
        }
    } while (i <= j);
    if (left < j) {
        SortFiles(files, left, j);
    }
    if (i < right) {
        SortFiles(files, i, right);
    }
}

void Swap(std::pair<std::wstring, FILETIME>& a, std::pair<std::wstring, FILETIME>& b)
{
    std::pair<std::wstring, FILETIME> temp = a;
    a = b;
    b = temp;
};

bool Comparator(const FILETIME& a, const FILETIME& b)
{
        return CompareFileTime(&a, &b) > 0;
}


