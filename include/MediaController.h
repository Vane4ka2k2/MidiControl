/**
 * @file MediaController.h
 * @brief Модуль эмуляции медиа-клавиш, макросов и вызова горячих системных комбинаций.
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <windows.h>

/**
 * @class MediaController
 * @brief Набор статических методов отправки виртуальных клавиш в Windows API.
 */
class MediaController {
public:
    /** @brief Переключение Пауза / Воспроизведение (VK_MEDIA_PLAY_PAUSE). */
    static void PlayPause();

    /** @brief Переход к следующему треку (VK_MEDIA_NEXT_TRACK). */
    static void NextTrack();

    /** @brief Переход к предыдущему треку (VK_MEDIA_PREV_TRACK). */
    static void PrevTrack();

    /** @brief Переключение отключения звука Master Mute (VK_VOLUME_MUTE). */
    static void MasterMute();

    /** @brief Запуск приложения скриншота экрана Snipping Tool (Win + Shift + S). */
    static void SnippingTool();

    /** @brief Показать рабочий стол / Свернуть все окна (Win + D). */
    static void ShowDesktop();

    /** @brief Запуск мессенджера Telegram по URI протоколу (tg://). */
    static void LaunchTelegram();

    /** @brief Перемотка медиафайла или видео вперед (Стрелка вправо). */
    static void SeekForward();

    /** @brief Перемотка медиафайла или видео назад (Стрелка влево). */
    static void SeekBackward();

    /** @brief Масштабирование веб-страницы вперед / Увеличение (Ctrl + Plus). */
    static void ZoomIn();

    /** @brief Масштабирование веб-страницы назад / Уменьшение (Ctrl + Minus). */
    static void ZoomOut();
};
