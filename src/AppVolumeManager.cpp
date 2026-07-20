/**
 * @file AppVolumeManager.cpp
 * @brief Реализация модуля управления мастер-громкостью и громкостью приложений через WASAPI.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "AppVolumeManager.h"
#include <algorithm>
#include <psapi.h>

AppVolumeManager::AppVolumeManager() {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return;

    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator
    );
    if (FAILED(hr)) return;

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) return;

    pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pMasterVolume);
    pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void**)&pSessionManager);

    initialized = true;
}

AppVolumeManager::~AppVolumeManager() {
    if (pSessionManager) pSessionManager->Release();
    if (pMasterVolume) pMasterVolume->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();
}

void AppVolumeManager::SetMasterVolume(float scalar) {
    if (!pMasterVolume) return;
    if (scalar < 0.0f) scalar = 0.0f;
    if (scalar > 1.0f) scalar = 1.0f;
    pMasterVolume->SetMasterVolumeLevelScalar(scalar, NULL);
}

float AppVolumeManager::GetMasterVolume() {
    if (!pMasterVolume) return 0.0f;
    float vol = 0.0f;
    pMasterVolume->GetMasterVolumeLevelScalar(&vol);
    return vol;
}

void AppVolumeManager::SetAppGroupVolume(const std::vector<std::wstring>& targetApps, float scalar) {
    if (!pSessionManager) return;
    if (scalar < 0.0f) scalar = 0.0f;
    if (scalar > 1.0f) scalar = 1.0f;

    IAudioSessionEnumerator* pSessionEnum = nullptr;
    if (FAILED(pSessionManager->GetSessionEnumerator(&pSessionEnum)) || !pSessionEnum) return;

    int count = 0;
    pSessionEnum->GetCount(&count);

    for (int i = 0; i < count; ++i) {
        IAudioSessionControl* pSessionControl = nullptr;
        if (SUCCEEDED(pSessionEnum->GetSession(i, &pSessionControl)) && pSessionControl) {
            IAudioSessionControl2* pSessionControl2 = nullptr;
            if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2)) && pSessionControl2) {
                DWORD pid = 0;
                pSessionControl2->GetProcessId(&pid);
                if (pid != 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                    if (hProcess) {
                        WCHAR szProcessName[MAX_PATH] = { 0 };
                        if (GetModuleBaseNameW(hProcess, NULL, szProcessName, MAX_PATH) > 0) {
                            std::wstring exeName(szProcessName);
                            std::transform(exeName.begin(), exeName.end(), exeName.begin(), ::tolower);

                            for (const auto& targetApp : targetApps) {
                                std::wstring targetLower = targetApp;
                                std::transform(targetLower.begin(), targetLower.end(), targetLower.begin(), ::tolower);

                                if (exeName == targetLower) {
                                    ISimpleAudioVolume* pSimpleVolume = nullptr;
                                    if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleVolume)) && pSimpleVolume) {
                                        pSimpleVolume->SetMasterVolume(scalar, NULL);
                                        pSimpleVolume->Release();
                                    }
                                    break;
                                }
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }
                pSessionControl2->Release();
            }
            pSessionControl->Release();
        }
    }
    pSessionEnum->Release();
}
