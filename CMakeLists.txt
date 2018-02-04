# CMakeList for VoiceFusion
cmake_minimum_required(VERSION 3.5)
project(VoiceFusion)

# CMake path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# Package Finders
find_package(PortAudio REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS})

find_package(FFTW3 REQUIRED COMPONENTS fftw3f)
include_directories(${FFTW3_INCLUDE_DIR})

# Executable
add_executable(voicefusion src/voice_fusion.cpp)
target_link_libraries(voicefusion m)
target_link_libraries(voicefusion ${PORTAUDIO_LIBRARIES})
target_link_libraries(voicefusion ${FFTW3_LIBRARIES})
