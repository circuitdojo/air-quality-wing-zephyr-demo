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

# Adding custom overlay to add settings that aren't included in the SDK (yet) for the nRF9160 Feather
if(${BOARD} STREQUAL circuitdojo_feather_nrf9160ns)
message(STATUS "Adding .overlay for mcuboot.")
list(APPEND mcuboot_DTC_OVERLAY_FILE
  "${CMAKE_CURRENT_SOURCE_DIR}/mcuboot/circuitdojo_feather_nrf9160ns.overlay"
  )
endif()

# Set environment variables
set(ZEPHYR_TOOLCHAIN_VARIANT gnuarmemb)
# set(GNUARMEMB_TOOLCHAIN_PATH "C:\\Program Files (x86)\\GNU Tools Arm Embedded\\9 2019-q4-major")
set(GNUARMEMB_TOOLCHAIN_PATH /Users/jaredwolff/gcc-arm-none-eabi-9-2019-q4-major)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(air-quality-wing-demo)

target_sources(app PRIVATE src/main.c)
