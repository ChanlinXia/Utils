/*********************************************************************************************************
*
*   @author   Created by Chanlin on 2026/7/9.
*   @version  1.0
*   @update
*********************************************************************************************************/

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

/*********************************************************************************************************
*                                               Header File
*********************************************************************************************************/
#include <stdint.h>

/*********************************************************************************************************
*                                               Config
*********************************************************************************************************/
#define TIME_CONFIG_TYPE_WIDTH    32

/*********************************************************************************************************
*                                               Type
*********************************************************************************************************/
#if TIME_CONFIG_TYPE_WIDTH == 16

typedef uint16_t unsigned_time_type_t;
typedef int16_t  time_type_t;

#elif TIME_CONFIG_TYPE_WIDTH == 32

typedef uint32_t unsigned_time_type_t;
typedef int32_t  time_type_t;

#elif TIME_CONFIG_TYPE_WIDTH == 64

/* 64bit时间戳由两个32bit组成 */
typedef uint32_t unsigned_time_type_t;
typedef int32_t  time_type_t;

#else

#error "Unsupported TIME_CONFIG_TYPE_WIDTH"

#endif

/*********************************************************************************************************
*                                               Structure
*********************************************************************************************************/
typedef struct timestamp_t
{
    volatile unsigned_time_type_t low;

#if TIME_CONFIG_TYPE_WIDTH == 64
    volatile unsigned_time_type_t high;
#endif

} timestamp_t;

/*********************************************************************************************************
*                                               API
*********************************************************************************************************/

/*********************************************************************************************************
*   Clear timestamp
*********************************************************************************************************/
static inline void Timestamp_Clear(volatile timestamp_t *time)
{
    time->low = 0;

#if TIME_CONFIG_TYPE_WIDTH == 64
    time->high = 0;
#endif
}

/*********************************************************************************************************
*   Write timestamp
*********************************************************************************************************/
static inline void Timestamp_Write(
    volatile timestamp_t *dst,
    const timestamp_t *src)
{
    dst->low = src->low;

#if TIME_CONFIG_TYPE_WIDTH == 64
    dst->high = src->high;
#endif
}

/*********************************************************************************************************
*   Compare timestamp
*
*   return
*       <0 : a<b
*        0 : a==b
*       >0 : a>b
*********************************************************************************************************/
static inline int Timestamp_Compare(
    const timestamp_t *a,
    const timestamp_t *b)
{
#if TIME_CONFIG_TYPE_WIDTH == 64

    if(a->high < b->high)
        return -1;

    if(a->high > b->high)
        return 1;

#endif

    if(a->low < b->low)
        return -1;

    if(a->low > b->low)
        return 1;

    return 0;
}

/*********************************************************************************************************
*   Whether a is before b
*********************************************************************************************************/
static inline int Timestamp_Before(
        const timestamp_t *a,
        const timestamp_t *b)
{
    return Timestamp_Compare(a,b) < 0;
}

/*********************************************************************************************************
*   Whether a is after b
*********************************************************************************************************/
static inline int Timestamp_After(
        const timestamp_t *a,
        const timestamp_t *b)
{
    return Timestamp_Compare(a,b) > 0;
}

/*********************************************************************************************************
*   Whether equal
*********************************************************************************************************/
static inline int Timestamp_Equal(
        const timestamp_t *a,
        const timestamp_t *b)
{
    return Timestamp_Compare(a,b) == 0;
}

/*********************************************************************************************************
*   Get interval
*
*   note:
*       Only valid for timeout calculation.
*********************************************************************************************************/
static inline void Timestamp_Diff(
    timestamp_t* diff,
    const timestamp_t *a,
    const timestamp_t *b)
{
    // timestamp_t diff;

    diff->low = a->low - b->low;
#if TIME_CONFIG_TYPE_WIDTH == 64
    diff->high = a->high - b->high;

    if(a->low < b->low)
    {
        diff->high--;
    }
#endif


    // return diff;
}
/*********************************************************************************************************
*   Timeout check
*********************************************************************************************************/
static inline int Timestamp_IsExpired(
        const timestamp_t *start,
        const timestamp_t *now,
        const timestamp_t *timeout)
{
    timestamp_t diff;

    Timestamp_Diff(
        &diff,
        now,
        start);

    return Timestamp_Compare(
        &diff,
        timeout) >= 0;
}

/*********************************************************************************************************
*   Timestamp Add
*
*   result = a + b
*********************************************************************************************************/
static inline void Timestamp_Add(
    timestamp_t *result,
    const timestamp_t *a,
    const timestamp_t *b)
{
    result->low = a->low + b->low;

#if TIME_CONFIG_TYPE_WIDTH == 64
    result->high = a->high + b->high;

    /* Carry */
    if (result->low < a->low)
    {
        result->high++;
    }
#endif
}

/*********************************************************************************************************
*   Timestamp Add Ticks
*
*   time += ticks
*********************************************************************************************************/
static inline void Timestamp_AddTicks(
    volatile timestamp_t *time,
    unsigned_time_type_t ticks)
{
#if TIME_CONFIG_TYPE_WIDTH == 64

    unsigned_time_type_t old_low = time->low;

    time->low += ticks;

    /* Carry */
    if (time->low < old_low)
    {
        time->high++;
    }

#else

    time->low += ticks;

#endif
}

#endif