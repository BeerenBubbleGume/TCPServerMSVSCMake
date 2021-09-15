# CMakeList.txt: проект CMake для TCPServerMSVSCMake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
cmake_minimum_required (VERSION 3.8)

add_subdirectory("src")


# Добавьте источник в исполняемый файл этого проекта.
add_executable (TCPServerMSVSCMake "TCPServerMSVSCMake.cpp" "TCPServerMSVSCMake.h" "src/serv/ServerUV.h" "src/serv/ServerUV.cpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
IF(WIN32)
	target_include_directories(TCPServerMSVSCMake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/uv.h)
	target_link_libraries(TCPServerMSVSCMake ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib)
ELSE()
	target_include_directories(TCPServerMSVSCMake PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/)
	target_link_libraries(TCPServerMSVSCMake ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libuv.so)
ENDIF()