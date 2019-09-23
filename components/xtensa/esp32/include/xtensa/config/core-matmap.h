/* 
 * xtensa/config/core-matmap.h -- Memory access and translation mapping
 *	parameters (CHAL) of the Xtensa processor core configuration.
 *
 *  If you are using Xtensa Tools, see <xtensa/config/core.h> (which includes
 *  this file) for more details.
 *
 *  In the Xtensa processor products released to date, all parameters
 *  defined in this file are derivable (at least in theory) from
 *  information contained in the core-isa.h header file.
 *  In particular, the following core configuration parameters are relevant:
 *	XCHAL_HAVE_CACHEATTR
 *	XCHAL_HAVE_MIMIC_CACHEATTR
 *	XCHAL_HAVE_XLT_CACHEATTR
 *	XCHAL_HAVE_PTP_MMU
 *	XCHAL_ITLB_ARF_ENTRIES_LOG2
 *	XCHAL_DTLB_ARF_ENTRIES_LOG2
 *	XCHAL_DCACHE_IS_WRITEBACK
 *	XCHAL_ICACHE_SIZE		(presence of I-cache)
 *	XCHAL_DCACHE_SIZE		(presence of D-cache)
 *	XCHAL_HW_VERSION_MAJOR
 *	XCHAL_HW_VERSION_MINOR
 */

/* Customer ID=11657; Build=0x5fe96; Copyright (c) 1999-2016 Tensilica Inc.

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
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */


#ifndef XTENSA_CONFIG_CORE_MATMAP_H
#define XTENSA_CONFIG_CORE_MATMAP_H


/*----------------------------------------------------------------------
			CACHE (MEMORY ACCESS) ATTRIBUTES
  ----------------------------------------------------------------------*/


/*  Cache Attribute encodings -- lists of access modes for each cache attribute:  */
#define XCHAL_FCA_LIST		XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_BYPASS	XCHAL_SEP \
				XTHAL_FAM_BYPASS	XCHAL_SEP \
				XTHAL_FAM_BYPASS	XCHAL_SEP \
				XTHAL_FAM_BYPASS	XCHAL_SEP \
				XTHAL_FAM_BYPASS	XCHAL_SEP \
				XTHAL_FAM_BYPASS	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION	XCHAL_SEP \
				XTHAL_FAM_EXCEPTION
#define XCHAL_LCA_LIST		XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION	XCHAL_SEP \
				XTHAL_LAM_BYPASSG	XCHAL_SEP \
				XTHAL_LAM_EXCEPTION
#define XCHAL_SCA_LIST		XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION	XCHAL_SEP \
				XTHAL_SAM_BYPASS	XCHAL_SEP \
				XTHAL_SAM_EXCEPTION


/*
 *  Specific encoded cache attribute values of general interest.
 *  If a specific cache mode is not available, the closest available
 *  one is returned instead (eg. writethru instead of writeback,
 *  bypass instead of writethru).
 */
#define XCHAL_CA_BYPASS  		2	/* cache disabled (bypassed) mode */
#define XCHAL_CA_BYPASSBUF  		6	/* cache disabled (bypassed) bufferable mode */
#define XCHAL_CA_WRITETHRU		2	/* cache enabled (write-through) mode */
#define XCHAL_CA_WRITEBACK		2	/* cache enabled (write-back) mode */
#define XCHAL_HAVE_CA_WRITEBACK_NOALLOC	0	/* write-back no-allocate availability */
#define XCHAL_CA_WRITEBACK_NOALLOC	2	/* cache enabled (write-back no-allocate) mode */
#define XCHAL_CA_BYPASS_RW  		0	/* cache disabled (bypassed) mode (no exec) */
#define XCHAL_CA_WRITETHRU_RW		0	/* cache enabled (write-through) mode (no exec) */
#define XCHAL_CA_WRITEBACK_RW		0	/* cache enabled (write-back) mode (no exec) */
#define XCHAL_CA_WRITEBACK_NOALLOC_RW	0	/* cache enabled (write-back no-allocate) mode (no exec) */
#define XCHAL_CA_ILLEGAL		15	/* no access allowed (all cause exceptions) mode */
#define XCHAL_CA_ISOLATE		0	/* cache isolate (accesses go to cache not memory) mode */


/*----------------------------------------------------------------------
				MMU
  ----------------------------------------------------------------------*/

/*
 *  General notes on MMU parameters.
 *
 *  Terminology:
 *	ASID = address-space ID (acts as an "extension" of virtual addresses)
 *	VPN  = virtual page number
 *	PPN  = physical page number
 *	CA   = encoded cache attribute (access modes)
 *	TLB  = translation look-aside buffer (term is stretched somewhat here)
 *	I    = instruction (fetch accesses)
 *	D    = data (load and store accesses)
 *	way  = each TLB (ITLB and DTLB) consists of a number of "ways"
 *		that simultaneously match the virtual address of an access;
 *		a TLB successfully translates a virtual address if exactly
 *		one way matches the vaddr; if none match, it is a miss;
 *		if multiple match, one gets a "multihit" exception;
 *		each way can be independently configured in terms of number of
 *		entries, page sizes, which fields are writable or constant, etc.
 *	set  = group of contiguous ways with exactly identical parameters
 *	ARF  = auto-refill; hardware services a 1st-level miss by loading a PTE
 *		from the page table and storing it in one of the auto-refill ways;
 *		if this PTE load also misses, a miss exception is posted for s/w.
 *	min-wired = a "min-wired" way can be used to map a single (minimum-sized)
 * 		page arbitrarily under program control; it has a single entry,
 *		is non-auto-refill (some other way(s) must be auto-refill),
 *		all its fields (VPN, PPN, ASID, CA) are all writable, and it
 *		supports the XCHAL_MMU_MIN_PTE_PAGE_SIZE page size (a current
 *		restriction is that this be the only page size it supports).
 *
 *  TLB way entries are virtually indexed.
 *  TLB ways that support multiple page sizes:
 *	- must have all writable VPN and PPN fields;
 *	- can only use one page size at any given time (eg. setup at startup),
 *	  selected by the respective ITLBCFG or DTLBCFG special register,
 *	  whose bits n*4+3 .. n*4 index the list of page sizes for way n
 *	  (XCHAL_xTLB_SETm_PAGESZ_LOG2_LIST for set m corresponding to way n);
 *	  this list may be sparse for auto-refill ways because auto-refill
 *	  ways have independent lists of supported page sizes sharing a
 *	  common encoding with PTE entries; the encoding is the index into
 *	  this list; unsupported sizes for a given way are zero in the list;
 *	  selecting unsupported sizes results in undefined hardware behaviour;
 *	- is only possible for ways 0 thru 7 (due to ITLBCFG/DTLBCFG definition).
 */

#define XCHAL_MMU_ASID_INVALID		0	/* ASID value indicating invalid address space */
#define XCHAL_MMU_ASID_KERNEL		0	/* ASID value indicating kernel (ring 0) address space */
#define XCHAL_MMU_SR_BITS		0	/* number of size-restriction bits supported */
#define XCHAL_MMU_CA_BITS		4	/* number of bits needed to hold cache attribute encoding */
#define XCHAL_MMU_MAX_PTE_PAGE_SIZE	29	/* max page size in a PTE structure (log2) */
#define XCHAL_MMU_MIN_PTE_PAGE_SIZE	29	/* min page size in a PTE structure (log2) */


/***  Instruction TLB:  ***/

#define XCHAL_ITLB_WAY_BITS		0	/* number of bits holding the ways */
#define XCHAL_ITLB_WAYS			1	/* number of ways (n-way set-associative TLB) */
#define XCHAL_ITLB_ARF_WAYS		0	/* number of auto-refill ways */
#define XCHAL_ITLB_SETS			1	/* number of sets (groups of ways with identical settings) */

/*  Way set to which each way belongs:  */
#define XCHAL_ITLB_WAY0_SET		0

/*  Ways sets that are used by hardware auto-refill (ARF):  */
#define XCHAL_ITLB_ARF_SETS		0	/* number of auto-refill sets */

/*  Way sets that are "min-wired" (see terminology comment above):  */
#define XCHAL_ITLB_MINWIRED_SETS	0	/* number of "min-wired" sets */


/*  ITLB way set 0 (group of ways 0 thru 0):  */
#define XCHAL_ITLB_SET0_WAY			0	/* index of first way in this way set */
#define XCHAL_ITLB_SET0_WAYS			1	/* number of (contiguous) ways in this way set */
#define XCHAL_ITLB_SET0_ENTRIES_LOG2		3	/* log2(number of entries in this way) */
#define XCHAL_ITLB_SET0_ENTRIES			8	/* number of entries in this way (always a power of 2) */
#define XCHAL_ITLB_SET0_ARF			0	/* 1=autorefill by h/w, 0=non-autorefill (wired/constant/static) */
#define XCHAL_ITLB_SET0_PAGESIZES		1	/* number of supported page sizes in this way */
#define XCHAL_ITLB_SET0_PAGESZ_BITS		0	/* number of bits to encode the page size */
#define XCHAL_ITLB_SET0_PAGESZ_LOG2_MIN		29	/* log2(minimum supported page size) */
#define XCHAL_ITLB_SET0_PAGESZ_LOG2_MAX		29	/* log2(maximum supported page size) */
#define XCHAL_ITLB_SET0_PAGESZ_LOG2_LIST	29	/* list of log2(page size)s, separated by XCHAL_SEP;
							   2^PAGESZ_BITS entries in list, unsupported entries are zero */
#define XCHAL_ITLB_SET0_ASID_CONSTMASK		0	/* constant ASID bits; 0 if all writable */
#define XCHAL_ITLB_SET0_VPN_CONSTMASK		0x00000000	/* constant VPN bits, not including entry index bits; 0 if all writable */
#define XCHAL_ITLB_SET0_PPN_CONSTMASK		0xE0000000	/* constant PPN bits, including entry index bits; 0 if all writable */
#define XCHAL_ITLB_SET0_CA_CONSTMASK		0	/* constant CA bits; 0 if all writable */
#define XCHAL_ITLB_SET0_ASID_RESET		0	/* 1 if ASID reset values defined (and all writable); 0 otherwise */
#define XCHAL_ITLB_SET0_VPN_RESET		0	/* 1 if VPN reset values defined (and all writable); 0 otherwise */
#define XCHAL_ITLB_SET0_PPN_RESET		0	/* 1 if PPN reset values defined (and all writable); 0 otherwise */
#define XCHAL_ITLB_SET0_CA_RESET		1	/* 1 if CA reset values defined (and all writable); 0 otherwise */
/*  Constant VPN values for each entry of ITLB way set 0 (because VPN_CONSTMASK is non-zero):  */
#define XCHAL_ITLB_SET0_E0_VPN_CONST		0x00000000
#define XCHAL_ITLB_SET0_E1_VPN_CONST		0x20000000
#define XCHAL_ITLB_SET0_E2_VPN_CONST		0x40000000
#define XCHAL_ITLB_SET0_E3_VPN_CONST		0x60000000
#define XCHAL_ITLB_SET0_E4_VPN_CONST		0x80000000
#define XCHAL_ITLB_SET0_E5_VPN_CONST		0xA0000000
#define XCHAL_ITLB_SET0_E6_VPN_CONST		0xC0000000
#define XCHAL_ITLB_SET0_E7_VPN_CONST		0xE0000000
/*  Constant PPN values for each entry of ITLB way set 0 (because PPN_CONSTMASK is non-zero):  */
#define XCHAL_ITLB_SET0_E0_PPN_CONST		0x00000000
#define XCHAL_ITLB_SET0_E1_PPN_CONST		0x20000000
#define XCHAL_ITLB_SET0_E2_PPN_CONST		0x40000000
#define XCHAL_ITLB_SET0_E3_PPN_CONST		0x60000000
#define XCHAL_ITLB_SET0_E4_PPN_CONST		0x80000000
#define XCHAL_ITLB_SET0_E5_PPN_CONST		0xA0000000
#define XCHAL_ITLB_SET0_E6_PPN_CONST		0xC0000000
#define XCHAL_ITLB_SET0_E7_PPN_CONST		0xE0000000
/*  Reset CA values for each entry of ITLB way set 0 (because SET0_CA_RESET is non-zero):  */
#define XCHAL_ITLB_SET0_E0_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E1_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E2_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E3_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E4_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E5_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E6_CA_RESET		0x02
#define XCHAL_ITLB_SET0_E7_CA_RESET		0x02


/***  Data TLB:  ***/

#define XCHAL_DTLB_WAY_BITS		0	/* number of bits holding the ways */
#define XCHAL_DTLB_WAYS			1	/* number of ways (n-way set-associative TLB) */
#define XCHAL_DTLB_ARF_WAYS		0	/* number of auto-refill ways */
#define XCHAL_DTLB_SETS			1	/* number of sets (groups of ways with identical settings) */

/*  Way set to which each way belongs:  */
#define XCHAL_DTLB_WAY0_SET		0

/*  Ways sets that are used by hardware auto-refill (ARF):  */
#define XCHAL_DTLB_ARF_SETS		0	/* number of auto-refill sets */

/*  Way sets that are "min-wired" (see terminology comment above):  */
#define XCHAL_DTLB_MINWIRED_SETS	0	/* number of "min-wired" sets */


/*  DTLB way set 0 (group of ways 0 thru 0):  */
#define XCHAL_DTLB_SET0_WAY			0	/* index of first way in this way set */
#define XCHAL_DTLB_SET0_WAYS			1	/* number of (contiguous) ways in this way set */
#define XCHAL_DTLB_SET0_ENTRIES_LOG2		3	/* log2(number of entries in this way) */
#define XCHAL_DTLB_SET0_ENTRIES			8	/* number of entries in this way (always a power of 2) */
#define XCHAL_DTLB_SET0_ARF			0	/* 1=autorefill by h/w, 0=non-autorefill (wired/constant/static) */
#define XCHAL_DTLB_SET0_PAGESIZES		1	/* number of supported page sizes in this way */
#define XCHAL_DTLB_SET0_PAGESZ_BITS		0	/* number of bits to encode the page size */
#define XCHAL_DTLB_SET0_PAGESZ_LOG2_MIN		29	/* log2(minimum supported page size) */
#define XCHAL_DTLB_SET0_PAGESZ_LOG2_MAX		29	/* log2(maximum supported page size) */
#define XCHAL_DTLB_SET0_PAGESZ_LOG2_LIST	29	/* list of log2(page size)s, separated by XCHAL_SEP;
							   2^PAGESZ_BITS entries in list, unsupported entries are zero */
#define XCHAL_DTLB_SET0_ASID_CONSTMASK		0	/* constant ASID bits; 0 if all writable */
#define XCHAL_DTLB_SET0_VPN_CONSTMASK		0x00000000	/* constant VPN bits, not including entry index bits; 0 if all writable */
#define XCHAL_DTLB_SET0_PPN_CONSTMASK		0xE0000000	/* constant PPN bits, including entry index bits; 0 if all writable */
#define XCHAL_DTLB_SET0_CA_CONSTMASK		0	/* constant CA bits; 0 if all writable */
#define XCHAL_DTLB_SET0_ASID_RESET		0	/* 1 if ASID reset values defined (and all writable); 0 otherwise */
#define XCHAL_DTLB_SET0_VPN_RESET		0	/* 1 if VPN reset values defined (and all writable); 0 otherwise */
#define XCHAL_DTLB_SET0_PPN_RESET		0	/* 1 if PPN reset values defined (and all writable); 0 otherwise */
#define XCHAL_DTLB_SET0_CA_RESET		1	/* 1 if CA reset values defined (and all writable); 0 otherwise */
/*  Constant VPN values for each entry of DTLB way set 0 (because VPN_CONSTMASK is non-zero):  */
#define XCHAL_DTLB_SET0_E0_VPN_CONST		0x00000000
#define XCHAL_DTLB_SET0_E1_VPN_CONST		0x20000000
#define XCHAL_DTLB_SET0_E2_VPN_CONST		0x40000000
#define XCHAL_DTLB_SET0_E3_VPN_CONST		0x60000000
#define XCHAL_DTLB_SET0_E4_VPN_CONST		0x80000000
#define XCHAL_DTLB_SET0_E5_VPN_CONST		0xA0000000
#define XCHAL_DTLB_SET0_E6_VPN_CONST		0xC0000000
#define XCHAL_DTLB_SET0_E7_VPN_CONST		0xE0000000
/*  Constant PPN values for each entry of DTLB way set 0 (because PPN_CONSTMASK is non-zero):  */
#define XCHAL_DTLB_SET0_E0_PPN_CONST		0x00000000
#define XCHAL_DTLB_SET0_E1_PPN_CONST		0x20000000
#define XCHAL_DTLB_SET0_E2_PPN_CONST		0x40000000
#define XCHAL_DTLB_SET0_E3_PPN_CONST		0x60000000
#define XCHAL_DTLB_SET0_E4_PPN_CONST		0x80000000
#define XCHAL_DTLB_SET0_E5_PPN_CONST		0xA0000000
#define XCHAL_DTLB_SET0_E6_PPN_CONST		0xC0000000
#define XCHAL_DTLB_SET0_E7_PPN_CONST		0xE0000000
/*  Reset CA values for each entry of DTLB way set 0 (because SET0_CA_RESET is non-zero):  */
#define XCHAL_DTLB_SET0_E0_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E1_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E2_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E3_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E4_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E5_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E6_CA_RESET		0x02
#define XCHAL_DTLB_SET0_E7_CA_RESET		0x02




#endif /*XTENSA_CONFIG_CORE_MATMAP_H*/

