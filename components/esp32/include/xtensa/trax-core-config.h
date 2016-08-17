/* Definitions for Xtensa processor config info needed for TRAX.

   Copyright (c) 2005-2011 Tensilica Inc.

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

#ifndef TRAX_CORE_CONFIG_H
#define TRAX_CORE_CONFIG_H

#include "xtensa-params.h"

/*
 *  Vector Enumerations.
 */

/*  These must match the LX2.0 and later traceport spec:  */
#define VEC_NO_VECTOR	0
#define VEC_FIRST	VEC_RESET	/* first valid vector */
#define VEC_RESET	1
#define VEC_DEBUG	2
#define VEC_NMI		3
#define VEC_USER	4
#define VEC_KERNEL	5
#define VEC_DOUBLE	6
#define VEC_MEMERR	7
#define VEC_RESERVED8	8
#define VEC_RESERVED9	9
#define VEC_WINO4	10
#define VEC_WINU4	11
#define VEC_WINO8	12
#define VEC_WINU8	13
#define VEC_WINO12	14
#define VEC_WINU12	15
#define VEC_INTLEVEL2	16
#define VEC_INTLEVEL3	17
#define VEC_INTLEVEL4	18
#define VEC_INTLEVEL5	19
#define VEC_INTLEVEL6	20
/*  These are internal, i.e. don't appear like this on traceport:  */
#define VEC_DEBUG_OCD	21
#define VEC_UNKNOWN	22
/* Enumerations 23 through 31 are also reserved, but putting */
/* placeholders here seems wasteful and unnecessary. */
#define VEC_COUNT	23

/*  Other branch (change-of-PC-flow) type encodings;
 *  if PC changes due to an exception or interrupt vector,
 *  one of the VEC_* values above is used, otherwise
 *  (or if it's unknown whether it's due to an exception/interrupt)
 *  one of the following is used:  */

#define BRANCH_IS_VEC(n)	((n) < VEC_COUNT)	/* is known to be except/interrupt? */
#define BRANCH_OR_VEC		24	/* unknown type of branch (branch/exception/interrupt/etc) */
#define BRANCH_UNKNOWN		25	/* unknown type of branch (anything but except/interrupt) */
#define BRANCH_UNKNOWN_ERR	26	/* like BRANCH_UNKNOWN with known error (non-branch instr) */
#define BRANCH_LOOPBACK		28	/* zero-overhead loopback (from LEND to LBEG) */
#define BRANCH_CONDTAKEN	29	/* conditional branch taken (or LOOP{NEZ,GTZ} loop skip) */
#define BRANCH_JUMP		30	/* jump (unconditional branch, i.e. J or JX) */
#define BRANCH_IS_CALL(n)	(((n) & ~3) == 32)	/* is a function call? */
#define BRANCH_CALL0		32	/* non-windowed function call (CALL0, CALLX0) */
#define BRANCH_CALL4		33	/* windowed function call (CALL4, CALLX4) */
#define BRANCH_CALL8		34	/* windowed function call (CALL8, CALLX8) */
#define BRANCH_CALL12		35	/* windowed function call (CALL12, CALLX12) */
#define BRANCH_IS_RETURN(n)	((n) >= 36)		/* is any kind of return? */
#define BRANCH_IS_CALLRETURN(n)	(((n) & ~1) == 36)	/* is a function return? */
#define BRANCH_RET		36	/* non-windowed function return (RET or RET.N) */
#define BRANCH_RETW		37	/* windowed function return (RETW or RETW.N) */
#define BRANCH_IS_EIRETURN(n)	((n) >= 38)		/* is an except/inter. return? */
#define BRANCH_RFE		38	/* RFE or RFUE */
#define BRANCH_RFDE		39	/* RFDE */
#define BRANCH_RFWO		40	/* RFWO */
#define BRANCH_RFWU		41	/* RFWU */
#define BRANCH_RFI_2		42	/* RFI 2 */
#define BRANCH_RFI_3		43	/* RFI 3 */
#define BRANCH_RFI_4		44	/* RFI 4 */
#define BRANCH_RFI_5		45	/* RFI 5 */
#define BRANCH_RFI_6		46	/* RFI 6 */
#define BRANCH_RFI_NMI		47	/* RFI NMILEVEL */
#define BRANCH_RFI_DEBUG	48	/* RFI DEBUGLEVEL */
#define BRANCH_RFME		49	/* RFME */
#define BRANCH_COUNT		50	/* (number of defined BRANCH_xxx values) */



typedef struct {
  unsigned	vaddr;
  unsigned	vaddr2;			/* for static vectors only (reloc vectors option) */
  int		is_configured;
} trax_vector_t;


/*
 *  This structure describes those portion of a Tensilica processor's
 *  configuration that are useful for trace.
 */
typedef struct {
  char **	isa_dlls;
  char *	core_name;		/* (XPG core name, not necessarily same as XTENSA_CORE) */
  int		big_endian;		/* 0 = little-endian, 1 = big-endian */
  int		has_loops;		/* 1 = zero overhead loops configured */
  int		has_autorefill;		/* 1 = TLB autorefill (MMU) configured */
  unsigned	max_instr_size;		/* in bytes (eg. 3, 4, 8, ...) */
  unsigned	int_level_max;		/* number of interrupt levels configured (without NMI) */
  int		debug_level;		/* debug intlevel, 0 if debug not configured */
  int		nmi_level;		/* NMI intlevel, 0 if NMI not configured */
  unsigned	targethw_min;		/* min. targeted hardware version (XTENSA_HWVERSION_<rel>) */
  unsigned	targethw_max;		/* max. targeted hardware version (XTENSA_HWVERSION_<rel>) */
  int		reloc_vectors;		/* 0 = fixed vectors, 1 = relocatable vectors */
  int		statvec_select;		/* 0 = stat vec base 0, 1 = stat vec base 1 (SW default) */
  int		vecbase_align;		/* number of bits to align VECBASE (32 - bits in VECBASE) */
  unsigned	statvec_base0;		/* static vector base 0 */
  unsigned	statvec_base1;		/* static vector base 1 */
  unsigned	vecbase_reset;		/* reset value of VECBASE */
  trax_vector_t	vectors[VEC_COUNT];	/* all vectors... */
} trax_core_config_t;


/*  Globals:  */
//extern const char * const trax_vector_short_names[/*VEC_COUNT*/];	// nobody uses this one
extern const char * const trax_vector_names[/*VEC_COUNT*/];

/*  Prototypes:  */
extern int  trax_read_params (trax_core_config_t *c, xtensa_params p);
extern int  trax_vector_from_address(trax_core_config_t *config, unsigned long vaddr, unsigned long *vecbases);

#endif  /* TRAX_CORE_CONFIG_H */

