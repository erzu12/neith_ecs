file(GLOB_RECURSE SRC_DIR
    "include/ecs.h"
)

add_library(${PROJECT_NAME} STATIC
    ${SRC_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
