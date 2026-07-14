cmake_minimum_required(VERSION 3.12)
project(benchmark C)

# 后端选择（可在命令行覆盖）
set(BENCHMARK_BACKEND "DWT" CACHE STRING "Backend: DWT, LINUX, DUMMY")

# 计数器位宽
if(BENCHMARK_BACKEND STREQUAL "LINUX")
    set(BENCHMARK_COUNTER_WIDTH 64)
else()
    set(BENCHMARK_COUNTER_WIDTH 32)
endif()

#find_library(stm32cubemx REQUIRED)

# 静态库
add_library(benchmark STATIC)

# 公共头文件路径
target_include_directories(benchmark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# 宏定义（位宽）
target_compile_definitions(benchmark PUBLIC
    BENCHMARK_COUNTER_WIDTH=${BENCHMARK_COUNTER_WIDTH}
)

# DWT 后端需要 CMSIS 头文件，CubeMX 已提供 MX_Include_Dirs

# 选择后端源文件（假设源文件在 src/ 子目录下）
if(BENCHMARK_BACKEND STREQUAL "DWT")
    set(BACKEND_SRC Src/benchmark_dwt_m3.c)
#    target_include_directories(benchmark PUBLIC ${MX_Include_Dirs})
#    targ
    target_link_libraries(benchmark PUBLIC stm32cubemx)
elseif(BENCHMARK_BACKEND STREQUAL "LINUX")
    set(BACKEND_SRC Src/benchmark_linux.c)
else()
    set(BACKEND_SRC Src/benchmark_dummy.c)
endif()

target_sources(benchmark PRIVATE ${BACKEND_SRC})