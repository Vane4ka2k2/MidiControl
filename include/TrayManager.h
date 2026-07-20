/**
 * @file TrayManager.h
 * @brief Модуль интеграции с системным треем Windows (System Tray Notification Area).
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <windows.h>
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)      ///< Пользовательское событие иконки трея
#define ID_TRAY_SHOW_MAPPING 1000      ///< ID меню: Показать карту функций
#define ID_TRAY_SHOW_CONSOLE 1001      ///< ID меню: Показать/скрыть консоль отладки
#define ID_TRAY_AUTOSTART    1003      ///< ID меню: Включить/выключить автозапуск
#define ID_TRAY_EXIT         1004      ///< ID меню: Завершение работы программы

/**
 * @class TrayManager
 * @brief Класс управления фоновой иконкой трея и вызова контекстного меню по правому клику.
 */
class TrayManager {
private:
    static HWND hwndTray;        ///< Невидимое утилитарное окно обработки сообщений трея
    static NOTIFYICONDATAW nid;  ///< Структура данных иконки Shell_NotifyIcon

public:
    /**
     * @brief Добавление иконки в системный трей Windows.
     * @param hInstance Дескриптор приложения.
     */
    static void Initialize(HINSTANCE hInstance);

    /**
     * @brief Удаление иконки из трея при закрытии приложения.
     */
    static void Remove();

private:
    /**
     * @brief Обработчик Win32 сообщений клика по иконке трея.
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Проверка наличия записи автозапуска приложения в реестре Windows HKCU.
     * @return true если автозапуск включен.
     */
    static bool IsAutostartEnabled();

    /**
     * @brief Включение или выключение записи автозапуска в реестре Windows.
     */
    static void ToggleAutostart();
};
