cmake_minimum_required(VERSION 3.22)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(modgen VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(externals/json)

add_definitions( -DMODULE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/libraries/" )
add_definitions( -DLIBDOC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/ps4libdoc/system/common/lib/" )

add_executable(modgen
    main.cpp
)

target_link_libraries(modgen PRIVATE nlohmann_json::nlohmann_json)

