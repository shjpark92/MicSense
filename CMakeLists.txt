# CMakeList for VoiceFusion
cmake_minimum_required(VERSION 3.5)
project(VoiceFusion)

# Linker
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(GCC_COVERAGE_COMPILE_FLAGS "-save-temps")

# CMake path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# Package Finders
find_package(PortAudio REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS})

find_package(SNDFILE REQUIRED)
include_directories(${SNDFILE_INCLUDE_DIRS})

# Included these directories
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

# Set main source
set(SOURCE src/voice_fusion.cpp)

# Executable
add_executable(voice_fusion ${SOURCE})
target_link_libraries(voice_fusion ${PORTAUDIO_LIBRARIES})
target_link_libraries(voice_fusion ${SNDFILE_LIBRARIES})
target_link_libraries(voice_fusion m) # math.h

add_executable(bgm_player src/bgm_player.cpp)
target_link_libraries(bgm_player ${PORTAUDIO_LIBRARIES})
target_link_libraries(bgm_player ${SNDFILE_LIBRARIES})
target_link_libraries(bgm_player m)
