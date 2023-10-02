cmake_minimum_required(VERSION 3.14)
project(NeithECS)

file(GLOB_RECURSE SRC_DIR
    "src/*.cpp"
    "src/*.h"
)

file(GLOB_RECURSE TEST_DIR
    "test/*.cpp"
    "test/*.h"
)

add_executable(${PROJECT_NAME}
    ${SRC_DIR}
)

# Add any additional dependencies or libraries here
include_directories(.)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)

# Link the ecs library
# target_link_libraries(NeithECS ecs)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(${PROJECT_NAME}_test
    ${TEST_DIR}
)
target_link_libraries(
    ${PROJECT_NAME}_test
    GTest::gtest_main
)

target_include_directories(${PROJECT_NAME}_test PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
