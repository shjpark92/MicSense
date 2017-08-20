# CMakeList for MicSense
cmake_minimum_required(VERSION 3.8)
project(MicSense)

# CMake path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# Package Finders
find_package(PortAudio REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS})

find_package(FFTW3 REQUIRED COMPONENTS fftw3f)
include_directories(${FFTW3_INCLUDE_DIR})

# Executable
add_executable(micsense src/micsense.cpp)
target_link_libraries(micsense ${PORTAUDIO_LIBRARIES})
target_link_libraries(micsense ${FFTW3_LIBRARIES})
