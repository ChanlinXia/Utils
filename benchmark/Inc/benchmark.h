/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/7/13.
*   @version  1.0
*   @update   
*********************************************************************************************************/


#ifndef BENCHMARK_H
#define BENCHMARK_H

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
*                                               Public Macro
*********************************************************************************************************/
/* 若 CMake 未传递位宽定义，默认回退为 32 位（安全） */
#ifndef BENCHMARK_COUNTER_WIDTH
    #define BENCHMARK_COUNTER_WIDTH 32
#endif

/* 根据 CMake 传递的位宽，统一命名为 benchmark_val_t */
#if BENCHMARK_COUNTER_WIDTH == 64
    typedef uint64_t benchmark_val_t;
#else
    typedef uint32_t benchmark_val_t;
#endif

/*********************************************************************************************************
*                                               Enum
*********************************************************************************************************/
/* 无 */

/*********************************************************************************************************
*                                               Public Structure
*********************************************************************************************************/
/* 无 */

/*********************************************************************************************************
*                                               API List
*********************************************************************************************************/

/**
 * @brief 初始化基准测试模块
 * @param cpu_freq_mhz CPU主频（MHz），Cortex-M3 传入 SystemCoreClock
 * @return 0=成功，-1=失败
 */
int Benchmark_Init(uint32_t cpu_freq_mhz);

/**
 * @brief 开始计时，记录当前时间戳
 */
void Benchmark_Begin(void);

/**
 * @brief 结束计时，记录当前时间戳
 */
void Benchmark_End(void);

/**
 * @brief 获取 Begin 到 End 之间的 CPU 周期数差值
 * @return benchmark_val_t 周期数（类型由 CMake 自动决定，M3 下为 uint32_t）
 */
benchmark_val_t Benchmark_GetElapsedCycle(void);

/**
 * @brief 获取 Begin 到 End 之间的时间差值（微秒）
 * @return benchmark_val_t 微秒数（类型由 CMake 自动决定）
 */
benchmark_val_t Benchmark_GetElapsedTime(void);

/* ==================== 第二层：平台特性接口 ==================== */

/**
 * @brief 获取平均 CPI（Cycle Per Instruction）计数值增量
 * @return benchmark_val_t CPI 增量（M3 下为 uint32_t）
 */
benchmark_val_t Benchmark_GetCPICount(void);

/**
 * @brief 获取 Load/Store 单元事件计数增量
 * @return benchmark_val_t LSU 增量
 */
benchmark_val_t Benchmark_GetLSUCount(void);

/**
 * @brief 获取睡眠周期计数增量
 * @return benchmark_val_t Sleep 增量
 */
benchmark_val_t Benchmark_GetSleepCount(void);

/**
 * @brief 获取中断触发次数（注：Cortex-M3 DWT 硬件无直接支持，返回 0）
 * @return benchmark_val_t 中断次数
 */
benchmark_val_t Benchmark_GetInterruptCount(void);

#ifdef __cplusplus
}
#endif

#endif //BENCHMARK_H

/*********************************************************************************************************
*   End of File
*********************************************************************************************************/