/**
 * @file OSDWindow.cpp
 * @brief Реализация выплывающего OSD-индикатора с анимированным таймером скрытия и потокобезопасным GUI-диспатчингом.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "OSDWindow.h"
#include "MidiController.h"

HWND OSDWindow::hwndOSD = NULL;
std::mutex OSDWindow::osdMutex;
std::wstring OSDWindow::currentTitle = L"";
std::wstring OSDWindow::currentStatusText = L"";
int OSDWindow::currentPercent = -1;

void OSDWindow::Initialize(HINSTANCE hInstance) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MidiControlOSD";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassExW(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int width = 340;
    int height = 75;
    int posX = (screenW - width) / 2;
    int posY = screenH - 140;

    hwndOSD = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        L"MidiControlOSD", L"MidiControl OSD",
        WS_POPUP,
        posX, posY, width, height,
        NULL, NULL, hInstance, NULL
    );

    SetLayeredWindowAttributes(hwndOSD, RGB(0, 0, 0), 220, LWA_COLORKEY | LWA_ALPHA);
}

void OSDWindow::Show(const std::wstring& title, int percent, const std::wstring& statusText) {
    if (!MidiController::g_Config.showOsd || !hwndOSD) return;

    {
        std::lock_guard<std::mutex> lock(osdMutex);
        currentTitle = title;
        currentPercent = percent;
        currentStatusText = statusText;
    }

    PostMessageW(hwndOSD, WM_OSD_SHOW, 0, 0);
}

LRESULT CALLBACK OSDWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_OSD_SHOW:
        ShowWindow(hwnd, SW_SHOWNA);
        InvalidateRect(hwnd, NULL, TRUE);
        SetTimer(hwnd, 1, MidiController::g_Config.osdDurationMs, NULL);
        return 0;

    case WM_TIMER:
        KillTimer(hwnd, 1);
        ShowWindow(hwnd, SW_HIDE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        std::wstring title;
        std::wstring statusText;
        int percent = -1;

        {
            std::lock_guard<std::mutex> lock(osdMutex);
            title = currentTitle;
            statusText = currentStatusText;
            percent = currentPercent;
        }

        HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 35));
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(80, 80, 95));
        HGDIOBJ hOldBrush = SelectObject(hdc, bgBrush);
        HGDIOBJ hOldPen = SelectObject(hdc, borderPen);

        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 16, 16);

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(bgBrush);
        DeleteObject(borderPen);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(240, 240, 245));

        HFONT fontTitle = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HGDIOBJ hOldFont = SelectObject(hdc, fontTitle);

        RECT titleRect = { 16, 10, rect.right - 16, 32 };
        DrawTextW(hdc, title.c_str(), -1, &titleRect, DT_LEFT | DT_SINGLELINE);

        if (percent >= 0) {
            std::wstring percStr = std::to_wstring(percent) + L"%";
            RECT percRect = { 16, 10, rect.right - 16, 32 };
            DrawTextW(hdc, percStr.c_str(), -1, &percRect, DT_RIGHT | DT_SINGLELINE);

            RECT barBg = { 16, 40, rect.right - 16, 54 };
            HBRUSH barBgBrush = CreateSolidBrush(RGB(50, 50, 60));
            FillRect(hdc, &barBg, barBgBrush);
            DeleteObject(barBgBrush);

            int barWidth = barBg.right - barBg.left;
            int fillWidth = static_cast<int>((static_cast<float>(percent) / 100.0f) * barWidth);
            RECT barFill = { barBg.left, barBg.top, barBg.left + fillWidth, barBg.bottom };
            HBRUSH barFillBrush = CreateSolidBrush(RGB(0, 120, 215));
            FillRect(hdc, &barFill, barFillBrush);
            DeleteObject(barFillBrush);
        } else if (!statusText.empty()) {
            HFONT fontStatus = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            SelectObject(hdc, fontStatus);
            SetTextColor(hdc, RGB(0, 200, 120));

            RECT statusRect = { 16, 36, rect.right - 16, 62 };
            DrawTextW(hdc, statusText.c_str(), -1, &statusRect, DT_CENTER | DT_SINGLELINE);
            
            SelectObject(hdc, fontTitle);
            DeleteObject(fontStatus);
        }

        SelectObject(hdc, hOldFont);
        DeleteObject(fontTitle);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
