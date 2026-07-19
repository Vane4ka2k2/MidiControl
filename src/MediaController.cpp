#include "MediaController.h"

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
