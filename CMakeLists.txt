cmake_minimum_required(VERSION 3.28)
include("$ENV{IDF_PATH}/tools/cmake/project.cmake")
project(auto_floring)

idf_build_set_property(COMPILE_OPTIONS "-Wall" APPEND)