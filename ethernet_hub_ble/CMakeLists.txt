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

# Overlays
list(INSERT OVERLAY_CONFIG 0 config/golioth.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(air-quality-wing-demo)

# Application directories
add_subdirectory(src/ble)
add_subdirectory(src/event_manager)
add_subdirectory(src/network)

target_sources(app PRIVATE src/main.c)
