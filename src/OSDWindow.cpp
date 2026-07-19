#include "OSDWindow.h"
#include "MidiController.h"

HWND OSDWindow::hwndOSD = NULL;
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

    currentTitle = title;
    currentPercent = percent;
    currentStatusText = statusText;

    ShowWindow(hwndOSD, SW_SHOWNA);
    InvalidateRect(hwndOSD, NULL, TRUE);
    SetTimer(hwndOSD, 1, MidiController::g_Config.osdDurationMs, NULL);
}

LRESULT CALLBACK OSDWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        KillTimer(hwnd, 1);
        ShowWindow(hwnd, SW_HIDE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 35));
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(80, 80, 95));
        SelectObject(hdc, bgBrush);
        SelectObject(hdc, borderPen);
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 16, 16);
        DeleteObject(bgBrush);
        DeleteObject(borderPen);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(240, 240, 245));

        HFONT fontTitle = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        SelectObject(hdc, fontTitle);

        RECT titleRect = { 16, 10, rect.right - 16, 32 };
        DrawTextW(hdc, currentTitle.c_str(), -1, &titleRect, DT_LEFT | DT_SINGLELINE);

        if (currentPercent >= 0) {
            std::wstring percStr = std::to_wstring(currentPercent) + L"%";
            RECT percRect = { 16, 10, rect.right - 16, 32 };
            DrawTextW(hdc, percStr.c_str(), -1, &percRect, DT_RIGHT | DT_SINGLELINE);

            RECT barBg = { 16, 40, rect.right - 16, 54 };
            HBRUSH barBgBrush = CreateSolidBrush(RGB(50, 50, 60));
            FillRect(hdc, &barBg, barBgBrush);
            DeleteObject(barBgBrush);

            int barWidth = barBg.right - barBg.left;
            int fillWidth = static_cast<int>((static_cast<float>(currentPercent) / 100.0f) * barWidth);
            RECT barFill = { barBg.left, barBg.top, barBg.left + fillWidth, barBg.bottom };
            HBRUSH barFillBrush = CreateSolidBrush(RGB(0, 120, 215));
            FillRect(hdc, &barFill, barFillBrush);
            DeleteObject(barFillBrush);
        } else if (!currentStatusText.empty()) {
            HFONT fontStatus = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            SelectObject(hdc, fontStatus);
            SetTextColor(hdc, RGB(0, 200, 120));

            RECT statusRect = { 16, 36, rect.right - 16, 62 };
            DrawTextW(hdc, currentStatusText.c_str(), -1, &statusRect, DT_CENTER | DT_SINGLELINE);
            DeleteObject(fontStatus);
        }

        DeleteObject(fontTitle);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
