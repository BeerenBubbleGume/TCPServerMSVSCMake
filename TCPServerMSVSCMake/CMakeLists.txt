# CMakeList.txt: проект CMake для TCPServerMSVSCMake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TCPServerMSVSCMake "TCPServerMSVSCMake.cpp" "TCPServerMSVSCMake.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

add_subdirectory("libs")
target_include_directories(TCPServerMSVSCMake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/includes
											PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib)

find_library(uv uv ${CMAKE_CURRENT_SOURCE_DIR}libs/lib/)

target_link_libraries(TCPServerMSVSCMake uv)

