cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET x64-linux CACHE STRING "Vcpkg target triplet")

project(btester LANGUAGES CXX VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-larmadillo")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message("gnu")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "-pedantic -Wall -Wextra -Wno-sign-compare -Wconversion -O3")
endif ()

# for saving assembly output
# set(CMAKE_CXX_FLAGS "-save-temps -fverbose-asm -larmadillo")

# for saving profiling output
# set(CMAKE_CXX_FLAGS "-pg -larmadillo")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/71140c3ca7a87bb1b5b9c9f1500fea8858cce344.zip
)

FetchContent_MakeAvailable(googletest)

include(CTest)

add_subdirectory(lib)
add_subdirectory(tests)
include_directories(${SHARED_LIB_INCLUDE_DIR})

add_executable(btester main.cpp)

target_link_libraries(btester PRIVATE lib)