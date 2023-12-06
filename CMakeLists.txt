cmake_minimum_required(VERSION 3.15)
project(ogl)

set(CMAKE_CXX_STANDARD 14)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Configure assets header file
configure_file(src/engine/helpers/root_dir.h.in src/engine/helpers/root_dir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "OpenGL included at ${OpenGL_INCLUDE_DIR}")
message(STATUS "Found OpenGL: ${OpenGL_LIBRARY}")

# GLM
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
message(STATUS "Found GLM: ${GLM_LIBRARY}")

# GLFW
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
message(STATUS "Found GLFW3: ${GLFW3_LIBRARY}")

# ASSIMP
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
message(STATUS "Found ASSIMP: ${ASSIMP_LIBRARY}")

# STB_IMAGE
add_library(STB_IMAGE "thirdparty/stb_image/stb_image.cpp")
# GLAD
add_library(GLAD "thirdparty/glad/glad.c")

# Put all libraries into a variable
set(LIBS ${ASSIMP_LIBRARY} ${GLFW3_LIBRARY} STB_IMAGE GLAD)

# Define the include DIRs
include_directories("/usr/local/include")
include_directories(
        "src"
        "src/engine"
        "thirdparty/glad/include"
        "thirdparty/stb_image"
        "thirdparty/spdlog/include"
        ${GLM_INCLUDE_DIR}
        ${GLFW3_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
)
include_directories("thirdparty")

message("cmake_module_path: " ${CMAKE_MODULE_PATH})

file(GLOB_RECURSE rttr_cpp "thirdparty/rttr/*.cpp")

add_executable(${PROJECT_NAME} ${rttr_cpp} ${SOURCE_FILES})

# Define the link libraries
target_link_libraries(${PROJECT_NAME} "-framework OpenGL" ${LIBS})

add_subdirectory(tools)