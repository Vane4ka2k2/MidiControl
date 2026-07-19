#pragma once
#include <string>
#include <windows.h>

class OSDWindow {
private:
    static HWND hwndOSD;
    static std::wstring currentTitle;
    static std::wstring currentStatusText;
    static int currentPercent;

public:
    static void Initialize(HINSTANCE hInstance);
    static void Show(const std::wstring& title, int percent, const std::wstring& statusText = L"");

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
