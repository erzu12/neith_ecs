include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

file(GLOB_RECURSE TEST_DIR
    "*.cpp"
)

add_executable(${PROJECT_NAME}_test
    ${TEST_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}_test
    ${PROJECT_NAME}
    GTest::gtest_main
)

target_include_directories(${PROJECT_NAME}_test PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
