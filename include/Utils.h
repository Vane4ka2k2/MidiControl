/**
 * @file Utils.h
 * @brief Вспомогательные утилиты конвертации строк и кодировок для Windows API.
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <string>
#include <windows.h>

/**
 * @brief Преобразует строку в кодировке UTF-8 (std::string) в системную строку UTF-16 (std::wstring).
 * @param str Входная строка в кодировке UTF-8.
 * @return Результирующая широкая строка std::wstring для Win32 API.
 */
std::wstring Utf8ToWstring(const std::string& str);
