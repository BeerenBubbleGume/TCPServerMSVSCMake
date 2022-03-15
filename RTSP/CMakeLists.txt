# CMakeList.txt: проект CMake для RTSP; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("RTSP")
add_subdirectory("libs")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (RTSP "RTSP.cpp" "RTSP.hpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs+ -gstabs -D _DEBUG -O")

IF(WIN32)
	target_include_directories(RTSP PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/BasicUsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/liveMedia/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/groupsock/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/UsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/openssl/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/zlib.h
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/
								)
	target_link_libraries(RTSP	PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/liveMedia.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/BasicUsageEnvironment.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/UsageEnvironment.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/groupsock.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libssl.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libcrypto.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/pthread.lib
								)
ELSE()
	target_include_directories(RTSP PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/BasicUsageEnvironment
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/liveMedia
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/groupsock
								${CMAKE_CURRENT_SOURCE_DIR}/libs/include/UsageEnvironment)
	target_link_libraries(RTSP
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libliveMedia.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libBasicUsageEnvironment.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libUsageEnvironment.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libgroupsock.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libssl.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libcrypto.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libz.a
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libpthread.so
								pthread
								-ldl)
ENDIF()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
