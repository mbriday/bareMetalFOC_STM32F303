#to compile the project:
# mkdir _build
# cd _build
# cmake -D CMAKE_TOOLCHAIN_FILE=../../sys/cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug ..
# make
# make flash
cmake_minimum_required(VERSION 3.5)

#project name
project(5_SVPWM)

#application sources
set(SRCS
	    ${CMAKE_SOURCE_DIR}/main.cpp
		${CMAKE_SOURCE_DIR}/svpwmtester.cpp
)

include("../sys/cmake/labs.cmake")
