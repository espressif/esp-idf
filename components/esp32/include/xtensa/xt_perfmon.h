/*
 * Customer ID=11656; Build=0x5f626; Copyright (c) 2012 by Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __XT_PERFMON_H__
#define __XT_PERFMON_H__

#include <xtensa/config/core.h>
#include <xtensa/xt_perf_consts.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int counter_id_t;

/* xt_perf_init

   Initialize the performance monitor library.  Ordinarily, this
   function is called automatically via the .init section. If your
   environment does not support the .init section, you will need to
   call this function from your code.
*/

extern void xt_perf_init(void);

/* xt_perf_enable

   Turn on the performance monitor. Ordinarily, counting is off by 
   default. If you turn off performance monitor using xt_perf_disable or
   by call to a function that disables performance monitor, you can turn
   it on again via this function.
*/

extern void xt_perf_enable(void);

/* xt_perf_disable

   Turn off the performance monitor. If you want to suspend counting 
   events for a portion of your code, use this function and then call
   xt_perf_enable when you want to start again.
*/

extern void xt_perf_disable(void);

/* xt_perf_clear
   
   Disable performance monitor and clear all initialized hardware counters.
   All counter ids are invalid after call to this function and all hardware
   counters available for initialization.
*/

extern void xt_perf_clear (void);

/* xt_perf_counters_num

   Returns number of free hardware performance counters. After call to xt_perf_clear
   all counters are free and available for initialization. With each successful
   xt_perf_init_counter/xt_perf_init_event call this number is decreasing until
   no free counters available.
*/

extern int xt_perf_counters_num (void);

/* xt_perf_init_counter32
   
   Setup 32 bit performance counter. This function disables performance monitor
   if it was enabled.

   Returns zero based counter id on success or negative value if failed.
   This function may fail if there is insufficient number of free hardware
   counters or function arguments are invalid.

   The counter id returned on success can be used with xt_perf_reset_counter
   and xt_perf_counter32 functions.

   <selector> - events group, one of XTPERF_CNT constants defined in
                xt_perf_consts.h;
   <mask> - events mask for selected group. Mask bit fields for each 
            selector defined with XTPERF_MASK prefix in xt_perf_consts.h;
   <trace_level> - specifies interrupt levels at which to count events;
                   if trace_level is greater or equal to zero events are
		   counted only at interrupt levels below or equal to
		   trace_level; if trace_level is negative events are 
		   counted only at (-trace_level) interrupt level or higher.
*/
    

extern counter_id_t xt_perf_init_counter32 ( unsigned int  selector,
					     unsigned int  mask,
					              int  trace_level);

/* xt_perf_init_counter64
   
   Setup 64 bit performance counter. Library emulates 64 bit counters by handling
   profiling interrupt and recording overflows of 32 bit hardware counters.
   This function disables performance monitor if it was enabled.

   Returns zero based counter id on success or negative value if failed.
   This function may fail if there is insufficient number of free hardware
   counters or function arguments are invalid.

   The counter id returned on success can be used with xt_perf_reset_counter
   and xt_perf_counter64 functions.

   <selector> - events group, one of XTPERF_CNT constants defined in
                xt_perf_consts.h;
   <mask> - events mask for selected group. Mask bit fields for each 
            selector defined with XTPERF_MASK prefix in xt_perf_consts.h;
   <trace_level> - specifies interrupt levels at which to count events;
                   if trace_level is greater or equal to zero events are
		   counted only at interrupt levels below or equal to
		   trace_level; if trace_level is negative events are 
		   counted only at (-trace_level) interrupt level or higher.
*/

extern counter_id_t xt_perf_init_counter64 ( unsigned int  selector,
					     unsigned int  mask,
					              int  trace_level);

/* xt_perf_reset_counter

   Reset counter value to 0.
   Returns zero on success or non zero if failed.
*/

extern int xt_perf_reset_counter (counter_id_t counter_id);

/* xt_perf_counter32

   Read 32 bit counter value.

   Returns zero if counter id is not valid.
*/

extern unsigned int xt_perf_counter32 (counter_id_t counter_id);

/* xt_perf_counter64

   Read 64 bit counter value.

   Counter must be initialized using xt_perf_init_counter64 function.

   Returns zero if counter id is not valid.
*/

extern unsigned long long xt_perf_counter64 (counter_id_t counter_id);

  /* xt_perf_overflow32

     Read overflow flag of 32 bit counter. This flag is dropped when
     counter initialized or reset. Once counter overflows and wraps
     around the flag is set and stays set until counter reset.

     Returns negative value if counter id is invalid, zero if counter
     not overflowed, positive if in overflowed state.
  */

extern int xt_perf_overflow32 (counter_id_t counter_id);

#ifdef __cplusplus
}
#endif

#endif /* __XT_PERFMON_H__ */
