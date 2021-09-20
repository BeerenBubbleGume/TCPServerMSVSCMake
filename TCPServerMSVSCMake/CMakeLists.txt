# CMakeList.txt: ������ CMake ��� TCPServerMSVSCMake; �������� �������� ��� � �����������,
# ������� ����� ������ ��� ����������� �������.
cmake_minimum_required (VERSION 3.8)

add_subdirectory("src")


# �������� �������� � ����������� ���� ����� �������.
add_executable (TCPServerMSVSCMake	"src/serv/Net.h"
									"src/serv/Net.cpp"
									"src/serv/NetBuffer.cpp"
									"src/serv/NetBuffer.h"
									"src/serv/NetSock.h"
									"src/serv/NetSock.cpp"
									"src/serv/NetSocketUV.h"
									"src/serv/NetSocketUV.cpp"
									"src/serv/ServerUV.h"
									"src/serv/ServerUV.cpp"
									"TCPServerMSVSCMake.cpp"
									"TCPServerMSVSCMake.h")

# TODO: �������� ����� � ������� �������, ���� ��� ����������.
IF(WIN32)
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/uv.h)
	target_link_libraries(TCPServerMSVSCMake 
							${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib)
ELSE()
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/)
	target_link_libraries(TCPServerMSVSCMake
							${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libuv.so)
ENDIF()