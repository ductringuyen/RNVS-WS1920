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
CMAKE_COMMAND = /home/viet/viet.Programmierung/C/CLion/clion-2019.3.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/viet/viet.Programmierung/C/CLion/clion-2019.3.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/peer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/peer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/peer.dir/flags.make

CMakeFiles/peer.dir/peer.c.o: CMakeFiles/peer.dir/flags.make
CMakeFiles/peer.dir/peer.c.o: ../peer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/peer.dir/peer.c.o"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/peer.dir/peer.c.o   -c /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/peer.c

CMakeFiles/peer.dir/peer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/peer.dir/peer.c.i"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/peer.c > CMakeFiles/peer.dir/peer.c.i

CMakeFiles/peer.dir/peer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/peer.dir/peer.c.s"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/peer.c -o CMakeFiles/peer.dir/peer.c.s

CMakeFiles/peer.dir/hashing.c.o: CMakeFiles/peer.dir/flags.make
CMakeFiles/peer.dir/hashing.c.o: ../hashing.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/peer.dir/hashing.c.o"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/peer.dir/hashing.c.o   -c /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/hashing.c

CMakeFiles/peer.dir/hashing.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/peer.dir/hashing.c.i"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/hashing.c > CMakeFiles/peer.dir/hashing.c.i

CMakeFiles/peer.dir/hashing.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/peer.dir/hashing.c.s"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/hashing.c -o CMakeFiles/peer.dir/hashing.c.s

CMakeFiles/peer.dir/dht.c.o: CMakeFiles/peer.dir/flags.make
CMakeFiles/peer.dir/dht.c.o: ../dht.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/peer.dir/dht.c.o"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/peer.dir/dht.c.o   -c /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/dht.c

CMakeFiles/peer.dir/dht.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/peer.dir/dht.c.i"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/dht.c > CMakeFiles/peer.dir/dht.c.i

CMakeFiles/peer.dir/dht.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/peer.dir/dht.c.s"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/dht.c -o CMakeFiles/peer.dir/dht.c.s

# Object files for target peer
peer_OBJECTS = \
"CMakeFiles/peer.dir/peer.c.o" \
"CMakeFiles/peer.dir/hashing.c.o" \
"CMakeFiles/peer.dir/dht.c.o"

# External object files for target peer
peer_EXTERNAL_OBJECTS =

peer: CMakeFiles/peer.dir/peer.c.o
peer: CMakeFiles/peer.dir/hashing.c.o
peer: CMakeFiles/peer.dir/dht.c.o
peer: CMakeFiles/peer.dir/build.make
peer: CMakeFiles/peer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable peer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/peer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/peer.dir/build: peer

.PHONY : CMakeFiles/peer.dir/build

CMakeFiles/peer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/peer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/peer.dir/clean

CMakeFiles/peer.dir/depend:
	cd /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug /home/viet/viet.Programmierung/C/RNVS-WS1920/jft.DHT/cmake-build-debug/CMakeFiles/peer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/peer.dir/depend

