#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string ConfigParser::ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void ConfigParser::LoadConfig(const std::string& filepath, AppConfig& config) {
    std::string json = ReadFile(filepath);
    SetDefaultConfig(config);

    if (!json.empty() && json.find("\"show_osd\": false") != std::string::npos) {
        config.showOsd = false;
    }
}

void ConfigParser::SetDefaultConfig(AppConfig& config) {
    config.deviceName = "Minilab";
    config.showOsd = true;
    config.osdDurationMs = 1500;

    // Fader 1: Общая громкость
    FaderConfig f1;
    f1.ccArturia = 82; f1.ccDaw = 14;
    f1.type = "master_volume"; f1.label = "Общая громкость";
    config.faders[1] = f1;

    // Fader 2: Браузер
    FaderConfig f2;
    f2.ccArturia = 83; f2.ccDaw = 15;
    f2.type = "app_volume"; f2.label = "Браузер";
    f2.apps = { L"chrome.exe", L"msedge.exe", L"firefox.exe", L"browser.exe", L"opera.exe" };
    config.faders[2] = f2;

    // Fader 3: Голос / Дискорд
    FaderConfig f3;
    f3.ccArturia = 85; f3.ccDaw = 30;
    f3.type = "app_volume"; f3.label = "Голос / Дискорд";
    f3.apps = { L"discord.exe", L"telegram.exe", L"teams.exe", L"skype.exe" };
    config.faders[3] = f3;

    // Fader 4: Медиа / Музыка
    FaderConfig f4;
    f4.ccArturia = 17; f4.ccDaw = 31;
    f4.type = "app_volume"; f4.label = "Медиа / Музыка";
    f4.apps = { L"spotify.exe", L"yandexmusic.exe", L"vlc.exe", L"foobar2000.exe", L"wmplayer.exe" };
    config.faders[4] = f4;

    // Пэды
    PadConfig p1; p1.cc = 102; p1.note = 36; p1.action = "toggle_mic_mute"; p1.label = "Микрофон"; config.pads[1] = p1;
    PadConfig p2; p2.cc = 103; p2.note = 37; p2.action = "toggle_master_mute"; p2.label = "Общий звук"; config.pads[2] = p2;
    PadConfig p3; p3.cc = 104; p3.note = 38; p3.action = "media_play_pause"; p3.label = "Пауза / Воспроизведение"; config.pads[3] = p3;
    PadConfig p4; p4.cc = 105; p4.note = 39; p4.action = "media_next"; p4.label = "Следующий трек"; config.pads[4] = p4;
    PadConfig p5; p5.cc = 106; p5.note = 40; p5.action = "media_prev"; p5.label = "Предыдущий трек"; config.pads[5] = p5;
    PadConfig p6; p6.cc = 107; p6.note = 41; p6.action = "smart_ducking"; p6.label = "Приглушение"; config.pads[6] = p6;
    PadConfig p7; p7.cc = 108; p7.note = 42; p7.action = "snipping_tool"; p7.label = "Скриншот"; config.pads[7] = p7;
    PadConfig p8; p8.cc = 109; p8.note = 43; p8.action = "toggle_mic_mute"; p8.label = "Микрофон (Alt)"; config.pads[8] = p8;
}
