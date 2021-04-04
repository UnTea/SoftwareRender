cmake_minimum_required(VERSION 3.17)

project(SoftwareRender)

add_library(Libraries STATIC TGA.hpp TGA.cpp MyGeometry.h WavefrontModel.cpp WavefrontModel.h)
target_include_directories(Libraries PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})