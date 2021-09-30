# CMakeList.txt: ������ CMake ��� TCPServerMSVSCMake; �������� �������� ��� � �����������,
# ������� ����� ������ ��� ����������� �������.
cmake_minimum_required (VERSION 3.8)

add_subdirectory("src")


# �������� �������� � ����������� ���� ����� �������.
add_executable (TCPServerMSVSCMake	"src/serv/Net.hpp"
									"src/serv/Net.cpp"
									"src/serv/NetSock.hpp"
									"src/serv/NetSock.cpp"
									"src/serv/NetSocketUV.hpp"
									"src/serv/NetSocketUV.cpp"
									"src/serv/ServerUV.hpp"
									"src/serv/ServerUV.cpp"
									"TCPServerMSVSCMake.cpp"
									"src/serv/utils.hpp"
									"src/serv/utils.cpp"
									"TCPServerMSVSCMake.hpp")

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