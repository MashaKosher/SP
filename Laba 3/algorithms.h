#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <set>

void ListFiles(const std::wstring& directory); // ������� ������ ������ � ����������� ������� ����������
std::wstring CreateLine(const std::wstring& fileName, int count); // ���������� ������ ������� "count)fileName\r\n"
void SortFiles(std::vector<std::wstring>& files, int left, int right, bool (*comparator)(const std::wstring&, const std::wstring&)); // quicksort ��� ����� � ������������
void Swap(std::wstring& a, std::wstring& b); 
bool Comparator(const std::wstring& a, const std::wstring& b); // true, ���� a < b, ����� - false
std::wstring RemoveFileExtension(const std::wstring& filePath); // ������� ���������� �����, ������������ �������� - ��� �����
bool FontFileExists(const wchar_t* filePath); // true, ���� ���� ttf ����������, ����� - false
bool LoadCustomFont(const wchar_t* fontFilePath); // true, ���� ������� ��������� �����, ����� - false
int CALLBACK EnumFontFamExProc(const LOGFONTA* lpLogFont, const TEXTMETRICA*, DWORD FontType, LPARAM lParam); // �������-�������, ������� ����� ������� ��� ������� ���������� ������
void ListFonts(std::set<std::wstring>& fontNames); // ������� ������������ ���� ��������� �������
void RemoveCustomFonts(); // ������������ �������� ����� ������������� AddResourseEx �� ������� LoadCustomFont
//std::wstring millisecondsToTimeString(long long milliseconds); // �������������� ��������. � ������ �:�:�:���