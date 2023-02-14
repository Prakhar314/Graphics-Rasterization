cmake_minimum_required(VERSION 3.0)

project(a1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_library(a1 src/hw.cpp src/sw.cpp)
target_link_libraries(a1 GLEW::GLEW glm::glm OpenGL::GL SDL2::SDL2)

add_executable(e1 examples/e1.cpp)
target_link_libraries(e1 a1)

add_executable(e2 examples/e2.cpp)
target_link_libraries(e2 a1)

add_executable(e3 examples/e3.cpp)
target_link_libraries(e3 a1)

add_executable(e4 examples/e4.cpp)
target_link_libraries(e4 a1)

add_executable(e5 examples/e5.cpp)
target_link_libraries(e5 a1)

add_executable(clock examples/clock.cpp)
target_link_libraries(clock a1)

add_executable(cube examples/cube.cpp)
target_link_libraries(cube a1)