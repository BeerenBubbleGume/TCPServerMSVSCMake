# CMakeList.txt: ������ CMake ��� TCPServerMSVSCMake; �������� �������� ��� � �����������,
# ������� ����� ������ ��� ����������� �������.
cmake_minimum_required (VERSION 3.8)

add_subdirectory("src")


# �������� �������� � ����������� ���� ����� �������.
add_executable (TCPServerMSVSCMake	"src/serv/NetSock.hpp"
									"src/serv/NetSock.cpp"
									"src/serv/NetSocketUV.hpp"
									"src/serv/NetSocketUV.cpp"
									"src/serv/ServerUV.hpp"
									"src/serv/ServerUV.cpp"
									"TCPServerMSVSCMake.cpp"
									"src/serv/utils.hpp"
									"src/serv/utils.cpp"
									"TCPServerMSVSCMake.hpp" 
									"src/serv/includes.hpp")
# TODO: �������� ����� � ������� �������, ���� ��� ����������.
IF(WIN32)
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/uv.h
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/BasicUsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/liveMedia/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/groupsock/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/UsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/openssl/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/zlib.h)
	target_link_libraries(TCPServerMSVSCMake 
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/UsageEnvironment.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/liveMedia.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/groupsock.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/BasicUsageEnvironment.lib
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/pthread.lib)
ELSE()
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/BasicUsageEnvironment
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/liveMedia
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/groupsock
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/UsageEnvironment)
	target_link_libraries(TCPServerMSVSCMake
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libuv_a.a
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