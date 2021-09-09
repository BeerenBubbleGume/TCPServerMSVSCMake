# CMakeList.txt: проект CMake для TCPServerMSVSCMake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (TCPServerMSVSCMake "TCPServerMSVSCMake.cpp" "TCPServerMSVSCMake.hpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

target_include_directories(TCPServerMSVSCMake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}../libs/includes
											PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}../libs/includes/uv)

add_library(uvlib SHARED IMPORTED)

target_link_libraries(uvlib
	${CMAKE_CURRENT_SOURCE_DIR}../libs/lib/uv.lib)

add_subdirectory("libs")