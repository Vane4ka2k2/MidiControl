#include "TrayManager.h"
#include "MappingWindow.h"
#include "Utils.h"

HWND TrayManager::hwndTray = NULL;
NOTIFYICONDATAW TrayManager::nid = {};

void TrayManager::Initialize(HINSTANCE hInstance) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MidiControlTray";
    RegisterClassExW(&wc);

    hwndTray = CreateWindowExW(0, L"MidiControlTray", L"MidiControl Tray", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwndTray;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"MidiControl - Arturia Minilab 3");

    Shell_NotifyIconW(NIM_ADD, &nid);
}

void TrayManager::Remove() {
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

LRESULT CALLBACK TrayManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONDBLCLK) {
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = CreatePopupMenu();
            bool isConsoleVisible = IsWindowVisible(GetConsoleWindow()) == TRUE;

            std::wstring strMapping = Utf8ToWstring("Назначение клавиш / Карта функций");
            std::wstring strConsole = Utf8ToWstring("Окно отладки консоли");
            std::wstring strAutostart = Utf8ToWstring("Автозапуск с Windows");
            std::wstring strExit = Utf8ToWstring("Выход");

            AppendMenuW(hMenu, MF_STRING, ID_TRAY_SHOW_MAPPING, strMapping.c_str());
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING | (isConsoleVisible ? MF_CHECKED : 0), ID_TRAY_SHOW_CONSOLE, strConsole.c_str());
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING | (IsAutostartEnabled() ? MF_CHECKED : 0), ID_TRAY_AUTOSTART, strAutostart.c_str());
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, strExit.c_str());

            SetForegroundWindow(hwnd);
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);

            if (cmd == ID_TRAY_SHOW_MAPPING) {
                MappingWindow::Show();
            } else if (cmd == ID_TRAY_SHOW_CONSOLE) {
                HWND hwndConsole = GetConsoleWindow();
                if (isConsoleVisible) ShowWindow(hwndConsole, SW_HIDE);
                else ShowWindow(hwndConsole, SW_SHOW);
            } else if (cmd == ID_TRAY_AUTOSTART) {
                ToggleAutostart();
            } else if (cmd == ID_TRAY_EXIT) {
                PostQuitMessage(0);
            }
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool TrayManager::IsAutostartEnabled() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD type = 0;
        LONG result = RegQueryValueExW(hKey, L"MidiControl", NULL, &type, NULL, NULL);
        RegCloseKey(hKey);
        return result == ERROR_SUCCESS;
    }
    return false;
}

void TrayManager::ToggleAutostart() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE | KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (IsAutostartEnabled()) {
            RegDeleteValueW(hKey, L"MidiControl");
            MessageBoxW(NULL, Utf8ToWstring("Автозапуск отключен.").c_str(), L"MidiControl", MB_OK | MB_ICONINFORMATION);
        } else {
            WCHAR exePath[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            RegSetValueExW(hKey, L"MidiControl", 0, REG_SZ, (BYTE*)exePath, (wcslen(exePath) + 1) * sizeof(WCHAR));
            MessageBoxW(NULL, Utf8ToWstring("Автозапуск при входе в Windows включен!").c_str(), L"MidiControl", MB_OK | MB_ICONINFORMATION);
        }
        RegCloseKey(hKey);
    }
}
