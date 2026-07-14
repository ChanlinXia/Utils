/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/7/13.
*   @version  1.0
*   @update
*********************************************************************************************************/

/*********************************************************************************************************
*                                              Header File
*********************************************************************************************************/
#include "../Inc/benchmark.h"

/* 统一使用结构体指针访问，无论是否使用 CMSIS */
#if defined(__ARM_ARCH_7M__) || defined(__CORTEX_M3)
    /* 若使用 CMSIS，直接包含头文件，DWT 和 CoreDebug 已定义 */
    #include "stm32f103xe.h"
    #include "core_cm3.h"
#else
    /* 非 CMSIS 环境：手动定义 DWT 和 CoreDebug 结构体及指针 */
    #include <stdint.h>

    /* DWT 寄存器结构（只定义用到的） */
    typedef struct {
        volatile uint32_t CTRL;      /* 0x00 */
        volatile uint32_t CYCCNT;    /* 0x04 */
        volatile uint32_t CPICNT;    /* 0x08 */
        volatile uint32_t LSUCNT;    /* 0x0C */
        volatile uint32_t SLEEPCNT;  /* 0x10 */
        volatile uint32_t FOLDCNT;   /* 0x14 (未使用) */
        volatile uint32_t PCSR;      /* 0x18 (未使用) */
    } DWT_Type;

    /* CoreDebug 寄存器结构（只定义用到的 DEMCR） */
    typedef struct {
        volatile uint32_t DHCSR;     /* 0x00 */
        volatile uint32_t DCRSR;     /* 0x04 */
        volatile uint32_t DCRDR;     /* 0x08 */
        volatile uint32_t DEMCR;     /* 0x0C */
    } CoreDebug_Type;

    /* 基址（ARM Cortex-M3 标准地址） */
    #define DWT_BASE         (0xE0001000UL)
    #define CoreDebug_BASE   (0xE000EDF0UL)

    /* 定义全局指针 */
    #define DWT              ((DWT_Type *)DWT_BASE)
    #define CoreDebug        ((CoreDebug_Type *)CoreDebug_BASE)

    /* 位掩码定义（与 CMSIS 保持一致） */
    #define CoreDebug_DEMCR_TRCENA_Msk    (1UL << 24)
    #define DWT_CTRL_CYCCNTENA_Msk        (1UL << 0)
    #define DWT_CTRL_CPIEVTENA_Msk        (1UL << 4)
    #define DWT_CTRL_LSUEVTENA_Msk        (1UL << 5)
    #define DWT_CTRL_SLEEPEVTENA_Msk      (1UL << 6)
#endif

/*********************************************************************************************************
*                                              Private Macro
*********************************************************************************************************/
/* 无额外宏 */

/*********************************************************************************************************
*                                              Static Declaration and Variable
*********************************************************************************************************/
static uint32_t s_cpu_freq_mhz = 0;      /* CPU 主频，用于时间换算 */
static uint32_t s_start_cycle = 0;
static uint32_t s_end_cycle = 0;
static uint32_t s_start_cpi = 0;
static uint32_t s_end_cpi = 0;
static uint32_t s_start_lsu = 0;
static uint32_t s_end_lsu = 0;
static uint32_t s_start_sleep = 0;
static uint32_t s_end_sleep = 0;

/*********************************************************************************************************
*                                              Static Function
*********************************************************************************************************/

/**
 * @brief 读取 DWT 当前周期计数器值
 * @return uint32_t 当前 CYCCNT
 */
static inline uint32_t read_dwt_cycle(void)
{
    return DWT->CYCCNT;
}

/**
 * @brief 读取 DWT CPI 计数器
 */
static inline uint32_t read_dwt_cpi(void)
{
    return DWT->CPICNT;
}

/**
 * @brief 读取 DWT LSU 计数器
 */
static inline uint32_t read_dwt_lsu(void)
{
    return DWT->LSUCNT;
}

/**
 * @brief 读取 DWT Sleep 计数器
 */
static inline uint32_t read_dwt_sleep(void)
{
    return DWT->SLEEPCNT;
}

/*********************************************************************************************************
*                                              API
*********************************************************************************************************/

/**
 * @brief 初始化 DWT 外设并设置 CPU 频率
 * @param cpu_freq_mhz CPU 主频（Hz）
 * @return 0=成功，-1=失败（频率为 0）
 */
int Benchmark_Init(uint32_t cpu_freq_mhz)
{
    if (cpu_freq_mhz == 0) {
        return -1;
    }
    s_cpu_freq_mhz = cpu_freq_mhz;

    /* 使能调试跟踪（TRCENA） */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 复位所有计数器（可选） */
    DWT->CYCCNT   = 0;
    DWT->CPICNT   = 0;
    DWT->LSUCNT   = 0;
    DWT->SLEEPCNT = 0;

    /* 使能 CYCCNT 计数器 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* 使能 CPI、LSU、Sleep 事件计数器 */
    DWT->CTRL |= (DWT_CTRL_CPIEVTENA_Msk |
                  DWT_CTRL_LSUEVTENA_Msk |
                  DWT_CTRL_SLEEPEVTENA_Msk);

    return 0;
}

/**
 * @brief 记录起始时刻的所有计数器值
 */
void Benchmark_Begin(void)
{
    s_start_cycle = read_dwt_cycle();
    s_start_cpi   = read_dwt_cpi();
    s_start_lsu   = read_dwt_lsu();
    s_start_sleep = read_dwt_sleep();
}

/**
 * @brief 记录结束时刻的所有计数器值
 */
void Benchmark_End(void)
{
    s_end_cycle = read_dwt_cycle();
    s_end_cpi   = read_dwt_cpi();
    s_end_lsu   = read_dwt_lsu();
    s_end_sleep = read_dwt_sleep();
}

/**
 * @brief 获取起始与结束之间的周期差值
 * @return benchmark_val_t 周期数（M3 下自动为 uint32_t）
 */
benchmark_val_t Benchmark_GetElapsedCycle(void)
{
    /* 32位无符号减法，即使溢出也是模运算，数学结果正确 */
    return (benchmark_val_t)(s_end_cycle - s_start_cycle);
}

/**
 * @brief 获取起始与结束之间的时间差值（微秒）
 * @return benchmark_val_t 微秒数（M3 下自动为 uint32_t）
 */
benchmark_val_t Benchmark_GetElapsedTime(void)
{
    if (s_cpu_freq_mhz == 0) {
        return 0;
    }
    uint32_t cycles = (uint32_t)Benchmark_GetElapsedCycle();
    /* 内部使用 unsigned long long 防止中间乘法溢出，但最终强转为 benchmark_val_t */
    return (benchmark_val_t)(cycles / s_cpu_freq_mhz);
}

/**
 * @brief 获取 CPI 计数增量
 * @return benchmark_val_t 增量值
 */
benchmark_val_t Benchmark_GetCPICount(void)
{
    return (benchmark_val_t)(s_end_cpi - s_start_cpi);
}

/**
 * @brief 获取 LSU 计数增量
 * @return benchmark_val_t 增量值
 */
benchmark_val_t Benchmark_GetLSUCount(void)
{
    return (benchmark_val_t)(s_end_lsu - s_start_lsu);
}

/**
 * @brief 获取 Sleep 周期计数增量
 * @return benchmark_val_t 增量值
 */
benchmark_val_t Benchmark_GetSleepCount(void)
{
    return (benchmark_val_t)(s_end_sleep - s_start_sleep);
}

/**
 * @brief 获取中断计数（Cortex-M3 DWT 硬件不支持，恒返回 0）
 * @return benchmark_val_t 0
 */
benchmark_val_t Benchmark_GetInterruptCount(void)
{
    return 0;
}

/*********************************************************************************************************
*   End of File
*********************************************************************************************************/