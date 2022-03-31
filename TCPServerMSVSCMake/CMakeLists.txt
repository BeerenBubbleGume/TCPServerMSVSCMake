# CMakeList.txt: ������ CMake ��� TCPServerMSVSCMake; �������� �������� ��� � �����������,
# ������� ����� ������ ��� ����������� �������.
cmake_minimum_required (VERSION 3.8)

add_subdirectory("src")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs+ -gstabs -D _DEBUG -O")
# �������� �������� � ����������� ���� ����� �������.
add_executable (TCPServerMSVSCMake	"src/serv/NetSock.hpp"
									"src/serv/NetSock.cpp"
									"src/serv/NetSocketUV.hpp"
									"src/serv/NetSocketUV.cpp"
									"TCPServerMSVSCMake.cpp"
									"src/serv/utils.hpp"
									"src/serv/RTSP.cpp"
									"src/serv/RTSP.hpp"
									"src/serv/utils.cpp"
									"TCPServerMSVSCMake.hpp" 
									"src/serv/includes.hpp"
									"src/serv/utf.hpp"
									"src/serv/utf.cpp")
# TODO: �������� ����� � ������� �������, ���� ��� ����������.
IF(WIN32)
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/BasicUsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/UsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/liveMedia/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/groupsock/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/openssl/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/pthread/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/glib-2.0/
								)
	target_link_libraries(TCPServerMSVSCMake 
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib
								)
ELSE()
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/BasicUsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/UsageEnvironment/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/liveMedia/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/groupsock/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/openssl/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/glib-2.0/
								)
	target_link_libraries(TCPServerMSVSCMake
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libuv.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libBasicUsageEnvironment.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libUsageEnvironment.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libliveMedia.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libgroupsock.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libopenssl.a
								stdc++fs
								pthread
								-ldl
								)
ENDIF()
