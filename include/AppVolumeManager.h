/**
 * @file AppVolumeManager.h
 * @brief Модуль взаимодействия с Windows WASAPI Core Audio API для управления общей громкостью и громкостью процессов.
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>

/**
 * @class AppVolumeManager
 * @brief Класс для работы с аудиосессиями и регулировки уровня громкости Windows.
 */
class AppVolumeManager {
private:
    IMMDeviceEnumerator* pEnumerator = nullptr;       ///< COM-указатель перечислителя устройств
    IMMDevice* pDevice = nullptr;                     ///< COM-указатель аудиоустройства вывода по умолчанию
    IAudioEndpointVolume* pMasterVolume = nullptr;     ///< COM-интерфейс мастер-громкости
    IAudioSessionManager2* pSessionManager = nullptr; ///< COM-интерфейс менеджера аудиосессий приложений
    bool initialized = false;                         ///< Флаг успешной инициализации COM

public:
    /**
     * @brief Конструктор. Инициализирует COM подсистему и службы WASAPI.
     */
    AppVolumeManager();

    /**
     * @brief Деструктор. Освобождает COM-указатели.
     */
    ~AppVolumeManager();

    /**
     * @brief Проверка успешности инициализации WASAPI.
     * @return true если WASAPI подсистема готова к работе.
     */
    bool IsInitialized() const { return initialized; }

    /**
     * @brief Установка общей мастер-громкости Windows.
     * @param scalar Нормированное значение громкости от 0.0f до 1.0f.
     */
    void SetMasterVolume(float scalar);

    /**
     * @brief Получение текущего уровня общей мастер-громкости.
     * @return Значение громкости от 0.0f до 1.0f.
     */
    float GetMasterVolume();

    /**
     * @brief Установка громкости для группы целевых процессов/приложений.
     * @param targetApps Вектор имён исполняемых файлов (напр. L"chrome.exe").
     * @param scalar Нормированное значение громкости от 0.0f до 1.0f.
     */
    void SetAppGroupVolume(const std::vector<std::wstring>& targetApps, float scalar);
};
