/**
 * @file MediaController.cpp
 * @brief Реализация вызова мультимедийных клавиш, сочетаний Win32 и протокола Telegram с использованием SendInput.
 * @author Vane4ka2k2
 * @date 2026
 */

#include "MediaController.h"
#include <shellapi.h>
#include <vector>

namespace {
    static void SendKeyPress(WORD vk) {
        INPUT inputs[2] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = vk;

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = vk;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(2, inputs, sizeof(INPUT));
    }

    static void SendKeyCombo(const std::vector<WORD>& vks) {
        std::vector<INPUT> inputs;
        inputs.reserve(vks.size() * 2);

        for (WORD vk : vks) {
            INPUT in = {};
            in.type = INPUT_KEYBOARD;
            in.ki.wVk = vk;
            inputs.push_back(in);
        }
        for (auto it = vks.rbegin(); it != vks.rend(); ++it) {
            INPUT in = {};
            in.type = INPUT_KEYBOARD;
            in.ki.wVk = *it;
            in.ki.dwFlags = KEYEVENTF_KEYUP;
            inputs.push_back(in);
        }
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
}

void MediaController::PlayPause() {
    SendKeyPress(VK_MEDIA_PLAY_PAUSE);
}

void MediaController::NextTrack() {
    SendKeyPress(VK_MEDIA_NEXT_TRACK);
}

void MediaController::PrevTrack() {
    SendKeyPress(VK_MEDIA_PREV_TRACK);
}

void MediaController::MasterMute() {
    SendKeyPress(VK_VOLUME_MUTE);
}

void MediaController::SnippingTool() {
    SendKeyCombo({ VK_LWIN, VK_SHIFT, 'S' });
}

void MediaController::ShowDesktop() {
    SendKeyCombo({ VK_LWIN, 'D' });
}

void MediaController::LaunchTelegram() {
    ShellExecuteW(NULL, L"open", L"tg://", NULL, NULL, SW_SHOWNORMAL);
}

void MediaController::SeekForward() {
    SendKeyPress(VK_RIGHT);
}

void MediaController::SeekBackward() {
    SendKeyPress(VK_LEFT);
}

void MediaController::ZoomIn() {
    SendKeyCombo({ VK_CONTROL, VK_OEM_PLUS });
}

void MediaController::ZoomOut() {
    SendKeyCombo({ VK_CONTROL, VK_OEM_MINUS });
}
