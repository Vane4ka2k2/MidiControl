#pragma once
#include <windows.h>
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SHOW_MAPPING 1000
#define ID_TRAY_SHOW_CONSOLE 1001
#define ID_TRAY_AUTOSTART    1003
#define ID_TRAY_EXIT         1004

class TrayManager {
private:
    static HWND hwndTray;
    static NOTIFYICONDATAW nid;

public:
    static void Initialize(HINSTANCE hInstance);
    static void Remove();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static bool IsAutostartEnabled();
    static void ToggleAutostart();
};
