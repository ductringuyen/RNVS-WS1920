# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/RNVS1920/Block6

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/RNVS1920/Block6/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ntpclient.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ntpclient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ntpclient.dir/flags.make

CMakeFiles/ntpclient.dir/ntp.c.o: CMakeFiles/ntpclient.dir/flags.make
CMakeFiles/ntpclient.dir/ntp.c.o: ../ntp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/RNVS1920/Block6/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ntpclient.dir/ntp.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ntpclient.dir/ntp.c.o   -c /mnt/d/RNVS1920/Block6/ntp.c

CMakeFiles/ntpclient.dir/ntp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ntpclient.dir/ntp.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/RNVS1920/Block6/ntp.c > CMakeFiles/ntpclient.dir/ntp.c.i

CMakeFiles/ntpclient.dir/ntp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ntpclient.dir/ntp.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/RNVS1920/Block6/ntp.c -o CMakeFiles/ntpclient.dir/ntp.c.s

CMakeFiles/ntpclient.dir/ntpclient.c.o: CMakeFiles/ntpclient.dir/flags.make
CMakeFiles/ntpclient.dir/ntpclient.c.o: ../ntpclient.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/RNVS1920/Block6/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ntpclient.dir/ntpclient.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ntpclient.dir/ntpclient.c.o   -c /mnt/d/RNVS1920/Block6/ntpclient.c

CMakeFiles/ntpclient.dir/ntpclient.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ntpclient.dir/ntpclient.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/RNVS1920/Block6/ntpclient.c > CMakeFiles/ntpclient.dir/ntpclient.c.i

CMakeFiles/ntpclient.dir/ntpclient.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ntpclient.dir/ntpclient.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/RNVS1920/Block6/ntpclient.c -o CMakeFiles/ntpclient.dir/ntpclient.c.s

# Object files for target ntpclient
ntpclient_OBJECTS = \
"CMakeFiles/ntpclient.dir/ntp.c.o" \
"CMakeFiles/ntpclient.dir/ntpclient.c.o"

# External object files for target ntpclient
ntpclient_EXTERNAL_OBJECTS =

ntpclient: CMakeFiles/ntpclient.dir/ntp.c.o
ntpclient: CMakeFiles/ntpclient.dir/ntpclient.c.o
ntpclient: CMakeFiles/ntpclient.dir/build.make
ntpclient: CMakeFiles/ntpclient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/RNVS1920/Block6/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable ntpclient"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ntpclient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ntpclient.dir/build: ntpclient

.PHONY : CMakeFiles/ntpclient.dir/build

CMakeFiles/ntpclient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ntpclient.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ntpclient.dir/clean

CMakeFiles/ntpclient.dir/depend:
	cd /mnt/d/RNVS1920/Block6/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/RNVS1920/Block6 /mnt/d/RNVS1920/Block6 /mnt/d/RNVS1920/Block6/cmake-build-debug /mnt/d/RNVS1920/Block6/cmake-build-debug /mnt/d/RNVS1920/Block6/cmake-build-debug/CMakeFiles/ntpclient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ntpclient.dir/depend

