cmake_minimum_required(VERSION 3.14)
project(NeithECS)


set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(src)
add_subdirectory(test)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/testapp")
    add_subdirectory(testapp)
endif()
