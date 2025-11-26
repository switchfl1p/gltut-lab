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

add_tutorial("Tut 02 Playing with Colors" "Tut02FragPosition" "FragPosition.cpp")
add_tutorial("Tut 02 Playing with Colors" "Tut02VertexColors" "VertexColors.cpp")

add_tutorial("Tut 03 OpenGLs Moving Triangle" "Tut03CPUPositionOffset" "CPUPositionOffset.cpp")
add_tutorial("Tut 03 OpenGLs Moving Triangle" "Tut03ShaderPositionOffset" "VertPositionOffset.cpp")
add_tutorial("Tut 03 OpenGLs Moving Triangle" "Tut03ShaderCalcOffset" "VertCalcOffset.cpp")
add_tutorial("Tut 03 OpenGLs Moving Triangle" "Tut03FragmentChangeColor" "FragChangeColor.cpp")

add_tutorial("Tut 04 Objects at Rest" "Tut04OrthographicCube" "OrthoCube.cpp")
add_tutorial("Tut 04 Objects at Rest" "Tut04ShaderPerspective" "ShaderPerspective.cpp")
add_tutorial("Tut 04 Objects at Rest" "Tut04MatrixPerspective" "MatrixPerspective.cpp")
add_tutorial("Tut 04 Objects at Rest" "Tut04AspectRatio" "AspectRatio.cpp")

add_tutorial("Tut 05 Objects in Depth" "Tut05OverlapNoDepth" "OverlapNoDepth.cpp")
add_tutorial("Tut 05 Objects in Depth" "Tut05BaseVertexWithOverlap" "BaseVertexOverlap.cpp")
add_tutorial("Tut 05 Objects in Depth" "Tut05DepthBuffering" "DepthBuffer.cpp")
add_tutorial("Tut 05 Objects in Depth" "Tut05VertexClipping" "VertexClipping.cpp")
add_tutorial("Tut 05 Objects in Depth" "Tut05DepthClamping" "DepthClamping.cpp")
add_tutorial("Tut 05 Objects in Depth" "Tut05DepthFighting" "DepthFighting.cpp")

add_tutorial("Tut 06 Objects in Motion" "Tut06Translation" "Translation.cpp")
add_tutorial("Tut 06 Objects in Motion" "Tut06Scale" "Scale.cpp")
add_tutorial("Tut 06 Objects in Motion" "Tut06Rotations" "Rotations.cpp")
add_tutorial("Tut 06 Objects in Motion" "Tut06Hierarchy" "Hierarchy.cpp")
add_tutorial("Tut 06 Objects in Motion" "Practice" "Practice.cpp")
add_tutorial("Tut 06 Objects in Motion" "HierarchyRewritten" "HierarchyRewritten.cpp")
add_tutorial("Tut 06 Objects in Motion" "HierarchyNoMStack" "HierarchyNoMStack.cpp")

add_tutorial("Tut 07 World in Motion" "Tut07WorldScene" "WorldScene.cpp")
add_tutorial("Tut 07 World in Motion" "Tut07WorldWithUBO" "WorldWithUBO.cpp")
add_tutorial("Tut 07 World in Motion" "FoldedMatrix" "FoldedMatrix.cpp")
add_tutorial("Tut 07 World in Motion" "FoldedMatrix2" "FoldedMatrix2.cpp")

add_tutorial("Tut 08 Getting Oriented" "Tut08GimbalLock" "GimbalLock.cpp")
add_tutorial("Tut 08 Getting Oriented" "Tut08QuaternionYPR" "QuaternionYPR.cpp")
add_tutorial("Tut 08 Getting Oriented" "Tut08CameraRelative" "CameraRelative.cpp")
add_tutorial("Tut 08 Getting Oriented" "Tut08Interpolation" "Interpolation.cpp")
add_tutorial("Tut 08 Getting Oriented" "Tut08Ex1" "Ex1.cpp")
