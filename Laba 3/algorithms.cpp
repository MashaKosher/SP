#include "algorithms.h"
#include "globals.h"
#include <cwchar> // ��� wcscpy
#include <locale>
#include <codecvt>
#include <iomanip>
#include <sstream>
#include "utils.h"


void ListFiles(const std::wstring& directory)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // ��������� ������ ��� ������ ������
    std::wstring searchPath = directory + L"\\*";

    // �������� ����� ������
    hFind = FindFirstFile(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return; // ������, ���� �� ������� ������� ����������
    }

    do {
        const std::wstring fileName = findFileData.cFileName;

        // ���������� "." � ".."
        if (fileName != L"." && fileName != L"..") {
            // ���� ��� ����������, ���������� ������� ��
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ListFiles(directory + L"\\" + fileName);
            }
            else {
                // ��������� ���� � �������
                gl::table->AddRecord(fileName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind); // ��������� ���������� ������
};

std::wstring CreateLine(const std::wstring &fileName, int count)
{
    std::wstring line = std::to_wstring(count) + L") " + fileName + L"\r\n";
    return line;
};

void SortFiles(std::vector<std::wstring>& files, int left, int right, bool (*comparator)(const std::wstring&, const std::wstring&)) {
    if (left >= right) return;

    // ����������� �������
    std::wstring mid = files[left + (right - left) / 2];
    int i = left;
    int j = right;

    do {
        // ���������� ��������, ������� ������ ������������
        while (comparator(files[i], mid)) {
            i++;
        }
        // ���������� ��������, ������� ������ ������������
        while (comparator(mid, files[j])) {
            j--;
        }

        // ������ �������� �������
        if (i <= j) {
            Swap(files[i], files[j]);
            i++;
            j--;
        }
    } while (i <= j);

    // ����������� ������ ��� ����� � ������ �����
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
    // ����� ������� ��������� '.' � ������
    size_t dotPos = filePath.find_first_of(L'.');

    std::wstring res = filePath;
    
    // ��������, ���� �� ����������
    if (dotPos != std::wstring::npos)
        // ������� ���������� �� ������ � ������ �����
        res.erase(dotPos);
    return res;
}

bool FontFileExists(const wchar_t* filePath) {
    // ���������, ���������� �� ����
    DWORD fileAttr = GetFileAttributesW(filePath);
    bool res = (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));

    // ���� �� ����������, ���������� ��������
    if (!res)
        return res;

    std::wstring fontPath(filePath);

    // ��������� �� ���������� ttf
    size_t dotPos = fontPath.find_last_of(L'.');
    fontPath = fontPath.substr(dotPos);
    if (fontPath != L".ttf")
        res = false;
    return res;
}

bool LoadCustomFont(const wchar_t* fontFilePath)
{
    DWORD result = AddFontResourceEx(fontFilePath, FR_PRIVATE, 0);
    // result = 0, ���� ��������� ����� �� ������� ���� �� ��� ��������
    if (result != 0) {
        // ��������� ���� � ���������� ������ � ���������� ������
        gl::fontPaths.push_back(fontFilePath);
        return true;
    }
    return false;
}

int CALLBACK EnumFontFamExProc(const LOGFONTA* lpLogFont, const TEXTMETRICA*, DWORD FontType, LPARAM lParam) {
    // �������� ������ ��� ���������� ���� �������
    auto* fontNames = reinterpret_cast<std::set<std::wstring>*>(lParam);

    // ���������, �������� �� ����� TrueType
    if (FontType & TRUETYPE_FONTTYPE) {
        // ����������� ����� ������ �� char � wstring
        std::wstring fontName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(lpLogFont->lfFaceName);
        fontNames->insert(fontName); // ��������� ��� ������ � ���������
    }

    return 1; // ���������� ������������
}

void ListFonts(std::set<std::wstring>& fontNames) {
    // �������� �������� ����������
    HDC hdc = GetDC(NULL);
    if (hdc) {
        // ����������� ������ (�� ����� ������ ��� ��������� ����� ������� ����� � fontNames)
        EnumFontFamiliesExA(hdc, NULL, EnumFontFamExProc, reinterpret_cast<LPARAM>(&fontNames), 0);
        ReleaseDC(NULL, hdc);
    }
}

void RemoveCustomFonts()
{
    // ���������� �� ����������� �������, ������� ������ ���� � ��������� �������, ��� �������� ������������
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
