macro(check_c99)
    set(CMAKE_C_STANDARD 99)
endmacro(check_c99)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CMake)

check_c99()

find_package(PortAudio REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS})

find_package(FFTW3 REQUIRED COMPONENTS fftw3f)
include_directories(${FFTW3_INCLUDE_DIR})

add_library(bgm_player bgm_player.cpp)
