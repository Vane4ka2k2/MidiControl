#pragma once
#include <windows.h>

class MappingWindow {
private:
    static HWND hwndMapping;

public:
    static void Initialize(HINSTANCE hInstance);
    static void Show();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
