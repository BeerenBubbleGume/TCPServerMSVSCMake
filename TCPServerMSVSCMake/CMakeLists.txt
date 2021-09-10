# CMakeList.txt: проект CMake для TCPServerMSVSCMake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TCPServerMSVSCMake WIN32 "TCPServerMSVSCMake.cpp" "TCPServerMSVSCMake.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

add_subdirectory("libs")
include_directories(${CMAKE_SOURCE_DIR}/libs/includes/)
link_directories(${CMAKE_SOURCE_DIR}/libs/lib/)

target_link_libraries(TCPServerMSVSCMake uv)
