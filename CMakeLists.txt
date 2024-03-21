cmake_minimum_required(VERSION 3.0)
project(miniDebugger)

add_compile_options(-std=c++14)

include_directories(dependencies/libelfin
                    dependencies/linenoise
                    include)

aux_source_directory(src/ ALL_SRC)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hello examples/hello.cc)
set_target_properties(hello PROPERTIES COMPILE_FLAGS "-gdwarf-2 -O0 -fno-omit-frame-pointer")
add_executable(unwinding examples/stackUnwinding.cc)
set_target_properties(unwinding PROPERTIES COMPILE_FLAGS "-gdwarf-2 -O0 -fno-omit-frame-pointer")

add_executable(miniDebugger ${ALL_SRC} dependencies/linenoise/linenoise.c)

# Link miniDebugger with libdwarf++.so and libelf++.so
target_link_libraries(miniDebugger PRIVATE
    /home/ccy/Desktop/miniDebugger/dependencies/libelfin/dwarf/libdwarf++.so 
    /home/ccy/Desktop/miniDebugger/dependencies/libelfin/elf/libelf++.so)
