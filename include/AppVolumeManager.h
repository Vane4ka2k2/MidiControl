#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>

class AppVolumeManager {
private:
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pMasterVolume = nullptr;
    IAudioSessionManager2* pSessionManager = nullptr;
    bool initialized = false;

public:
    AppVolumeManager();
    ~AppVolumeManager();

    bool IsInitialized() const { return initialized; }
    void SetMasterVolume(float scalar);
    float GetMasterVolume();
    void SetAppGroupVolume(const std::vector<std::wstring>& targetApps, float scalar);
};
