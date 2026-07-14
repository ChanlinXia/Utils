cmake_minimum_required(VERSION 3.20)

project(Utils C)

add_subdirectory(container)
add_subdirectory(time)
add_subdirectory(benchmark)
#add_subdirectory(algorithm)
#add_subdirectory(crc)
#add_subdirectory(math)
#add_subdirectory(bit)
#add_subdirectory(string)

add_library(Utils INTERFACE)
#    benchmark/benchmark.h
#    benchmark/benchmark_dwt_m3.c)

target_link_libraries(Utils
    INTERFACE
    container
    time
#    algorithm
#    crc
#    math
#    bit
#    string
)

