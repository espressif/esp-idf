/*
 * xtensa/core-macros.h -- C specific definitions
 *                         that depend on CORE configuration
 */

/*
 * Copyright (c) 2012 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef XTENSA_CACHE_H
#define XTENSA_CACHE_H

#include <xtensa/config/core.h>

/*  Only define things for C code.  */
#if !defined(_ASMLANGUAGE) && !defined(_NOCLANGUAGE) && !defined(__ASSEMBLER__)



/***************************   CACHE   ***************************/

/* All the macros are in the lower case now and some of them 
 * share the name with the existing functions from hal.h.
 * Including this header file will define XTHAL_USE_CACHE_MACROS 
 * which directs hal.h not to use the functions.
 */

/*
 *  Single-cache-line operations in C-callable inline assembly.
 *  Essentially macro versions (uppercase) of:
 *
 *	xthal_icache_line_invalidate(void *addr);
 *	xthal_icache_line_lock(void *addr);
 *	xthal_icache_line_unlock(void *addr);
 *	xthal_icache_sync(void);
 *
 *  NOTE:  unlike the above functions, the following macros do NOT
 *  execute the xthal_icache_sync() as part of each line operation.
 *  This sync must be called explicitly by the caller.  This is to
 *  allow better optimization when operating on more than one line.
 *
 *	xthal_dcache_line_invalidate(void *addr);
 *	xthal_dcache_line_writeback(void *addr);
 *	xthal_dcache_line_writeback_inv(void *addr);
 *	xthal_dcache_line_lock(void *addr);
 *	xthal_dcache_line_unlock(void *addr);
 *	xthal_dcache_sync(void);
 *	xthal_dcache_line_prefetch_for_write(void *addr);
 *	xthal_dcache_line_prefetch_for_read(void *addr);
 *
 *  All are made memory-barriers, given that's how they're typically used
 *  (ops operate on a whole line, so clobbers all memory not just *addr).
 *
 *  NOTE:  All the block block cache ops and line prefetches are implemented
 *  using intrinsics so they are better optimized regarding memory barriers etc.
 *  
 * All block downgrade functions exist in two forms: with and without
 * the 'max' parameter: This parameter allows compiler to optimize
 * the functions whenever the parameter is smaller than the cache size.
 *
 *	xthal_dcache_block_invalidate(void *addr, unsigned size);
 *	xthal_dcache_block_writeback(void *addr, unsigned size);
 *	xthal_dcache_block_writeback_inv(void *addr, unsigned size);
 *	xthal_dcache_block_invalidate_max(void *addr, unsigned size, unsigned max);
 *	xthal_dcache_block_writeback_max(void *addr, unsigned size, unsigned max);
 *	xthal_dcache_block_writeback_inv_max(void *addr, unsigned size, unsigned max);
 *
 *      xthal_dcache_block_prefetch_for_read(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_for_write(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_modify(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_read_write(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_for_read_grp(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_for_write_grp(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_modify_grp(void *addr, unsigned size);
 *      xthal_dcache_block_prefetch_read_write_grp(void *addr, unsigned size)
 *
 *	xthal_dcache_block_wait();
 *	xthal_dcache_block_required_wait();
 *	xthal_dcache_block_abort();
 *	xthal_dcache_block_prefetch_end();
 *	xthal_dcache_block_newgrp();
 */

/***   INSTRUCTION CACHE   ***/

#define XTHAL_USE_CACHE_MACROS

#if XCHAL_ICACHE_SIZE > 0
# define xthal_icache_line_invalidate(addr)	do { void *__a = (void*)(addr); \
		__asm__ __volatile__("ihi %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
#else
# define xthal_icache_line_invalidate(addr)	do {/*nothing*/} while(0)
#endif

#if XCHAL_ICACHE_SIZE > 0 && XCHAL_ICACHE_LINE_LOCKABLE
# define xthal_icache_line_lock(addr)	do { void *__a = (void*)(addr);	 	\
		__asm__ __volatile__("ipfl %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
# define xthal_icache_line_unlock(addr)	do { void *__a = (void*)(addr); 	\
		__asm__ __volatile__("ihu %0, 0" :: "a"(__a) : "memory");	\
		} while(0)
#else
# define xthal_icache_line_lock(addr)		do {/*nothing*/} while(0)
# define xthal_icache_line_unlock(addr)		do {/*nothing*/} while(0)
#endif

/*
 * Even if a config doesn't have caches, an isync is still needed
 * when instructions in any memory are modified, whether by a loader
 * or self-modifying code.  Therefore, this macro always produces
 * an isync, whether or not an icache is present.
 */
#define xthal_icache_sync()							\
		__asm__ __volatile__("isync":::"memory")


/***   DATA CACHE   ***/

#if XCHAL_DCACHE_SIZE > 0

# include <xtensa/tie/xt_datacache.h>

# define xthal_dcache_line_invalidate(addr)	do { void *__a = (void*)(addr); \
		__asm__ __volatile__("dhi %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
# define xthal_dcache_line_writeback(addr)	do { void *__a = (void*)(addr); \
		__asm__ __volatile__("dhwb %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
# define xthal_dcache_line_writeback_inv(addr)	do { void *__a = (void*)(addr); \
		__asm__ __volatile__("dhwbi %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
# define xthal_dcache_sync()							\
		__asm__ __volatile__("" /*"dsync"?*/:::"memory")
# define xthal_dcache_line_prefetch_for_read(addr) do {				\
		XT_DPFR((const int*)addr, 0);					\
		} while(0)
#else
# define xthal_dcache_line_invalidate(addr)		do {/*nothing*/} while(0)
# define xthal_dcache_line_writeback(addr)		do {/*nothing*/} while(0)
# define xthal_dcache_line_writeback_inv(addr)		do {/*nothing*/} while(0)
# define xthal_dcache_sync()				__asm__ __volatile__("":::"memory")
# define xthal_dcache_line_prefetch_for_read(addr)	do {/*nothing*/} while(0)
#endif

#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_LINE_LOCKABLE
# define xthal_dcache_line_lock(addr)	do { void *__a = (void*)(addr); 	\
		__asm__ __volatile__("dpfl %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
# define xthal_dcache_line_unlock(addr)	do { void *__a = (void*)(addr); 	\
		__asm__ __volatile__("dhu %0, 0" :: "a"(__a) : "memory"); 	\
		} while(0)
#else
# define xthal_dcache_line_lock(addr)		do {/*nothing*/} while(0)
# define xthal_dcache_line_unlock(addr)		do {/*nothing*/} while(0)
#endif

#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_IS_WRITEBACK

# define xthal_dcache_line_prefetch_for_write(addr) do {			\
		XT_DPFW((const int*)addr, 0);					\
		} while(0)
#else
# define xthal_dcache_line_prefetch_for_write(addr)	do {/*nothing*/} while(0)
#endif


/*****   Block Operations   *****/

#if XCHAL_DCACHE_SIZE > 0 && XCHAL_HAVE_CACHE_BLOCKOPS

/* upgrades */

# define _XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, type)		\
		{						\
		type((const int*)addr, size);			\
		}

/*downgrades */

# define _XTHAL_DCACHE_BLOCK_DOWNGRADE(addr, size, type)	\
	unsigned _s = size;					\
	unsigned _a = addr;					\
	do {							\
		unsigned __s = (_s > XCHAL_DCACHE_SIZE) ? 	\
				XCHAL_DCACHE_SIZE : _s; 	\
		type((const int*)_a, __s);			\
		_s -= __s; 					\
		_a += __s;					\
	} while(_s > 0);

# define _XTHAL_DCACHE_BLOCK_DOWNGRADE_MAX(addr, size, type, max)	\
	if (max <= XCHAL_DCACHE_SIZE) {					\
		unsigned _s = size;					\
		unsigned _a = addr;					\
		type((const int*)_a, _s);				\
	}								\
	else {								\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE(addr, size, type);	\
	}

# define xthal_dcache_block_invalidate(addr, size)	do { 		\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE(addr, size, XT_DHI_B);	\
		} while(0)
# define xthal_dcache_block_writeback(addr, size)	do {		\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE(addr, size, XT_DHWB_B);	\
		} while(0)
# define xthal_dcache_block_writeback_inv(addr, size)	do {		\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE(addr, size, XT_DHWBI_B);	\
		} while(0)

# define xthal_dcache_block_invalidate_max(addr, size, max)	do {		\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE_MAX(addr, size, XT_DHI_B, max);	\
		} while(0)
# define xthal_dcache_block_writeback_max(addr, size, max)	do { 		\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE_MAX(addr, size, XT_DHWB_B, max);	\
		} while(0)
# define xthal_dcache_block_writeback_inv_max(addr, size, max)	do {		\
		_XTHAL_DCACHE_BLOCK_DOWNGRADE_MAX(addr, size, XT_DHWBI_B, max);	\
		} while(0)

/* upgrades that are performed even with write-thru caches  */

# define xthal_dcache_block_prefetch_read_write(addr, size) do {	\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFW_B);	\
		} while(0)
# define xthal_dcache_block_prefetch_read_write_grp(addr, size) do {	\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFW_BF);	\
		} while(0)
# define xthal_dcache_block_prefetch_for_read(addr, size) do {		\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFR_B);	\
		} while(0)
# define xthal_dcache_block_prefetch_for_read_grp(addr, size) do {	\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFR_BF);	\
		} while(0)

/* abort all or end optional block cache operations */
# define xthal_dcache_block_abort()	do { 		\
		XT_PFEND_A();				\
		} while(0)
# define xthal_dcache_block_end()	do { 		\
		XT_PFEND_O();				\
		} while(0)

/* wait for all/required block cache operations to finish */
# define xthal_dcache_block_wait()	do { 		\
		XT_PFWAIT_A();				\
		} while(0)
# define xthal_dcache_block_required_wait()	do { 	\
		XT_PFWAIT_R();				\
		} while(0)
/* Start a new group */
# define xthal_dcache_block_newgrp()	do { 		\
		XT_PFNXT_F();				\
		} while(0)
#else
# define xthal_dcache_block_invalidate(addr, size)		do {/*nothing*/} while(0)
# define xthal_dcache_block_writeback(addr, size)		do {/*nothing*/} while(0)
# define xthal_dcache_block_writeback_inv(addr, size)		do {/*nothing*/} while(0)
# define xthal_dcache_block_invalidate_max(addr, size, max)	do {/*nothing*/} while(0)
# define xthal_dcache_block_writeback_max(addr, size, max)	do {/*nothing*/} while(0)
# define xthal_dcache_block_writeback_inv_max(addr, size, max) 	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_read_write(addr, size)	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_read_write_grp(addr, size)	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_for_read(addr, size)	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_for_read_grp(addr, size)	do {/*nothing*/} while(0)
# define xthal_dcache_block_end()   				do {/*nothing*/} while(0)
# define xthal_dcache_block_abort()  				do {/*nothing*/} while(0)
# define xthal_dcache_block_wait()  				do {/*nothing*/} while(0)
# define xthal_dcache_block_required_wait()   			do {/*nothing*/} while(0)
# define xthal_dcache_block_newgrp()				do {/*nothing*/} while(0)
#endif

#if XCHAL_DCACHE_SIZE > 0 && XCHAL_HAVE_CACHE_BLOCKOPS && XCHAL_DCACHE_IS_WRITEBACK

# define xthal_dcache_block_prefetch_for_write(addr, size) do {	\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFW_B);	\
		} while(0)
# define xthal_dcache_block_prefetch_modify(addr, size) do {		\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFM_B);	\
		} while(0)
# define xthal_dcache_block_prefetch_for_write_grp(addr, size) do {	\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFW_BF);	\
		} while(0)
# define xthal_dcache_block_prefetch_modify_grp(addr, size) do {	\
		_XTHAL_DCACHE_BLOCK_UPGRADE(addr, size, XT_DPFM_BF);	\
		} while(0)
#else
# define xthal_dcache_block_prefetch_for_write(addr, size)	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_modify(addr, size) 	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_for_write_grp(addr, size)	do {/*nothing*/} while(0)
# define xthal_dcache_block_prefetch_modify_grp(addr, size)  	do {/*nothing*/} while(0)
#endif

/***************************   INTERRUPTS   ***************************/

/*
 *  Macro versions of:
 *	unsigned  xthal_get_intenable( void );
 *	void      xthal_set_intenable( unsigned );
 *	unsigned  xthal_get_interrupt( void );
 *	void      xthal_set_intset( unsigned );
 *	void      xthal_set_intclear( unsigned );
 *	unsigned  xthal_get_ccount(void);
 *	void      xthal_set_ccompare(int, unsigned);
 *	unsigned  xthal_get_ccompare(int);
 *
 *  NOTE: for {set,get}_ccompare, the first argument MUST be a decimal constant.
 */

#if XCHAL_HAVE_INTERRUPTS
# define XTHAL_GET_INTENABLE()	({ int __intenable; \
				__asm__("rsr.intenable %0" : "=a"(__intenable)); \
				__intenable; })
# define XTHAL_SET_INTENABLE(v)	do { int __intenable = (int)(v); \
			__asm__ __volatile__("wsr.intenable %0" :: "a"(__intenable):"memory"); \
				} while(0)
# define XTHAL_GET_INTERRUPT()	({ int __interrupt; \
				__asm__ __volatile__("rsr.interrupt %0" : "=a"(__interrupt)); \
				__interrupt; })
# define XTHAL_SET_INTSET(v)	do { int __interrupt = (int)(v); \
			__asm__ __volatile__("wsr.intset %0" :: "a"(__interrupt):"memory"); \
				} while(0)
# define XTHAL_SET_INTCLEAR(v)	do { int __interrupt = (int)(v); \
			__asm__ __volatile__("wsr.intclear %0" :: "a"(__interrupt):"memory"); \
				} while(0)
# define XTHAL_GET_CCOUNT()	({ int __ccount; \
				__asm__ __volatile__("rsr.ccount %0" : "=a"(__ccount)); \
				__ccount; })
# define XTHAL_SET_CCOUNT(v)	do { int __ccount = (int)(v); \
			__asm__ __volatile__("wsr.ccount %0" :: "a"(__ccount):"memory"); \
				} while(0)
# define _XTHAL_GET_CCOMPARE(n)	({ int __ccompare; \
				__asm__("rsr.ccompare" #n " %0" : "=a"(__ccompare)); \
				__ccompare; })
# define XTHAL_GET_CCOMPARE(n)	_XTHAL_GET_CCOMPARE(n)
# define _XTHAL_SET_CCOMPARE(n,v) do { int __ccompare = (int)(v); \
			__asm__ __volatile__("wsr.ccompare" #n " %0 ; esync" :: "a"(__ccompare):"memory"); \
				} while(0)
# define XTHAL_SET_CCOMPARE(n,v) _XTHAL_SET_CCOMPARE(n,v)
#else
# define XTHAL_GET_INTENABLE()		0
# define XTHAL_SET_INTENABLE(v)		do {/*nothing*/} while(0)
# define XTHAL_GET_INTERRUPT()		0
# define XTHAL_SET_INTSET(v)		do {/*nothing*/} while(0)
# define XTHAL_SET_INTCLEAR(v)		do {/*nothing*/} while(0)
# define XTHAL_GET_CCOUNT()		0
# define XTHAL_SET_CCOUNT(v)		do {/*nothing*/} while(0)
# define XTHAL_GET_CCOMPARE(n)		0
# define XTHAL_SET_CCOMPARE(n,v)	do {/*nothing*/} while(0)
#endif


/***************************   MISC   ***************************/

/*
 *  Macro or inline versions of:
 *	void	  xthal_clear_regcached_code( void );
 *	unsigned  xthal_get_prid( void );
 *	unsigned  xthal_compare_and_set( int *addr, int testval, int setval );
 */

#if XCHAL_HAVE_LOOPS
# define XTHAL_CLEAR_REGCACHED_CODE()		\
		__asm__ __volatile__("wsr.lcount %0" :: "a"(0) : "memory")
#else
# define XTHAL_CLEAR_REGCACHED_CODE()		do {/*nothing*/} while(0)
#endif

#if XCHAL_HAVE_PRID
# define XTHAL_GET_PRID()	({ int __prid; \
				__asm__("rsr.prid %0" : "=a"(__prid)); \
				__prid; })
#else
# define XTHAL_GET_PRID()	0
#endif


static inline unsigned  XTHAL_COMPARE_AND_SET( int *addr, int testval, int setval )
{
    int result;

#if XCHAL_HAVE_S32C1I && XCHAL_HW_MIN_VERSION_MAJOR >= 2200
    __asm__ __volatile__ (
	"   wsr.scompare1 %2 \n"
	"   s32c1i %0, %3, 0 \n"
	    : "=a"(result) : "0" (setval), "a" (testval), "a" (addr)
	    : "memory");
#elif XCHAL_HAVE_INTERRUPTS
    int tmp;
    __asm__ __volatile__ (
        "   rsil   %4, 15 \n"		// %4 == saved ps
        "   l32i   %0, %3, 0 \n"	// %0 == value to test, return val
        "   bne    %2, %0, 9f \n"	// test
        "   s32i   %1, %3, 0 \n"	// write the new value
	"9: wsr.ps %4 ; rsync \n"	// restore the PS
	: "=a"(result) 
	: "0" (setval), "a" (testval), "a" (addr), "a" (tmp)
	: "memory");
#else
    __asm__ __volatile__ (
        "   l32i  %0, %3, 0 \n"		// %0 == value to test, return val
        "   bne   %2, %0, 9f \n"	// test
        "   s32i  %1, %3, 0 \n"		// write the new value
	"9: \n"
	    : "=a"(result) : "0" (setval), "a" (testval), "a" (addr)
	    : "memory");
#endif
    return result;
}

#if XCHAL_HAVE_EXTERN_REGS

static inline unsigned XTHAL_RER (unsigned int reg)
{
  unsigned result;

  __asm__ __volatile__ (
	"   rer     %0, %1"
	: "=a" (result) : "a" (reg) : "memory");

  return result;
}

static inline void XTHAL_WER (unsigned reg, unsigned value)
{
  __asm__ __volatile__ (
	"   wer     %0, %1"
	: : "a" (value), "a" (reg) : "memory");
}

#endif /* XCHAL_HAVE_EXTERN_REGS */

#endif /* C code */

#endif /*XTENSA_CACHE_H*/

