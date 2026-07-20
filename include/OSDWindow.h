/**
 * @file OSDWindow.h
 * @brief Модуль создания и отображения всплывающего графического индикатора (OSD Overlay).
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <string>
#include <windows.h>

/**
 * @class OSDWindow
 * @brief Управляет полупрозрачным всплывающим окном OSD поверх всех окон для показа уровня громкости и статусов.
 */
class OSDWindow {
private:
    static HWND hwndOSD;                  ///< Дескриптор окна OSD
    static std::wstring currentTitle;      ///< Отображаемый заголовок
    static std::wstring currentStatusText; ///< Отображаемый текстовый статус
    static int currentPercent;            ///< Отображаемый процент (0..100) или -1

public:
    /**
     * @brief Инициализация и регистрация класса окна OSD.
     * @param hInstance Дескриптор экземпляра приложения.
     */
    static void Initialize(HINSTANCE hInstance);

    /**
     * @brief Отобразить плашку OSD на экране.
     * @param title Заголовок действия или имя фейдера.
     * @param percent Процент прогресс-бара (0..100) либо -1 если отображается статус.
     * @param statusText Дополнительная текстовая строка статуса.
     */
    static void Show(const std::wstring& title, int percent, const std::wstring& statusText = L"");

private:
    /**
     * @brief Оконная процедура Win32 для отрисовки OSD.
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
