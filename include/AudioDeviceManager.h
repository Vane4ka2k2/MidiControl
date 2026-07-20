#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <string>
#include <vector>

struct AudioOutputDevice {
    std::wstring id;
    std::wstring name;
};

class AudioDeviceManager {
private:
    IMMDeviceEnumerator* pEnumerator = nullptr;
    bool initialized = false;

public:
    AudioDeviceManager();
    ~AudioDeviceManager();

    std::vector<AudioOutputDevice> GetActiveOutputDevices();
    std::wstring CycleOutputDevice();
};
