#to compile the project:
# mkdir _build
# cd _build
# cmake -D CMAKE_TOOLCHAIN_FILE=../../sys/cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug ..
# make
# make flash
cmake_minimum_required(VERSION 3.5)

#with that, cmake will not try to compile a simple test program
# (that may fail for an embedded target)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

#project name
project(2_blinkInt)

#application sources
set(SRCS
	    ${CMAKE_SOURCE_DIR}/main.cpp
)

include("../sys/cmake/labs.cmake")
