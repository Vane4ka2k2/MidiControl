/**
 * @file AudioDeviceManager.h
 * @brief Модуль переключения устройств воспроизведения звука в Windows по умолчанию.
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <string>
#include <vector>

/**
 * @struct AudioOutputDevice
 * @brief Структурное описание подключённого звукового устройства вывода.
 */
struct AudioOutputDevice {
    std::wstring id;   ///< Системный идентификатор устройства Endpoint ID
    std::wstring name; ///< Название устройства (Friendly Name)
};

/**
 * @class AudioDeviceManager
 * @brief Класс для получения списка аудиоустройств и переключения дефолтного аудиовыхода через IPolicyConfig.
 */
class AudioDeviceManager {
private:
    IMMDeviceEnumerator* pEnumerator = nullptr; ///< COM перечислитель аудиоустройств
    bool initialized = false;                   ///< Флаг успешной инициализации

public:
    /**
     * @brief Конструктор AudioDeviceManager.
     */
    AudioDeviceManager();

    /**
     * @brief Деструктор.
     */
    ~AudioDeviceManager();

    /**
     * @brief Перечисление всех активных аудиоустройств воспроизведения в системе.
     * @return Вектор устройств AudioOutputDevice.
     */
    std::vector<AudioOutputDevice> GetActiveOutputDevices();

    /**
     * @brief Переключение на следующее доступное устройство вывода звука по кругу.
     * @return Название вновь выбранного аудиоустройства.
     */
    std::wstring CycleOutputDevice();
};
