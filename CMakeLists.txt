cmake_minimum_required(VERSION 3.0)
project(miniDebugger)

add_compile_options(-std=c++14)

include_directories(dependencies/libelfin
                    dependencies/linenoise
                    include)

aux_source_directory(src/ ALL_SRC)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# target_link_libraries(miniDebugger
#                     ${PROJECT_SOURCE_DIR}/dependencies/libelfin/dwarf/libdwarf++.so
#                     ${PROJECT_SOURCE_DIR}/dependencies/libelfin/elf/libelf++.so)
target_link_libraries(
                    ${PROJECT_SOURCE_DIR}/dependencies/libelfin/dwarf/libdwarf++.so
                    ${PROJECT_SOURCE_DIR}/dependencies/libelfin/elf/libelf++.so)

add_executable(hello examples/hello.cc)
set_target_properties(hello
                      PROPERTIES COMPILE_FLAGS "-g -O0")
add_executable(miniDebugger
                ${ALL_SRC}
                dependencies/linenoise/linenoise.c)