# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
project ("TCPServerMSVSCMake")

# Включите подпроекты.
add_subdirectory ("TCPServerMSVSCMake")
#add_subdirectory("RTSP")