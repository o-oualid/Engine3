cmake_minimum_required(VERSION 3.15)
project(mygame)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/arch:AVX2)
else ()
    add_compile_options(-mavx2)
endif ()

set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build")

find_package(Vulkan REQUIRED)


add_executable(mygame WIN32 Source/win32_vulkan_main.cpp)

target_include_directories(mygame PRIVATE "Source")
target_link_libraries(mygame Vulkan::Vulkan)
