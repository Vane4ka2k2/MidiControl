/**
 * @file MidiController.cpp
 * @brief Реализация приёма MIDI-сигналов (winmm), маппинга органов управления и диспетчеризации действий.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "MidiController.h"
#include "MediaController.h"
#include "OSDWindow.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <mutex>
#include <psapi.h>

AppConfig MidiController::g_Config;
AppVolumeManager MidiController::g_AppVolume;

namespace {
    static std::mutex g_StateMutex;
    static bool g_SmartDuckingActive = false;
    static float g_PreDuckingVolume = 0.5f;
    static BYTE g_LastEncoder2Val = 64;
    static BYTE g_LastEncoder3Val = 64;
    static bool g_PadState[128] = { false };
}

bool MidiController::Initialize(const std::string& configPath) {
    ConfigParser::LoadConfig(configPath, g_Config);

    UINT numDevs = midiInGetNumDevs();
    if (numDevs == 0) {
        std::cerr << "[Ошибка] Не найдено ни одного MIDI-устройства ввода.\n";
        return false;
    }

    targetDeviceIndex = -1;
    std::wstring searchName = Utf8ToWstring(g_Config.deviceName);
    std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

    for (UINT i = 0; i < numDevs; ++i) {
        MIDIINCAPSW caps;
        if (midiInGetDevCapsW(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            std::wcout << L" [" << i << L"] " << caps.szPname << L"\n";
            std::wstring devName(caps.szPname);
            std::wstring devNameLower = devName;
            std::transform(devNameLower.begin(), devNameLower.end(), devNameLower.begin(), ::tolower);

            if (targetDeviceIndex == -1 && 
               (devNameLower.find(searchName) != std::wstring::npos || 
                devNameLower.find(L"minilab") != std::wstring::npos ||
                devNameLower.find(L"arturia") != std::wstring::npos)) {
                targetDeviceIndex = static_cast<int>(i);
            }
        }
    }

    if (targetDeviceIndex == -1 && numDevs > 0) targetDeviceIndex = 0;
    return targetDeviceIndex != -1;
}

void MidiController::Start() {
    if (targetDeviceIndex == -1) return;

    MMRESULT result = midiInOpen(
        &hMidiIn,
        targetDeviceIndex,
        reinterpret_cast<DWORD_PTR>(MidiInProc),
        0,
        CALLBACK_FUNCTION
    );

    if (result == MMSYSERR_NOERROR) {
        midiInStart(hMidiIn);
        std::cout << "\n[Успех] Прослушивание MIDI запущенно на устройстве [" << targetDeviceIndex << "]!\n";
    }
}

void MidiController::Stop() {
    if (hMidiIn) {
        midiInStop(hMidiIn);
        midiInClose(hMidiIn);
        hMidiIn = nullptr;
    }
}

void MidiController::ExecutePadAction(const PadConfig& pad) {
    std::wstring label = Utf8ToWstring(pad.label);

    if (pad.action == "master_mute") {
        MediaController::MasterMute();
        OSDWindow::Show(Utf8ToWstring("Звук"), -1, Utf8ToWstring("MUTE TOGGLE"));
    } else if (pad.action == "show_desktop") {
        MediaController::ShowDesktop();
        OSDWindow::Show(Utf8ToWstring("Рабочий стол"), -1, Utf8ToWstring("WIN + D"));
    } else if (pad.action == "launch_telegram") {
        MediaController::LaunchTelegram();
        OSDWindow::Show(Utf8ToWstring("Запуск программы"), -1, Utf8ToWstring("TELEGRAM"));
    } else if (pad.action == "media_play_pause") {
        MediaController::PlayPause();
        OSDWindow::Show(Utf8ToWstring("Медиаплеер"), -1, Utf8ToWstring("PLAY / PAUSE"));
    } else if (pad.action == "media_next") {
        MediaController::NextTrack();
        OSDWindow::Show(Utf8ToWstring("Медиаплеер"), -1, Utf8ToWstring("СЛЕДУЮЩИЙ ТРЕК"));
    } else if (pad.action == "media_prev") {
        MediaController::PrevTrack();
        OSDWindow::Show(Utf8ToWstring("Медиаплеер"), -1, Utf8ToWstring("ПРЕДЫДУЩИЙ ТРЕК"));
    } else if (pad.action == "smart_ducking") {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (!g_SmartDuckingActive) {
            g_PreDuckingVolume = g_AppVolume.GetMasterVolume();
            g_AppVolume.SetMasterVolume(0.1f);
            g_SmartDuckingActive = true;
            OSDWindow::Show(Utf8ToWstring("Приглушение"), -1, Utf8ToWstring("АКТИВИРОВАНО (10%)"));
        } else {
            g_AppVolume.SetMasterVolume(g_PreDuckingVolume);
            g_SmartDuckingActive = false;
            OSDWindow::Show(Utf8ToWstring("Приглушение"), -1, Utf8ToWstring("ОТКЛЮЧЕНО"));
        }
    } else if (pad.action == "snipping_tool") {
        MediaController::SnippingTool();
        OSDWindow::Show(Utf8ToWstring("Скриншот"), -1, Utf8ToWstring("ВЫБОР ОБЛАСТИ"));
    }
}

void MidiController::SetFocusedAppVolume(float scalar) {
    HWND hwndFocused = GetForegroundWindow();
    if (!hwndFocused) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwndFocused, &pid);
    if (pid == 0) return;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return;

    WCHAR szProcessName[MAX_PATH] = { 0 };
    if (GetModuleBaseNameW(hProcess, NULL, szProcessName, MAX_PATH) > 0) {
        std::wstring exeName(szProcessName);
        std::vector<std::wstring> singleApp = { exeName };
        g_AppVolume.SetAppGroupVolume(singleApp, scalar);
        
        std::wstring title = Utf8ToWstring("Активное окно (") + exeName + L")";
        int percent = static_cast<int>(scalar * 100.0f + 0.5f);
        OSDWindow::Show(title, percent);
    }
    CloseHandle(hProcess);
}

void MidiController::HandleEncoder2(BYTE ccValue) {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (ccValue > g_LastEncoder2Val) {
        MediaController::SeekForward();
        OSDWindow::Show(Utf8ToWstring("Перемотка"), -1, Utf8ToWstring("ВПЕРЕД ⏩"));
    } else if (ccValue < g_LastEncoder2Val) {
        MediaController::SeekBackward();
        OSDWindow::Show(Utf8ToWstring("Перемотка"), -1, Utf8ToWstring("НАЗАД ⏪"));
    }
    g_LastEncoder2Val = ccValue;
}

void MidiController::HandleEncoder3(BYTE ccValue) {
    std::lock_guard<std::mutex> lock(g_StateMutex);
    if (ccValue > g_LastEncoder3Val) {
        MediaController::ZoomIn();
        OSDWindow::Show(Utf8ToWstring("Масштаб"), -1, Utf8ToWstring("УВЕЛИЧИТЬ 🔍+"));
    } else if (ccValue < g_LastEncoder3Val) {
        MediaController::ZoomOut();
        OSDWindow::Show(Utf8ToWstring("Масштаб"), -1, Utf8ToWstring("УМЕНЬШИТЬ 🔍-"));
    }
    g_LastEncoder3Val = ccValue;
}

void CALLBACK MidiController::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA) {
        BYTE status = LOBYTE(LOWORD(dwParam1));
        BYTE data1  = HIBYTE(LOWORD(dwParam1)); // Номер контроллера (CC) или ноты
        BYTE data2  = LOBYTE(HIWORD(dwParam1)); // Значение (0-127) или Velocity / Pressure

        BYTE messageType = status & 0xF0;
        BYTE channel     = (status & 0x0F) + 1;

        // 1. Обработка Control Change (0xB0)
        if (messageType == 0xB0) {
            BYTE ccNumber = data1;
            BYTE ccValue  = data2;
            float scalar  = static_cast<float>(ccValue) / 127.0f;
            int percent   = static_cast<int>(scalar * 100.0f + 0.5f);

            // Энкодеры
            for (const auto& [id, encoder] : g_Config.encoders) {
                if (ccNumber == encoder.cc) {
                    if (encoder.action == "focused_app_volume") {
                        SetFocusedAppVolume(scalar);
                    } else if (encoder.action == "seek_media") {
                        HandleEncoder2(ccValue);
                    } else if (encoder.action == "zoom_browser") {
                        HandleEncoder3(ccValue);
                    }
                    return;
                }
            }

            // Фейдеры (Мастер-громкость)
            for (const auto& [id, fader] : g_Config.faders) {
                if (ccNumber == fader.ccArturia || ccNumber == fader.ccDaw) {
                    std::wstring label = Utf8ToWstring(fader.label);
                    g_AppVolume.SetMasterVolume(scalar);
                    OSDWindow::Show(label, percent);
                    std::cout << "[Громкость] " << fader.label << " -> " << percent << "%\n";
                    return;
                }
            }

            // Пэды по CC
            for (const auto& [id, pad] : g_Config.pads) {
                if (ccNumber == pad.cc && ccValue > 0) {
                    ExecutePadAction(pad);
                    return;
                }
            }

            std::cout << "[MIDI CC] Канал " << static_cast<int>(channel)
                      << " | CC: " << static_cast<int>(ccNumber)
                      << " | Значение: " << static_cast<int>(ccValue) << "\n";
        }
        // 2. Обработка Note On (0x90) и Polyphonic Key Pressure / Aftertouch (0xA0) — Пэды
        else if (messageType == 0x90 || messageType == 0xA0) {
            BYTE noteNumber = data1;
            BYTE val        = data2;

            if (val > 0) {
                bool shouldTrigger = false;
                {
                    std::lock_guard<std::mutex> lock(g_StateMutex);
                    if (!g_PadState[noteNumber]) {
                        g_PadState[noteNumber] = true;
                        shouldTrigger = true;
                    }
                }

                if (shouldTrigger) {
                    for (const auto& [id, pad] : g_Config.pads) {
                        if (noteNumber == pad.note || noteNumber == pad.cc || (noteNumber >= 36 && noteNumber <= 43 && (noteNumber - 35) == id)) {
                            ExecutePadAction(pad);
                            return;
                        }
                    }

                    std::cout << "[MIDI Pad Triggered] Канал " << static_cast<int>(channel)
                              << " | Нота: " << static_cast<int>(noteNumber)
                              << " | Значение: " << static_cast<int>(val) << "\n";
                }
            } else {
                std::lock_guard<std::mutex> lock(g_StateMutex);
                g_PadState[noteNumber] = false;
            }
        }
        else if (messageType == 0x80) { // Note Off (0x80)
            BYTE noteNumber = data1;
            std::lock_guard<std::mutex> lock(g_StateMutex);
            g_PadState[noteNumber] = false;
        }
        else {
            std::cout << "[MIDI Event] Тип: 0x" << std::hex << static_cast<int>(messageType)
                      << " | Data1: " << std::dec << static_cast<int>(data1)
                      << " | Data2: " << static_cast<int>(data2) << "\n";
        }
    }
}
