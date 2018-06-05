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


   $Id: //depot/rel/Eaglenest/Xtensa/OS/target-os-src/hal.h.tpp#4 $

   Copyright (c) 1999-2014 Cadence Design Systems, Inc.

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
#define XTHAL_RELEASE_MAJOR	11000
#define XTHAL_RELEASE_MINOR	3
#define XTHAL_RELEASE_NAME	"11.0.3"
#define XTHAL_REL_11	1
#define XTHAL_REL_11_0	1
#define XTHAL_REL_11_0_3	1

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

/* invalidate the caches */

extern void xthal_icache_region_invalidate( void *addr, unsigned size );
extern void xthal_dcache_region_invalidate( void *addr, unsigned size );
# ifndef XTHAL_USE_CACHE_MACROS
extern void xthal_icache_line_invalidate(void *addr);
extern void xthal_dcache_line_invalidate(void *addr);
# endif
/* write dirty data back */
extern void xthal_dcache_region_writeback( void *addr, unsigned size );
# ifndef XTHAL_USE_CACHE_MACROS
extern void xthal_dcache_line_writeback(void *addr);
# endif
/* write dirty data back and invalidate */
extern void xthal_dcache_region_writeback_inv( void *addr, unsigned size );
# ifndef XTHAL_USE_CACHE_MACROS
extern void xthal_dcache_line_writeback_inv(void *addr);
/* sync icache and memory */
extern void xthal_icache_sync( void );
/* sync dcache and memory */
extern void xthal_dcache_sync( void );
#endif

/* get number of icache ways enabled */
extern unsigned int xthal_icache_get_ways(void);
/* set number of icache ways enabled */
extern void xthal_icache_set_ways(unsigned int ways);
/* get number of dcache ways enabled */
extern unsigned int xthal_dcache_get_ways(void);
/* set number of dcache ways enabled */
extern void xthal_dcache_set_ways(unsigned int ways);

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
#define XTHAL_MAX_INTTYPES		8	/* number of interrupt types */

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

/* invalidate the caches */
extern void xthal_icache_all_invalidate( void );
extern void xthal_dcache_all_invalidate( void );
/* write dirty data back */
extern void xthal_dcache_all_writeback( void );
/* write dirty data back and invalidate */
extern void xthal_dcache_all_writeback_inv( void );
/* prefetch and lock specified memory range into cache */
extern void xthal_icache_region_lock( void *addr, unsigned size );
extern void xthal_dcache_region_lock( void *addr, unsigned size );
# ifndef XTHAL_USE_CACHE_MACROS
extern void xthal_icache_line_lock(void *addr);
extern void xthal_dcache_line_lock(void *addr);
# endif
/* unlock from cache */
extern void xthal_icache_all_unlock( void );
extern void xthal_dcache_all_unlock( void );
extern void xthal_icache_region_unlock( void *addr, unsigned size );
extern void xthal_dcache_region_unlock( void *addr, unsigned size );
# ifndef XTHAL_USE_CACHE_MACROS
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

/*  Convert between virtual and physical addresses (through static maps only):  */
/*** WARNING: these two functions may go away in a future release; don't depend on them! ***/
extern int  xthal_static_v2p( unsigned vaddr, unsigned *paddrp );
extern int  xthal_static_p2v( unsigned paddr, unsigned *vaddrp, unsigned cached );

#define XCHAL_SUCCESS 0
#define XCHAL_ADDRESS_MISALIGNED -1
#define XCHAL_INEXACT -2
#define XCHAL_INVALID_ADDRESS -3
#define XCHAL_UNSUPPORTED_ON_THIS_ARCH -4
#define XCHAL_NO_PAGES_MAPPED -5
#define XTHAL_NO_MAPPING -6

#define XCHAL_CA_R   (0xC0 | 0x40000000)
#define XCHAL_CA_RX  (0xD0 | 0x40000000)
#define XCHAL_CA_RW  (0xE0 | 0x40000000)
#define XCHAL_CA_RWX (0xF0 | 0x40000000)

extern int xthal_set_region_translation(void* vaddr, void* paddr, unsigned size, unsigned cache_atr, unsigned flags);
extern int xthal_v2p(void*, void**, unsigned*, unsigned*);
extern int xthal_invalidate_region(void* addr);
extern int xthal_set_region_translation_raw(void *vaddr, void *paddr, unsigned cattr);

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

#ifdef INCLUDE_DEPRECATED_HAL_CODE
extern const unsigned char Xthal_have_old_exc_arch;
extern const unsigned char Xthal_have_mmu;
extern const unsigned int  Xthal_num_regs;
extern const unsigned char Xthal_num_iroms;
extern const unsigned char Xthal_num_irams;
extern const unsigned char Xthal_num_droms;
extern const unsigned char Xthal_num_drams;
extern const unsigned int  Xthal_configid0;
extern const unsigned int  Xthal_configid1;
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

