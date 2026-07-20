/**
 * @file main.cpp
 * @brief Точка входа в приложение MidiControl и главный цикл обработки сообщений Windows.
 * @author Vane4ka2k2
 * @date 2026
 */

#include <iostream>
#include <windows.h>
#include "OSDWindow.h"
#include "MappingWindow.h"
#include "TrayManager.h"
#include "MidiController.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HINSTANCE hInstance = GetModuleHandle(NULL);

    std::cout << "=======================================================\n";
    std::cout << "   Полнофункциональный MIDI-контроллер Windows (Arturia)\n";
    std::cout << "=======================================================\n\n";

    OSDWindow::Initialize(hInstance);
    MappingWindow::Initialize(hInstance);
    TrayManager::Initialize(hInstance);

    MidiController controller;
    if (controller.Initialize("config.json")) {
        controller.Start();
    }

    std::cout << "\nПрограмма работает в фоновом режиме и свернута в системный трей (возле часов).\n";
    std::cout << "Для открытия карты элементов щелкните по иконке в трее -> 'Назначение клавиш / Карта функций'.\n\n";

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    controller.Stop();
    TrayManager::Remove();

    return 0;
}
