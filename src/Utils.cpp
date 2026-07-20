/**
 * @file Utils.cpp
 * @brief Реализация служебных утилит конвертации строк UTF-8 в UTF-16 (std::wstring).
 * @author Vane4ka2k2
 * @date 2026
 */

#include "Utils.h"

std::wstring Utf8ToWstring(const std::string& str) {
    if (str.empty()) return L"";
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], count);
    return wstr;
}
