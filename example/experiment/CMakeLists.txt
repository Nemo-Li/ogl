cmake_minimum_required(VERSION 3.15)
project(experiment)

set(CMAKE_CXX_STANDARD 17)

add_executable(demo_particle particle.cpp shader.cpp Texture.cpp control.cpp)
target_link_libraries(demo_particle "-framework OpenGL" ${GLFW3_LIBRARY} GLAD ${GLM_LIBRARY} STB_IMAGE)