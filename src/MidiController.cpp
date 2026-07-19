#include "MidiController.h"
#include "MediaController.h"
#include "OSDWindow.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <psapi.h>

AppConfig MidiController::g_Config;
AppVolumeManager MidiController::g_AppVolume;
MicrophoneManager MidiController::g_MicManager;

static bool g_SmartDuckingActive = false;
static float g_PreDuckingVolume = 0.5f;

bool MidiController::Initialize(const std::string& configPath) {
    ConfigParser::LoadConfig(configPath, g_Config);

    UINT numDevs = midiInGetNumDevs();
    if (numDevs == 0) {
        std::cerr << "[Ошибка] Не найдено ни одного MIDI-устройства ввода.\n";
        return false;
    }

    targetDeviceIndex = -1;
    for (UINT i = 0; i < numDevs; ++i) {
        MIDIINCAPSW caps;
        if (midiInGetDevCapsW(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            std::wcout << L" [" << i << L"] " << caps.szPname << L"\n";
            std::wstring devName(caps.szPname);

            if (targetDeviceIndex == -1 && 
               (devName.find(L"Minilab") != std::wstring::npos || 
                devName.find(L"MINILAB") != std::wstring::npos ||
                devName.find(L"Arturia") != std::wstring::npos ||
                devName.find(L"ARTURIA") != std::wstring::npos)) {
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

    if (pad.action == "toggle_mic_mute") {
        bool isMuted = g_MicManager.ToggleMute();
        std::wstring statusStr = isMuted ? Utf8ToWstring("ВЫКЛЮЧЕН") : Utf8ToWstring("ВКЛЮЧЕН");
        OSDWindow::Show(Utf8ToWstring("Микрофон"), -1, statusStr);
        std::wcout << L"[Пэд] Микрофон: " << statusStr << L"\n";
    } else if (pad.action == "toggle_master_mute") {
        MediaController::MasterMute();
        OSDWindow::Show(Utf8ToWstring("Общий звук Mute"), -1, Utf8ToWstring("ПЕРЕКЛЮЧЕНО"));
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
        if (!g_SmartDuckingActive) {
            g_PreDuckingVolume = g_AppVolume.GetMasterVolume();
            g_AppVolume.SetMasterVolume(0.15f);
            g_SmartDuckingActive = true;
            OSDWindow::Show(Utf8ToWstring("Приглушение"), -1, Utf8ToWstring("АКТИВИРОВАНО (15%)"));
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

void CALLBACK MidiController::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (wMsg == MIM_DATA) {
        BYTE status = LOBYTE(LOWORD(dwParam1));
        BYTE data1  = HIBYTE(LOWORD(dwParam1)); // Номер контроллера (CC) или ноты
        BYTE data2  = LOBYTE(HIWORD(dwParam1)); // Значение (0-127) или Velocity

        BYTE messageType = status & 0xF0;
        BYTE channel     = (status & 0x0F) + 1;

        // 1. Обработка Control Change (0xB0)
        if (messageType == 0xB0) {
            BYTE ccNumber = data1;
            BYTE ccValue  = data2;
            float scalar  = static_cast<float>(ccValue) / 127.0f;
            int percent   = static_cast<int>(scalar * 100.0f + 0.5f);

            // Проверяем Энкодер 1 (CC 74) -> Громкость активного окна
            if (ccNumber == 74) {
                SetFocusedAppVolume(scalar);
                return;
            }

            // Проверяем фейдеры (Faders 1..4)
            for (const auto& [id, fader] : g_Config.faders) {
                if (ccNumber == fader.ccArturia || ccNumber == fader.ccDaw) {
                    std::wstring label = Utf8ToWstring(fader.label);

                    if (fader.type == "master_volume") {
                        g_AppVolume.SetMasterVolume(scalar);
                        OSDWindow::Show(label, percent);
                        std::cout << "[Громкость] " << fader.label << " -> " << percent << "%\n";
                    } else if (fader.type == "app_volume") {
                        g_AppVolume.SetAppGroupVolume(fader.apps, scalar);
                        OSDWindow::Show(label, percent);
                        std::cout << "[Громкость] " << fader.label << " -> " << percent << "%\n";
                    }
                    return;
                }
            }

            // Проверяем пэды по CC
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
        // 2. Обработка Note On (0x90)
        else if (messageType == 0x90) {
            BYTE noteNumber = data1;
            BYTE velocity   = data2;

            if (velocity > 0) { // Нажатие пэда
                for (const auto& [id, pad] : g_Config.pads) {
                    if (noteNumber == pad.note || noteNumber == pad.cc || (noteNumber >= 36 && noteNumber <= 43 && (noteNumber - 35) == id)) {
                        ExecutePadAction(pad);
                        return;
                    }
                }

                std::cout << "[MIDI Note ON] Канал " << static_cast<int>(channel)
                          << " | Нота: " << static_cast<int>(noteNumber)
                          << " | Сила (Velocity): " << static_cast<int>(velocity) << "\n";
            }
        }
        else if (messageType == 0x80) {
            // Игнорируем отпускание пэда
        }
        else {
            std::cout << "[MIDI Event] Тип: 0x" << std::hex << static_cast<int>(messageType)
                      << " | Data1: " << std::dec << static_cast<int>(data1)
                      << " | Data2: " << static_cast<int>(data2) << "\n";
        }
    }
}
