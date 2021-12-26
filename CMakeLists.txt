cmake_minimum_required(VERSION 3.15)
project(ogl)

set(CMAKE_CXX_STANDARD 14)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/thirdparty/imgui/*.cpp
        )

# Add header files
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

# Configure assets header file
configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

# Add .lib files
link_directories(${CMAKE_SOURCE_DIR}/lib)

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "OpenGL included at ${OpenGL_INCLUDE_DIR}")
message(STATUS "Found OpenGL: ${OpenGL_LIBRARY}")

# GLFW
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
message(STATUS "Found GLFW3: ${GLFW3_LIBRARY}")

# ASSIMP
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
message(STATUS "Found ASSIMP: ${ASSIMP_LIBRARY}")

# OPENCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# STB_IMAGE
add_library(STB_IMAGE "thirdparty/stb_image.cpp")

# GLAD
add_library(GLAD "thirdparty/glad.c")

# Put all libraries into a variable
set(LIBS ${ASSIMP_LIBRARY} ${GLFW3_LIBRARY} STB_IMAGE GLAD)

# Define the include DIRs
include_directories(
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/thirdparty/glm"
        "${CMAKE_SOURCE_DIR}/thirdparty/imgui"
        "${CMAKE_SOURCE_DIR}/thirdparty/imgui/backends"
        ${GLFW3_INCLUDE_DIR}
)

message("cmake_module_path: " ${CMAKE_MODULE_PATH})

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} src/filter/IFilter.h src/filter/SoulOut.cpp src/filter/SoulOut.h)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} "-framework OpenGL" ${LIBS} ${OpenCV_LIBS})