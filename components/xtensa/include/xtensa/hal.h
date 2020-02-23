/*
   xtensa/hal.h -- contains a definition of the Core HAL interface

   All definitions in this header file are independent of any specific
   Xtensa processor configuration.  Thus software (eg. OS, application,
   etc) can include this header file and be compiled into configuration-
   independent objects that can be distributed and eventually linked
   to the HAL library (libhal.a) to create a configuration-specific
   final executable.

   Certain definitions, however, are release/version-specific -- such as
   the XTHAL_RELEASE_xxx macros (or additions made in later versions).


   $Id: //depot/rel/Foxhill/dot.9/Xtensa/OS/target-os-src/hal.h.tpp#1 $

   Copyright (c) 1999-2015 Cadence Design Systems, Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef XTENSA_HAL_H
#define XTENSA_HAL_H

/****************************************************************************
	    Definitions Useful for Any Code, USER or PRIVILEGED
 ****************************************************************************/


/*----------------------------------------------------------------------
	   Constant Definitions  (shared with assembly)
  ----------------------------------------------------------------------*/

/*
 *  Software (Xtensa Tools) version information.  Not configuration-specific!
 *
 *  NOTE:  "release" is a misnomer here, these are really product "version"
 *	numbers.  A "release" is a collection of product versions
 *	made available at once (together) to customers.
 *	In the past, release and version names all matched in T####.# form,
 *	making the distinction irrelevant.  This is no longer the case.
 */
#define XTHAL_RELEASE_MAJOR	12000
#define XTHAL_RELEASE_MINOR	9
#define XTHAL_RELEASE_NAME	"12.0.9"
#define XTHAL_REL_12	1
#define XTHAL_REL_12_0	1
#define XTHAL_REL_12_0_9	1

/*  HAL version numbers (these names are for backward compatibility):  */
#define XTHAL_MAJOR_REV		XTHAL_RELEASE_MAJOR
#define XTHAL_MINOR_REV		XTHAL_RELEASE_MINOR
/*
 *  A bit of software release/version history on values of XTHAL_{MAJOR,MINOR}_REV:
 *
 *	SW Version	MAJOR	MINOR		Comment
 *	=======		=====	=====		=======
 *	T1015.n		n/a	n/a		(HAL not yet available)
 *	T1020.{0,1,2}	0	1		(HAL beta)
 *	T1020.{3,4}	0	2		First release.
 *	T1020.n (n>4)	0	2 or >3		(TBD)
 *	T1030.0		0	1		(HAL beta)
 *	T1030.{1,2}	0	3		Equivalent to first release.
 *	T1030.n (n>=3)	0	>= 3		(TBD)
 *	T1040.n		1040	n		Full CHAL available from T1040.2
 *	T1050.n		1050	n		.
 *	6.0.n		6000	n		Xtensa Tools v6   (RA-200x.n)
 *	7.0.n		7000	n		Xtensa Tools v7   (RB-200x.n)
 *	7.1.n		7010	n		Xtensa Tools v7.1 (RB-200x.(n+2))
 *	8.0.n		8000	n		Xtensa Tools v8   (RC-20xx.n)
 *	9.0.n		9000	n		Xtensa Tools v9   (RD-201x.n)
 *	10.0.n		10000	n		Xtensa Tools v10  (RE-201x.n)
 *
 *
 *  Note:  there is a distinction between the software version with
 *  which something is compiled (accessible using XTHAL_RELEASE_* macros)
 *  and the software version with which the HAL library was compiled
 *  (accessible using Xthal_release_* global variables).  This
 *  distinction is particularly relevant for vendors that distribute
 *  configuration-independent binaries (eg. an OS), where their customer
 *  might link it with a HAL of a different Xtensa software version.
 *  In this case, it may be appropriate for the OS to verify at run-time
 *  whether XTHAL_RELEASE_* and Xthal_release_* are compatible.
 *  [Guidelines as to which version is compatible with which are not
 *  currently provided explicitly, but might be inferred from reading
 *  OSKit documentation for all releases -- compatibility is also highly
 *  dependent on which HAL features are used.  Each version is usually
 *  backward compatible, with very few exceptions if any.]
 */

/*  Version comparison operators (among major/minor pairs):  */
#define XTHAL_REL_GE(maja,mina, majb,minb)	((maja) > (majb) || \
						 ((maja) == (majb) && (mina) >= (minb)))
#define XTHAL_REL_GT(maja,mina, majb,minb)	((maja) > (majb) || \
						 ((maja) == (majb) && (mina) > (minb)))
#define XTHAL_REL_LE(maja,mina, majb,minb)	((maja) < (majb) || \
						 ((maja) == (majb) && (mina) <= (minb)))
#define XTHAL_REL_LT(maja,mina, majb,minb)	((maja) < (majb) || \
						 ((maja) == (majb) && (mina) < (minb)))
#define XTHAL_REL_EQ(maja,mina, majb,minb)	((maja) == (majb) && (mina) == (minb))

/*  Fuzzy (3-way) logic operators:  */
#define XTHAL_MAYBE		-1	/* 0=NO, 1=YES, -1=MAYBE */
#define XTHAL_FUZZY_AND(a,b)	(((a)==0 || (b)==0) ? 0 : ((a)==1 && (b)==1) ? 1 : XTHAL_MAYBE)
#define XTHAL_FUZZY_OR(a,b)	(((a)==1 || (b)==1) ? 1 : ((a)==0 && (b)==0) ? 0 : XTHAL_MAYBE)
#define XTHAL_FUZZY_NOT(a)	(((a)==0 || (a)==1) ? (1-(a)) : XTHAL_MAYBE)


/*
 *  Architectural limit, independent of configuration:
 */
#define XTHAL_MAX_CPS		8	/* max number of coprocessors (0..7) */

/*  Misc:  */
#define XTHAL_LITTLEENDIAN		0
#define XTHAL_BIGENDIAN			1



#if !defined(_ASMLANGUAGE) && !defined(_NOCLANGUAGE) && !defined(__ASSEMBLER__)
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/*----------------------------------------------------------------------
				HAL
  ----------------------------------------------------------------------*/

/* Constant to be checked in build = (XTHAL_MAJOR_REV<<16)|XTHAL_MINOR_REV */
extern const unsigned int Xthal_rev_no;


/*----------------------------------------------------------------------
			Optional/Custom Processor State
  ----------------------------------------------------------------------*/

/* save & restore the extra processor state */
extern void xthal_save_extra(void *base);
extern void xthal_restore_extra(void *base);

extern void xthal_save_cpregs(void *base, int);
extern void xthal_restore_cpregs(void *base, int);
/* versions specific to each coprocessor id */
extern void xthal_save_cp0(void *base);
extern void xthal_save_cp1(void *base);
extern void xthal_save_cp2(void *base);
extern void xthal_save_cp3(void *base);
extern void xthal_save_cp4(void *base);
extern void xthal_save_cp5(void *base);
extern void xthal_save_cp6(void *base);
extern void xthal_save_cp7(void *base);
extern void xthal_restore_cp0(void *base);
extern void xthal_restore_cp1(void *base);
extern void xthal_restore_cp2(void *base);
extern void xthal_restore_cp3(void *base);
extern void xthal_restore_cp4(void *base);
extern void xthal_restore_cp5(void *base);
extern void xthal_restore_cp6(void *base);
extern void xthal_restore_cp7(void *base);
/* pointers to each of the functions above */
extern void* Xthal_cpregs_save_fn[XTHAL_MAX_CPS];
extern void* Xthal_cpregs_restore_fn[XTHAL_MAX_CPS];
/* similarly for non-windowed ABI (may be same or different) */
extern void* Xthal_cpregs_save_nw_fn[XTHAL_MAX_CPS];
extern void* Xthal_cpregs_restore_nw_fn[XTHAL_MAX_CPS];

/*extern void xthal_save_all_extra(void *base);*/
/*extern void xthal_restore_all_extra(void *base);*/

/* space for processor state */
extern const unsigned int Xthal_extra_size;
extern const unsigned int Xthal_extra_align;
extern const unsigned int Xthal_cpregs_size[XTHAL_MAX_CPS];
extern const unsigned int Xthal_cpregs_align[XTHAL_MAX_CPS];
extern const unsigned int Xthal_all_extra_size;
extern const unsigned int Xthal_all_extra_align;
/* coprocessor names */
extern const char * const Xthal_cp_names[XTHAL_MAX_CPS];

/* initialize the extra processor */
/*extern void xthal_init_extra(void);*/
/* initialize the TIE coprocessor */
/*extern void xthal_init_cp(int);*/

/* initialize the extra processor */
extern void xthal_init_mem_extra(void *);
/* initialize the TIE coprocessor */
extern void xthal_init_mem_cp(void *, int);

/* the number of TIE coprocessors contiguous from zero (for Tor2) */
extern const unsigned int Xthal_num_coprocessors;

/* actual number of coprocessors */
extern const unsigned char Xthal_cp_num;
/* index of highest numbered coprocessor, plus one */
extern const unsigned char Xthal_cp_max;
/* index of highest allowed coprocessor number, per cfg, plus one */
/*extern const unsigned char Xthal_cp_maxcfg;*/
/* bitmask of which coprocessors are present */
extern const unsigned int  Xthal_cp_mask;

/* read & write extra state register */
/*extern int xthal_read_extra(void *base, unsigned reg, unsigned *value);*/
/*extern int xthal_write_extra(void *base, unsigned reg, unsigned value);*/

/* read & write a TIE coprocessor register */
/*extern int xthal_read_cpreg(void *base, int cp, unsigned reg, unsigned *value);*/
/*extern int xthal_write_cpreg(void *base, int cp, unsigned reg, unsigned value);*/

/* return coprocessor number based on register */
/*extern int xthal_which_cp(unsigned reg);*/


/*----------------------------------------------------------------------
   				Register Windows
  ----------------------------------------------------------------------*/

/* number of registers in register window */
extern const unsigned int  Xthal_num_aregs;
extern const unsigned char Xthal_num_aregs_log2;


/*----------------------------------------------------------------------
   				Cache
  ----------------------------------------------------------------------*/

/* size of the cache lines in log2(bytes) */
extern const unsigned char Xthal_icache_linewidth;
extern const unsigned char Xthal_dcache_linewidth;
/* size of the cache lines in bytes (2^linewidth) */
extern const unsigned short Xthal_icache_linesize;
extern const unsigned short Xthal_dcache_linesize;

/* size of the caches in bytes (ways * 2^(linewidth + setwidth)) */
extern const unsigned int  Xthal_icache_size;
extern const unsigned int  Xthal_dcache_size;
/* cache features */
extern const unsigned char Xthal_dcache_is_writeback;

/* cache region operations*/
extern void xthal_icache_region_invalidate( void *addr, unsigned size );
extern void xthal_dcache_region_invalidate( void *addr, unsigned size );
extern void xthal_dcache_region_writeback( void *addr, unsigned size );
extern void xthal_dcache_region_writeback_inv( void *addr, unsigned size );

#ifndef XTHAL_USE_CACHE_MACROS
/* cache line operations*/
extern void xthal_icache_line_invalidate(void *addr);
extern void xthal_dcache_line_invalidate(void *addr);
extern void xthal_dcache_line_writeback(void *addr);
extern void xthal_dcache_line_writeback_inv(void *addr);
/* sync icache and memory */
extern void xthal_icache_sync( void );
/* sync dcache and memory */
extern void xthal_dcache_sync( void );
#endif

/* get/set number of icache ways enabled */
extern unsigned int xthal_icache_get_ways(void);
extern void         xthal_icache_set_ways(unsigned int ways);
/* get/set number of dcache ways enabled */
extern unsigned int xthal_dcache_get_ways(void);
extern void         xthal_dcache_set_ways(unsigned int ways);

/* coherency (low-level -- not normally called directly) */
extern void xthal_cache_coherence_on( void );
extern void xthal_cache_coherence_off( void );
/* coherency (high-level) */
extern void xthal_cache_coherence_optin( void );
extern void xthal_cache_coherence_optout( void );

/*
 *  Cache prefetch control.
 *  The parameter to xthal_set_cache_prefetch() contains both
 *  a PREFCTL register value and a mask of which bits to actually modify.
 *  This allows easily combining field macros (below) by ORing,
 *  leaving unspecified fields unmodified.
 *
 *  For backward compatibility with the older version of this routine
 *  (that took 15-bit value and mask in a 32-bit parameter, for pre-RF
 *  cores with only the lower 15 bits of PREFCTL defined), the 32-bit
 *  value and mask are staggered as follows in a 64-bit parameter:
 *	param[63:48] are PREFCTL[31:16]		if param[31] is set
 *	param[47:32] are mask[31:16]		if param[31] is set
 *	param[31]    is set if mask is used, 0 if not
 *	param[31:16] are mask[15:0]		if param[31] is set
 *	param[31:16] are PREFCTL[31:16]		if param[31] is clear
 *	param[15:0]  are PREFCTL[15:0]
 *
 *  Limitation:  PREFCTL register bit 31 cannot be set without masking,
 *  and bit 15 must always be set when using masking, so it is hoped that
 *  these two bits will remain reserved, read-as-zero in PREFCTL.
 */
#define XTHAL_PREFETCH_ENABLE		-1	/* enable inst+data prefetch */
#define XTHAL_PREFETCH_DISABLE		0xFFFF0000 /* disab inst+data prefetch*/
#define XTHAL_DCACHE_PREFETCH(n)	(0x800F0000+((n)&0xF))   /* data-side */
#define XTHAL_DCACHE_PREFETCH_OFF	XTHAL_DCACHE_PREFETCH(0) /* disable */
#define XTHAL_DCACHE_PREFETCH_LOW	XTHAL_DCACHE_PREFETCH(4) /* less aggr.*/
#define XTHAL_DCACHE_PREFETCH_MEDIUM	XTHAL_DCACHE_PREFETCH(5) /* mid aggr. */
#define XTHAL_DCACHE_PREFETCH_HIGH	XTHAL_DCACHE_PREFETCH(8) /* more aggr.*/
#define XTHAL_DCACHE_PREFETCH_L1_OFF	0x90000000	/* to prefetch buffers*/
#define XTHAL_DCACHE_PREFETCH_L1	0x90001000	/* direct to L1 dcache*/
#define XTHAL_ICACHE_PREFETCH(n)	(0x80F00000+(((n)&0xF)<<4)) /* i-side */
#define XTHAL_ICACHE_PREFETCH_OFF	XTHAL_ICACHE_PREFETCH(0) /* disable */
#define XTHAL_ICACHE_PREFETCH_LOW	XTHAL_ICACHE_PREFETCH(4) /* less aggr.*/
#define XTHAL_ICACHE_PREFETCH_MEDIUM	XTHAL_ICACHE_PREFETCH(5) /* mid aggr. */
#define XTHAL_ICACHE_PREFETCH_HIGH	XTHAL_ICACHE_PREFETCH(8) /* more aggr.*/
#define XTHAL_ICACHE_PREFETCH_L1_OFF	0xA0000000	/* (not implemented) */
#define XTHAL_ICACHE_PREFETCH_L1	0xA0002000	/* (not implemented) */
#define _XTHAL_PREFETCH_BLOCKS(n)	((n)<0?0:(n)<5?(n):(n)<15?((n)>>1)+2:9)
#define XTHAL_PREFETCH_BLOCKS(n)	(0x0000000F80000000ULL + \
					(((unsigned long long)_XTHAL_PREFETCH_BLOCKS(n))<<48))

extern int  xthal_get_cache_prefetch( void );
extern int  xthal_set_cache_prefetch( int );
extern int  xthal_set_cache_prefetch_long( unsigned long long );
/*  Only use the new extended function from now on:  */
#define xthal_set_cache_prefetch	xthal_set_cache_prefetch_long
#define xthal_set_cache_prefetch_nw	xthal_set_cache_prefetch_long_nw


/*----------------------------------------------------------------------
   				Debug
  ----------------------------------------------------------------------*/

/*  1 if debug option configured, 0 if not:  */
extern const int Xthal_debug_configured;

/*  Set (plant) and remove software breakpoint, both synchronizing cache:  */
extern unsigned int xthal_set_soft_break(void *addr);
extern void         xthal_remove_soft_break(void *addr, unsigned int);


/*----------------------------------------------------------------------
   				Disassembler
  ----------------------------------------------------------------------*/

/*  Max expected size of the return buffer for a disassembled instruction (hint only):  */
#define XTHAL_DISASM_BUFSIZE	80

/*  Disassembly option bits for selecting what to return:  */
#define XTHAL_DISASM_OPT_ADDR	0x0001	/* display address */
#define XTHAL_DISASM_OPT_OPHEX	0x0002	/* display opcode bytes in hex */
#define XTHAL_DISASM_OPT_OPCODE	0x0004	/* display opcode name (mnemonic) */
#define XTHAL_DISASM_OPT_PARMS	0x0008	/* display parameters */
#define XTHAL_DISASM_OPT_ALL	0x0FFF	/* display everything */

/* routine to get a string for the disassembled instruction */
extern int xthal_disassemble( unsigned char *instr_buf, void *tgt_addr,
		       char *buffer, unsigned buflen, unsigned options );

/* routine to get the size of the next instruction. Returns 0 for
   illegal instruction */
extern int xthal_disassemble_size( unsigned char *instr_buf );


/*----------------------------------------------------------------------
			Instruction/Data RAM/ROM Access
  ----------------------------------------------------------------------*/

extern void* xthal_memcpy(void *dst, const void *src, unsigned len);
extern void* xthal_bcopy(const void *src, void *dst, unsigned len);


/*----------------------------------------------------------------------
                           MP Synchronization
  ----------------------------------------------------------------------*/

extern int      xthal_compare_and_set( int *addr, int test_val, int compare_val );

/*extern const char  Xthal_have_s32c1i;*/


/*----------------------------------------------------------------------
                             Miscellaneous
  ----------------------------------------------------------------------*/

extern const unsigned int  Xthal_release_major;
extern const unsigned int  Xthal_release_minor;
extern const char * const  Xthal_release_name;
extern const char * const  Xthal_release_internal;

extern const unsigned char Xthal_memory_order;
extern const unsigned char Xthal_have_windowed;
extern const unsigned char Xthal_have_density;
extern const unsigned char Xthal_have_booleans;
extern const unsigned char Xthal_have_loops;
extern const unsigned char Xthal_have_nsa;
extern const unsigned char Xthal_have_minmax;
extern const unsigned char Xthal_have_sext;
extern const unsigned char Xthal_have_clamps;
extern const unsigned char Xthal_have_mac16;
extern const unsigned char Xthal_have_mul16;
extern const unsigned char Xthal_have_fp;
extern const unsigned char Xthal_have_speculation;
extern const unsigned char Xthal_have_threadptr;

extern const unsigned char Xthal_have_pif;
extern const unsigned short Xthal_num_writebuffer_entries;

extern const unsigned int  Xthal_build_unique_id;
/*  Version info for hardware targeted by software upgrades:  */
extern const unsigned int  Xthal_hw_configid0;
extern const unsigned int  Xthal_hw_configid1;
extern const unsigned int  Xthal_hw_release_major;
extern const unsigned int  Xthal_hw_release_minor;
extern const char * const  Xthal_hw_release_name;
extern const char * const  Xthal_hw_release_internal;

/*  Clear any remnant code-dependent state (i.e. clear loop count regs).  */
extern void	xthal_clear_regcached_code( void );

#ifdef __cplusplus
}
#endif
#endif /*!_ASMLANGUAGE && !_NOCLANGUAGE && !__ASSEMBLER__ */





/****************************************************************************
    Definitions Useful for PRIVILEGED (Supervisory or Non-Virtualized) Code
 ****************************************************************************/


#ifndef XTENSA_HAL_NON_PRIVILEGED_ONLY

/*----------------------------------------------------------------------
	   Constant Definitions  (shared with assembly)
  ----------------------------------------------------------------------*/

/*
 *  Architectural limits, independent of configuration.
 *  Note that these are ISA-defined limits, not micro-architecture implementation
 *  limits enforced by the Xtensa Processor Generator (which may be stricter than
 *  these below).
 */
#define XTHAL_MAX_INTERRUPTS	32	/* max number of interrupts (0..31) */
#define XTHAL_MAX_INTLEVELS	16	/* max number of interrupt levels (0..15) */
					/* (as of T1040, implementation limit is 7: 0..6) */
#define XTHAL_MAX_TIMERS	4	/* max number of timers (CCOMPARE0..CCOMPARE3) */
					/* (as of T1040, implementation limit is 3: 0..2) */

/*  Interrupt types:  */
#define XTHAL_INTTYPE_UNCONFIGURED	0
#define XTHAL_INTTYPE_SOFTWARE		1
#define XTHAL_INTTYPE_EXTERN_EDGE	2
#define XTHAL_INTTYPE_EXTERN_LEVEL	3
#define XTHAL_INTTYPE_TIMER		4
#define XTHAL_INTTYPE_NMI		5
#define XTHAL_INTTYPE_WRITE_ERROR	6
#define XTHAL_INTTYPE_PROFILING		7
#define XTHAL_INTTYPE_IDMA_DONE		8
#define XTHAL_INTTYPE_IDMA_ERR		9
#define XTHAL_INTTYPE_GS_ERR		10
#define XTHAL_INTTYPE_SG_ERR		10	/* backward compatibility name - deprecated */
#define XTHAL_MAX_INTTYPES		11	/* number of interrupt types */

/*  Timer related:  */
#define XTHAL_TIMER_UNCONFIGURED	-1	/* Xthal_timer_interrupt[] value for non-existent timers */
#define XTHAL_TIMER_UNASSIGNED	XTHAL_TIMER_UNCONFIGURED	/* (for backwards compatibility only) */

/*  Local Memory ECC/Parity:  */
#define XTHAL_MEMEP_PARITY	1
#define XTHAL_MEMEP_ECC		2
/*  Flags parameter to xthal_memep_inject_error():  */
#define XTHAL_MEMEP_F_LOCAL		0	/* local memory (default) */
#define XTHAL_MEMEP_F_DCACHE_DATA	4	/* data cache data */
#define XTHAL_MEMEP_F_DCACHE_TAG	5	/* data cache tag */
#define XTHAL_MEMEP_F_ICACHE_DATA	6	/* instruction cache data */
#define XTHAL_MEMEP_F_ICACHE_TAG	7	/* instruction cache tag */
#define XTHAL_MEMEP_F_CORRECTABLE	16	/* inject correctable error
						   (default is non-corr.) */


/*  Access Mode bits (tentative):  */	/* bit abbr unit short_name       PPC equ - Description */
#define XTHAL_AMB_EXCEPTION	0	/* 001 E EX fls: EXception        none
					   exception on any access (aka "illegal") */
#define XTHAL_AMB_HITCACHE	1	/* 002 C CH fls: use Cache on Hit ~(I CI)
					   [or H HC]  way from tag match;
					   [or U UC] (ISA: same except Isolate case) */
#define XTHAL_AMB_ALLOCATE	2	/* 004 A AL fl?: ALlocate         none
					   [or F FI fill] refill cache on miss, way from LRU
					   (ISA: Read/Write Miss Refill) */
#define XTHAL_AMB_WRITETHRU	3	/* 008 W WT --s: WriteThrough     W WT
					   store immediately to memory (ISA: same) */
#define XTHAL_AMB_ISOLATE	4	/* 010 I IS fls: ISolate          none
					   use cache regardless of hit-vs-miss,
					   way from vaddr (ISA: use-cache-on-miss+hit) */
#define XTHAL_AMB_GUARD		5	/* 020 G GU ?l?: GUard            G *
					   non-speculative; spec/replay refs not permitted */
#define XTHAL_AMB_COHERENT	6	/* 040 M MC ?ls: Mem/MP Coherent  M
					   on read, other CPU/bus-master may need to supply data;
					   on write, maybe redirect to or flush other CPU dirty line; etc */
#if 0
#define XTHAL_AMB_BUFFERABLE	x	/* 000 B BU --s: BUfferable       ?
					   write response may return earlier than from final destination */
#define XTHAL_AMB_ORDERED	x	/* 000 O OR fls: ORdered          G *
					   mem accesses cannot be out of order */
#define XTHAL_AMB_FUSEWRITES	x	/* 000 F FW --s: FuseWrites       none
					   allow combining/merging/coalescing multiple writes
					   (to same datapath data unit) into one
					   (implied by writeback) */
#define XTHAL_AMB_TRUSTED	x	/* 000 T TR ?l?: TRusted          none
					   memory will not bus error (if it does,
					   handle as fatal imprecise interrupt) */
#define XTHAL_AMB_PREFETCH	x	/* 000 P PR fl?: PRefetch         none
					   on refill, read line+1 into prefetch buffers */
#define XTHAL_AMB_STREAM	x	/* 000 S ST ???: STreaming        none
					   access one of N stream buffers */
#endif /*0*/

#define XTHAL_AM_EXCEPTION	(1<<XTHAL_AMB_EXCEPTION)
#define XTHAL_AM_HITCACHE	(1<<XTHAL_AMB_HITCACHE)
#define XTHAL_AM_ALLOCATE	(1<<XTHAL_AMB_ALLOCATE)
#define XTHAL_AM_WRITETHRU	(1<<XTHAL_AMB_WRITETHRU)
#define XTHAL_AM_ISOLATE	(1<<XTHAL_AMB_ISOLATE)
#define XTHAL_AM_GUARD		(1<<XTHAL_AMB_GUARD)
#define XTHAL_AM_COHERENT	(1<<XTHAL_AMB_COHERENT)
#if 0
#define XTHAL_AM_BUFFERABLE	(1<<XTHAL_AMB_BUFFERABLE)
#define XTHAL_AM_ORDERED	(1<<XTHAL_AMB_ORDERED)
#define XTHAL_AM_FUSEWRITES	(1<<XTHAL_AMB_FUSEWRITES)
#define XTHAL_AM_TRUSTED	(1<<XTHAL_AMB_TRUSTED)
#define XTHAL_AM_PREFETCH	(1<<XTHAL_AMB_PREFETCH)
#define XTHAL_AM_STREAM		(1<<XTHAL_AMB_STREAM)
#endif /*0*/

/*
 *  Allowed Access Modes (bit combinations).
 *
 *  Columns are:
 *  "FOGIWACE"
 *	Access mode bits (see XTHAL_AMB_xxx above).
 *	<letter> = bit is set
 *	'-'      = bit is clear
 *	'.'      = bit is irrelevant / don't care, as follows:
 *			E=1 makes all others irrelevant
 *			W,F relevant only for stores
 *  "2345"
 *	Indicates which Xtensa releases support the corresponding
 *	access mode.  Releases for each character column are:
 *		2 = prior to T1020.2:   T1015 (V1.5), T1020.0, T1020.1
 *		3 = T1020.2 and later:  T1020.2+, T1030
 *		4 = T1040
 *		5 = T1050 (maybe), LX1, LX2, LX2.1
 *		7 = LX2.2
 *		8 = LX3, LX4
 *		9 = LX5
 *	And the character column contents are:
 *		<number> = supported by release(s)
 *		"." = unsupported by release(s)
 *		"?" = support unknown
 */
						/* foMGIWACE 2345789 */
/*  For instruction fetch:  */
#define XTHAL_FAM_EXCEPTION		0x001	/* ........E 2345789 exception */
/*efine XTHAL_FAM_ISOLATE*/	      /*0x012*/	/* .---I.-C- ....... isolate */
#define XTHAL_FAM_BYPASS		0x000	/* .----.--- 2345789 bypass */
/*efine XTHAL_FAM_NACACHED*/	      /*0x002*/	/* .----.-C- ....... cached no-allocate (frozen) */
#define XTHAL_FAM_CACHED		0x006	/* .----.AC- 2345789 cached */
/*  For data load:  */
#define XTHAL_LAM_EXCEPTION		0x001	/* ........E 2345789 exception */
#define XTHAL_LAM_ISOLATE		0x012	/* .---I.-C- 2345789 isolate */
#define XTHAL_LAM_BYPASS		0x000	/* .O---.--- 2...... bypass speculative */
#define XTHAL_LAM_BYPASSG		0x020	/* .O-G-.--- .345789 bypass guarded */
#define XTHAL_LAM_CACHED_NOALLOC	0x002	/* .O---.-C- 2345789 cached no-allocate speculative */
#define XTHAL_LAM_NACACHED		XTHAL_LAM_CACHED_NOALLOC
#define XTHAL_LAM_NACACHEDG		0x022	/* .O-G-.-C- .?..... cached no-allocate guarded */
#define XTHAL_LAM_CACHED		0x006	/* .----.AC- 2345789 cached speculative */
#define XTHAL_LAM_COHCACHED		0x046	/* .-M--.AC- ....*89 cached speculative MP-coherent */
/*  For data store:  */
#define XTHAL_SAM_EXCEPTION		0x001	/* ........E 2345789 exception */
#define XTHAL_SAM_ISOLATE		0x032	/* .--GI--C- 2345789 isolate */
#define XTHAL_SAM_BYPASS		0x028	/* -O-G-W--- 2345789 bypass */
#define XTHAL_SAM_WRITETHRU		0x02A	/* -O-G-W-C- 2345789 writethrough */
/*efine XTHAL_SAM_WRITETHRU_ALLOC*/   /*0x02E*/	/* -O-G-WAC- ....... writethrough allocate */
#define XTHAL_SAM_WRITEBACK		0x026	/* F--G--AC- ...5789 writeback */
#define XTHAL_SAM_WRITEBACK_NOALLOC	0x022	/* ?--G---C- .....89 writeback no-allocate */
#define XTHAL_SAM_COHWRITEBACK		0x066	/* F-MG--AC- ....*89 writeback MP-coherent */
/*  For PIF attributes:  */			/* -PIwrWCBUUUU ...9 */
#define XTHAL_PAM_BYPASS		0x000	/* xxx00000xxxx ...9 bypass non-bufferable */
#define XTHAL_PAM_BYPASS_BUF		0x010	/* xxx0000bxxxx ...9 bypass */
#define XTHAL_PAM_CACHED_NOALLOC	0x030	/* xxx0001bxxxx ...9 cached no-allocate */
#define XTHAL_PAM_WRITETHRU		0x0B0	/* xxx0101bxxxx ...9 writethrough (WT) */
#define XTHAL_PAM_WRITEBACK_NOALLOC	0x0F0	/* xxx0111bxxxx ...9 writeback no-alloc (WBNA) */
#define XTHAL_PAM_WRITEBACK		0x1F0	/* xxx1111bxxxx ...9 writeback (WB) */
/*efine XTHAL_PAM_NORMAL*/	      /*0x050*/	/* xxx0010bxxxx .... (unimplemented) */
/*efine XTHAL_PAM_WRITETHRU_WA*/      /*0x130*/	/* xxx1001bxxxx .... (unimplemented, less likely) */
/*efine XTHAL_PAM_WRITETHRU_RWA*/     /*0x1B0*/	/* xxx1101bxxxx .... (unimplemented, less likely) */
/*efine XTHAL_PAM_WRITEBACK_WA*/      /*0x170*/	/* xxx1011bxxxx .... (unimplemented, less likely) */


#if 0
/*
    Cache attribute encoding for CACHEATTR (per ISA):
    (Note:  if this differs from ISA Ref Manual, ISA has precedence)

	Inst-fetches	Loads		Stores
	-------------	------------	-------------
0x0	FCA_EXCEPTION	LCA_NACACHED	SCA_WRITETHRU	cached no-allocate (previously misnamed "uncached")
0x1	FCA_CACHED	LCA_CACHED	SCA_WRITETHRU	cached
0x2	FCA_BYPASS	LCA_BYPASS_G*	SCA_BYPASS	bypass cache (what most people call uncached)
0x3	FCA_CACHED	LCA_CACHED	SCA_WRITEALLOCF	write-allocate
		     or LCA_EXCEPTION	SCA_EXCEPTION	(if unimplemented)
0x4	FCA_CACHED	LCA_CACHED	SCA_WRITEBACK[M] write-back [MP-coherent]
		     or LCA_EXCEPTION	SCA_EXCEPTION	(if unimplemented)
0x5	FCA_CACHED	LCA_CACHED	SCA_WRITEBACK_NOALLOC	write-back no-allocate
     or	FCA_EXCEPTION	LCA_EXCEPTION	SCA_EXCEPTION	(if unimplemented)
0x6..D	FCA_EXCEPTION	LCA_EXCEPTION	SCA_EXCEPTION	(reserved)
0xE	FCA_EXCEPTION	LCA_ISOLATE	SCA_ISOLATE	isolate
0xF	FCA_EXCEPTION	LCA_EXCEPTION	SCA_EXCEPTION	illegal
     *  Prior to T1020.2?, guard feature not supported, this defaulted to speculative (no _G)
*/
#endif /*0*/


#if !defined(_ASMLANGUAGE) && !defined(_NOCLANGUAGE) && !defined(__ASSEMBLER__)
#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------
   				Register Windows
  ----------------------------------------------------------------------*/

/*  This spill any live register windows (other than the caller's):
 *  (NOTE:  current implementation require privileged code, but
 *   a user-callable implementation is possible.)  */
extern void      xthal_window_spill( void );


/*----------------------------------------------------------------------
			Optional/Custom Processor State
  ----------------------------------------------------------------------*/

/* validate & invalidate the TIE register file */
extern void xthal_validate_cp(int);
extern void xthal_invalidate_cp(int);

/* read and write cpenable register */
extern void xthal_set_cpenable(unsigned);
extern unsigned xthal_get_cpenable(void);


/*----------------------------------------------------------------------
   				Interrupts
  ----------------------------------------------------------------------*/

/* the number of interrupt levels */
extern const unsigned char Xthal_num_intlevels;
/* the number of interrupts */
extern const unsigned char Xthal_num_interrupts;
/* the highest level of interrupts masked by PS.EXCM */
extern const unsigned char Xthal_excm_level;

/* mask for level of interrupts */
extern const unsigned int Xthal_intlevel_mask[XTHAL_MAX_INTLEVELS];
/* mask for level 0 to N interrupts */
extern const unsigned int Xthal_intlevel_andbelow_mask[XTHAL_MAX_INTLEVELS];

/* level of each interrupt */
extern const unsigned char Xthal_intlevel[XTHAL_MAX_INTERRUPTS];

/* type per interrupt */
extern const unsigned char Xthal_inttype[XTHAL_MAX_INTERRUPTS];

/* masks of each type of interrupt */
extern const unsigned int Xthal_inttype_mask[XTHAL_MAX_INTTYPES];

/* interrupt numbers assigned to each timer interrupt */
extern const int Xthal_timer_interrupt[XTHAL_MAX_TIMERS];

/*  INTENABLE,INTERRUPT,INTSET,INTCLEAR register access functions:  */
extern unsigned  xthal_get_intenable( void );
extern void      xthal_set_intenable( unsigned );
extern unsigned  xthal_get_interrupt( void );
#define xthal_get_intread	xthal_get_interrupt	/* backward compatibility */

/*  These two functions are deprecated. Use the newer functions
    xthal_interrupt_trigger and xthal_interrupt_clear instead.  */
extern void      xthal_set_intset( unsigned );
extern void      xthal_set_intclear( unsigned );


/*----------------------------------------------------------------------
   				Debug
  ----------------------------------------------------------------------*/

/*  Number of instruction and data break registers:  */
extern const int Xthal_num_ibreak;
extern const int Xthal_num_dbreak;


/*----------------------------------------------------------------------
   				Core Counter
  ----------------------------------------------------------------------*/

/* counter info */
extern const unsigned char Xthal_have_ccount;	/* set if CCOUNT register present */
extern const unsigned char Xthal_num_ccompare;	/* number of CCOMPAREn registers */

/* get CCOUNT register (if not present return 0) */
extern unsigned xthal_get_ccount(void);

/* set and get CCOMPAREn registers (if not present, get returns 0) */
extern void     xthal_set_ccompare(int, unsigned);
extern unsigned xthal_get_ccompare(int);


/*----------------------------------------------------------------------
                             Miscellaneous
  ----------------------------------------------------------------------*/

extern const unsigned char Xthal_have_prid;
extern const unsigned char Xthal_have_exceptions;
extern const unsigned char Xthal_xea_version;
extern const unsigned char Xthal_have_interrupts;
extern const unsigned char Xthal_have_highlevel_interrupts;
extern const unsigned char Xthal_have_nmi;

extern unsigned xthal_get_prid( void );


/*----------------------------------------------------------------------
		Virtual interrupt prioritization (DEPRECATED)
  ----------------------------------------------------------------------*/

/*  Convert between interrupt levels (as per PS.INTLEVEL) and virtual interrupt priorities:  */
extern unsigned	xthal_vpri_to_intlevel(unsigned vpri);
extern unsigned	xthal_intlevel_to_vpri(unsigned intlevel);

/*  Enables/disables given set (mask) of interrupts; returns previous enabled-mask of all ints:  */
/*  These functions are deprecated. Use xthal_interrupt_enable and xthal_interrupt_disable instead.  */
extern unsigned	xthal_int_enable(unsigned);
extern unsigned	xthal_int_disable(unsigned);

/*  Set/get virtual priority of an interrupt:  */
extern int	xthal_set_int_vpri(int intnum, int vpri);
extern int	xthal_get_int_vpri(int intnum);

/*  Set/get interrupt lockout level for exclusive access to virtual priority data structures:  */
extern void	xthal_set_vpri_locklevel(unsigned intlevel);
extern unsigned	xthal_get_vpri_locklevel(void);

/*  Set/get current virtual interrupt priority:  */
extern unsigned	xthal_set_vpri(unsigned vpri);
extern unsigned	xthal_get_vpri(void);
extern unsigned	xthal_set_vpri_intlevel(unsigned intlevel);
extern unsigned	xthal_set_vpri_lock(void);


/*----------------------------------------------------------------------
	Generic Interrupt Trampolining Support (DEPRECATED)
  ----------------------------------------------------------------------*/

typedef void (XtHalVoidFunc)(void);

/*  Bitmask of interrupts currently trampolining down:  */
extern unsigned Xthal_tram_pending;

/*
 *  Bitmask of which interrupts currently trampolining down synchronously are
 *  actually enabled; this bitmask is necessary because INTENABLE cannot hold
 *  that state (sync-trampolining interrupts must be kept disabled while
 *  trampolining);  in the current implementation, any bit set here is not set
 *  in INTENABLE, and vice-versa; once a sync-trampoline is handled (at level
 *  one), its enable bit must be moved from here to INTENABLE:
 */
extern unsigned Xthal_tram_enabled;

/*  Bitmask of interrupts configured for sync trampolining:  */
extern unsigned Xthal_tram_sync;

/*  Trampoline support functions:  */
extern unsigned  xthal_tram_pending_to_service( void );
extern void      xthal_tram_done( unsigned serviced_mask );
extern int       xthal_tram_set_sync( int intnum, int sync );
extern XtHalVoidFunc* xthal_set_tram_trigger_func( XtHalVoidFunc *trigger_fn );


/*----------------------------------------------------------------------
			Internal Memories
  ----------------------------------------------------------------------*/

extern const unsigned char Xthal_num_instrom;
extern const unsigned char Xthal_num_instram;
extern const unsigned char Xthal_num_datarom;
extern const unsigned char Xthal_num_dataram;
extern const unsigned char Xthal_num_xlmi;

/*  Each of the following arrays contains at least one entry,
 *  or as many entries as needed if more than one:  */
extern const unsigned int  Xthal_instrom_vaddr[];
extern const unsigned int  Xthal_instrom_paddr[];
extern const unsigned int  Xthal_instrom_size [];
extern const unsigned int  Xthal_instram_vaddr[];
extern const unsigned int  Xthal_instram_paddr[];
extern const unsigned int  Xthal_instram_size [];
extern const unsigned int  Xthal_datarom_vaddr[];
extern const unsigned int  Xthal_datarom_paddr[];
extern const unsigned int  Xthal_datarom_size [];
extern const unsigned int  Xthal_dataram_vaddr[];
extern const unsigned int  Xthal_dataram_paddr[];
extern const unsigned int  Xthal_dataram_size [];
extern const unsigned int  Xthal_xlmi_vaddr[];
extern const unsigned int  Xthal_xlmi_paddr[];
extern const unsigned int  Xthal_xlmi_size [];


/*----------------------------------------------------------------------
   				Cache
  ----------------------------------------------------------------------*/

/* number of cache sets in log2(lines per way) */
extern const unsigned char Xthal_icache_setwidth;
extern const unsigned char Xthal_dcache_setwidth;
/* cache set associativity (number of ways) */
extern const unsigned int  Xthal_icache_ways;
extern const unsigned int  Xthal_dcache_ways;
/* cache features */
extern const unsigned char Xthal_icache_line_lockable;
extern const unsigned char Xthal_dcache_line_lockable;

/* cache attribute register control (used by other HAL routines) */
extern unsigned xthal_get_cacheattr( void );
extern unsigned xthal_get_icacheattr( void );
extern unsigned xthal_get_dcacheattr( void );
extern void     xthal_set_cacheattr( unsigned );
extern void     xthal_set_icacheattr( unsigned );
extern void     xthal_set_dcacheattr( unsigned );
/* set cache attribute (access modes) for a range of memory */
extern int      xthal_set_region_attribute( void *addr, unsigned size,
					    unsigned cattr, unsigned flags );
/*  Bits of flags parameter to xthal_set_region_attribute():  */
#define XTHAL_CAFLAG_EXPAND		0x000100	/* only expand allowed access to range, don't reduce it */
#define XTHAL_CAFLAG_EXACT		0x000200	/* return error if can't apply change to exact range specified */
#define XTHAL_CAFLAG_NO_PARTIAL		0x000400	/* don't apply change to regions partially covered by range */
#define XTHAL_CAFLAG_NO_AUTO_WB		0x000800	/* don't writeback data after leaving writeback attribute */
#define XTHAL_CAFLAG_NO_AUTO_INV	0x001000	/* don't invalidate after disabling cache (entering bypass) */

/* enable caches */
extern void xthal_icache_enable( void );	/* DEPRECATED */
extern void xthal_dcache_enable( void );	/* DEPRECATED */
/* disable caches */
extern void xthal_icache_disable( void );	/* DEPRECATED */
extern void xthal_dcache_disable( void );	/* DEPRECATED */

/* whole cache operations (privileged) */
extern void xthal_icache_all_invalidate( void );
extern void xthal_dcache_all_invalidate( void );
extern void xthal_dcache_all_writeback( void );
extern void xthal_dcache_all_writeback_inv( void );
extern void xthal_icache_all_unlock( void );
extern void xthal_dcache_all_unlock( void );

/* address-range cache operations (privileged) */
/* prefetch and lock specified memory range into cache */
extern void xthal_icache_region_lock( void *addr, unsigned size );
extern void xthal_dcache_region_lock( void *addr, unsigned size );
/* unlock from cache */
extern void xthal_icache_region_unlock( void *addr, unsigned size );
extern void xthal_dcache_region_unlock( void *addr, unsigned size );

/* huge-range cache operations (privileged) (EXPERIMENTAL) */
extern void xthal_icache_hugerange_invalidate( void *addr, unsigned size );
extern void xthal_icache_hugerange_unlock( void *addr, unsigned size );
extern void xthal_dcache_hugerange_invalidate( void *addr, unsigned size );
extern void xthal_dcache_hugerange_unlock( void *addr, unsigned size );
extern void xthal_dcache_hugerange_writeback( void *addr, unsigned size );
extern void xthal_dcache_hugerange_writeback_inv( void *addr, unsigned size );

# ifndef XTHAL_USE_CACHE_MACROS
/* cache line operations (privileged) */
extern void xthal_icache_line_lock(void *addr);
extern void xthal_dcache_line_lock(void *addr);
extern void xthal_icache_line_unlock(void *addr);
extern void xthal_dcache_line_unlock(void *addr);
# endif



/*----------------------------------------------------------------------
                         Local Memory ECC/Parity
  ----------------------------------------------------------------------*/

/*  Inject memory errors; flags is bit combination of XTHAL_MEMEP_F_xxx:  */
extern void xthal_memep_inject_error(void *addr, int size, int flags);



/*----------------------------------------------------------------------
                         Memory Management Unit
  ----------------------------------------------------------------------*/

extern const unsigned char Xthal_have_spanning_way;
extern const unsigned char Xthal_have_identity_map;
extern const unsigned char Xthal_have_mimic_cacheattr;
extern const unsigned char Xthal_have_xlt_cacheattr;
extern const unsigned char Xthal_have_cacheattr;
extern const unsigned char Xthal_have_tlbs;

extern const unsigned char Xthal_mmu_asid_bits;		/* 0 .. 8 */
extern const unsigned char Xthal_mmu_asid_kernel;
extern const unsigned char Xthal_mmu_rings;		/* 1 .. 4 (perhaps 0 if no MMU and/or no protection?) */
extern const unsigned char Xthal_mmu_ring_bits;
extern const unsigned char Xthal_mmu_sr_bits;
extern const unsigned char Xthal_mmu_ca_bits;
extern const unsigned int  Xthal_mmu_max_pte_page_size;
extern const unsigned int  Xthal_mmu_min_pte_page_size;

extern const unsigned char Xthal_itlb_way_bits;
extern const unsigned char Xthal_itlb_ways;
extern const unsigned char Xthal_itlb_arf_ways;
extern const unsigned char Xthal_dtlb_way_bits;
extern const unsigned char Xthal_dtlb_ways;
extern const unsigned char Xthal_dtlb_arf_ways;

/* Return error codes for hal functions */

/* function sucessful, operation completed as expected */
#define XTHAL_SUCCESS                                    0
/* XTHAL_CAFLAGS_NO_PARTIAL was specified, and no full region is
 * covered by the address range. */
#define XTHAL_NO_REGIONS_COVERED                        -1
/* The XTHAL_CAFLAGS_EXACT flag was given, but no exact mapping is possible. */
#define XTHAL_INEXACT                                   -2
/* The supplied address doesn't correspond to the start of a region. */
#define XTHAL_INVALID_ADDRESS                           -3
/* This functionality is not available on this architecture. */
#define XTHAL_UNSUPPORTED                               -4
/* Translation failed because vaddr and paddr were not aligned. */
#define XTHAL_ADDRESS_MISALIGNED                        -5
/* There is mapping for the supplied address. */
#define XTHAL_NO_MAPPING                                -6
/* The requested access rights are not supported */
#define XTHAL_BAD_ACCESS_RIGHTS                         -7
/* The requested memory type is not supported */
#define XTHAL_BAD_MEMORY_TYPE                           -8
/* The entries supplied are not properly aligned to the MPU's background map. */
#define XTHAL_MAP_NOT_ALIGNED                           -9
/* There are not enough MPU entries available to do the requeste mapping. */
#define XTHAL_OUT_OF_ENTRIES                           -10
/* The entries supplied are not properly ordered for the MPU. */
#define XTHAL_OUT_OF_ORDER_MAP                         -11
/* an invalid argument such as a null pointer was supplied to the function */
#define XTHAL_INVALID                                  -12
/* specified region is of zero size, therefore no mapping is done. */
#define XTHAL_ZERO_SIZED_REGION                        -13
/* specified range wraps around '0' */
#define XTHAL_INVALID_ADDRESS_RANGE                    -14

/*
   For backward compatibility we retain the following inconsistenly named
   constants. Do not use them as they may be removed in a future release.
 */
#define XCHAL_SUCCESS XTHAL_SUCCESS
#define XCHAL_ADDRESS_MISALIGNED XTHAL_ADDRESS_MISALIGNED
#define XCHAL_INEXACT XTHAL_INEXACT
#define XCHAL_INVALID_ADDRESS XTHAL_INVALID_ADDRESS
#define XCHAL_UNSUPPORTED_ON_THIS_ARCH XTHAL_UNSUPPORTED
#define XCHAL_NO_PAGES_MAPPED XTHAL_NO_REGIONS_COVERED


/*  Convert between virtual and physical addresses (through static maps only)
 *  WARNING: these two functions may go away in a future release;
 *  don't depend on them!
*/
extern int  xthal_static_v2p( unsigned vaddr, unsigned *paddrp );
extern int  xthal_static_p2v( unsigned paddr, unsigned *vaddrp, unsigned cached );

extern int xthal_set_region_translation(void* vaddr, void* paddr,
                  unsigned size, unsigned cache_atr, unsigned flags);
extern int xthal_v2p(void*, void**, unsigned*, unsigned*);
extern int xthal_invalidate_region(void* addr);
extern int xthal_set_region_translation_raw(void *vaddr, void *paddr, unsigned cattr);

/*------------------------------------------------------------------------
	MPU (Memory Protection Unit)
-------------------------------------------------------------------------*/

/*
 *  General notes on MPU (Memory Protection Unit):
 *
 *  The MPU supports setting the access rights (read, write, execute) as
 *  well as the memory type (cacheablity, ...)
 *  for regions of memory. The granularity can be as small as 32 bytes.
 *  (XCHAL_MPU_ALIGN specifies the granularity for any specific MPU config)
 *
 *  The MPU doesn't support mapping between virtual and physical addresses.
 *
 *  The MPU contains a fixed number of map changeable forground map entries,
 *  and a background map which is fixed at configuration time.
 *
 *  Each entry has a start address (up to 27 bits), valid flag,
 *  access rights (4 bits), and memory type (9 bits);
 *
 */


/*
    MPU access rights constants:
    Only the combinations listed below are supported by the MPU.
*/

#define XTHAL_AR_NONE    0 /* no access */
#define XTHAL_AR_R       4 /* Kernel read, User no access*/
#define XTHAL_AR_RX      5 /* Kernel read/execute, User no access */
#define XTHAL_AR_RW      6 /* Kernel read/write, User no access */
#define XTHAL_AR_RWX     7 /* Kernel read/write/execute, User no access */
#define XTHAL_AR_Ww      8 /* Kernel write, User write */
#define XTHAL_AR_RWrwx   9 /* Kernel read/write , User read/write/execute */
#define XTHAL_AR_RWr    10 /* Kernel read/write, User read */
#define XTHAL_AR_RWXrx  11 /* Kernel read/write/execute, User read/execute */
#define XTHAL_AR_Rr     12 /* Kernel read, User read */
#define XTHAL_AR_RXrx   13 /* Kernel read/execute, User read/execute */
#define XTHAL_AR_RWrw   14 /* Kernel read/write, User read/write */
#define XTHAL_AR_RWXrwx 15 /* Kernel read/write/execute,
                                    User read/write/execute */

#define XTHAL_AR_WIDTH  4  /* # bits used to encode access rights */

/* If the bit XTHAL_MPU_USE_EXISTING_ACCESS_RIGHTS is set in the accessRights
 * argument to xthal_mpu_set_region_attribute(), or to the cattr argument of
 * xthal_set_region_attribute() then the existing access rights for the first
 * byte of the region will be used as the access rights of the new region.
 */
#define XTHAL_MPU_USE_EXISTING_ACCESS_RIGHTS     0x00002000

/* If the bit XTHAL_MPU_USE_EXISTING_MEMORY_TYPE is set in the memoryType
 * argument to xthal_mpu_set_region_attribute(), or to the cattr argument of
 * xthal_set_region_attribute() then the existing memory type for the first
 * byte of the region will be used as the memory type of the new region.
 */
#define XTHAL_MPU_USE_EXISTING_MEMORY_TYPE       0x00004000

/* The following groups of constants are bit-wise or'd together to specify
 * the memory type as input to the macros and functions that accept an
 * unencoded memory type specifier:
 * XTHAL_ENCODE_MEMORY_TYPE, xthal_encode_memory_type,
 * xthal_mpu_set_region_attribute(), and xthal_set_region_attribute().
 *
 * example:
 * XTHAL_MEM_DEVICE | XTHAL_MEM_INTERRUPTIBLE | XTHAL_MEM_SYSTEM_SHARABLE
 *
 * or
 * XTHAL_MEM_WRITEBACK |  XTHAL_MEM_INNER_SHAREABLE
 *
 * If it is desired to specify different attributes for the system and
 * local cache, then macro XTHAL_MEM_PROC_CACHE is used:
 *
 * XTHAL_MEM_PROC_CACHE(XTHAL_MEM_WRITEBACK, XTHAL_MEM_WRITETHRU)
 *
 * indicates the shared cache is writeback, but the processor's local cache
 * is writethrough.
 *
 */

/* The following group of constants are used to specify cache attributes of
 * an MPU entry.  If the processors local cache and the system's shared cache
 * have the same attributes (or if there aren't distinct local and shared
 * caches) then the constant can be used directly.  If different attributes
 * for the shared and local caches, then use these constants as the parameters
 * to the XTHAL_MEM_PROC_CACHE() macro.
 */
#define XTHAL_MEM_DEVICE                         0x00008000
#define XTHAL_MEM_NON_CACHEABLE                  0x00090000
#define XTHAL_MEM_WRITETHRU_NOALLOC              0x00080000
#define XTHAL_MEM_WRITETHRU                      0x00040000
#define XTHAL_MEM_WRITETHRU_WRITEALLOC           0x00060000
#define XTHAL_MEM_WRITEBACK_NOALLOC              0x00050000
#define XTHAL_MEM_WRITEBACK                      0x00070000

/* Indicates a read is interruptible. Only applicable to devices */
#define XTHAL_MEM_INTERRUPTIBLE                  0x08000000

/* Indicates if writes to this memory are bufferable ... only applicable
 * to devices, and non-cacheable memory.
 */
#define XTHAL_MEM_BUFFERABLE                     0x01000000

/* The following group of constants indicates the scope of the sharing of
 * the memory region. XTHAL_MEM_INNER_SHAREABLE and XTHAL_MEM_OUTER_SHARABLE are
 * only applicable to cacheable regions. XTHAL_MEM_SYSTEM_SHAREABLE is only
 * applicable to devices and non-cacheable regions.
 */
#define XTHAL_MEM_NON_SHAREABLE                  0x00000000
#define XTHAL_MEM_INNER_SHAREABLE                0x02000000
#define XTHAL_MEM_OUTER_SHAREABLE                0x04000000
#define XTHAL_MEM_SYSTEM_SHAREABLE               0x06000000


/*
 * This macro is needed when the cache attributes are different for the shared
 * and processor's local caches. For example:
 *
 * XTHAL_MEM_PROC_CACHE(XTHAL_MEM_WRITEBACK, XTHAL_MEM_NON_CACHEABLE)
 * creates a memory type that is writeback cacheable in the system cache, and not
 * cacheable in the processor's local cache.
 */
#define XTHAL_MEM_PROC_CACHE(system, processor) \
    (((system) & 0x000f0000) | (((processor) & 0x000f0000 ) << 4) | \
    (((system) & XTHAL_MEM_DEVICE) | ((processor) & XTHAL_MEM_DEVICE)))

/*
 * This macro converts a bit-wise combination of the XTHAL_MEM_... constants
 * to the corresponding MPU memory type (9-bits).
 *
 * Unsupported combinations are mapped to the best available substitute.
 *
 * The same functionality plus error checking is available from
 * xthal_encode_memory_type().
 */
#define XTHAL_ENCODE_MEMORY_TYPE(x) \
    (((x) & 0xffffe000) ? \
    (_XTHAL_MEM_IS_DEVICE((x)) ?  _XTHAL_ENCODE_DEVICE((x)) : \
      (_XTHAL_IS_SYSTEM_NONCACHEABLE((x)) ? \
        _XTHAL_ENCODE_SYSTEM_NONCACHEABLE((x)) : \
        _XTHAL_ENCODE_SYSTEM_CACHEABLE((x)))) : (x))

/*
 * This structure is used to represent each MPU entry (both foreground and
 * background). The internal representation of the structure is subject to
 * change, so it should only be accessed by the XTHAL_MPU_ENTRY_... macros
 * below.
 */
typedef struct xthal_MPU_entry
{
	uint32_t as;	/* virtual start address, and valid bit */
	uint32_t at;	/* access rights, and memory type (and space for entry index) */
} xthal_MPU_entry;

extern const xthal_MPU_entry Xthal_mpu_bgmap[];




/*
 * XTHAL_MPU_ENTRY creates an MPU entry from its component values.  It is
 * intended for initializing an MPU map.  Example:
 *     const struct xthal_MPU_entry mpumap[] =
            { XTHAL_MPU_ENTRY( 0x00000000, 1, XTHAL_AR_RWXrwx, XTHAL_MEM_WRITEBACK),
              XTHAL_MPU_ENTRY( 0xE0000000, 1, XTHAL_AR_RWXrwx,
              XTHAL_MEM_NON_CACHEABLE | XTHAL_MEM_BUFFERABLE),
              XTHAL_MPU_ENTRY( 0xF0000000, 1, XTHAL_AR_RWX,
              XTHAL_MEM_NON_CACHEABLE | XTHAL_MEM_BUFFERABLE) };
       xthal_write_map(mpumap, sizeof(mpumap) / sizeof(struct xthal_MPU_entry));
 *
 */
#define XTHAL_MPU_ENTRY(vaddr, valid, access, memtype) \
    { (((vaddr) & 0xffffffe0) | ((valid & 0x1))), \
    (((XTHAL_ENCODE_MEMORY_TYPE(memtype)) << 12) | (((access) & 0xf) << 8)) }

/*
 * These macros get (or set) the specified field of the MPU entry.
 */
#define XTHAL_MPU_ENTRY_GET_VSTARTADDR(x) ((x).as & 0xffffffe0)

#define XTHAL_MPU_ENTRY_SET_VSTARTADDR(x, vaddr) (x).as = \
    (((x).as) & 0x1) | ((vaddr) & 0xffffffe0)

#define XTHAL_MPU_ENTRY_GET_VALID(x) (((x).as & 0x1))

#define XTHAL_MPU_ENTRY_SET_VALID(x, valid) (x).as = \
    (((x).as & 0xfffffffe) | ((valid) & 0x1))
#define XTHAL_MPU_ENTRY_GET_ACCESS(x) ((((x).at) >> 8) & 0xf)

#define XTHAL_MPU_ENTRY_SET_ACCESS(x, accessRights) ((x).at = \
        ((x).at & 0xfffff0ff) | (((accessRights) & 0xf) << 8))

#define XTHAL_MPU_ENTRY_GET_MEMORY_TYPE(x) ((((x).at) >> 12) & 0x1ff)

#define XTHAL_MPU_ENTRY_SET_MEMORY_TYPE(x, memtype) ((x).at = \
        ((x).at & 0xffe00fff) | (((XTHAL_ENCODE_MEMORY_TYPE(memtype)) & 0x1ff) << 12))

/*
 * These functions accept encoded access rights, and return 1 if the
 * supplied memory type has the property specified by the function name,
 * otherwise they return 0.
 */
extern int32_t xthal_is_kernel_readable(uint32_t accessRights);
extern int32_t xthal_is_kernel_writeable(uint32_t accessRights);
extern int32_t xthal_is_kernel_executable(uint32_t accessRights);
extern int32_t xthal_is_user_readable(uint32_t accessRights);
extern int32_t xthal_is_user_writeable (uint32_t accessRights);
extern int32_t xthal_is_user_executable(uint32_t accessRights);


/*
 * This function converts a bit-wise combination of the XTHAL_MEM_.. constants
 * to the corresponding MPU memory type (9-bits).
 *
 * If none of the XTHAL_MEM_.. bits are present in the argument, then
 * bits 4-12 (9-bits) are returned ... this supports using an already encoded
 * memoryType (perhaps obtained from an xthal_MPU_entry structure) as input
 * to xthal_set_region_attribute().
 *
 * This function first checks that the supplied constants are a valid and
 * supported combination.  If not, it returns XTHAL_BAD_MEMORY_TYPE.
 */
extern int xthal_encode_memory_type(uint32_t x);

/*
 * This function accepts a 9-bit memory type value (such as returned by
 * XTHAL_MEM_ENTRY_GET_MEMORY_TYPE() or xthal_encode_memory_type(). They
 * return 1 if the memoryType has the property specified in the function
 * name and 0 otherwise.
 */
extern int32_t xthal_is_cacheable(uint32_t memoryType);
extern int32_t xthal_is_writeback(uint32_t memoryType);
extern int32_t xthal_is_device(uint32_t  memoryType);

/*
 * Copies the current MPU entry list into 'entries' which
 * must point to available memory of at least
 * sizeof(struct xthal_MPU_entry) * XCHAL_MPU_ENTRIES.
 *
 * This function returns XTHAL_SUCCESS.
 * XTHAL_INVALID, or
 * XTHAL_UNSUPPORTED.
 */
extern int32_t xthal_read_map(struct xthal_MPU_entry* entries);

/*
 * Writes the map pointed to by 'entries' to the MPU. Before updating
 * the map, it commits any uncommitted
 * cache writes, and invalidates the cache if necessary.
 *
 * This function does not check for the correctness of the map.  Generally
 * xthal_check_map() should be called first to check the map.
 *
 * If n == 0 then the existing map is cleared, and no new map is written
 * (useful for returning to reset state)
 *
 * If (n > 0 && n < XCHAL_MPU_ENTRIES) then a new map is written with
 * (XCHAL_MPU_ENTRIES-n) padding entries added to ensure a properly ordered
 * map.  The resulting foreground map will be equivalent to the map vector
 * fg, but the position of the padding entries should not be relied upon.
 *
 * If n == XCHAL_MPU_ENTRIES then the complete map as specified by fg is
 * written.
 *
 * The CACHEADRDIS register will be set to enable caching any 512MB region
 * that is overlapped by an MPU region with a cacheable memory type.
 * Caching will be disabled if none of the 512 MB region is cacheable.
 *
 * xthal_write_map() disables the MPU foreground map during the MPU
 * update and relies on the background map.
 *
 * As a result any interrupt that does not meet the following conditions
 * must be disabled before calling xthal_write_map():
 *    1) All code and data needed for the interrupt must be
 *       mapped by the background map with sufficient access rights.
 *    2) The interrupt code must not access the MPU.
 *
 */
extern void xthal_write_map(const struct xthal_MPU_entry* entries, uint32_t n);

/*
 * Checks if entry vector 'entries' of length 'n' is a valid MPU access map.
 * Returns:
 *    XTHAL_SUCCESS if valid,
 *    XTHAL_OUT_OF_ENTRIES
 *    XTHAL_MAP_NOT_ALIGNED,
 *    XTHAL_BAD_ACCESS_RIGHTS,
 *    XTHAL_OUT_OF_ORDER_MAP, or
 *    XTHAL_UNSUPPORTED if config doesn't have an MPU.
 */
extern int xthal_check_map(const struct xthal_MPU_entry* entries, uint32_t n);

/*
 * Returns the MPU entry that maps 'vaddr'. If 'infgmap' is non-NULL then
 * *infgmap is set to 1 if 'vaddr' is mapped by the foreground map, and
 * *infgmap is set to 0 if 'vaddr' is mapped by the background map.
 */
extern struct xthal_MPU_entry xthal_get_entry_for_address(void* vaddr,
	int32_t* infgmap);

/*
 * Scans the supplied MPU map and returns a value suitable for writing to
 * the CACHEADRDIS register:
 * Bits 0-7    -> 1 if there are no cacheable areas in the corresponding 512MB
 * region and 0 otherwise.
 * Bits 8-31   -> undefined.
 * This function can accept a partial memory map in the same manner
 * xthal_write_map() does, */
extern uint32_t
xthal_calc_cacheadrdis(const struct xthal_MPU_entry* e, uint32_t n);

/*
 * This function is intended as an MPU specific version of
 * xthal_set_region_attributes(). xthal_set_region_attributes() calls
 * this function for MPU configurations.
 *
 * This function sets the attributes for the region [vaddr, vaddr+size)
 * in the MPU.
 *
 * Depending on the state of the MPU this function will require from
 * 0 to 3 unused MPU entries.
 *
 * This function typically will move, add, and subtract entries from
 * the MPU map during execution, so that the resulting map may
 * be quite different than when the function was called.
 *
 * This function does make the following guarantees:
 *    1) The MPU access map remains in a valid state at all times
 *       during its execution.
 *    2) At all points during (and after) completion the memoryType
 *       and accessRights remain the same for all addresses
 *       that are not in the range [vaddr, vaddr+size).
 *    3) If XTHAL_SUCCESS is returned, then the range
 *       [vaddr, vaddr+size) will have the accessRights and memoryType
 *       specified.
 *    4) The CACHEADRDIS register will be set to enable caching any 512MB region
 *       that is overlapped by an MPU region with a cacheable memory type.
 *       Caching will be disabled if none of the 512 MB region is cacheable.
 *
 * The accessRights parameter should be either a 4-bit value corresponding
 * to an MPU access mode (as defined by the XTHAL_AR_.. constants), or
 * XTHAL_MPU_USE_EXISTING_ACCESS_RIGHTS.
 *
 * The memoryType parameter should be either a bit-wise or-ing of XTHAL_MEM_..
 * constants that represent a valid MPU memoryType, a 9-bit MPU memoryType
 * value, or XTHAL_MPU_USE_EXISTING_MEMORY_TYPE.
 *
 * In addition to the error codes that xthal_set_region_attribute()
 * returns, this function can also return: XTHAL_BAD_ACCESS_RIGHTS
 * (if the access rights bits map to an unsupported combination), or
 * XTHAL_OUT_OF_MAP_ENTRIES (if there are not enough unused MPU entries)
 *
 * If this function is called with an invalid MPU map, then this function
 * will return one of the codes that is returned by xthal_check_map().
 *
 * The flag, XTHAL_CAFLAG_EXPAND, is not supported
 *
 */

extern int xthal_mpu_set_region_attribute(void* vaddr, size_t size,
	int32_t accessRights, int32_t memoryType, uint32_t flags);

/* The following are internal implementation macros.  These should not
 * be directly used except by the hal code and headers.
*/

/*
 * Layout of the MPU specifier for: XTHAL_ENCODE_MEMORY_TYPE(),
 * xthal_encode_memory_type(), xthal_set_region_attribute(),
 * and xthal_mpu_set_region_attribute(). THIS IS SUBJECT TO CHANGE:
 *
 * Bits 0-3  - reserved for pass through of accessRights
 * Bits 4-12 - reserved for pass through of memoryType bits
 * Bit  13   - indicates to use existing access rights of region
 * Bit  14   - indicates to use existing memory type of region
 * Bit  15   - indicates device
 * Bit  16-19- system cache properties
 * Bit  20-23- local cache properties
 * Bit  24   - indicates bufferable
 * Bit  25-26- encodes shareability (1=inner, 2=outer, 3=system)
 * Bit  27   - indicates interruptible
 * Bits 28-31- reserved for future use
 */
#define _XTHAL_SYSTEM_CACHE_BITS 0x000f0000
#define _XTHAL_LOCAL_CACHE_BITS 0x00f00000
#define _XTHAL_MEM_SYSTEM_RWC_MASK 0x00070000
#define _XTHAL_MEM_LOCAL_RWC_MASK  0x00700000
#define _XTHAL_SHIFT_RWC 16

#define _XTHAL_MEM_ANY_SHAREABLE(x) (((x) & XTHAL_MEM_SYSTEM_SHAREABLE) ? 1 : 0)

#define _XTHAL_MEM_INNER_SHAREABLE(x) ((((x) & XTHAL_MEM_SYSTEM_SHAREABLE) \
        == XTHAL_MEM_INNER_SHAREABLE) ? 1 : 0)

#define _XTHAL_MEM_IS_BUFFERABLE(x) (((x) & XTHAL_MEM_BUFFERABLE) ? 1 : 0)

#define _XTHAL_MEM_IS_DEVICE(x) (((x) & XTHAL_MEM_DEVICE) ? 1 : 0)

#define _XTHAL_NON_CACHEABLE_DOMAIN(x) \
    (_XTHAL_MEM_IS_DEVICE(x) || _XTHAL_MEM_ANY_SHAREABLE(x)? 0x3 : 0)

#define _XTHAL_CACHEABLE_DOMAIN(x)  (_XTHAL_MEM_ANY_SHAREABLE(x) ? \
        0x3 : 0x1)

#define _XTHAL_MEM_CACHE_MASK(x) ((x) & _XTHAL_SYSTEM_CACHE_BITS)

#define _XTHAL_IS_SYSTEM_NONCACHEABLE(x) \
        (((_XTHAL_MEM_CACHE_MASK(x) & XTHAL_MEM_NON_CACHEABLE) == \
                XTHAL_MEM_NON_CACHEABLE) ? 1 : 0)

#define _XTHAL_ENCODE_DEVICE(x) \
         (((((x) & XTHAL_MEM_INTERRUPTIBLE) ? 1 : 0) << 3) | \
         (_XTHAL_NON_CACHEABLE_DOMAIN(x) << 1) | _XTHAL_MEM_IS_BUFFERABLE(x))

#define _XTHAL_ENCODE_SYSTEM_NONCACHEABLE(x) \
        (0x18 | (_XTHAL_NON_CACHEABLE_DOMAIN(x) << 1) \
                | _XTHAL_MEM_IS_BUFFERABLE(x))

#define _XTHAL_ENCODE_SYSTEM_CACHEABLE(x) \
        (((((((x) & _XTHAL_LOCAL_CACHE_BITS) >> 4) & XTHAL_MEM_NON_CACHEABLE) == \
                XTHAL_MEM_NON_CACHEABLE) ? 1 : 0) ? \
         (_XTHAL_CACHEABLE_DOMAIN(x) << 4) : \
         _XTHAL_ENCODE_SYSTEM_CACHEABLE_LOCAL_CACHEABLE(x)) | \
         ((_XTHAL_MEM_INNER_SHAREABLE(x) << 3) | \
                  (_XTHAL_MEM_CACHE_MASK(x) & _XTHAL_MEM_SYSTEM_RWC_MASK) \
                  >> _XTHAL_SHIFT_RWC)

#define _XTHAL_ENCODE_SYSTEM_CACHEABLE_LOCAL_CACHEABLE(x) \
        ((_XTHAL_CACHEABLE_DOMAIN(x) << 7) | (((((x) & _XTHAL_LOCAL_CACHE_BITS) ? \
                ((x) & _XTHAL_LOCAL_CACHE_BITS) : \
                (_XTHAL_MEM_CACHE_MASK(x) << 4)) \
        & (_XTHAL_MEM_LOCAL_RWC_MASK)) >> _XTHAL_SHIFT_RWC ))

/* End of internal macros */

/* The functions and constants below here have been deprecated.*/
#define XTHAL_MEM_NON_CACHED                     XTHAL_MEM_NON_CACHEABLE
#define XTHAL_MEM_NON_SHARED                     XTHAL_MEM_NON_SHAREABLE
#define XTHAL_MEM_INNER_SHARED                   XTHAL_MEM_INNER_SHAREABLE
#define XTHAL_MEM_OUTER_SHARED                   XTHAL_MEM_OUTER_SHAREABLE
#define XTHAL_MEM_SYSTEM_SHARED                  XTHAL_MEM_SYSTEM_SHAREABLE
#define XTHAL_MEM_SW_SHAREABLE 0

#define xthal_is_cached(memoryType) (xthal_is_cacheable((memoryType)))
extern int32_t xthal_read_background_map(struct xthal_MPU_entry* entries);

/* end deprecated functions and constants */

#ifdef __cplusplus
}
#endif
#endif /*!_ASMLANGUAGE && !_NOCLANGUAGE && !__ASSEMBLER__ */

#endif /* !XTENSA_HAL_NON_PRIVILEGED_ONLY */




/****************************************************************************
		EXPERIMENTAL and DEPRECATED Definitions
 ****************************************************************************/


#if !defined(_ASMLANGUAGE) && !defined(_NOCLANGUAGE) && !defined(__ASSEMBLER__)
#ifdef __cplusplus
extern "C" {
#endif

#ifdef INCLUDE_DEPRECATED_HAL_DEBUG_CODE
#define XTHAL_24_BIT_BREAK		0x80000000
#define XTHAL_16_BIT_BREAK		0x40000000
extern const unsigned short	Xthal_ill_inst_16[16];
#define XTHAL_DEST_REG		0xf0000000	/* Mask for destination register */
#define XTHAL_DEST_REG_INST	0x08000000	/* Branch address is in register */
#define XTHAL_DEST_REL_INST	0x04000000	/* Branch address is relative */
#define XTHAL_RFW_INST		0x00000800
#define XTHAL_RFUE_INST		0x00000400
#define XTHAL_RFI_INST		0x00000200
#define XTHAL_RFE_INST		0x00000100
#define XTHAL_RET_INST		0x00000080
#define XTHAL_BREAK_INST	0x00000040
#define XTHAL_SYSCALL_INST	0x00000020
#define XTHAL_LOOP_END		0x00000010	/* Not set by xthal_inst_type */
#define XTHAL_JUMP_INST		0x00000008	/* Call or jump instruction */
#define XTHAL_BRANCH_INST	0x00000004	/* Branch instruction */
#define XTHAL_24_BIT_INST	0x00000002
#define XTHAL_16_BIT_INST   0x00000001
typedef struct xthal_state {
    unsigned	pc;
    unsigned	ar[16];
    unsigned	lbeg;
    unsigned	lend;
    unsigned	lcount;
    unsigned	extra_ptr;
    unsigned	cpregs_ptr[XTHAL_MAX_CPS];
} XTHAL_STATE;
extern unsigned int xthal_inst_type(void *addr);
extern unsigned int xthal_branch_addr(void *addr);
extern unsigned int xthal_get_npc(XTHAL_STATE *user_state);
#endif /* INCLUDE_DEPRECATED_HAL_DEBUG_CODE */

#ifdef __cplusplus
}
#endif
#endif /*!_ASMLANGUAGE && !_NOCLANGUAGE && !__ASSEMBLER__ */

#endif /*XTENSA_HAL_H*/

