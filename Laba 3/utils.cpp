#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>




std::wstring millisecondsToTimeString(long long milliseconds) {
    long long hours = milliseconds / 3600000;
    milliseconds %= 3600000;
    long long minutes = milliseconds / 60000;
    milliseconds %= 60000;
    long long seconds = milliseconds / 1000;
    long long millis = milliseconds % 1000;

    std::wstringstream result;
    result << std::setw(2) << std::setfill(L'0') << hours << L':'
        << std::setw(2) << std::setfill(L'0') << minutes << L':'
        << std::setw(2) << std::setfill(L'0') << seconds << L':'
        << std::setw(3) << std::setfill(L'0') << millis;

    return result.str();
}

void PrintMessage(HWND hwnd, std::wstring event, int columnNum, int rowNum, DWORD ellapsedTime) {
    if (rowNum < 0) {
        MessageBox(hwnd, (L"Событие: нажатие на Заголовок " L"\nНомер столбца: " + std::to_wstring(columnNum) + L"\nВремя возникновения: " + millisecondsToTimeString(ellapsedTime)).c_str(), L"", MB_OK);
    }
    else {
        MessageBox(hwnd, (L"Событие: " + event + L"\nНомер столбца: " + std::to_wstring(columnNum) + L"\nНомер ячейки: " + std::to_wstring(rowNum) + L"\nВремя возникновения: " + millisecondsToTimeString(ellapsedTime)).c_str(), L"", MB_OK);

    }
};

