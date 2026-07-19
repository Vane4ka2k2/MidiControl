#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

class MicrophoneManager {
private:
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pMicDevice = nullptr;
    IAudioEndpointVolume* pMicVolume = nullptr;
    bool initialized = false;

public:
    MicrophoneManager();
    ~MicrophoneManager();

    bool ToggleMute();
};
