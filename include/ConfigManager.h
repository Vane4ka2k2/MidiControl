/**
 * @file ConfigManager.h
 * @brief Модуль структуры конфигурации и загрузки настроек из файла config.json.
 * @author Vane4ka2k2
 * @date 2026
 */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <windows.h>

/**
 * @struct FaderConfig
 * @brief Структура конфигурации отдельного фейдера.
 */
struct FaderConfig {
    BYTE ccArturia = 0;             ///< Номер CC контроллера в режиме Arturia/User
    BYTE ccDaw = 0;                 ///< Номер CC контроллера в режиме DAWs
    std::string type;               ///< Тип управления ("master_volume", "app_volume")
    std::string label;              ///< Текстовая метка для OSD индикатора
    std::vector<std::wstring> apps; ///< Список имен исполняемых файлов целевых приложений
};

/**
 * @struct PadConfig
 * @brief Структура конфигурации отдельного пэда.
 */
struct PadConfig {
    BYTE cc = 0;                    ///< Номер CC контроллера
    BYTE note = 0;                  ///< Номер MIDI-ноты (Note On)
    std::string action;             ///< Идентификатор действия (напр., "cycle_audio_device", "show_desktop")
    std::string label;              ///< Текстовое описание действия
};

/**
 * @struct EncoderConfig
 * @brief Структура конфигурации отдельного энкодера.
 */
struct EncoderConfig {
    BYTE cc = 0;        ///< Номер CC контроллера
    std::string action; ///< Идентификатор действия ("focused_app_volume", "seek_media", "zoom_browser")
    std::string label;  ///< Текстовое описание действия
};

/**
 * @struct AppConfig
 * @brief Главная структура настроек приложения.
 */
struct AppConfig {
    std::string deviceName = "Minilab";    ///< Подстрока поиска MIDI-устройства
    bool showOsd = true;                  ///< Включить выплывающее OSD-уведомление
    int osdDurationMs = 1500;              ///< Длительность показа OSD плашки в мс
    std::map<int, FaderConfig> faders;     ///< Карта конфигураций фейдеров (Ключ 1..4)
    std::map<int, EncoderConfig> encoders; ///< Карта конфигураций энкодеров (Ключ 1..3)
    std::map<int, PadConfig> pads;         ///< Карта конфигураций пэдов (Ключ 1..8)
};

/**
 * @class ConfigParser
 * @brief Класс чтения и парсинга файла конфигурации config.json.
 */
class ConfigParser {
public:
    /**
     * @brief Чтение текстового содержимого файла.
     * @param filepath Путь к файлу.
     * @return Содержимое файла в виде строки.
     */
    static std::string ReadFile(const std::string& filepath);

    /**
     * @brief Загрузка конфигурации из JSON-файла.
     * @param filepath Путь к config.json.
     * @param config Результирующая структура параметров AppConfig.
     */
    static void LoadConfig(const std::string& filepath, AppConfig& config);

private:
    /**
     * @brief Заполнение конфигурации параметрами по умолчанию.
     * @param config Структура для заполнения.
     */
    static void SetDefaultConfig(AppConfig& config);
};
