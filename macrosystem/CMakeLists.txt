
project(macrosystem)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

include_directories(${PROTO_INCLUDE_DIRS})

file(GLOB MACRO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_compile_options(-fPIC)
add_library(${PROJECT_NAME} SHARED
        ${MACRO_SRC}
        )


target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} lua -lpthread -lm -lz)


