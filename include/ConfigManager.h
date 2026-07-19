#pragma once
#include <string>
#include <vector>
#include <map>
#include <windows.h>

struct FaderConfig {
    BYTE ccArturia = 0;
    BYTE ccDaw = 0;
    std::string type;
    std::string label;
    std::vector<std::wstring> apps;
};

struct PadConfig {
    BYTE cc = 0;
    BYTE note = 0;
    std::string action;
    std::string label;
};

struct AppConfig {
    std::string deviceName = "Minilab";
    bool showOsd = true;
    int osdDurationMs = 1500;
    std::map<int, FaderConfig> faders;
    std::map<int, PadConfig> pads;
};

class ConfigParser {
public:
    static std::string ReadFile(const std::string& filepath);
    static void LoadConfig(const std::string& filepath, AppConfig& config);

private:
    static void SetDefaultConfig(AppConfig& config);
};
