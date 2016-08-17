/*
 * Customer ID=11656; Build=0x5f626; Copyright (c) 2005-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

#ifndef __XT_PROFILER_H__
#define __XT_PROFILER_H__

#include <xtensa/config/core.h>

#if XCHAL_NUM_PERF_COUNTERS
/* Performance monitor counters constants */
#include <xtensa/xt_perf_consts.h>

#endif /* XCHAL_NUM_PERF_COUNTERS */


#ifdef __cplusplus
extern "C" {
#endif

/* This file defines an interface that allows a program being profiled
   to control when and how it is profiled, whether it is running under
   the instruction set simulator or under the hardware profiler.

   Both ISS and HWP implement this interface, although in different
   ways. Both also do the right thing if you don't call any of these
   functions.
*/


/* 
xt_profile_init

   ISS: a no op.

   HWP: Initialize the profiler.  Ordinarily, this function is called 
   automatically via the .init section. If your environment does not 
   support the .init section, you will need to call this function 
   by hand.
*/
extern void xt_profile_init(void);

/* 
xt_profile_add_memory

   ISS: a no op.

   HWP:
   Makes "buf_size" bytes at "buf" available to the hardware profiler.
   This buffer should be initialized to zeros prior to this call.

   The hardware profiler has already estimated the amount of memory it needs,
   but under certain circumstances may still run out of memory.  If so, you can
   provide more memory with this routine.

*/
extern void xt_profile_add_memory(void * buf, unsigned int buf_size);


/* xt_profile_enable

   Turn on the profiler. Ordinarily, profiling is on by default. 
   If you turn off profiling using xt_profile_disable, You can turn 
   it on again via this function.
*/
extern void xt_profile_enable(void);

/* xt_profile_disable

   Turn off the profiler. If you don't want to profile a portion of your code,
   use this function and then xt_profile_enable when you want to start again.
*/
extern void xt_profile_disable(void);

/* xt_profile_save_and_reset

   Save and reset the profiler's data.
   If there were errors, either during profiling or while attempting to
   write the data, no data will be written and this function will
   return non-zero.
   
*/
extern int xt_profile_save_and_reset(void);

/* xt_profile_get_frequency

   ISS: always returns 1.

   HWP:
   Returns the number of cycles between samples for timer based profiler.
   In performance counters based profiler always returns 1.
*/
extern unsigned int xt_profile_get_frequency(void);

/* xt_profile_set_frequency

   ISS: a no op.

   HWP:
   Set the number of cycles between samples for timer based profiler.
   Ignored in performance counters based profiler.

   sample frequency is the number of cycles to wait between samples.  It should
   be a multiple of 1024.
   
   If you set the sample frequency to a different value than was passed in xt_profile_init,
   then the labels in the output will reflect the later frequency, even though some samples may
   have been taken at the earlier frequency.  Typically this does not make a significant difference
   in the results if this function is called early enough.
*/
extern void xt_profile_set_frequency(unsigned int sample_frequency);

/* xt_profile_num_errors

   ISS: always returns 0

   HWP:
   Returns the number of errors that occured while taking samples.  Typically these
   are out of memory errors and you need to pass a bigger buffer to 
   xt_profile_add_memory
*/
extern int xt_profile_num_errors(void);


#if XCHAL_NUM_PERF_COUNTERS


/* xt_profile_randomize

   ISS: not available

   HWP: Available in performance monitor based profiler.

   Turns on or off sampling period randomization mode. Period randomization
   helps to avoid aliasing problems when code being profiled is highly periodic.
   Profiler maintains same average sampling period but individual sampling
   steps may vary.
   Period randomization is turned off by default.

   <value> - non zero turns randomization on,
             zero turns randomization off.
*/

extern void xt_profile_randomization(int value);

/* xt_profile_config_clear

   ISS: not available

   HWP: Available in performance monitor based profiler.

   Stops profiling if it was enabled and clears performance counters
   parameters. Accumulated profile data stays in memory and will be
   saved when xt_profile_save_and_reset is called or at program exit.
   Number of configured performance counters is zero after this 
   function call.
*/

extern void xt_profile_config_clear(void);


/* xt_profile_config_num

   ISS: not available

   HWP: Available in performance monitor based profiler.

   Returns number of free performance counters.
*/

extern int xt_profile_config_num(void);


/* xt_profile_config_counter error codes
*/

#define XTPROF_ERR_OUT_OF_MEM            -1
#define XTPROF_ERR_INVALID_ARGS          -2
#define XTPROF_ERR_NOT_ENOUGH_COUNTERS   -3
#define XTPROF_ERR_DEFUNCT               -4

/* xt_profile_config_counter

   ISS: not available

   HWP: Available in performance monitor based profiler.

   Allocating and initializing one or more performance counter for sampling.
   Even though event may require multiple performance counters allocated the
   profile data for event is merged and dumped into single gmon file.
   This function disables profiling if it was enabled.

   Returns 0 on success, non zero if failed:
   XTPROF_ERR_OUT_OF_MEM - memory allocation failed;
   XTPROF_ERR_INVALID_ARGS - invalid function parameters;
   XTPROF_ERR_NOT_ENOUGH_COUNTERS - not enough free performance counters available;
   XTPROF_ERR_DEFUNCT - profiling is disabled because of previous errors
                        (xt_profile_num_errors() is non zero)

   <selector> - events group, one of XTPERF_CNT constants defined in xt_perf_consts.h
   <mask> - events mask for selected group. Mask bit fields for each 
            selector defined with XTPERF_MASK prefix in xt_perf_consts.h
   <trace_level> - specifies interrupt levels at which to take samples;
                   if trace_level is greater or equal to zero samples are
		   taken only at interrupt levels below or equal to
		   trace_level; if trace_level is negative samples are taken
		   only at (-trace_level) interrupt level or higher.
   <period> - sampling period; 1 - record every event, 2 - record every 
              other event and so on;
              Please note - there is overhead associated with events recording,
	      high frequency events may produce incorrect profile when period
	      is too small.
*/
   
extern int xt_profile_config_counter ( unsigned int  selector,
				       unsigned int  mask,
				                int  trace_level,
				       unsigned int  period);



#endif /* XCHAL_NUM_PERF_COUNTERS */

#ifdef __cplusplus
}
#endif

#endif /* __XT_PROFILER_H__ */
