#include "MicrophoneManager.h"

MicrophoneManager::MicrophoneManager() {
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator
    );
    if (FAILED(hr)) return;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pMicDevice);
    if (FAILED(hr)) return;

    pMicDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pMicVolume);
    initialized = true;
}

MicrophoneManager::~MicrophoneManager() {
    if (pMicVolume) pMicVolume->Release();
    if (pMicDevice) pMicDevice->Release();
    if (pEnumerator) pEnumerator->Release();
}

bool MicrophoneManager::ToggleMute() {
    if (!pMicVolume) return false;
    BOOL isMuted = FALSE;
    pMicVolume->GetMute(&isMuted);
    BOOL newMute = !isMuted;
    pMicVolume->SetMute(newMute, NULL);
    return (newMute == TRUE);
}
