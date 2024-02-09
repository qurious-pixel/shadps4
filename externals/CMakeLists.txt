
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MSVC)
#if it is clang and MSVC we will add a static lib
else() # all the other cases probably could do build it
	set(CRYPTOPP_BUILD_TESTING OFF)
	set(CRYPTOPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cryptopp/)
	add_subdirectory(cryptopp-cmake EXCLUDE_FROM_ALL)
endif()
