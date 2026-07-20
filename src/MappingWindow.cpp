/**
 * @file MappingWindow.cpp
 * @brief Реализация окна графической карты функций пэдов и энкодеров.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "MappingWindow.h"

HWND MappingWindow::hwndMapping = NULL;

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
    int width = 570;
    int height = 480;
    int posX = (screenW - width) / 2;
    int posY = (screenH - height) / 2;

    hwndMapping = CreateWindowExW(
        0,
        L"MidiControlMapping",
        L"MidiControl — Карта функций Arturia Minilab 3",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        posX, posY, width, height,
        NULL, NULL, hInstance, NULL
    );
}

void MappingWindow::Show() {
    if (!hwndMapping) return;
    ShowWindow(hwndMapping, SW_SHOW);
    SetForegroundWindow(hwndMapping);
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
        SelectObject(hdc, fontHeader);
        SetTextColor(hdc, RGB(0, 160, 255));
        TextOutW(hdc, 24, 20, L"Карта функций Arturia Minilab 3", 31);
        DeleteObject(fontHeader);

        HFONT fontSection = CreateFontW(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT fontText = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        // Секция 1: Фейдеры
        SelectObject(hdc, fontSection);
        SetTextColor(hdc, RGB(240, 200, 80));
        TextOutW(hdc, 24, 60, L"ФЕЙДЕРЫ (FADERS):", 17);

        SelectObject(hdc, fontText);
        SetTextColor(hdc, RGB(220, 220, 225));
        TextOutW(hdc, 36, 88,  L"• Fader 1 (CC 82 / 14) — Общая мастер-громкость Windows", 54);

        // Секция 2: Пэды
        SelectObject(hdc, fontSection);
        SetTextColor(hdc, RGB(0, 210, 140));
        TextOutW(hdc, 24, 124, L"ПЭДЫ (PADS 1–8):", 16);

        SelectObject(hdc, fontText);
        SetTextColor(hdc, RGB(220, 220, 225));
        TextOutW(hdc, 36, 152, L"• Pad 1 (Нота 36 / CC 102) — Переключение вывода (Наушники ↔ Динамики)", 70);
        TextOutW(hdc, 36, 174, L"• Pad 2 (Нота 37 / CC 103) — Рабочий стол / Свернуть окна (Win + D)", 66);
        TextOutW(hdc, 36, 196, L"• Pad 3 (Нота 38 / CC 104) — Быстрый запуск Telegram", 52);
        TextOutW(hdc, 36, 218, L"• Pad 4 (Нота 39 / CC 105) — Воспроизведение / Пауза (Play/Pause)", 64);
        TextOutW(hdc, 36, 240, L"• Pad 5 (Нота 40 / CC 106) — Следующий трек", 43);
        TextOutW(hdc, 36, 262, L"• Pad 6 (Нота 41 / CC 107) — Предыдущий трек", 44);
        TextOutW(hdc, 36, 284, L"• Pad 7 (Нота 42 / CC 108) — Приглушение звука (Smart Ducking 15%)", 66);
        TextOutW(hdc, 36, 306, L"• Pad 8 (Нота 43 / CC 109) — Скриншот области экрана (Win+Shift+S)", 66);

        // Секция 3: Энкодеры
        SelectObject(hdc, fontSection);
        SetTextColor(hdc, RGB(180, 140, 255));
        TextOutW(hdc, 24, 342, L"ЭНКОДЕРЫ (ENCODERS):", 19);

        SelectObject(hdc, fontText);
        SetTextColor(hdc, RGB(220, 220, 225));
        TextOutW(hdc, 36, 370, L"• Encoder 1 (CC 74) — Громкость активного фокусного окна", 56);
        TextOutW(hdc, 36, 392, L"• Encoder 2 (CC 71) — Перемотка видео / трека (Вперед / Назад)", 61);
        TextOutW(hdc, 36, 414, L"• Encoder 3 (CC 76) — Масштабирование браузера (Zoom In / Out)", 62);

        DeleteObject(fontSection);
        DeleteObject(fontText);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
