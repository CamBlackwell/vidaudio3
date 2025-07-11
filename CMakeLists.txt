cmake_minimum_required(VERSION 3.0)

if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  set(OpenCV_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/share/opencv")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/share/opencv")
else()
  message(FATAL_ERROR "Couldn't find vcpkg. Please either:\n"
          "1. Install vcpkg and set the VCPKG_ROOT environment variable, or\n"
          "2. Initialize the vcpkg submodule with 'git submodule update --init'")
endif()

project(vidaudio3)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(RtAudio CONFIG REQUIRED) 
include_directories(${OpenCV_INCLUDE_DIRS})

find_program(CLANG_TIDY_EXECUTABLE clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})
endif()

add_executable(vidaudio3 main.cpp webcam_info.cpp infotoaudio.cpp synth.cpp scales.h)
target_link_libraries(vidaudio3 ${OpenCV_LIBS}  RtAudio::rtaudio)