#
# Copyright (c) 2022 Circuit Dojo LLC
#

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/scan.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gatt.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/services/aqw_c.c)