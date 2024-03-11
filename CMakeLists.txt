cmake_minimum_required(VERSION 3.0)
project(miniDebugger)

add_compile_options(-std=c++14)

include_directories(dependencies/libelfin
                    dependencies/linenoise
                    include)

aux_source_directory(src)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(miniDebugger
                    ${PROJECT_SOURCE_DIR}/dependencies/libelfin/dwarf/libdwarf++.so
                    ${PROJECT_SOURCE_DIR}/dependencies/libelfin/elf/libelf++.so
                    )   