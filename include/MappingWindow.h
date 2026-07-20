/**
 * @file MappingWindow.h
 * @brief Модуль создания окна графической шпаргалки (карты функций клавиш).
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <windows.h>

/**
 * @class MappingWindow
 * @brief Управляет окном справочника назначения контроллеров Arturia Minilab 3.
 */
class MappingWindow {
private:
    static HWND hwndMapping; ///< Дескриптор окна карты функций

public:
    /**
     * @brief Инициализация и создание окна справочника.
     * @param hInstance Дескриптор приложения.
     */
    static void Initialize(HINSTANCE hInstance);

    /**
     * @brief Отобразить окно шпаргалки поверх остальных окон.
     */
    static void Show();

private:
    /**
     * @brief Оконная процедура Win32 для отрисовки текстовой карты элементов.
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
