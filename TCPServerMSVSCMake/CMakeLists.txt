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
									"src/serv/FF_encoder.cpp"
									"src/serv/FF_encoder.hpp"
									"src/serv/utils.hpp"
									"src/serv/utils.cpp"
									"TCPServerMSVSCMake.hpp" 
									"src/serv/includes.hpp"
									"src/serv/utf.hpp"
									"src/serv/utf.cpp")
# TODO: �������� ����� � ������� �������, ���� ��� ����������.
IF(WIN32)
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								
								)
	target_link_libraries(TCPServerMSVSCMake 
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib
								)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} WIN32 _WIN32")
ELSE()
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/liveMedia/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/groupsock/
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/BasicUsageEnvironment
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/UsageEnvironment
								
								)
	target_link_libraries(TCPServerMSVSCMake
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libuv.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libliveMedia.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libgroupsock.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libBasicUsageEnvironment.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libUsageEnvironment.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavformat.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavdevice.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavcodec.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavutil.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libswresample.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavfilter.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libpostproc.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libswscale.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libyasm.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libz.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libcrypto.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/liblzma.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libmp3lame.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libfdk-aac.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libvpx.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libopus.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libx264.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libx265.a
								stdc++fs
								pthread
								rt
								bz2
								ssl
								-ldl
								)
ENDIF()
