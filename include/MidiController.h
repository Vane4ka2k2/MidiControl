/**
 * @file MidiController.h
 * @brief Главный модуль подключения к MIDI-устройству ввода и обработки событий в реальном времени.
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <windows.h>
#include <mmeapi.h>
#include <string>
#include "ConfigManager.h"
#include "AppVolumeManager.h"

/**
 * @class MidiController
 * @brief Класс прослушивания входящих MIDI-сигналов и распределения команд на соответствующие модули.
 */
class MidiController {
private:
    HMIDIIN hMidiIn = nullptr;      ///< Дескриптор открытого MIDI-входа Windows Multimedia
    int targetDeviceIndex = -1;     ///< Индекс обнаруженного MIDI-устройства Arturia Minilab

public:
    static AppConfig g_Config;           ///< Глобальный объект конфигурации
    static AppVolumeManager g_AppVolume; ///< Глобальный объект управления громкостью WASAPI

    /**
     * @brief Инициализация и автопоиск устройства Arturia Minilab.
     * @param configPath Путь к файлу config.json.
     * @return true если найдено подходящее MIDI-устройство.
     */
    bool Initialize(const std::string& configPath);

    /**
     * @brief Запуск прослушивания входящих событий MIDI.
     */
    void Start();

    /**
     * @brief Остановка прослушивания и закрытие хэндла MIDI.
     */
    void Stop();

private:
    /**
     * @brief Callback-функция обработки входящих сообщений winmm (MIM_DATA).
     */
    static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

    /**
     * @brief Выполнение действия, назначенного на пэд.
     * @param pad Конфигурация нажатого пэда.
     */
    static void ExecutePadAction(const PadConfig& pad);

    /**
     * @brief Установка громкости для текущего фокусного/активного окна на столе.
     * @param scalar Нормированное значение громкости от 0.0f до 1.0f.
     */
    static void SetFocusedAppVolume(float scalar);

    /**
     * @brief Обработка вращения Энкодера 2 (Перемотка трека).
     * @param ccValue Входящее значение CC контроллера.
     */
    static void HandleEncoder2(BYTE ccValue);

    /**
     * @brief Обработка вращения Энкодера 3 (Масштабирование браузера).
     * @param ccValue Входящее значение CC контроллера.
     */
    static void HandleEncoder3(BYTE ccValue);
};
