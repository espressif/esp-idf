/*
 *  xtensa-libdb-macros.h
 */

/* $Id: //depot/rel/Eaglenest/Xtensa/Software/libdb/xtensa-libdb-macros.h#1 $ */

/* Copyright (c) 2004-2008 Tensilica Inc.

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

#ifndef __H_LIBDB_MACROS
#define __H_LIBDB_MACROS

/*
 *  This header file provides macros used to construct, identify and use
 *  "target numbers" that are assigned to various types of Xtensa processor
 *  registers and states.  These target numbers are used by GDB in the remote
 *  protocol, and are thus used by all GDB debugger agents (targets).
 *  They are also used in ELF debugger information sections (stabs, dwarf, etc).
 *
 *  These macros are separated from xtensa-libdb.h because they are needed
 *  by certain debugger agents that do not use or have access to libdb,
 *  e.g. the OCD daemon, RedBoot, XMON, etc.
 *
 *  For the time being, for compatibility with certain 3rd party debugger
 *  software vendors, target numbers are limited to 16 bits.  It is
 *  conceivable that this will be extended in the future to 32 bits.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef uint32
 #define uint32	unsigned int
#endif
#ifndef int32
 #define int32	int
#endif


/*
 *  Macros to form register "target numbers" for various standard registers/states:
 */
#define XTENSA_DBREGN_INVALID		-1		/* not a valid target number */
#define XTENSA_DBREGN_A(n)		(0x0000+(n))	/* address registers a0..a15 */
#define XTENSA_DBREGN_B(n)		(0x0010+(n))	/* boolean bits b0..b15 */
#define XTENSA_DBREGN_PC		 0x0020		/* program counter */
				      /* 0x0021		   RESERVED for use by Tensilica */
#define XTENSA_DBREGN_BO(n)		(0x0022+(n))	/* boolean octuple-bits bo0..bo1 */
#define XTENSA_DBREGN_BQ(n)		(0x0024+(n))	/* boolean quadruple-bits bq0..bq3 */
#define XTENSA_DBREGN_BD(n)		(0x0028+(n))	/* boolean double-bits bd0..bd7 */
#define XTENSA_DBREGN_F(n)		(0x0030+(n))	/* floating point registers f0..f15 */
#define XTENSA_DBREGN_VEC(n)		(0x0040+(n))	/* Vectra vec regs v0..v15 */
#define XTENSA_DBREGN_VSEL(n)		(0x0050+(n))	/* Vectra sel s0..s3 (V1) ..s7 (V2) */
#define XTENSA_DBREGN_VALIGN(n)		(0x0058+(n))	/* Vectra valign regs u0..u3 */
#define XTENSA_DBREGN_VCOEFF(n)		(0x005C+(n))	/* Vectra I vcoeff regs c0..c1 */
				      /* 0x005E..0x005F	   RESERVED for use by Tensilica */
#define XTENSA_DBREGN_AEP(n)		(0x0060+(n))	/* HiFi2 Audio Engine regs aep0..aep7 */
#define XTENSA_DBREGN_AEQ(n)		(0x0068+(n))	/* HiFi2 Audio Engine regs aeq0..aeq3 */
				      /* 0x006C..0x00FF	   RESERVED for use by Tensilica */
#define XTENSA_DBREGN_AR(n)		(0x0100+(n))	/* physical address regs ar0..ar63
							   (note: only with window option) */
				      /* 0x0140..0x01FF	   RESERVED for use by Tensilica */
#define XTENSA_DBREGN_SREG(n)		(0x0200+(n))	/* special registers 0..255 (core) */
#define XTENSA_DBREGN_BR		XTENSA_DBREGN_SREG(0x04)	/* all 16 boolean bits, BR */
#define XTENSA_DBREGN_MR(n)		XTENSA_DBREGN_SREG(0x20+(n))	/* MAC16 registers m0..m3 */
#define XTENSA_DBREGN_UREG(n)		(0x0300+(n))	/* user registers 0..255 (TIE) */
				      /* 0x0400..0x0FFF	   RESERVED for use by Tensilica */
				      /* 0x1000..0x1FFF	   user-defined regfiles */
				      /* 0x2000..0xEFFF	   other states (and regfiles) */
#define XTENSA_DBREGN_DBAGENT(n)	(0xF000+(n))	/* non-processor "registers" 0..4095 for
							   3rd-party debugger agent defined use */
				      /* > 0xFFFF (32-bit) RESERVED for use by Tensilica */
/*#define XTENSA_DBREGN_CONTEXT(n)	(0x02000000+((n)<<20))*/	/* add this macro's value to a target
							   number to identify a specific context 0..31
							   for context-replicated registers */
#define XTENSA_DBREGN_MASK		0xFFFF		/* mask of valid target_number bits */
#define XTENSA_DBREGN_WRITE_SIDE	0x04000000	/* flag to request write half of a register
							   split into distinct read and write entries
							   with the same target number (currently only
							   valid in a couple of libdb API functions;
							   see xtensa-libdb.h for details) */

/*
 *  Macros to identify specific ranges of target numbers (formed above):
 *  NOTE:  any context number (or other upper 12 bits) are considered
 *  modifiers and are thus stripped out for identification purposes.
 */
#define XTENSA_DBREGN_IS_VALID(tn)	(((tn) & ~0xFFFF) == 0)	/* just tests it's 16-bit unsigned */
#define XTENSA_DBREGN_IS_A(tn)		(((tn) & 0xFFF0)==0x0000)	/* is a0..a15 */
#define XTENSA_DBREGN_IS_B(tn)		(((tn) & 0xFFF0)==0x0010)	/* is b0..b15 */
#define XTENSA_DBREGN_IS_PC(tn)		(((tn) & 0xFFFF)==0x0020)	/* is program counter */
#define XTENSA_DBREGN_IS_BO(tn)		(((tn) & 0xFFFE)==0x0022)	/* is bo0..bo1 */
#define XTENSA_DBREGN_IS_BQ(tn)		(((tn) & 0xFFFC)==0x0024)	/* is bq0..bq3 */
#define XTENSA_DBREGN_IS_BD(tn)		(((tn) & 0xFFF8)==0x0028)	/* is bd0..bd7 */
#define XTENSA_DBREGN_IS_F(tn)		(((tn) & 0xFFF0)==0x0030)	/* is f0..f15 */
#define XTENSA_DBREGN_IS_VEC(tn)	(((tn) & 0xFFF0)==0x0040)	/* is v0..v15 */
#define XTENSA_DBREGN_IS_VSEL(tn)	(((tn) & 0xFFF8)==0x0050)	/* is s0..s7 (s0..s3 in V1) */
#define XTENSA_DBREGN_IS_VALIGN(tn)	(((tn) & 0xFFFC)==0x0058)	/* is u0..u3 */
#define XTENSA_DBREGN_IS_VCOEFF(tn)	(((tn) & 0xFFFE)==0x005C)	/* is c0..c1 */
#define XTENSA_DBREGN_IS_AEP(tn)	(((tn) & 0xFFF8)==0x0060)	/* is aep0..aep7 */
#define XTENSA_DBREGN_IS_AEQ(tn)	(((tn) & 0xFFFC)==0x0068)	/* is aeq0..aeq3 */
#define XTENSA_DBREGN_IS_AR(tn)		(((tn) & 0xFFC0)==0x0100)	/* is ar0..ar63 */
#define XTENSA_DBREGN_IS_SREG(tn)	(((tn) & 0xFF00)==0x0200)	/* is special register */
#define XTENSA_DBREGN_IS_BR(tn)		(((tn) & 0xFFFF)==XTENSA_DBREGN_SREG(0x04))	/* is BR */
#define XTENSA_DBREGN_IS_MR(tn)		(((tn) & 0xFFFC)==XTENSA_DBREGN_SREG(0x20))	/* m0..m3 */
#define XTENSA_DBREGN_IS_UREG(tn)	(((tn) & 0xFF00)==0x0300)	/* is user register */
#define XTENSA_DBREGN_IS_DBAGENT(tn)	(((tn) & 0xF000)==0xF000)	/* is non-processor */
/*#define XTENSA_DBREGN_IS_CONTEXT(tn)	(((tn) & 0x02000000) != 0)*/	/* specifies context # */

/*
 *  Macros to extract register index from a register "target number"
 *  when a specific range has been identified using one of the _IS_ macros above.
 *  These macros only return a useful value if the corresponding _IS_ macro returns true.
 */
#define XTENSA_DBREGN_A_INDEX(tn)	((tn) & 0x0F)		/* 0..15  for a0..a15 */
#define XTENSA_DBREGN_B_INDEX(tn)	((tn) & 0x0F)		/* 0..15  for b0..b15 */
#define XTENSA_DBREGN_BO_INDEX(tn)	((tn) & 0x01)		/* 0..1   for bo0..bo1 */
#define XTENSA_DBREGN_BQ_INDEX(tn)	((tn) & 0x03)		/* 0..3   for bq0..bq3 */
#define XTENSA_DBREGN_BD_INDEX(tn)	((tn) & 0x07)		/* 0..7   for bd0..bd7 */
#define XTENSA_DBREGN_F_INDEX(tn)	((tn) & 0x0F)		/* 0..15  for f0..f15 */
#define XTENSA_DBREGN_VEC_INDEX(tn)	((tn) & 0x0F)		/* 0..15  for v0..v15 */
#define XTENSA_DBREGN_VSEL_INDEX(tn)	((tn) & 0x07)		/* 0..7   for s0..s7 */
#define XTENSA_DBREGN_VALIGN_INDEX(tn)	((tn) & 0x03)		/* 0..3   for u0..u3 */
#define XTENSA_DBREGN_VCOEFF_INDEX(tn)	((tn) & 0x01)		/* 0..1   for c0..c1 */
#define XTENSA_DBREGN_AEP_INDEX(tn)	((tn) & 0x07)		/* 0..7   for aep0..aep7 */
#define XTENSA_DBREGN_AEQ_INDEX(tn)	((tn) & 0x03)		/* 0..3   for aeq0..aeq3 */
#define XTENSA_DBREGN_AR_INDEX(tn)	((tn) & 0x3F)		/* 0..63  for ar0..ar63 */
#define XTENSA_DBREGN_SREG_INDEX(tn)	((tn) & 0xFF)		/* 0..255 for special registers */
#define XTENSA_DBREGN_MR_INDEX(tn)	((tn) & 0x03)		/* 0..3   for m0..m3 */
#define XTENSA_DBREGN_UREG_INDEX(tn)	((tn) & 0xFF)		/* 0..255 for user registers */
#define XTENSA_DBREGN_DBAGENT_INDEX(tn)	((tn) & 0xFFF)		/* 0..4095 for non-processor */
/*#define XTENSA_DBREGN_CONTEXT_INDEX(tn)	(((tn) >> 20) & 0x1F)*/	/* 0..31  context numbers */




#ifdef __cplusplus
}
#endif
      
#endif /* __H_LIBDB_MACROS */

