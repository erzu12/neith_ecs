cmake_minimum_required(VERSION 3.14)
project(NeithECS)

add_executable(NeithECS main.cpp ecs.cpp ecs.h)

# Add any additional dependencies or libraries here
include_directories(.)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Link the ecs library
# target_link_libraries(NeithECS ecs)
