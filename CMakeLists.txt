cmake_minimum_required(VERSION 3.20)
project(kralica LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

find_package(OpenGL REQUIRED)

# ===================================================================== GLAD
# Headers live in  libs/glad/glad.h (or glad/gl.h)  +  libs/KHR/khrplatform.h
# so "libs/" must be on the include path (khrplatform is included as <KHR/...>).

# --- locate a compiled GLAD source file ---------------------------------
file(GLOB GLAD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad.c")

# --- locate the public header to know which flavour we have -------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/glad.h")
    set(GLAD_FLAVOUR "1")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/gl.h")
    set(GLAD_FLAVOUR "2")
else()
    message(FATAL_ERROR
        "No GLAD header found. Expected libs/glad/glad.h (GLAD 1) "
        "or libs/glad/gl.h (GLAD 2).")
endif()

# --- if there is no .c file, synthesise one via the implementation macro -
if(NOT GLAD_SOURCES)
    if(GLAD_FLAVOUR STREQUAL "2")
        set(_glad_impl_macro "GLAD_GL_IMPLEMENTATION")
    else()
        message(FATAL_ERROR
            "GLAD 1 needs libs/glad/glad.c; only libs/glad/glad.h was found. "
            "Re-generate GLAD with 'Generate a loader' ticked, or switch to GLAD 2.")
    endif()

    set(_glad_impl_file "${CMAKE_CURRENT_BINARY_DIR}/glad_impl.c")
    file(WRITE "${_glad_impl_file}"
"#define ${_glad_impl_macro}\n#include <glad/gl.h>\n")
    set(GLAD_SOURCES "${_glad_impl_file}")
    message(STATUS "GLAD: no source file in libs/glad/, using header implementation.")
endif()

add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs")
target_link_libraries(glad PUBLIC OpenGL::GL)

# ===================================================================== GLFW
include(FetchContent)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(glfw)

# ====================================================================== GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(glm)

# ================================================================ EXECUTABLE
add_executable(kralica
    src/main-ai.cpp
    src/window-ai.cpp
    src/shader-ai.cpp
    src/camera-ai.cpp
    src/renderer-ai.cpp)

target_include_directories(kralica PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"        
    "${CMAKE_CURRENT_SOURCE_DIR}/libs") 

target_link_libraries(kralica PRIVATE
    glad
    glfw
    glm::glm
    OpenGL::GL)

target_compile_definitions(kralica PRIVATE
    SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# Warnings
if(MSVC)
    target_compile_options(kralica PRIVATE /W4)
else()
    target_compile_options(kralica PRIVATE -Wall -Wextra)
endif()