cmake_minimum_required(VERSION 3.10)

project(Tape CXX)
set(CMAKE_CXX_STANDARD 17)

# This condition is used for Windows
if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

# Form the path to the folder in the build directory
set(folder_path "${CMAKE_BINARY_DIR}/tmp")

# Check if the folder exists
if(NOT EXISTS "${folder_path}")
    # If the folder does not exist, create it
    file(MAKE_DIRECTORY "${folder_path}")
endif()


set(JSON_FILES json.h json.cpp)
set(GENERATOR_FILES File_generator.cpp File_generator.h)
set(SORT_FILES Sorter.cpp Sorter.h)
set(TAPE_FILES Tape.cpp Tape.h)

add_executable(tape main.cpp ${JSON_FILES} ${GENERATOR_FILES} ${SORT_FILES} ${TAPE_FILES} Tests.h)