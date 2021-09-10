# CMakeList.txt: проект CMake для TCPServerMSVSCMake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TCPServerMSVSCMake "TCPServerMSVSCMake.cpp" "TCPServerMSVSCMake.h" "libs/includes/uv/aix.h" "libs/includes/uv/android-ifaddrs.h" "libs/includes/uv/bsd.h" "libs/includes/uv/darwin.h" "libs/includes/uv/errno.h" "libs/includes/uv/linux.h" "libs/includes/uv/os390.h" "libs/includes/uv/posix.h" "libs/includes/uv/stdint-msvc2008.h" "libs/includes/uv/sunos.h" "libs/includes/uv/threadpool.h" "libs/includes/uv/tree.h" "libs/includes/uv/unix.h" "libs/includes/uv/version.h" "libs/includes/uv/win.h" "libs/includes/uv.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

add_subdirectory("libs")
target_include_directories(TCPServerMSVSCMake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/includes)

find_library(uv uv.lib ${CMAKE_CURRENT_SOURCE_DIR}libs/lib/uv.lib)

target_link_libraries(TCPServerMSVSCMake uv)

