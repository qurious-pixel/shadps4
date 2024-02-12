# CryptoPP
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MSVC)
    # If it is clang and MSVC we will add a static lib
    add_subdirectory(cryptoppwin EXCLUDE_FROM_ALL)
    target_include_directories(cryptoppwin INTERFACE cryptoppwin/include)
else()
    set(CRYPTOPP_BUILD_TESTING OFF)
    set(CRYPTOPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cryptopp/)
    add_subdirectory(cryptopp-cmake EXCLUDE_FROM_ALL)
endif()

# Zlib-Ng
set(ZLIB_ENABLE_TESTS OFF)
set(WITH_GTEST OFF)
set(WITH_NEW_STRATEGIES ON)
set(WITH_NATIVE_INSTRUCTIONS ON)
add_subdirectory(zlib-ng)
