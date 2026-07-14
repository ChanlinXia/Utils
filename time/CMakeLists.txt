cmake_minimum_required(VERSION 3.20)

project(time C)

# ============================================================
# Source Files
# ============================================================

set(TIME_SRCS
    system_time/system_time.c
#    delay/delay.c
#    clock/clock.c
)

# ============================================================
# Include Directories
# ============================================================

set(TIME_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/timestamp
    ${CMAKE_CURRENT_SOURCE_DIR}/system_time
#    ${CMAKE_CURRENT_SOURCE_DIR}/delay
#    ${CMAKE_CURRENT_SOURCE_DIR}/clock
)

# ============================================================
# Library
# ============================================================

add_library(time STATIC
    ${TIME_SRCS}
)

target_include_directories(time
    PUBLIC
    ${TIME_INCLUDES}
)

target_link_libraries(time
    PUBLIC
#    bsp
)