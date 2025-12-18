cmake_minimum_required(VERSION 3.16)
project(CRT_Simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL and GLUT
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Primary executables (canonical file names)
if(EXISTS "${CMAKE_SOURCE_DIR}/CRT_2D.cpp")
    add_executable(CRT_2D CRT_2D.cpp)
    target_link_libraries(CRT_2D ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GLU)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/CRT_3D.cpp")
    add_executable(CRT_3D CRT_3D.cpp)
    target_link_libraries(CRT_3D ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GLU)
endif()

# Optional executables for legacy filenames with spaces
if(EXISTS "${CMAKE_SOURCE_DIR}/2D CRT Version.cpp")
    add_executable(CRT_2D_Legacy "2D CRT Version.cpp")
    target_link_libraries(CRT_2D_Legacy ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GLU)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/3D CRT Version.cpp")
    add_executable(CRT_3D_Legacy "3D CRT Version.cpp")
    target_link_libraries(CRT_3D_Legacy ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} GLU)
endif()
