#pragma once

#include <windows.h>
#include <vector>
#include <string>

void ListFiles(const std::wstring& directory); // функция поиска файлов с рекурсивным обходом директорий
std::wstring CreateLine(const std::wstring& fileName, int count); // возвращает строку формата "count)fileName\r\n"
void SortFiles(std::vector<std::pair<std::wstring, FILETIME>>& files, int left, int right); // quicksort для строк с компаратором
void Swap(std::pair<std::wstring, FILETIME>& a, std::pair<std::wstring, FILETIME>& b);
bool Comparator(const FILETIME& a, const FILETIME& b); // true, если a < b, иначе - false
std::wstring RemoveFileExtension(const std::wstring& filePath); // удаляет расширение файла, возвращаемое значение - имя файла