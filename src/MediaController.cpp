/**
 * @file MediaController.cpp
 * @brief Реализация вызова мультимедийных клавиш, сочетаний Win32 и протокола Telegram.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "MediaController.h"
#include <shellapi.h>

void MediaController::PlayPause() {
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, 0, 0);
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::NextTrack() {
    keybd_event(VK_MEDIA_NEXT_TRACK, 0, 0, 0);
    keybd_event(VK_MEDIA_NEXT_TRACK, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::PrevTrack() {
    keybd_event(VK_MEDIA_PREV_TRACK, 0, 0, 0);
    keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::MasterMute() {
    keybd_event(VK_VOLUME_MUTE, 0, 0, 0);
    keybd_event(VK_VOLUME_MUTE, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::SnippingTool() {
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event(VK_SHIFT, 0, 0, 0);
    keybd_event('S', 0, 0, 0);
    keybd_event('S', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::ShowDesktop() {
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event('D', 0, 0, 0);
    keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::LaunchTelegram() {
    ShellExecuteW(NULL, L"open", L"tg://", NULL, NULL, SW_SHOWNORMAL);
}

void MediaController::SeekForward() {
    keybd_event(VK_RIGHT, 0, 0, 0);
    keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::SeekBackward() {
    keybd_event(VK_LEFT, 0, 0, 0);
    keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::ZoomIn() {
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(VK_OEM_PLUS, 0, 0, 0);
    keybd_event(VK_OEM_PLUS, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void MediaController::ZoomOut() {
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(VK_OEM_MINUS, 0, 0, 0);
    keybd_event(VK_OEM_MINUS, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}
