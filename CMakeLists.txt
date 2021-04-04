cmake_minimum_required(VERSION 3.17)
project(SoftwareRender)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Libraries)
set(SOURCE Sources/main.cpp)
add_executable(SoftwareRender ${SOURCE})
target_link_libraries(SoftwareRender Libraries)