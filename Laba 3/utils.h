#pragma once

// �������������� ��������. � ������ �:�:�:���
std::wstring millisecondsToTimeString(long long milliseconds); 

// ���������� ���������� � ����������� � ������ ��� ������� �� ������
void PrintMessage(HWND hwnd, std::wstring event, int columnNum, int rowNum, DWORD ellapsedTime); 


