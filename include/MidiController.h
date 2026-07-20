#pragma once
#include <windows.h>
#include <mmeapi.h>
#include <string>
#include "ConfigManager.h"
#include "AppVolumeManager.h"
#include "AudioDeviceManager.h"

class MidiController {
private:
    HMIDIIN hMidiIn = nullptr;
    int targetDeviceIndex = -1;

public:
    static AppConfig g_Config;
    static AppVolumeManager g_AppVolume;
    static AudioDeviceManager g_AudioDevManager;

    bool Initialize(const std::string& configPath);
    void Start();
    void Stop();

private:
    static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
    static void ExecutePadAction(const PadConfig& pad);
    static void SetFocusedAppVolume(float scalar);
    static void HandleEncoder2(BYTE ccValue);
    static void HandleEncoder3(BYTE ccValue);
};
