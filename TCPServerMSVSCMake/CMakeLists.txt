# CMakeList.txt: ������ CMake ��� TCPServerMSVSCMake; �������� �������� ��� � �����������,
# ������� ����� ������ ��� ����������� �������.
cmake_minimum_required (VERSION 3.8)

add_subdirectory("src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs+ -gstabs -D _DEBUG -O")
# �������� �������� � ����������� ���� ����� �������.
add_executable (TCPServerMSVSCMake	"src/serv/NetSock.hpp"
									"src/serv/NetSock.cpp"
									"src/serv/NetSocketUV.hpp"
									"src/serv/NetSocketUV.cpp"
									"TCPServerMSVSCMake.cpp"
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
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavcodec
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavdevice
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavfilter
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavutil
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libswresample
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libswcale)
	target_link_libraries(TCPServerMSVSCMake 
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/uv.lib
								)
ELSE()
	target_include_directories(TCPServerMSVSCMake PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavcodec
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavdevice
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavfilter
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libavutil
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libswresample
								#${CMAKE_CURRENT_SOURCE_DIR}/libs/includes/libswscale
								)
	target_link_libraries(TCPServerMSVSCMake
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libuv_a.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavcodec.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavdevice.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavfilter.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libavutil.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libswresample.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libswscale.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libfdk-aac.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libmp3lame.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libopus.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libpostproc.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libvpx.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libx264.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libyasm.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libz.a
								${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/liblzma.a
								pthread
								-ldl
								)
ENDIF()
