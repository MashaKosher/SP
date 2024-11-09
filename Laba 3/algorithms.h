#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <set>

void ListFiles(const std::wstring& directory); // функция поиска файлов с рекурсивным обходом директорий
std::wstring CreateLine(const std::wstring& fileName, int count); // возвращает строку формата "count)fileName\r\n"
void SortFiles(std::vector<std::wstring>& files, int left, int right, bool (*comparator)(const std::wstring&, const std::wstring&)); // quicksort для строк с компаратором
void Swap(std::wstring& a, std::wstring& b); 
bool Comparator(const std::wstring& a, const std::wstring& b); // true, если a < b, иначе - false
std::wstring RemoveFileExtension(const std::wstring& filePath); // удаляет расширение файла, возвращаемое значение - имя файла
bool FontFileExists(const wchar_t* filePath); // true, если файл ttf существует, иначе - false
bool LoadCustomFont(const wchar_t* fontFilePath); // true, если удалось загрузить шрифт, иначе - false
int CALLBACK EnumFontFamExProc(const LOGFONTA* lpLogFont, const TEXTMETRICA*, DWORD FontType, LPARAM lParam); // коллбэк-функция, которая будет вызвана для каждого найденного шрифта
void ListFonts(std::set<std::wstring>& fontNames); // функция перечисления всех доступных шрифтов
void RemoveCustomFonts(); // освобождение ресурсов после использования AddResourseEx из функции LoadCustomFont
//std::wstring millisecondsToTimeString(long long milliseconds); // преобразование миллисек. в формат ч:м:с:млс