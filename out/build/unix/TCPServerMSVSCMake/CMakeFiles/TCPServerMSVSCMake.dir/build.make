# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix

# Include any dependencies generated for this target.
include TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.make

# Include the progress variables for this target.
include TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/progress.make

# Include the compile flags for this target's objects.
include TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o: ../../../TCPServerMSVSCMake/src/serv/NetSock.cpp
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o -MF CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o.d -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o -c /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/NetSock.cpp

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.i"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/NetSock.cpp > CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.i

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.s"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/NetSock.cpp -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.s

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o: ../../../TCPServerMSVSCMake/src/serv/NetSocketUV.cpp
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o -MF CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o.d -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o -c /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/NetSocketUV.cpp

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.i"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/NetSocketUV.cpp > CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.i

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.s"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/NetSocketUV.cpp -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.s

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o: ../../../TCPServerMSVSCMake/src/serv/ServerUV.cpp
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o -MF CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o.d -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o -c /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/ServerUV.cpp

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.i"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/ServerUV.cpp > CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.i

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.s"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/ServerUV.cpp -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.s

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o: ../../../TCPServerMSVSCMake/TCPServerMSVSCMake.cpp
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o -MF CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o.d -o CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o -c /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/TCPServerMSVSCMake.cpp

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.i"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/TCPServerMSVSCMake.cpp > CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.i

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.s"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/TCPServerMSVSCMake.cpp -o CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.s

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o: ../../../TCPServerMSVSCMake/src/serv/utils.cpp
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o -MF CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o.d -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o -c /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/utils.cpp

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.i"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/utils.cpp > CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.i

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.s"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/utils.cpp -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.s

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/flags.make
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o: ../../../TCPServerMSVSCMake/src/serv/utf.cpp
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o -MF CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o.d -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o -c /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/utf.cpp

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.i"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/utf.cpp > CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.i

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.s"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake/src/serv/utf.cpp -o CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.s

# Object files for target TCPServerMSVSCMake
TCPServerMSVSCMake_OBJECTS = \
"CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o" \
"CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o" \
"CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o" \
"CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o" \
"CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o" \
"CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o"

# External object files for target TCPServerMSVSCMake
TCPServerMSVSCMake_EXTERNAL_OBJECTS =

TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSock.cpp.o
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/NetSocketUV.cpp.o
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/ServerUV.cpp.o
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/TCPServerMSVSCMake.cpp.o
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utils.cpp.o
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/src/serv/utf.cpp.o
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/build.make
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libuv_a.a
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libavcodec.a
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libavdevice.a
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libavfilter.a
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libavutil.a
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libswresample.a
TCPServerMSVSCMake/TCPServerMSVSCMake: ../../../TCPServerMSVSCMake/libs/lib/libswscale.a
TCPServerMSVSCMake/TCPServerMSVSCMake: TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable TCPServerMSVSCMake"
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TCPServerMSVSCMake.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/build: TCPServerMSVSCMake/TCPServerMSVSCMake
.PHONY : TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/build

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/clean:
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake && $(CMAKE_COMMAND) -P CMakeFiles/TCPServerMSVSCMake.dir/cmake_clean.cmake
.PHONY : TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/clean

TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/depend:
	cd /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/TCPServerMSVSCMake /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake /home/dkhaziev/Документы/GitHub/TCPServerMSVSCMake/out/build/unix/TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : TCPServerMSVSCMake/CMakeFiles/TCPServerMSVSCMake.dir/depend

