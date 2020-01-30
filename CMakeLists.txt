cmake_minimum_required(VERSION 3.10)

project(compiler)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall")

include_directories(
    compiler/include
    spdlog/include
    cxxopts/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

file(GLOB SOURCES "compiler/src/*.cpp")

bison_target(lang_bison compiler/src/lang.y ${CMAKE_CURRENT_BINARY_DIR}/bison.tab.cpp COMPILE_FLAGS -d)
flex_target(lang_flex compiler/src/lang.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp)
add_flex_bison_dependency(lang_flex lang_bison)

add_executable(cmp
    ${SOURCES}
    ${BISON_lang_bison_OUTPUTS}
    ${FLEX_lang_flex_OUTPUTS}
)
target_link_libraries(cmp
    l
)