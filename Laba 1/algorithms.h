#pragma once

#include <windows.h>
#include <vector>
#include <string>

void ListFiles(const std::wstring& directory); // ������� ������ ������ � ����������� ������� ����������
std::wstring CreateLine(const std::wstring& fileName, int count); // ���������� ������ ������� "count)fileName\r\n"
void SortFiles(std::vector<std::pair<std::wstring, FILETIME>>& files, int left, int right); // quicksort ��� ����� � ������������
void Swap(std::pair<std::wstring, FILETIME>& a, std::pair<std::wstring, FILETIME>& b);
bool Comparator(const FILETIME& a, const FILETIME& b); // true, ���� a < b, ����� - false
std::wstring RemoveFileExtension(const std::wstring& filePath); // ������� ���������� �����, ������������ �������� - ��� �����