#
# Copyright (c) 2021 Circuit Dojo LLC
#

cmake_minimum_required(VERSION 3.13.4)

# Determine the build type
if (NOT BUILD_TYPE)
  set(BUILD_TYPE debug)
endif()

# Print out build type
message(STATUS "Build type: ${BUILD_TYPE} 🚀")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(air-quality-wing-demo)

target_sources(app PRIVATE src/main.c)
