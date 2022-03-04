#
# Copyright (c) 2022 Circuit Dojo LLC
#

add_subdirectory(app_ble)

target_include_directories(app PRIVATE .)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app_ble.c)