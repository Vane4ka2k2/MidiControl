cmake_minimum_required(VERSION 3.16)
project(MidiControl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение кодировки UTF-8 для компилятора MSVC
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Директория с заголовочными файлами
include_directories(include)

set(SOURCES
    src/main.cpp
    src/AppVolumeManager.cpp
    src/ConfigManager.cpp
    src/MappingWindow.cpp
    src/MediaController.cpp
    src/MidiController.cpp
    src/OSDWindow.cpp
    src/TrayManager.cpp
    src/Utils.cpp
)

add_executable(MidiControl ${SOURCES})

# Системные библиотеки Windows
target_link_libraries(MidiControl PRIVATE winmm ole32 gdi32 user32 shell32 advapi32 psapi)
