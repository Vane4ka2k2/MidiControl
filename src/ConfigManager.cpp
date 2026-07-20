/**
 * @file ConfigManager.cpp
 * @brief Реализация парсера файла конфигурации config.json.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "ConfigManager.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace {
    enum class JsonType { Null, Bool, Number, String, Array, Object };

    struct JsonValue {
        JsonType type = JsonType::Null;
        bool boolVal = false;
        double numVal = 0.0;
        std::string strVal;
        std::vector<JsonValue> arrVal;
        std::map<std::string, JsonValue> objVal;

        bool has(const std::string& key) const {
            return type == JsonType::Object && objVal.find(key) != objVal.end();
        }

        const JsonValue& get(const std::string& key) const {
            static JsonValue nullVal;
            if (type == JsonType::Object) {
                auto it = objVal.find(key);
                if (it != objVal.end()) return it->second;
            }
            return nullVal;
        }
    };

    class JsonParserHelper {
        std::string src;
        size_t pos = 0;

        void SkipWhitespace() {
            while (pos < src.size() && (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\r' || src[pos] == '\n')) {
                pos++;
            }
        }

        char Peek() {
            SkipWhitespace();
            if (pos >= src.size()) return '\0';
            return src[pos];
        }

        char Get() {
            SkipWhitespace();
            if (pos >= src.size()) return '\0';
            return src[pos++];
        }

    public:
        JsonParserHelper(const std::string& input) : src(input) {}

        JsonValue Parse() {
            SkipWhitespace();
            char c = Peek();
            if (c == '{') return ParseObject();
            if (c == '[') return ParseArray();
            if (c == '"') return ParseString();
            if (c == 't' || c == 'f') return ParseBool();
            if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return ParseNumber();
            return JsonValue();
        }

    private:
        JsonValue ParseObject() {
            JsonValue val;
            val.type = JsonType::Object;
            Get(); // Consume '{'
            SkipWhitespace();
            if (Peek() == '}') { Get(); return val; }

            while (pos < src.size()) {
                SkipWhitespace();
                if (Peek() != '"') break;
                JsonValue keyVal = ParseString();
                SkipWhitespace();
                if (Get() != ':') break;
                JsonValue subVal = Parse();
                val.objVal[keyVal.strVal] = subVal;

                SkipWhitespace();
                char c = Get();
                if (c == '}') break;
                if (c != ',') break;
            }
            return val;
        }

        JsonValue ParseArray() {
            JsonValue val;
            val.type = JsonType::Array;
            Get(); // Consume '['
            SkipWhitespace();
            if (Peek() == ']') { Get(); return val; }

            while (pos < src.size()) {
                val.arrVal.push_back(Parse());
                SkipWhitespace();
                char c = Get();
                if (c == ']') break;
                if (c != ',') break;
            }
            return val;
        }

        JsonValue ParseString() {
            JsonValue val;
            val.type = JsonType::String;
            if (Get() != '"') return val;

            std::string res;
            while (pos < src.size()) {
                char c = src[pos++];
                if (c == '"') break;
                if (c == '\\' && pos < src.size()) {
                    char esc = src[pos++];
                    if (esc == 'n') res += '\n';
                    else if (esc == 't') res += '\t';
                    else if (esc == 'r') res += '\r';
                    else res += esc;
                } else {
                    res += c;
                }
            }
            val.strVal = res;
            return val;
        }

        JsonValue ParseBool() {
            JsonValue val;
            val.type = JsonType::Bool;
            SkipWhitespace();
            if (src.compare(pos, 4, "true") == 0) {
                val.boolVal = true;
                pos += 4;
            } else if (src.compare(pos, 5, "false") == 0) {
                val.boolVal = false;
                pos += 5;
            }
            return val;
        }

        JsonValue ParseNumber() {
            JsonValue val;
            val.type = JsonType::Number;
            SkipWhitespace();
            size_t start = pos;
            if (src[pos] == '-') pos++;
            while (pos < src.size() && (std::isdigit(static_cast<unsigned char>(src[pos])) || src[pos] == '.')) {
                pos++;
            }
            std::string numStr = src.substr(start, pos - start);
            try {
                val.numVal = std::stod(numStr);
            } catch (...) {
                val.numVal = 0;
            }
            return val;
        }
    };
}

std::string ConfigParser::ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void ConfigParser::LoadConfig(const std::string& filepath, AppConfig& config) {
    SetDefaultConfig(config);
    std::string jsonStr = ReadFile(filepath);
    if (jsonStr.empty()) return;

    JsonParserHelper parser(jsonStr);
    JsonValue root = parser.Parse();

    if (root.type != JsonType::Object) return;

    // device_name
    if (root.has("device_name") && root.get("device_name").type == JsonType::String) {
        config.deviceName = root.get("device_name").strVal;
    }

    // show_osd
    if (root.has("show_osd") && root.get("show_osd").type == JsonType::Bool) {
        config.showOsd = root.get("show_osd").boolVal;
    }

    // osd_duration_ms
    if (root.has("osd_duration_ms") && root.get("osd_duration_ms").type == JsonType::Number) {
        config.osdDurationMs = static_cast<int>(root.get("osd_duration_ms").numVal);
    }

    // faders
    if (root.has("faders") && root.get("faders").type == JsonType::Object) {
        const auto& fadersObj = root.get("faders").objVal;
        for (const auto& [faderKey, faderVal] : fadersObj) {
            if (faderVal.type != JsonType::Object) continue;

            int id = 0;
            if (faderKey.rfind("fader_", 0) == 0) {
                try { id = std::stoi(faderKey.substr(6)); } catch (...) {}
            }
            if (id <= 0) continue;

            FaderConfig f = config.faders[id];

            if (faderVal.has("cc_arturia") && faderVal.get("cc_arturia").type == JsonType::Number)
                f.ccArturia = static_cast<BYTE>(faderVal.get("cc_arturia").numVal);
            if (faderVal.has("cc_daw") && faderVal.get("cc_daw").type == JsonType::Number)
                f.ccDaw = static_cast<BYTE>(faderVal.get("cc_daw").numVal);
            if (faderVal.has("type") && faderVal.get("type").type == JsonType::String)
                f.type = faderVal.get("type").strVal;
            if (faderVal.has("label") && faderVal.get("label").type == JsonType::String)
                f.label = faderVal.get("label").strVal;
            if (faderVal.has("apps") && faderVal.get("apps").type == JsonType::Array) {
                f.apps.clear();
                for (const auto& appItem : faderVal.get("apps").arrVal) {
                    if (appItem.type == JsonType::String) {
                        f.apps.push_back(Utf8ToWstring(appItem.strVal));
                    }
                }
            }

            config.faders[id] = f;
        }
    }

    // encoders
    if (root.has("encoders") && root.get("encoders").type == JsonType::Object) {
        const auto& encObj = root.get("encoders").objVal;
        for (const auto& [encKey, encVal] : encObj) {
            if (encVal.type != JsonType::Object) continue;

            int id = 0;
            if (encKey.rfind("encoder_", 0) == 0) {
                try { id = std::stoi(encKey.substr(8)); } catch (...) {}
            }
            if (id <= 0) continue;

            EncoderConfig e = config.encoders[id];

            if (encVal.has("cc") && encVal.get("cc").type == JsonType::Number)
                e.cc = static_cast<BYTE>(encVal.get("cc").numVal);
            if (encVal.has("action") && encVal.get("action").type == JsonType::String)
                e.action = encVal.get("action").strVal;
            if (encVal.has("label") && encVal.get("label").type == JsonType::String)
                e.label = encVal.get("label").strVal;

            config.encoders[id] = e;
        }
    }

    // pads
    if (root.has("pads") && root.get("pads").type == JsonType::Object) {
        const auto& padsObj = root.get("pads").objVal;
        for (const auto& [padKey, padVal] : padsObj) {
            if (padVal.type != JsonType::Object) continue;

            int id = 0;
            if (padKey.rfind("pad_", 0) == 0) {
                try { id = std::stoi(padKey.substr(4)); } catch (...) {}
            }
            if (id <= 0) continue;

            PadConfig p = config.pads[id];

            if (padVal.has("cc") && padVal.get("cc").type == JsonType::Number)
                p.cc = static_cast<BYTE>(padVal.get("cc").numVal);
            if (padVal.has("note") && padVal.get("note").type == JsonType::Number)
                p.note = static_cast<BYTE>(padVal.get("note").numVal);
            if (padVal.has("action") && padVal.get("action").type == JsonType::String)
                p.action = padVal.get("action").strVal;
            if (padVal.has("label") && padVal.get("label").type == JsonType::String)
                p.label = padVal.get("label").strVal;

            config.pads[id] = p;
        }
    }
}

void ConfigParser::SetDefaultConfig(AppConfig& config) {
    config.deviceName = "Minilab";
    config.showOsd = true;
    config.osdDurationMs = 1000;
    config.faders.clear();
    config.encoders.clear();
    config.pads.clear();

    // Fader 1: Общая громкость
    FaderConfig f1;
    f1.ccArturia = 82; f1.ccDaw = 14;
    f1.type = "master_volume"; f1.label = "Общая громкость";
    config.faders[1] = f1;

    // Энкодеры
    EncoderConfig e1; e1.cc = 74; e1.action = "seek_media"; e1.label = "Перемотка трека"; config.encoders[1] = e1;
    EncoderConfig e2; e2.cc = 71; e2.action = "zoom_browser"; e2.label = "Масштаб страницы"; config.encoders[2] = e2;

    // Пэды
    PadConfig p1; p1.cc = 102; p1.note = 36; p1.action = "master_mute"; p1.label = "Отключить звук"; config.pads[1] = p1;
    PadConfig p2; p2.cc = 103; p2.note = 37; p2.action = "smart_ducking"; p2.label = "Приглушение"; config.pads[2] = p2;
    PadConfig p3; p3.cc = 104; p3.note = 38; p3.action = "media_prev"; p3.label = "Предыдущий трек"; config.pads[3] = p3;
    PadConfig p4; p4.cc = 105; p4.note = 39; p4.action = "media_play_pause"; p4.label = "Пауза / Воспроизведение"; config.pads[4] = p4;
    PadConfig p5; p5.cc = 106; p5.note = 40; p5.action = "media_next"; p5.label = "Следующий трек"; config.pads[5] = p5;
    PadConfig p6; p6.cc = 107; p6.note = 41; p6.action = "launch_telegram"; p6.label = "Запуск Telegram"; config.pads[6] = p6;
    PadConfig p7; p7.cc = 108; p7.note = 42; p7.action = "show_desktop"; p7.label = "Рабочий стол (Win+D)"; config.pads[7] = p7;
    PadConfig p8; p8.cc = 109; p8.note = 43; p8.action = "snipping_tool"; p8.label = "Скриншот"; config.pads[8] = p8;
}
