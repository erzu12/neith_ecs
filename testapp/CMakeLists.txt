
file(GLOB_RECURSE APP_DIR
    "*.cpp"
    )

add_executable(testapp
    ${APP_DIR}
    )


set_target_properties(testapp PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
    testapp
    ${PROJECT_NAME}
)

target_include_directories(testapp PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)
