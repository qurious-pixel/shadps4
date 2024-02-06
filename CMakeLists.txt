cmake_minimum_required(VERSION 3.22)

project(shadps4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)
qt_standard_project_setup()


add_subdirectory(externals/cryptopp)
add_subdirectory(src)

