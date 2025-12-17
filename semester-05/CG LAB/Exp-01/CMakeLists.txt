cmake_minimum_required(VERSION 3.10)
project(OpenGLDemo)

set(CMAKE_CXX_STANDARD 17)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

add_executable(OpenGLDemo main.cpp)

target_link_libraries(OpenGLDemo
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARY}
)
