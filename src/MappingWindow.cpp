/**
 * @file MappingWindow.cpp
 * @brief Реализация окна графической карты функций пэдов, энкодеров и фейдеров с динамической отрисовкой.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "MappingWindow.h"
#include "MidiController.h"
#include "Utils.h"
#include <string>

HWND MappingWindow::hwndMapping = NULL;

namespace {
    static void DrawLine(HDC hdc, int x, int y, const std::wstring& str) {
        TextOutW(hdc, x, y, str.c_str(), static_cast<int>(str.length()));
    }
}

void MappingWindow::Initialize(HINSTANCE hInstance) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MidiControlMapping";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int width = 640;
    int height = 560;
    int posX = (screenW - width) / 2;
    int posY = (screenH - height) / 2;

    hwndMapping = CreateWindowExW(
        0,
        L"MidiControlMapping",
        L"MidiControl — Карта функций устройств",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        posX, posY, width, height,
        NULL, NULL, hInstance, NULL
    );
}

void MappingWindow::Show() {
    if (!hwndMapping) return;
    ShowWindow(hwndMapping, SW_SHOW);
    SetForegroundWindow(hwndMapping);
    InvalidateRect(hwndMapping, NULL, TRUE);
}

LRESULT CALLBACK MappingWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        HBRUSH bgBrush = CreateSolidBrush(RGB(25, 25, 30));
        FillRect(hdc, &rect, bgBrush);
        DeleteObject(bgBrush);

        SetBkMode(hdc, TRANSPARENT);

        HFONT fontHeader = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HGDIOBJ hOldFont = SelectObject(hdc, fontHeader);
        SetTextColor(hdc, RGB(0, 160, 255));
        
        std::wstring titleText = Utf8ToWstring("Карта функций MIDI-контроллера (") + Utf8ToWstring(MidiController::g_Config.deviceName) + L")";
        DrawLine(hdc, 24, 16, titleText);

        HFONT fontSection = CreateFontW(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT fontText = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        int currentY = 52;

        // Секция 1: Фейдеры
        SelectObject(hdc, fontSection);
        SetTextColor(hdc, RGB(240, 200, 80));
        DrawLine(hdc, 24, currentY, L"ФЕЙДЕРЫ (FADERS):");
        currentY += 24;

        SelectObject(hdc, fontText);
        SetTextColor(hdc, RGB(220, 220, 225));

        for (const auto& [id, fader] : MidiController::g_Config.faders) {
            std::wstring line = L"• Fader " + std::to_wstring(id) + L" (CC " + std::to_wstring(fader.ccArturia) +
                L" / " + std::to_wstring(fader.ccDaw) + L") — " + Utf8ToWstring(fader.label);
            DrawLine(hdc, 36, currentY, line);
            currentY += 20;
        }

        currentY += 10;

        // Секция 2: Энкодеры
        SelectObject(hdc, fontSection);
        SetTextColor(hdc, RGB(180, 140, 255));
        DrawLine(hdc, 24, currentY, L"ЭНКОДЕРЫ (ENCODERS):");
        currentY += 24;

        SelectObject(hdc, fontText);
        SetTextColor(hdc, RGB(220, 220, 225));

        for (const auto& [id, encoder] : MidiController::g_Config.encoders) {
            std::wstring line = L"• Encoder " + std::to_wstring(id) + L" (CC " + std::to_wstring(encoder.cc) +
                L") — " + Utf8ToWstring(encoder.label);
            DrawLine(hdc, 36, currentY, line);
            currentY += 20;
        }

        currentY += 10;

        // Секция 3: Пэды
        SelectObject(hdc, fontSection);
        SetTextColor(hdc, RGB(0, 210, 140));
        DrawLine(hdc, 24, currentY, L"ПЭДЫ (PADS):");
        currentY += 24;

        SelectObject(hdc, fontText);
        SetTextColor(hdc, RGB(220, 220, 225));

        for (const auto& [id, pad] : MidiController::g_Config.pads) {
            std::wstring line = L"• Pad " + std::to_wstring(id) + L" (Нота " + std::to_wstring(pad.note) +
                L" / CC " + std::to_wstring(pad.cc) + L") — " + Utf8ToWstring(pad.label);
            DrawLine(hdc, 36, currentY, line);
            currentY += 20;
        }

        SelectObject(hdc, hOldFont);
        DeleteObject(fontHeader);
        DeleteObject(fontSection);
        DeleteObject(fontText);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
