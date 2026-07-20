#include "AudioDeviceManager.h"
#include <functiondiscoverykeys_devpkey.h>
#include <iostream>

// Недокументированный системный COM интерфейс IPolicyConfig для мгновенного переключения аудиовыхода в Windows
interface DECLSPEC_UUID("f8679f50-850a-41cf-9c72-430f60f30090") IPolicyConfig : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PULONG, PULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, DeviceShareMode*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, DeviceShareMode*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR wszDeviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

const CLSID CLSID_PolicyConfigClient = { 0x870AF99C, 0x171D, 0x4F9E, { 0xAF, 0x0D, 0xE6, 0x3D, 0xF4, 0x0C, 0x2B, 0xC9 } };
const IID IID_IPolicyConfig = __uuidof(IPolicyConfig);

AudioDeviceManager::AudioDeviceManager() {
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator
    );
    if (SUCCEEDED(hr)) {
        initialized = true;
    }
}

AudioDeviceManager::~AudioDeviceManager() {
    if (pEnumerator) pEnumerator->Release();
}

std::vector<AudioOutputDevice> AudioDeviceManager::GetActiveOutputDevices() {
    std::vector<AudioOutputDevice> devices;
    if (!pEnumerator) return devices;

    IMMDeviceCollection* pCollection = nullptr;
    if (FAILED(pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection)) || !pCollection) {
        return devices;
    }

    UINT count = 0;
    pCollection->GetCount(&count);

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* pDevice = nullptr;
        if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
            LPWSTR pwszID = nullptr;
            pDevice->GetId(&pwszID);

            IPropertyStore* pProps = nullptr;
            std::wstring friendlyName = L"Неизвестное устройство";

            if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pProps)) && pProps) {
                PROPVARIANT varName;
                PropVariantInit(&varName);
                if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName)) && varName.pwszVal) {
                    friendlyName = varName.pwszVal;
                }
                PropVariantClear(&varName);
                pProps->Release();
            }

            if (pwszID) {
                devices.push_back({ std::wstring(pwszID), friendlyName });
                CoTaskMemFree(pwszID);
            }
            pDevice->Release();
        }
    }
    pCollection->Release();
    return devices;
}

std::wstring AudioDeviceManager::CycleOutputDevice() {
    auto devices = GetActiveOutputDevices();
    if (devices.empty()) return L"Нет активных аудиоустройств";

    // Получаем текущее устройство по умолчанию
    std::wstring currentDefaultId = L"";
    IMMDevice* pDefaultDev = nullptr;
    if (SUCCEEDED(pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDev)) && pDefaultDev) {
        LPWSTR pwszID = nullptr;
        pDefaultDev->GetId(&pwszID);
        if (pwszID) {
            currentDefaultId = pwszID;
            CoTaskMemFree(pwszID);
        }
        pDefaultDev->Release();
    }

    // Находим индекс текущего устройства в списке
    int currentIndex = -1;
    for (size_t i = 0; i < devices.size(); ++i) {
        if (devices[i].id == currentDefaultId) {
            currentIndex = static_cast<int>(i);
            break;
        }
    }

    int nextIndex = (currentIndex + 1) % static_cast<int>(devices.size());
    const auto& nextDevice = devices[nextIndex];

    // Устанавливаем новое устройство по умолчанию в Windows
    IPolicyConfig* pPolicyConfig = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_PolicyConfigClient, NULL, CLSCTX_INPROC_SERVER, IID_IPolicyConfig, (void**)&pPolicyConfig);
    if (SUCCEEDED(hr) && pPolicyConfig) {
        pPolicyConfig->SetDefaultEndpoint(nextDevice.id.c_str(), eMultimedia);
        pPolicyConfig->SetDefaultEndpoint(nextDevice.id.c_str(), eCommunications);
        pPolicyConfig->Release();
    }

    return nextDevice.name;
}
