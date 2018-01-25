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
include tests/CMakeFiles/test-utils.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test-utils.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test-utils.dir/flags.make

tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o: tests/CMakeFiles/test-utils.dir/flags.make
tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o: ../tests/TestMtpUtils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && /usr/bin/ccache  g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o -c /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests/TestMtpUtils.cpp

tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-utils.dir/TestMtpUtils.cpp.i"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && /usr/bin/ccache  g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests/TestMtpUtils.cpp > CMakeFiles/test-utils.dir/TestMtpUtils.cpp.i

tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-utils.dir/TestMtpUtils.cpp.s"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && /usr/bin/ccache  g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests/TestMtpUtils.cpp -o CMakeFiles/test-utils.dir/TestMtpUtils.cpp.s

tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.requires:

.PHONY : tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.requires

tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.provides: tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/test-utils.dir/build.make tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.provides.build
.PHONY : tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.provides

tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.provides.build: tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o


# Object files for target test-utils
test__utils_OBJECTS = \
"CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o"

# External object files for target test-utils
test__utils_EXTERNAL_OBJECTS =

tests/test-utils: tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o
tests/test-utils: tests/CMakeFiles/test-utils.dir/build.make
tests/test-utils: libmtpserver.so.1.0.0
tests/test-utils: libusbhost/libusbhost.a
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_thread.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_system.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
tests/test-utils: /usr/lib/x86_64-linux-gnu/libpthread.so
tests/test-utils: tests/CMakeFiles/test-utils.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test-utils"
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-utils.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test-utils.dir/build: tests/test-utils

.PHONY : tests/CMakeFiles/test-utils.dir/build

tests/CMakeFiles/test-utils.dir/requires: tests/CMakeFiles/test-utils.dir/TestMtpUtils.cpp.o.requires

.PHONY : tests/CMakeFiles/test-utils.dir/requires

tests/CMakeFiles/test-utils.dir/clean:
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/test-utils.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test-utils.dir/clean

tests/CMakeFiles/test-utils.dir/depend:
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/tests /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/tests/CMakeFiles/test-utils.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/test-utils.dir/depend

