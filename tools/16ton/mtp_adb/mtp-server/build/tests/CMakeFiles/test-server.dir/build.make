# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build

# Include any dependencies generated for this target.
include tests/CMakeFiles/test-server.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test-server.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test-server.dir/flags.make

tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o: tests/CMakeFiles/test-server.dir/flags.make
tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o: ../tests/TestMtpServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && /usr/bin/ccache  g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-server.dir/TestMtpServer.cpp.o -c /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests/TestMtpServer.cpp

tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-server.dir/TestMtpServer.cpp.i"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && /usr/bin/ccache  g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests/TestMtpServer.cpp > CMakeFiles/test-server.dir/TestMtpServer.cpp.i

tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-server.dir/TestMtpServer.cpp.s"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && /usr/bin/ccache  g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests/TestMtpServer.cpp -o CMakeFiles/test-server.dir/TestMtpServer.cpp.s

tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.requires:

.PHONY : tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.requires

tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.provides: tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/test-server.dir/build.make tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.provides.build
.PHONY : tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.provides

tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.provides.build: tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o


# Object files for target test-server
test__server_OBJECTS = \
"CMakeFiles/test-server.dir/TestMtpServer.cpp.o"

# External object files for target test-server
test__server_EXTERNAL_OBJECTS =

tests/test-server: tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o
tests/test-server: tests/CMakeFiles/test-server.dir/build.make
tests/test-server: libmtpserver.so.1.0.0
tests/test-server: libusbhost/libusbhost.a
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_thread.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_system.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
tests/test-server: /usr/lib/x86_64-linux-gnu/libpthread.so
tests/test-server: tests/CMakeFiles/test-server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test-server"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test-server.dir/build: tests/test-server

.PHONY : tests/CMakeFiles/test-server.dir/build

tests/CMakeFiles/test-server.dir/requires: tests/CMakeFiles/test-server.dir/TestMtpServer.cpp.o.requires

.PHONY : tests/CMakeFiles/test-server.dir/requires

tests/CMakeFiles/test-server.dir/clean:
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/test-server.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test-server.dir/clean

tests/CMakeFiles/test-server.dir/depend:
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests/CMakeFiles/test-server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/test-server.dir/depend

