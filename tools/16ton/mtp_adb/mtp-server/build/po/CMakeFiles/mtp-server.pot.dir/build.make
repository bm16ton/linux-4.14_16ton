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

# Utility rule file for mtp-server.pot.

# Include the progress variables for this target.
include po/CMakeFiles/mtp-server.pot.dir/progress.make

mtp-server.pot: po/CMakeFiles/mtp-server.pot.dir/build.make
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/po && /usr/bin/xgettext --c++ --qt --add-comments=TRANSLATORS --keyword=tr --keyword=tr:1,2 -D /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server -s -p /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/po -o mtp-server.pot server/server.cpp
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/po && /usr/bin/msgmerge /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/po/fr.po /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/po/mtp-server.pot -o /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/po/fr.po
.PHONY : mtp-server.pot

# Rule to build all files generated by this target.
po/CMakeFiles/mtp-server.pot.dir/build: mtp-server.pot

.PHONY : po/CMakeFiles/mtp-server.pot.dir/build

po/CMakeFiles/mtp-server.pot.dir/clean:
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/po && $(CMAKE_COMMAND) -P CMakeFiles/mtp-server.pot.dir/cmake_clean.cmake
.PHONY : po/CMakeFiles/mtp-server.pot.dir/clean

po/CMakeFiles/mtp-server.pot.dir/depend:
	cd /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/po /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/po /home/maddocks/kernels/linux-gpd-4.14.14/tools/16ton/mtp_adb/mtp-server/build/po/CMakeFiles/mtp-server.pot.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : po/CMakeFiles/mtp-server.pot.dir/depend

