/*
 * xtensa/cacheasm.h -- assembler-specific cache related definitions
 *			that depend on CORE configuration
 *
 *  This file is logically part of xtensa/coreasm.h ,
 *  but is kept separate for modularity / compilation-performance.
 */

/*
 * Copyright (c) 2001-2014 Cadence Design Systems, Inc.
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

#ifndef XTENSA_CACHEASM_H
#define XTENSA_CACHEASM_H

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/xtensa-xer.h>
#include <xtensa/xtensa-versions.h>

/*
 *  This header file defines assembler macros of the form:
 *	<x>cache_<func>
 *  where <x> is 'i' or 'd' for instruction and data caches,
 *  and <func> indicates the function of the macro.
 *
 *  The following functions <func> are defined,
 *  and apply only to the specified cache (I or D):
 *
 *  reset
 *	Resets the cache.
 *
 *  sync
 *	Makes sure any previous cache instructions have been completed;
 *	ie. makes sure any previous cache control operations
 *	have had full effect and been synchronized to memory.
 *	Eg. any invalidate completed [so as not to generate a hit],
 *	any writebacks or other pipelined writes written to memory, etc.
 *
 *  invalidate_line		(single cache line)
 *  invalidate_region		(specified memory range)
 *  invalidate_all		(entire cache)
 *	Invalidates all cache entries that cache
 *	data from the specified memory range.
 *	NOTE: locked entries are not invalidated.
 *
 *  writeback_line		(single cache line)
 *  writeback_region		(specified memory range)
 *  writeback_all		(entire cache)
 *	Writes back to memory all dirty cache entries
 *	that cache data from the specified memory range,
 *	and marks these entries as clean.
 *	NOTE: on some future implementations, this might
 *		also invalidate.
 *	NOTE: locked entries are written back, but never invalidated.
 *	NOTE: instruction caches never implement writeback.
 *
 *  writeback_inv_line		(single cache line)
 *  writeback_inv_region	(specified memory range)
 *  writeback_inv_all		(entire cache)
 *	Writes back to memory all dirty cache entries
 *	that cache data from the specified memory range,
 *	and invalidates these entries (including all clean
 *	cache entries that cache data from that range).
 *	NOTE: locked entries are written back but not invalidated.
 *	NOTE: instruction caches never implement writeback.
 *
 *  lock_line			(single cache line)
 *  lock_region			(specified memory range)
 *	Prefetch and lock the specified memory range into cache.
 *	NOTE:  if any part of the specified memory range cannot
 *	be locked, a Load/Store Error (for dcache) or Instruction
 *	Fetch Error (for icache) exception occurs.  These macros don't
 *	do anything special (yet anyway) to handle this situation.
 *
 *  unlock_line			(single cache line)
 *  unlock_region		(specified memory range)
 *  unlock_all			(entire cache)
 *	Unlock cache entries that cache the specified memory range.
 *	Entries not already locked are unaffected.
 *
 *  coherence_on
 *  coherence_off
 *      Turn off and on cache coherence
 *
 */



/***************************   GENERIC -- ALL CACHES   ***************************/


/*
 *  The following macros assume the following cache size/parameter limits
 *  in the current Xtensa core implementation:
 *	cache size:	1024 bytes minimum
 *	line size:	16 - 64 bytes
 *	way count:	1 - 4
 *
 *  Minimum entries per way (ie. per associativity) = 1024 / 64 / 4 = 4
 *  Hence the assumption that each loop can execute four cache instructions.
 *
 *  Correspondingly, the offset range of instructions is assumed able to cover
 *  four lines, ie. offsets {0,1,2,3} * line_size are assumed valid for
 *  both hit and indexed cache instructions.  Ie. these offsets are all
 *  valid:  0, 16, 32, 48, 64, 96, 128, 192 (for line sizes 16, 32, 64).
 *  This is true of all original cache instructions
 *  (dhi, ihi, dhwb, dhwbi, dii, iii) which have offsets
 *  of 0 to 1020 in multiples of 4 (ie. 8 bits shifted by 2).
 *  This is also true of subsequent cache instructions
 *  (dhu, ihu, diu, iiu, diwb, diwbi, dpfl, ipfl) which have offsets
 *  of 0 to 240 in multiples of 16 (ie. 4 bits shifted by 4).
 *
 *  (Maximum cache size, currently 32k, doesn't affect the following macros.
 *  Cache ways > MMU min page size cause aliasing but that's another matter.)
 */



/*
 *  Macro to apply an 'indexed' cache instruction to the entire cache.
 *
 *  Parameters:
 *	cainst      instruction/ that takes an address register parameter
 *              and an offset parameter (in range 0 .. 3*linesize).
 *	size        size of cache in bytes
 *	linesize    size of cache line in bytes (always power-of-2)
 *	assoc_or1   number of associativities (ways/sets) in cache
 *                  if all sets affected by cainst,
 *                  or 1 if only one set (or not all sets) of the cache
 *                  is affected by cainst (eg. DIWB or DIWBI [not yet ISA defined]).
 *	aa, ab      unique address registers (temporaries).
 *	awb         set to other than a0 if wb type of instruction
 *	loopokay    1 allows use of zero-overhead loops, 0 does not
 *	immrange    range (max value) of cainst's immediate offset parameter, in bytes
 *              (NOTE: macro assumes immrange allows power-of-2 number of lines)
 */

	.macro	cache_index_all		cainst, size, linesize, assoc_or1, aa, ab, loopokay, maxofs, awb=a0

	//  Number of indices in cache (lines per way):
	.set	.Lindices, (\size / (\linesize * \assoc_or1))
	//  Number of indices processed per loop iteration (max 4):
	.set	.Lperloop, .Lindices
	.ifgt	.Lperloop - 4
	 .set	.Lperloop, 4
	.endif
	//  Also limit instructions per loop if cache line size exceeds immediate range:
	.set	.Lmaxperloop, (\maxofs / \linesize) + 1
	.ifgt	.Lperloop - .Lmaxperloop
	 .set	.Lperloop, .Lmaxperloop
	.endif
	//  Avoid addi of 128 which takes two instructions (addmi,addi):
	.ifeq	.Lperloop*\linesize - 128
	 .ifgt	.Lperloop - 1
	  .set	.Lperloop, .Lperloop / 2
	 .endif
	.endif

	//  \size byte cache, \linesize byte lines, \assoc_or1 way(s) affected by each \cainst.
	// XCHAL_ERRATUM_497 - don't execute using loop, to reduce the amount of added code
	.ifne	(\loopokay & XCHAL_HAVE_LOOPS && !XCHAL_ERRATUM_497)

	movi	\aa, .Lindices / .Lperloop		// number of loop iterations
	// Possible improvement: need only loop if \aa > 1 ;
	// however \aa == 1 is highly unlikely.
	movi	\ab, 0		// to iterate over cache
	loop		\aa, .Lend_cachex\@
	.set	.Li, 0 ;     .rept .Lperloop
	  \cainst	\ab, .Li*\linesize
	.set	.Li, .Li+1 ; .endr
	addi		\ab, \ab, .Lperloop*\linesize	// move to next line
.Lend_cachex\@:

	.else

	movi	\aa, (\size / \assoc_or1)
	// Possible improvement: need only loop if \aa > 1 ;
	// however \aa == 1 is highly unlikely.
	movi	\ab, 0		// to iterate over cache
	.ifne	((\awb !=a0) & XCHAL_ERRATUM_497)		// don't use awb if set to a0
	movi \awb, 0
	.endif
.Lstart_cachex\@:
	.set	.Li, 0 ;     .rept .Lperloop
	  \cainst	\ab, .Li*\linesize
	.set	.Li, .Li+1 ; .endr
	.ifne	((\awb !=a0) & XCHAL_ERRATUM_497)		// do memw after 8 cainst wb instructions
	addi \awb, \awb, .Lperloop
	blti \awb, 8, .Lstart_memw\@
	memw
	movi \awb, 0
.Lstart_memw\@:
	.endif
	addi		\ab, \ab, .Lperloop*\linesize	// move to next line
	bltu		\ab, \aa, .Lstart_cachex\@
	.endif

	.endm


/*
 *  Macro to apply a 'hit' cache instruction to a memory region,
 *  ie. to any cache entries that cache a specified portion (region) of memory.
 *  Takes care of the unaligned cases, ie. may apply to one
 *  more cache line than $asize / lineSize if $aaddr is not aligned.
 *
 *
 *  Parameters are:
 *	cainst	instruction/macro that takes an address register parameter
 *		and an offset parameter (currently always zero)
 *		and generates a cache instruction (eg. "dhi", "dhwb", "ihi", etc.)
 *	linesize_log2	log2(size of cache line in bytes)
 *	addr	register containing start address of region (clobbered)
 *	asize	register containing size of the region in bytes (clobbered)
 *	askew	unique register used as temporary
 *	awb		unique register used as temporary for erratum 497.
 *
 *  Note: A possible optimization to this macro is to apply the operation
 *  to the entire cache if the region exceeds the size of the cache
 *  by some empirically determined amount or factor.  Some experimentation
 *  is required to determine the appropriate factors, which also need
 *  to be tunable if required.
 */

	.macro	cache_hit_region	cainst, linesize_log2, addr, asize, askew, awb=a0

	//  Make \asize the number of iterations:
	extui	\askew, \addr, 0, \linesize_log2	// get unalignment amount of \addr
	add	\asize, \asize, \askew			// ... and add it to \asize
	addi	\asize, \asize, (1 << \linesize_log2) - 1	// round up!
	srli	\asize, \asize, \linesize_log2

	//  Iterate over region:
	.ifne	((\awb !=a0) & XCHAL_ERRATUM_497)		// don't use awb if set to a0
	movi \awb, 0
	.endif
	floopnez	\asize, cacheh\@
	\cainst		\addr, 0
	.ifne	((\awb !=a0) & XCHAL_ERRATUM_497)		// do memw after 8 cainst wb instructions
	addi \awb, \awb, 1
	blti \awb, 8, .Lstart_memw\@
	memw
	movi \awb, 0
.Lstart_memw\@:
	.endif
	addi		\addr, \addr, (1 << \linesize_log2)	// move to next line
	floopend	\asize, cacheh\@
	.endm





/***************************   INSTRUCTION CACHE   ***************************/


/*
 *  Reset/initialize the instruction cache by simply invalidating it:
 *  (need to unlock first also, if cache locking implemented):
 *
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	icache_reset	aa, ab, loopokay=0
	icache_unlock_all	\aa, \ab, \loopokay
	icache_invalidate_all	\aa, \ab, \loopokay
	.endm


/*
 * Synchronize after an instruction cache operation,
 * to be sure everything is in sync with memory as to be
 * expected following any previous instruction cache control operations.
 *
 * Even if a config doesn't have caches, an isync is still needed
 * when instructions in any memory are modified, whether by a loader
 * or self-modifying code.  Therefore, this macro always produces
 * an isync, whether or not an icache is present.
 *
 * Parameters are:
 *	ar	an address register (temporary) (currently unused, but may be used in future)
 */
	.macro	icache_sync	ar
	isync
	.endm



/*
 *  Invalidate a single line of the instruction cache.
 *  Parameters are:
 *	ar	address register that contains (virtual) address to invalidate
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	(optional) offset to add to \ar to compute effective address to invalidate
 *		(note: some number of lsbits are ignored)
 */
	.macro	icache_invalidate_line	ar, offset
#if XCHAL_ICACHE_SIZE > 0
	ihi	\ar, \offset		// invalidate icache line
	icache_sync	\ar
#endif
	.endm




/*
 *  Invalidate instruction  cache entries that cache a specified portion of memory.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	icache_invalidate_region	astart, asize, ac
#if XCHAL_ICACHE_SIZE > 0
	//  Instruction cache region invalidation:
	cache_hit_region	ihi, XCHAL_ICACHE_LINEWIDTH, \astart, \asize, \ac
	icache_sync	\ac
	//  End of instruction cache region invalidation
#endif
	.endm



/*
 *  Invalidate entire instruction cache.
 *
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	icache_invalidate_all	aa, ab, loopokay=1
#if XCHAL_ICACHE_SIZE > 0
	//  Instruction cache invalidation:
	cache_index_all		iii, XCHAL_ICACHE_SIZE, XCHAL_ICACHE_LINESIZE, XCHAL_ICACHE_WAYS, \aa, \ab, \loopokay, 1020
	icache_sync	\aa
	//  End of instruction cache invalidation
#endif
	.endm



/*
 *  Lock (prefetch & lock) a single line of the instruction cache.
 *
 *  Parameters are:
 *	ar	address register that contains (virtual) address to lock
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	offset to add to \ar to compute effective address to lock
 *		(note: some number of lsbits are ignored)
 */
	.macro	icache_lock_line	ar, offset
#if XCHAL_ICACHE_SIZE > 0 && XCHAL_ICACHE_LINE_LOCKABLE
	ipfl	\ar, \offset	/* prefetch and lock icache line */
	icache_sync	\ar
#endif
	.endm



/*
 *  Lock (prefetch & lock) a specified portion of memory into the instruction cache.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	icache_lock_region	astart, asize, ac
#if XCHAL_ICACHE_SIZE > 0 && XCHAL_ICACHE_LINE_LOCKABLE
	//  Instruction cache region lock:
	cache_hit_region	ipfl, XCHAL_ICACHE_LINEWIDTH, \astart, \asize, \ac
	icache_sync	\ac
	//  End of instruction cache region lock
#endif
	.endm



/*
 *  Unlock a single line of the instruction cache.
 *
 *  Parameters are:
 *	ar	address register that contains (virtual) address to unlock
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	offset to add to \ar to compute effective address to unlock
 *		(note: some number of lsbits are ignored)
 */
	.macro	icache_unlock_line	ar, offset
#if XCHAL_ICACHE_SIZE > 0 && XCHAL_ICACHE_LINE_LOCKABLE
	ihu	\ar, \offset	/* unlock icache line */
	icache_sync	\ar
#endif
	.endm



/*
 *  Unlock a specified portion of memory from the instruction cache.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	icache_unlock_region	astart, asize, ac
#if XCHAL_ICACHE_SIZE > 0 && XCHAL_ICACHE_LINE_LOCKABLE
	//  Instruction cache region unlock:
	cache_hit_region	ihu, XCHAL_ICACHE_LINEWIDTH, \astart, \asize, \ac
	icache_sync	\ac
	//  End of instruction cache region unlock
#endif
	.endm



/*
 *  Unlock entire instruction cache.
 *
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	icache_unlock_all	aa, ab, loopokay=1
#if XCHAL_ICACHE_SIZE > 0 && XCHAL_ICACHE_LINE_LOCKABLE
	//  Instruction cache unlock:
	cache_index_all		iiu, XCHAL_ICACHE_SIZE, XCHAL_ICACHE_LINESIZE, 1, \aa, \ab, \loopokay, 240
	icache_sync	\aa
	//  End of instruction cache unlock
#endif
	.endm





/***************************   DATA CACHE   ***************************/



/*
 *  Reset/initialize the data cache by simply invalidating it
 *  (need to unlock first also, if cache locking implemented):
 *
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	dcache_reset	aa, ab, loopokay=0
	dcache_unlock_all	\aa, \ab, \loopokay
	dcache_invalidate_all	\aa, \ab, \loopokay
	.endm




/*
 * Synchronize after a data cache operation,
 * to be sure everything is in sync with memory as to be
 * expected following any previous data cache control operations.
 *
 * Parameters are:
 *	ar	an address register (temporary) (currently unused, but may be used in future)
 */
	.macro	dcache_sync	ar, wbtype=0
#if XCHAL_DCACHE_SIZE > 0
	//  No synchronization is needed.
	//  (memw may be desired e.g. after writeback operation to help ensure subsequent
	//   external accesses are seen to follow that writeback, however that's outside
	//   the scope of this macro)

	//dsync
	.ifne	(\wbtype & XCHAL_ERRATUM_497)
	memw
	.endif
#endif
	.endm



/*
 * Turn on cache coherence.
 *
 * WARNING:  for RE-201x.x and later hardware, any interrupt that tries
 * to change MEMCTL will see its changes dropped if the interrupt comes
 * in the middle of this routine.  If this might be an issue, call this
 * routine with interrupts disabled.
 *
 * Parameters are:
 *	ar,at	two scratch address registers (both clobbered)
 */
	.macro	cache_coherence_on	ar at
#if XCHAL_DCACHE_IS_COHERENT
# if XCHAL_HW_MIN_VERSION >= XTENSA_HWVERSION_RE_2012_0
	/*  Have MEMCTL.  Enable snoop responses.  */
	rsr.memctl	\ar
	movi		\at, MEMCTL_SNOOP_EN
	or		\ar, \ar, \at
	wsr.memctl	\ar
# elif XCHAL_HAVE_EXTERN_REGS && XCHAL_HAVE_MX
	/* Opt into coherence for MX (for backward compatibility / testing).  */
	movi	\ar, 1
	movi	\at, XER_CCON
	wer	\ar, \at
	extw
# endif
#endif
	.endm



/*
 * Turn off cache coherence.
 *
 * NOTE:  this is generally preceded by emptying the cache;
 * see xthal_cache_coherence_optout() in hal/coherence.c for details.
 *
 * WARNING:  for RE-201x.x and later hardware, any interrupt that tries
 * to change MEMCTL will see its changes dropped if the interrupt comes
 * in the middle of this routine.  If this might be an issue, call this
 * routine with interrupts disabled.
 *
 * Parameters are:
 *	ar,at	two scratch address registers (both clobbered)
 */
	.macro	cache_coherence_off	ar at
#if XCHAL_DCACHE_IS_COHERENT
# if XCHAL_HW_MIN_VERSION >= XTENSA_HWVERSION_RE_2012_0
	/*  Have MEMCTL.  Disable snoop responses.  */
	rsr.memctl	\ar
	movi		\at, ~MEMCTL_SNOOP_EN
	and		\ar, \ar, \at
	wsr.memctl	\ar
# elif XCHAL_HAVE_EXTERN_REGS && XCHAL_HAVE_MX
	/* Opt out of coherence, for MX (for backward compatibility / testing).  */
	extw
	movi	\at, 0
	movi	\ar, XER_CCON
	wer	\at, \ar
	extw
# endif
#endif
	.endm



/*
 * Synchronize after a data store operation,
 * to be sure the stored data is completely off the processor
 * (and assuming there is no buffering outside the processor,
 *  that the data is in memory).  This may be required to
 * ensure that the processor's write buffers are emptied.
 * A MEMW followed by a read guarantees this, by definition.
 * We also try to make sure the read itself completes.
 *
 * Parameters are:
 *	ar	an address register (temporary)
 */
	.macro	write_sync	ar
	memw			// ensure previous memory accesses are complete prior to subsequent memory accesses
	l32i	\ar, sp, 0	// completing this read ensures any previous write has completed, because of MEMW
	//slot
	add	\ar, \ar, \ar	// use the result of the read to help ensure the read completes (in future architectures)
	.endm


/*
 *  Invalidate a single line of the data cache.
 *  Parameters are:
 *	ar	address register that contains (virtual) address to invalidate
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	(optional) offset to add to \ar to compute effective address to invalidate
 *		(note: some number of lsbits are ignored)
 */
	.macro	dcache_invalidate_line	ar, offset
#if XCHAL_DCACHE_SIZE > 0
	dhi	\ar, \offset
	dcache_sync	\ar
#endif
	.endm





/*
 *  Invalidate data cache entries that cache a specified portion of memory.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	dcache_invalidate_region	astart, asize, ac
#if XCHAL_DCACHE_SIZE > 0
	//  Data cache region invalidation:
	cache_hit_region	dhi, XCHAL_DCACHE_LINEWIDTH, \astart, \asize, \ac
	dcache_sync	\ac
	//  End of data cache region invalidation
#endif
	.endm



/*
 *  Invalidate entire data cache.
 *
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	dcache_invalidate_all	aa, ab, loopokay=1
#if XCHAL_DCACHE_SIZE > 0
	//  Data cache invalidation:
	cache_index_all		dii, XCHAL_DCACHE_SIZE, XCHAL_DCACHE_LINESIZE, XCHAL_DCACHE_WAYS, \aa, \ab, \loopokay, 1020
	dcache_sync	\aa
	//  End of data cache invalidation
#endif
	.endm



/*
 *  Writeback a single line of the data cache.
 *  Parameters are:
 *	ar	address register that contains (virtual) address to writeback
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	offset to add to \ar to compute effective address to writeback
 *		(note: some number of lsbits are ignored)
 */
	.macro	dcache_writeback_line	ar, offset
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_IS_WRITEBACK
	dhwb	\ar, \offset
	dcache_sync	\ar, wbtype=1
#endif
	.endm



/*
 *  Writeback dirty data cache entries that cache a specified portion of memory.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	dcache_writeback_region		astart, asize, ac, awb
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_IS_WRITEBACK
	//  Data cache region writeback:
	cache_hit_region	dhwb, XCHAL_DCACHE_LINEWIDTH, \astart, \asize, \ac, \awb
	dcache_sync	\ac, wbtype=1
	//  End of data cache region writeback
#endif
	.endm



/*
 *  Writeback entire data cache.
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	dcache_writeback_all	aa, ab, awb, loopokay=1
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_IS_WRITEBACK
	//  Data cache writeback:
	cache_index_all		diwb, XCHAL_DCACHE_SIZE, XCHAL_DCACHE_LINESIZE, 1, \aa, \ab, \loopokay, 240, \awb,
	dcache_sync	\aa, wbtype=1
	//  End of data cache writeback
#endif
	.endm



/*
 *  Writeback and invalidate a single line of the data cache.
 *  Parameters are:
 *	ar	address register that contains (virtual) address to writeback and invalidate
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	offset to add to \ar to compute effective address to writeback and invalidate
 *		(note: some number of lsbits are ignored)
 */
	.macro	dcache_writeback_inv_line	ar, offset
#if XCHAL_DCACHE_SIZE > 0
	dhwbi	\ar, \offset	/* writeback and invalidate dcache line */
	dcache_sync	\ar, wbtype=1
#endif
	.endm



/*
 *  Writeback and invalidate data cache entries that cache a specified portion of memory.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	dcache_writeback_inv_region	astart, asize, ac, awb
#if XCHAL_DCACHE_SIZE > 0
	//  Data cache region writeback and invalidate:
	cache_hit_region	dhwbi, XCHAL_DCACHE_LINEWIDTH, \astart, \asize, \ac, \awb
	dcache_sync	\ac, wbtype=1
	//  End of data cache region writeback and invalidate
#endif
	.endm



/*
 *  Writeback and invalidate entire data cache.
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	dcache_writeback_inv_all	aa, ab, awb, loopokay=1
#if XCHAL_DCACHE_SIZE > 0
	//  Data cache writeback and invalidate:
#if XCHAL_DCACHE_IS_WRITEBACK
	cache_index_all		diwbi, XCHAL_DCACHE_SIZE, XCHAL_DCACHE_LINESIZE, 1, \aa, \ab, \loopokay, 240, \awb
	dcache_sync	\aa, wbtype=1
#else /*writeback*/
	//  Data cache does not support writeback, so just invalidate: */
	dcache_invalidate_all	\aa, \ab, \loopokay
#endif /*writeback*/
	//  End of data cache writeback and invalidate
#endif
	.endm




/*
 *  Lock (prefetch & lock) a single line of the data cache.
 *
 *  Parameters are:
 *	ar	address register that contains (virtual) address to lock
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	offset to add to \ar to compute effective address to lock
 *		(note: some number of lsbits are ignored)
 */
	.macro	dcache_lock_line	ar, offset
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_LINE_LOCKABLE
	dpfl	\ar, \offset	/* prefetch and lock dcache line */
	dcache_sync	\ar
#endif
	.endm



/*
 *  Lock (prefetch & lock) a specified portion of memory into the data cache.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	dcache_lock_region	astart, asize, ac
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_LINE_LOCKABLE
	//  Data cache region lock:
	cache_hit_region	dpfl, XCHAL_DCACHE_LINEWIDTH, \astart, \asize, \ac
	dcache_sync	\ac
	//  End of data cache region lock
#endif
	.endm



/*
 *  Unlock a single line of the data cache.
 *
 *  Parameters are:
 *	ar	address register that contains (virtual) address to unlock
 *		(may get clobbered in a future implementation, but not currently)
 *	offset	offset to add to \ar to compute effective address to unlock
 *		(note: some number of lsbits are ignored)
 */
	.macro	dcache_unlock_line	ar, offset
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_LINE_LOCKABLE
	dhu	\ar, \offset	/* unlock dcache line */
	dcache_sync	\ar
#endif
	.endm



/*
 *  Unlock a specified portion of memory from the data cache.
 *  Parameters are:
 *	astart	start address (register gets clobbered)
 *	asize	size of the region in bytes (register gets clobbered)
 *	ac	unique register used as temporary
 */
	.macro	dcache_unlock_region	astart, asize, ac
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_LINE_LOCKABLE
	//  Data cache region unlock:
	cache_hit_region	dhu, XCHAL_DCACHE_LINEWIDTH, \astart, \asize, \ac
	dcache_sync	\ac
	//  End of data cache region unlock
#endif
	.endm



/*
 *  Unlock entire data cache.
 *
 *  Parameters:
 *	aa, ab		unique address registers (temporaries)
 */
	.macro	dcache_unlock_all	aa, ab, loopokay=1
#if XCHAL_DCACHE_SIZE > 0 && XCHAL_DCACHE_LINE_LOCKABLE
	//  Data cache unlock:
	cache_index_all		diu, XCHAL_DCACHE_SIZE, XCHAL_DCACHE_LINESIZE, 1, \aa, \ab,  \loopokay, 240
	dcache_sync	\aa
	//  End of data cache unlock
#endif
	.endm



/*
 * Get the number of enabled icache ways. Note that this may
 * be different from the value read from the MEMCTL register.
 *
 * Parameters:
 *	aa	address register where value is returned
 */
	.macro	icache_get_ways		aa
#if XCHAL_ICACHE_SIZE > 0
#if XCHAL_HAVE_ICACHE_DYN_WAYS
	// Read from MEMCTL and shift/mask
	rsr.memctl	\aa
	extui	\aa, \aa, MEMCTL_ICWU_SHIFT, MEMCTL_ICWU_BITS
	blti	\aa, XCHAL_ICACHE_WAYS, .Licgw
	movi	\aa, XCHAL_ICACHE_WAYS
.Licgw:
#else
	// All ways are always enabled
	movi	\aa, XCHAL_ICACHE_WAYS
#endif
#else
	// No icache
	movi	\aa, 0
#endif
	.endm



/*
 * Set the number of enabled icache ways.
 *
 * Parameters:
 *	aa      address register specifying number of ways (trashed)
 *	ab,ac	address register for scratch use (trashed)
 */
	.macro  icache_set_ways		aa, ab, ac
#if XCHAL_ICACHE_SIZE > 0
#if XCHAL_HAVE_ICACHE_DYN_WAYS
	movi	\ac, MEMCTL_ICWU_CLR_MASK	// set up to clear bits 18-22
	rsr.memctl	\ab
	and	\ab, \ab, \ac
	movi	\ac, MEMCTL_INV_EN		// set bit 23
	slli	\aa, \aa, MEMCTL_ICWU_SHIFT	// move to right spot
	or	\ab, \ab, \aa
	or	\ab, \ab, \ac
	wsr.memctl	\ab
	isync
#else
	// All ways are always enabled
#endif
#else
	// No icache
#endif
	.endm



/*
 * Get the number of enabled dcache ways. Note that this may
 * be different from the value read from the MEMCTL register.
 *
 * Parameters:
 *	aa	address register where value is returned
 */
	.macro	dcache_get_ways		aa
#if XCHAL_DCACHE_SIZE > 0
#if XCHAL_HAVE_DCACHE_DYN_WAYS
	// Read from MEMCTL and shift/mask
	rsr.memctl	\aa
	extui	\aa, \aa, MEMCTL_DCWU_SHIFT, MEMCTL_DCWU_BITS
	blti	\aa, XCHAL_DCACHE_WAYS, .Ldcgw
	movi	\aa, XCHAL_DCACHE_WAYS
.Ldcgw:
#else
	// All ways are always enabled
	movi	\aa, XCHAL_DCACHE_WAYS
#endif
#else
	// No dcache
	movi	\aa, 0
#endif
	.endm



/*
 * Set the number of enabled dcache ways.
 *
 * Parameters:
 *	aa	address register specifying number of ways (trashed)
 *	ab,ac	address register for scratch use (trashed)
 */
	.macro	dcache_set_ways		aa, ab, ac
#if (XCHAL_DCACHE_SIZE > 0) && XCHAL_HAVE_DCACHE_DYN_WAYS
	movi	\ac, MEMCTL_DCWA_CLR_MASK	// set up to clear bits 13-17
	rsr.memctl	\ab
	and	\ab, \ab, \ac			// clear ways allocatable
	slli	\ac, \aa, MEMCTL_DCWA_SHIFT
	or	\ab, \ab, \ac			// set ways allocatable
	wsr.memctl	\ab
#if XCHAL_DCACHE_IS_WRITEBACK
	// Check if the way count is increasing or decreasing
	extui	\ac, \ab, MEMCTL_DCWU_SHIFT, MEMCTL_DCWU_BITS			// bits 8-12 - ways in use
	bge	\aa, \ac, .Ldsw3						// equal or increasing
	slli	\ab, \aa, XCHAL_DCACHE_LINEWIDTH + XCHAL_DCACHE_SETWIDTH	// start way number
	slli	\ac, \ac, XCHAL_DCACHE_LINEWIDTH + XCHAL_DCACHE_SETWIDTH	// end way number
.Ldsw1:
	diwbui.p	\ab		// auto-increments ab
	bge	\ab, \ac, .Ldsw2
	beqz	\ab, .Ldsw2
	j	.Ldsw1
.Ldsw2:
	rsr.memctl	\ab
#endif
.Ldsw3:
	// No dirty data to write back, just set the new number of ways
	movi	\ac, MEMCTL_DCWU_CLR_MASK		// set up to clear bits 8-12
	and	\ab, \ab, \ac				// clear ways in use
	movi	\ac, MEMCTL_INV_EN
	or	\ab, \ab, \ac				// set bit 23
	slli	\aa, \aa, MEMCTL_DCWU_SHIFT
	or	\ab, \ab, \aa				// set ways in use
	wsr.memctl	\ab
#else
	// No dcache or no way disable support
#endif
	.endm

#endif /*XTENSA_CACHEASM_H*/
