/* Customer ID=11656; Build=0x5f626; Copyright (c) 2008-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
   These coded instructions, statements, and computer programs are the
   copyrighted works and confidential proprietary information of Tensilica Inc.
   They may not be modified, copied, reproduced, distributed, or disclosed to
   third parties in any manner, medium, or form, in whole or in part, without
   the prior written consent of Tensilica Inc.  */

#ifndef _XMP_LIBRARY_H
#define _XMP_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xtensa/config/core-isa.h>
#include <xtensa/config/core.h>
#include <xtensa/tie/xt_core.h>
#if XCHAL_HAVE_RELEASE_SYNC
#include <xtensa/tie/xt_sync.h>
#endif
#if XCHAL_HAVE_EXTERN_REGS
#include <xtensa/xtensa-xer.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include "xtensa/system/mpsystem.h"

/* 
   W A R N I N G:

   xmp library clients should treat all data structures in this file
   as opaque.  They are only public to enable users to declare them
   statically.
*/


/* -------------------------------------------------------------------------
   When using XMP on cache-incoherent systems, these macros are helpful
   to ensure that you are not reading stale data, and to ensure that
   the data you write makes it all the way back to main memory.
 */

#if !XCHAL_DCACHE_IS_COHERENT
#define XMP_WRITE_BACK_ELEMENT(x) xthal_dcache_region_writeback((void *)x, sizeof(*x))
#define XMP_INVALIDATE_ELEMENT(x) xthal_dcache_region_invalidate((void *)x, sizeof(*x))
#define XMP_WRITE_BACK_INVALIDATE_ELEMENT(x) xthal_dcache_region_writeback_inv((void *)x, sizeof(*x))
#define XMP_WRITE_BACK_ARRAY(x) xthal_dcache_region_writeback((void *)x, sizeof(x))
#define XMP_INVALIDATE_ARRAY(x) xthal_dcache_region_invalidate((void *)x, sizeof(x))
#define XMP_WRITE_BACK_INVALIDATE_ARRAY(x) xthal_dcache_region_writeback_inv((void *)x, sizeof(x))
#define XMP_WRITE_BACK_ARRAY_ELEMENTS(x, num_elements) xthal_dcache_region_writeback((void *)x, sizeof(*x) * num_elements)
#define XMP_INVALIDATE_ARRAY_ELEMENTS(x, num_elements) xthal_dcache_region_invalidate((void *)x, sizeof(*x) * num_elements)
#define XMP_WRITE_BACK_INVALIDATE_ARRAY_ELEMENTS(x, num_elements) xthal_dcache_region_writeback_inv((void *)x, sizeof(*x) * num_elements)
#else 
#define XMP_WRITE_BACK_ELEMENT(x)
#define XMP_INVALIDATE_ELEMENT(x)
#define XMP_WRITE_BACK_INVALIDATE_ELEMENT(x)
#define XMP_WRITE_BACK_ARRAY(x)
#define XMP_INVALIDATE_ARRAY(x)
#define XMP_WRITE_BACK_INVALIDATE_ARRAY(x)
#define XMP_WRITE_BACK_ARRAY_ELEMENTS(x, num_elements)
#define XMP_INVALIDATE_ARRAY_ELEMENTS(x, num_elements)
#define XMP_WRITE_BACK_INVALIDATE_ARRAY_ELEMENTS(x, num_elements)
#endif

/* -------------------------------------------------------------------------
   Initialization, error codes, constants and house-keeping

   Every core should call xmp_init with the number of cores in the
   system.

   xmp_init should be called before you use any global synchronization
   primitive or shared data. 

   Further, before you use a dynamically allocated synchronization
   primitives, you need to both initialize it by calling the
   xmp_*_init function, and you need to have called xmp_init, which
   sets up interrupt handlers and interrupt routing.

   The second parameter sets the interprocessor interrupt
   routing. Passing zero instructs the library to use the default
   routing, which will be suitable for most users.
   
*/
   
extern void xmp_init (int num_cores, unsigned int interrupt_routing);


/* If you want finer-grained control than that provided by xmp_init,
   you can the functions below individually--however, this is more
   inconvenient and requires greater understanding of the library's
   internals.  Don't use them directly unless you have a good reason.
*/

extern void xmp_unpack_shared (void);
extern void xmp_route_interrupts (unsigned int routing);

#if XCHAL_HAVE_MP_INTERRUPTS
extern void xmp_enable_ipi_interrupts (void);

/* Turn off certain things enabled by xmp_init  */
extern void xmp_disable_ipi_interrupts (void);
#endif

extern void xmp_end (void);

/* Only valid after xmp_init.  */
extern int xmp_num_cores (void);

/* How many cycles should a core wait before rechecking a
   synchronization variable?  Higher values will reduce memory
   transactions, but will also result in higher latency in returning
   from synchronization.
*/
extern void xmp_spin_wait_set_cycles (unsigned int limit);

/* If you would prefer to provide your own spin wait function,
   to go to sleep, etc.  Declare a function of this type, then call
   this function.  */
typedef void (*xmp_spin_wait_function_t)(void);
extern void xmp_spin_wait_set_function (xmp_spin_wait_function_t func);
extern void xmp_spin(void);

#define XMP_NO_OWNER        0x07
#define XMP_MUTEX_DESTROYED 0xFE
#define XMP_ERROR_FATAL     0xFD

#define XMP_MAX_CORES       0x4


static inline unsigned int xmp_prid (void) 
{
#if XCHAL_HAVE_PRID
  return XT_RSR_PRID() & 0xFF;
#else
  return 0;
#endif
}


/* -------------------------------------------------------------------------
   Tracing
   
   A core must set a trace_file if it wants any synchronization
   tracing to occur.  Sharing file descriptors among cores is very
   messy, so don't do it.  This, unfortunately, means that two cores
   contending for a mutex are not able to trace to the same file.

   Any object (except the atomic integer) can have tracing off or on.   
*/

extern void xmp_set_trace_file (FILE * file);
extern void xmp_trace (const char * fmt, ...);


/* -------------------------------------------------------------------------
   Memory Allocation Functions.

   These do what you would expect, only from shared memory instead of
   private memory.
*/

#if XCHAL_DCACHE_IS_COHERENT
extern void * xmp_malloc (size_t size);
extern void * xmp_calloc (size_t nmemb, size_t size);
extern void xmp_free (void * ptr);
#endif
extern void * xmp_sbrk(int size);

/* -------------------------------------------------------------------------
   xmp_atomic_int_t

   The most basic synchronization primitive in the xmp library.
   Atomic ints are sharable among processors, and even interrupt
   levels on the same processor. However, their semantics are fairly
   rudimentary.  All other primitives are based on these, therefore,
   changing this implementation affects all other primitives.

*/

typedef unsigned int xmp_atomic_int_t;

static inline xmp_atomic_int_t
xmp_coherent_l32ai(xmp_atomic_int_t * address)
{
  XMP_INVALIDATE_ELEMENT (address);
  return  XT_L32AI(address, 0);
}

static inline void
xmp_coherent_s32ri(xmp_atomic_int_t value, xmp_atomic_int_t  * address)
{
  XT_S32RI (value, address, 0);
  XMP_WRITE_BACK_ELEMENT (address);
}

#define XMP_ATOMIC_INT_INITIALIZER(value) (value)

/* xmp_atomic_int_init - Initialize an int prior to use

   Nonsynchronizing, Nonblocking 

   Usage: 
      value - initial value
      integer - points to an uninitialized integer

   On exit:
      initialized to given value

   Errors: none
*/

static inline void 
xmp_atomic_int_init (xmp_atomic_int_t * integer, int value)
{
  xmp_coherent_s32ri (value, integer);
}


/* xmp_atomic_int_value - Read the value 

   Nonsynchronizing, Nonblocking

   Usage: 
      integer - points to an int

   Returns:
      the value
*/

static inline int
xmp_atomic_int_value (xmp_atomic_int_t * integer)
{
  return xmp_coherent_l32ai (integer);
}


/* xmp_atomic_int_conditional_increment - Conditionally increment integer

   Synchronizing, nonblocking

   Usage: 
      integer - points to an initialized integer
      amount - how much to increment
      prev - believed value of the integer
                eg: prev = xmp_atomic_int_value (integer);
                    success = xmp_atomic_int_increment (integer, 1, prev);

   Returns: current value of integer - user should check if it matches
      the previous value of the integer. If it does, then the update
      was successful.

*/

#define USE_ASSEMBLY_IMPLEMENTATION 0

static inline int
xmp_atomic_int_conditional_increment (xmp_atomic_int_t * integer, int amount, int prev)
{
  int val;
  int saved;

#if USE_ASSEMBLY_IMPLEMENTATION
  /* %0 = prev
     %1 = saved
     %2 = atomic integer pointer
     %3 = amount
  */

  asm volatile ("wsr.scompare1 %0\n"
		"mov %1, %0\n"
		"add %0, %0, %3\n"
		"s32c1i %0, %2, 0\n"
		: "+&a" (prev), "+&a"(saved) : "a" (integer), "a" (amount));

  return prev;

#else

  XT_WSR_SCOMPARE1 (prev);
  val = prev + amount;
  saved = val;
  XT_S32C1I (val, integer, 0);

  return val;
#endif
}


/* xmp_atomic_int_increment - Increment integer

   Synchronizing, blocking

   Usage: 
      integer - points to an initialized integer
      amount - how much to increment

   Returns: new value of integer

*/

static inline int
xmp_atomic_int_increment (xmp_atomic_int_t * integer, int amount)
{
  int val;
  int saved;
#if USE_ASSEMBLY_IMPLEMENTATION
  /* %0 = val
     %1 = saved
     %2 = atomic integer pointer
     %3 = amount
  */
     
  asm volatile ("l32ai %0, %2, 0\n"
		"1:\n"
		"wsr.scompare1 %0\n"
		"mov %1, %0\n"
		"add %0, %0, %3\n"
		"s32c1i %0, %2, 0\n"
		"bne %0, %1, 1b\n" 
		: "+&a" (val), "+&a"(saved) : "a" (integer), "a" (amount));
#else
  /* Accurately naming "val" is tricky. Sometimes it will be what we
     want to be the new value, but sometimes it contains the value
     that is currently at the location.  */
  
  /* Load location's current value  */
  val = xmp_coherent_l32ai (integer);

  do {
    XT_WSR_SCOMPARE1 (val);
    saved = val;
    /* Change it to what we would like to store there--"new_val"  */
    val = val + amount;
    /* Possibly store new_val, but reload location's current value no
       matter what. */
    XT_S32C1I (val, integer, 0);
    if (val != saved)
      xmp_spin();
  } while (val != saved);

#endif
  return val + amount;
}


/* xmp_atomic_int_conditional_set - Set the value of an atomic integer

   Synchronizing, nonblocking

   Usage: 
      integer - points to an initialized integer
      from - believed value of the integer
                eg: prev = xmp_atomic_int_value (integer);
                    success = xmp_atomic_int_conditional_set (integer, 1, prev);
      to - new value

   Returns: current value of integer - user should check if it matches
      the previous value of the integer. If it does, then the update
      was successful.

*/

static inline int
xmp_atomic_int_conditional_set (xmp_atomic_int_t * integer, int from, int to)
{
  int val;

  /* Don't even try to update if the integer's value isn't what we
     think it should be.  This prevents acquiring this cache-line for
     writing and therefore prevents bus transactions when various
     cores contend.  */
  val = xmp_coherent_l32ai(integer);
  if (val == from) {
    XT_WSR_SCOMPARE1 (from);
    val = to;
    /* Possibly store to, but reload location's current value no
       matter what. */
    XT_S32C1I (val, integer, 0);
  }
  return val;
}


/* Macros to implement trivial spin locks.  These are very primitive, but
   can be useful when you don't need the higher-overhead synchronization.

   To use an xmp_atomic_int_t as a trivial spin lock, you should
   initialize it to zero first.
*/

#define XMP_SIMPLE_SPINLOCK_ACQUIRE(atomic_int_ptr)	\
  { while (xmp_atomic_int_conditional_set (atomic_int_ptr, 0, xmp_prid() + 1) != 0) \
      xmp_spin(); }
#define XMP_SIMPLE_SPINLOCK_RELEASE(atomic_int_ptr)	\
  { while (xmp_atomic_int_conditional_set (atomic_int_ptr, xmp_prid() + 1, 0) != xmp_prid() + 1) \
    xmp_spin(); }

#define XMP_SIMPLE_SPINLOCK_OWNER(atomic_int_ptr) (xmp_atomic_int_value(atomic_int_ptr) - 1)


/* -------------------------------------------------------------------------
   xmp_mutex_t - An even higher-level data structure to enforce
   mutual exclusion between cores.  A core which waits on a mutex might
   sleep with a waiti and be interrupted by an interrupt.

   Mutexes can be normal or recursive. For a normal mutex, a core
   attempting to acquire a mutex it already holds will result in
   deadlock. For a recursive mutex, a core will succeed in acquiring a
   mutex it already holds, and must release it as many times as it
   acquired it.

   Mutexes are not sharable between interrupt levels--because
   ownership is tracked by core, not thread.

   Like all xmp data structures, an object of type xmp_mutex_t
   should be treated by the programmer as opaque.  They are only
   public in this header file to allow them to be declared statically.

   For configurations with 16-byte cache lines, this has the most
   frequently used and changed data in the first line.

*/

#if XCHAL_DCACHE_IS_COHERENT
typedef struct xmp_mutex_t {
  xmp_atomic_int_t qlock;
  unsigned int qhead;
  unsigned int qtail;
  unsigned char queue[XMP_MAX_CORES];
  unsigned short held;

  unsigned char owner;
  unsigned char recursive : 1;
  unsigned char trace : 1;
  unsigned char system : 1;
  unsigned char unused : 5;
  const char * name;
} xmp_mutex_t __attribute__ ((aligned (XMP_MAX_DCACHE_LINESIZE)));


#define XMP_MUTEX_INITIALIZER(name)					\
  { 0, 0, -1, {XMP_NO_OWNER, XMP_NO_OWNER, XMP_NO_OWNER, XMP_NO_OWNER}, \
      0, XMP_NO_OWNER, XMP_MUTEX_FLAG_NORMAL, 0, 0, 0, name }

#define XMP_RECURSIVE_MUTEX_INITIALIZER(name)				\
  { 0, 0, -1, {XMP_NO_OWNER, XMP_NO_OWNER, XMP_NO_OWNER, XMP_NO_OWNER}, \
      0, XMP_NO_OWNER, XMP_MUTEX_FLAG_RECURSIVE, 0, 0, 0, name }

#define XMP_MUTEX_FLAG_NORMAL 0
#define XMP_MUTEX_FLAG_RECURSIVE 1

#define XMP_MUTEX_ACQUIRE_FAILED -1
#define XMP_MUTEX_ERROR_DESTROY_OWNED -2
#define XMP_MUTEX_ERROR_NOT_OWNED -3
#define XMP_MUTEX_ERROR_ALREADY_OWNED -4

/*
   xmp_mutex_init
   
   Nonsynchronizing
   Nonblocking 

   Usage: 
      mutex - points to an uninitialized mutex
      name - name if you want one, NULL if not.
      recursive - use recursive semantices      

   Returns
      zero on success (always succeeds)

*/

extern int xmp_mutex_init (xmp_mutex_t * mutex, 
			    const char * name, 
			    unsigned int recursive);

/*
   int xmp_mutex_destroy (xmp_mutex_t * mutex);
   
   Synchronizing - will fail if mutex is held by anyone -- including 
                   current processor
   Nonblocking 

   Usage: 
      mutex - points to a mutex

   Returns
      zero on success
      non-zero if mutex is held
*/

extern int xmp_mutex_destroy (xmp_mutex_t * mutex);


/*
   xmp_mutex_lock -- Synchronizing
   xmp_mutex_trylock

   Usage: 
      mutex - points to a mutex

   Returns
      zero on success
*/

extern int xmp_mutex_lock (xmp_mutex_t * mutex);
extern int xmp_mutex_trylock (xmp_mutex_t * mutex);


/*
   xmp_mutex_unlock
   
   Synchronizing
   Nonblocking 

   Usage: 
      mutex - points to a mutex

   Returns
      zero on success - mutex is released
      non-zero on failure - mutex is owned by another core 
                          - prid of processor that does own it
			    note that by the time this function
			    returns, the owner of the core may 
			    have changed.
*/

extern int xmp_mutex_unlock (xmp_mutex_t * mutex);


/*
   xmp_mutex_name
   
   Nonsynchronizing
   Nonblocking 

   Usage: 
      mutex - points to a mutex

   Returns the name of the given mutex, which may be NULL.
      
*/

const char * xmp_mutex_name (const xmp_mutex_t * mutex);


/* 
   xmp_mutex_trace_on
   xmp_mutex_trace_off

   Nonsynchronizing
   Nonblocking

   Turn off and on tracing for the mutex.

   These functions are only present in the debug version of the library.
*/

extern void xmp_mutex_trace_on (xmp_mutex_t * mutex);
extern void xmp_mutex_trace_off (xmp_mutex_t * mutex);


/* -------------------------------------------------------------------------
   xmp_condition_t

   Condition Variables following Mesa semantics. 

   Condition variables are not sharable among interrupt levels.

*/


typedef struct xmp_condition_t {
  unsigned int qhead;
  unsigned int qtail;
  unsigned char queue[XMP_MAX_CORES];
  unsigned int waiting[XMP_MAX_CORES];

  unsigned char trace : 1;
  unsigned char unused : 7;
  const char * name;
} xmp_condition_t __attribute__ ((aligned (XMP_MAX_DCACHE_LINESIZE)));


#define XMP_CONDITION_INITIALIZER(name)				\
  { 0, -1, {XMP_NO_OWNER, XMP_NO_OWNER, XMP_NO_OWNER, XMP_NO_OWNER}, \
      {0, 0, 0, 0}, 0, 0, name}


/* xmp_condition_init - Initialize a condition variable

   Nonsynchronizing, Nonblocking 

   Usage: 
      condition - pointer to an xmp_condition_t

   On exit:
      condition initialized

   Errors: none
*/

extern int xmp_condition_init (xmp_condition_t * condition, 
				const char * name);
extern int xmp_condition_destroy (xmp_condition_t * condition);


/* xmp_condition_wait - Wait for a condition variable

   Synchronizing, blocking 

   Usage: 
      condition - pointer to an xmp_condition_t
      mutex - pointer to an xmp_mutex_t already acquired by the calling
              process

   Errors: if the mutex isn't held by this core
*/

extern int xmp_condition_wait (xmp_condition_t * condition, 
				xmp_mutex_t * mutex);

/* xmp_condition_signal 

   - Signal the first (if any) core waiting on a condition variable

   You must hold the mutex you passed to xmp_condition_wait before
   calling this function.

   Synchronizing, nonblocking

   Usage: 
      condition - pointer to an xmp_condition_t

   Errors: none
*/

extern int xmp_condition_signal (xmp_condition_t * condition);


/* xmp_condition_broadcast

   - Signal all cores waiting on a condition variable

   Synchronizing, nonblocking

   You must hold the mutex you passed to xmp_condition_wait before
   calling this function.

   Usage: 
      condition - pointer to an xmp_condition_t

   Errors: none
*/

extern int xmp_condition_broadcast (xmp_condition_t * condition);


static inline const char * xmp_condition_name (const xmp_condition_t * condition)
{
  return condition->name;
}

/* 
   xmp_condition_trace_on
   xmp_condition_trace_off

   Nonsynchronizing
   Nonblocking

   Turn off and on statistics and tracing for the condition.  For
   tracing you must also set a trace file for the core.

   These functions are only present in the debug-version of the library.
*/

extern void xmp_condition_trace_on (xmp_condition_t * condition);
extern void xmp_condition_trace_off (xmp_condition_t * condition);

#endif /* XCHAL_DCACHE_IS_COHERENT */

/* -------------------------------------------------------------------------
   xmp_barrier_t

   Classic barriers that stop any core from continuing until a
   specified number of cores reach that point. Once the barrier allows
   cores through, the barrier is reset and will stop cores from
   progressing again.

   Barriers are not sharable among interrupt levels.

*/


typedef struct xmp_barrier_t 
{ 
  xmp_atomic_int_t count; 
  xmp_atomic_int_t state;
  xmp_atomic_int_t sleeping;
  unsigned short num_cores;
  unsigned short trace : 1;
  unsigned short system : 1;
  const char * name;
} xmp_barrier_t __attribute__ ((aligned (XMP_MAX_DCACHE_LINESIZE)));

#define XMP_BARRIER_INITIALIZER(number, name)	\
  { 0, 0, 0, number, 0, 0, name }


/* xmp_barrier_init - Initialize a barrier

   Nonsynchronizing, Nonblocking 

   Usage: 
      barrier - pointer to an xmp_barrier_t
      num_cores - number of cores needed to arrive at the 
                  barrier before any are allowed through
   On exit:
      barrier initialized

   Always returns zero.

   Errors: none
*/

extern int xmp_barrier_init (xmp_barrier_t * barrier, int num_cores, 
			      const char * name);


/* xmp_barrier_wait - Wait on a barrier

   Nonsynchronizing, Nonblocking 

   Usage: 
      barrier - pointer to an xmp_barrier_t
   On exit:
      Enough cores (as determined at the barrier's initialization)
      have reached the barrier.

   Errors: none
*/

extern int xmp_barrier_wait (xmp_barrier_t * barrier);


static inline const char * xmp_barrier_name (const xmp_barrier_t * barrier)
{
  return barrier->name;
}


/* 
   xmp_barrier_trace_on
   xmp_barrier_trace_off

   Nonsynchronizing
   Nonblocking

   Turn on and off tracing for the barrier.  For
   tracing you must also set a trace file for the core.

   These functions are only present in the debug-version of the library.
*/

extern void xmp_barrier_trace_on (xmp_barrier_t * barrier);
extern void xmp_barrier_trace_off (xmp_barrier_t * barrier);


/* -------------------------------------------------------------------------
   Portions of the library that are internal, but belong here for
   convenience.
*/

extern xmp_atomic_int_t _ResetSync;

static inline void 
xmp_initial_sync (int num_cores)
{
  xmp_atomic_int_increment (&_ResetSync, 1);
  while (xmp_coherent_l32ai (&_ResetSync) != num_cores)
    xmp_spin ();
}

#ifdef __cplusplus
}
#endif

#endif /* _XMP_LIBRARY_H */
