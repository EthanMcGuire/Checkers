# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Users\Ethan Mcguire\Desktop\Programming-Stuff\cmake-3.25.0-windows-x86_64\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Users\Ethan Mcguire\Desktop\Programming-Stuff\cmake-3.25.0-windows-x86_64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\Ethan Mcguire\Desktop\SFML-2.5.1"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\Ethan Mcguire\Desktop\Build"

# Include any dependencies generated for this target.
include src/SFML/Main/CMakeFiles/sfml-main.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/SFML/Main/CMakeFiles/sfml-main.dir/compiler_depend.make

# Include the progress variables for this target.
include src/SFML/Main/CMakeFiles/sfml-main.dir/progress.make

# Include the compile flags for this target's objects.
include src/SFML/Main/CMakeFiles/sfml-main.dir/flags.make

src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj: src/SFML/Main/CMakeFiles/sfml-main.dir/flags.make
src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj: src/SFML/Main/CMakeFiles/sfml-main.dir/includes_CXX.rsp
src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj: C:/Users/Ethan\ Mcguire/Desktop/SFML-2.5.1/src/SFML/Main/MainWin32.cpp
src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj: src/SFML/Main/CMakeFiles/sfml-main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\Ethan Mcguire\Desktop\Build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj"
	cd /d C:\Users\ETHANM~1\Desktop\Build\src\SFML\Main && C:\mingw32\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj -MF CMakeFiles\sfml-main.dir\MainWin32.cpp.obj.d -o CMakeFiles\sfml-main.dir\MainWin32.cpp.obj -c "C:\Users\Ethan Mcguire\Desktop\SFML-2.5.1\src\SFML\Main\MainWin32.cpp"

src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sfml-main.dir/MainWin32.cpp.i"
	cd /d C:\Users\ETHANM~1\Desktop\Build\src\SFML\Main && C:\mingw32\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Ethan Mcguire\Desktop\SFML-2.5.1\src\SFML\Main\MainWin32.cpp" > CMakeFiles\sfml-main.dir\MainWin32.cpp.i

src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sfml-main.dir/MainWin32.cpp.s"
	cd /d C:\Users\ETHANM~1\Desktop\Build\src\SFML\Main && C:\mingw32\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Ethan Mcguire\Desktop\SFML-2.5.1\src\SFML\Main\MainWin32.cpp" -o CMakeFiles\sfml-main.dir\MainWin32.cpp.s

# Object files for target sfml-main
sfml__main_OBJECTS = \
"CMakeFiles/sfml-main.dir/MainWin32.cpp.obj"

# External object files for target sfml-main
sfml__main_EXTERNAL_OBJECTS =

lib/libsfml-main.a: src/SFML/Main/CMakeFiles/sfml-main.dir/MainWin32.cpp.obj
lib/libsfml-main.a: src/SFML/Main/CMakeFiles/sfml-main.dir/build.make
lib/libsfml-main.a: src/SFML/Main/CMakeFiles/sfml-main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Users\Ethan Mcguire\Desktop\Build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ..\..\..\lib\libsfml-main.a"
	cd /d C:\Users\ETHANM~1\Desktop\Build\src\SFML\Main && $(CMAKE_COMMAND) -P CMakeFiles\sfml-main.dir\cmake_clean_target.cmake
	cd /d C:\Users\ETHANM~1\Desktop\Build\src\SFML\Main && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\sfml-main.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/SFML/Main/CMakeFiles/sfml-main.dir/build: lib/libsfml-main.a
.PHONY : src/SFML/Main/CMakeFiles/sfml-main.dir/build

src/SFML/Main/CMakeFiles/sfml-main.dir/clean:
	cd /d C:\Users\ETHANM~1\Desktop\Build\src\SFML\Main && $(CMAKE_COMMAND) -P CMakeFiles\sfml-main.dir\cmake_clean.cmake
.PHONY : src/SFML/Main/CMakeFiles/sfml-main.dir/clean

src/SFML/Main/CMakeFiles/sfml-main.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\Ethan Mcguire\Desktop\SFML-2.5.1" "C:\Users\Ethan Mcguire\Desktop\SFML-2.5.1\src\SFML\Main" "C:\Users\Ethan Mcguire\Desktop\Build" "C:\Users\Ethan Mcguire\Desktop\Build\src\SFML\Main" "C:\Users\Ethan Mcguire\Desktop\Build\src\SFML\Main\CMakeFiles\sfml-main.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : src/SFML/Main/CMakeFiles/sfml-main.dir/depend

