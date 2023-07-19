cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET x64-linux CACHE STRING "Vcpkg target triplet")

project(btester)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
include_directories(${SHARED_LIB_INCLUDE_DIR})

add_executable(btester main.cpp)

target_link_libraries(btester PRIVATE lib)