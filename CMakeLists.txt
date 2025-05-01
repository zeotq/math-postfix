cmake_minimum_required(VERSION 3.10)
project(LR24)

include(FetchContent)

FetchContent_Declare(
    structures
    GIT_REPOSITORY https://github.com/zeotq/structures.git
    GIT_TAG dev
)

FetchContent_MakeAvailable(structures)

file(GLOB SOURCES "src/*.c")
add_executable(my_app main.c ${SOURCES})

target_include_directories(my_app PRIVATE 
    include
    ${structures_SOURCE_DIR}/include
)

target_link_libraries(my_app PRIVATE structures m)