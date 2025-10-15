cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(AllTutorials)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_subdirectory(glsdk)
add_subdirectory(framework)

function(add_tutorial base_dir tgt_name)
    set(sources ${ARGN})
    list(TRANSFORM sources PREPEND "${base_dir}/")
    add_executable(${tgt_name} ${sources})
    target_link_libraries(${tgt_name} PRIVATE framework)
endfunction()

## copy all data files to the build directory
file(COPY "data" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB data_dirs "Tut*/data")
foreach(data_dir ${data_dirs})
    file(COPY "${data_dir}" DESTINATION "${CMAKE_BINARY_DIR}")
endforeach()

## Tutorials
add_tutorial("Tut 01 Hello Triangle" "Tut01" "Tut1.cpp")
