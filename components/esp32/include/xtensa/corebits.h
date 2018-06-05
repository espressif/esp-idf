/*
 * xtensa/corebits.h - Xtensa Special Register field positions, masks, values.
 *
 * (In previous releases, these were defined in specreg.h, a generated file.
 *  This file is not generated, ie. it is processor configuration independent.)
 */

/* $Id: //depot/rel/Eaglenest/Xtensa/OS/include/xtensa/corebits.h#2 $ */

/*
 * Copyright (c) 2005-2011 Tensilica Inc.
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

#ifndef XTENSA_COREBITS_H
#define XTENSA_COREBITS_H

/*  EXCCAUSE register fields:  */
#define EXCCAUSE_EXCCAUSE_SHIFT	0
#define EXCCAUSE_EXCCAUSE_MASK	0x3F
/*  EXCCAUSE register values:  */
/*
 *  General Exception Causes
 *  (values of EXCCAUSE special register set by general exceptions,
 *   which vector to the user, kernel, or double-exception vectors).
 */
#define EXCCAUSE_ILLEGAL		0	/* Illegal Instruction */
#define EXCCAUSE_SYSCALL		1	/* System Call (SYSCALL instruction) */
#define EXCCAUSE_INSTR_ERROR		2	/* Instruction Fetch Error */
# define EXCCAUSE_IFETCHERROR		2	/* (backward compatibility macro, deprecated, avoid) */
#define EXCCAUSE_LOAD_STORE_ERROR	3	/* Load Store Error */
# define EXCCAUSE_LOADSTOREERROR	3	/* (backward compatibility macro, deprecated, avoid) */
#define EXCCAUSE_LEVEL1_INTERRUPT	4	/* Level 1 Interrupt */
# define EXCCAUSE_LEVEL1INTERRUPT	4	/* (backward compatibility macro, deprecated, avoid) */
#define EXCCAUSE_ALLOCA			5	/* Stack Extension Assist (MOVSP instruction) for alloca */
#define EXCCAUSE_DIVIDE_BY_ZERO		6	/* Integer Divide by Zero */
#define EXCCAUSE_SPECULATION		7	/* Use of Failed Speculative Access (not implemented) */
#define EXCCAUSE_PRIVILEGED		8	/* Privileged Instruction */
#define EXCCAUSE_UNALIGNED		9	/* Unaligned Load or Store */
/* Reserved				10..11 */
#define EXCCAUSE_INSTR_DATA_ERROR	12	/* PIF Data Error on Instruction Fetch (RB-200x and later) */
#define EXCCAUSE_LOAD_STORE_DATA_ERROR	13	/* PIF Data Error on Load or Store (RB-200x and later) */
#define EXCCAUSE_INSTR_ADDR_ERROR	14	/* PIF Address Error on Instruction Fetch (RB-200x and later) */
#define EXCCAUSE_LOAD_STORE_ADDR_ERROR	15	/* PIF Address Error on Load or Store (RB-200x and later) */
#define EXCCAUSE_ITLB_MISS		16	/* ITLB Miss (no ITLB entry matches, hw refill also missed) */
#define EXCCAUSE_ITLB_MULTIHIT		17	/* ITLB Multihit (multiple ITLB entries match) */
#define EXCCAUSE_INSTR_RING		18	/* Ring Privilege Violation on Instruction Fetch */
/* Reserved				19 */	/* Size Restriction on IFetch (not implemented) */
#define EXCCAUSE_INSTR_PROHIBITED	20	/* Cache Attribute does not allow Instruction Fetch */
/* Reserved				21..23 */
#define EXCCAUSE_DTLB_MISS		24	/* DTLB Miss (no DTLB entry matches, hw refill also missed) */
#define EXCCAUSE_DTLB_MULTIHIT		25	/* DTLB Multihit (multiple DTLB entries match) */
#define EXCCAUSE_LOAD_STORE_RING	26	/* Ring Privilege Violation on Load or Store */
/* Reserved				27 */	/* Size Restriction on Load/Store (not implemented) */
#define EXCCAUSE_LOAD_PROHIBITED	28	/* Cache Attribute does not allow Load */
#define EXCCAUSE_STORE_PROHIBITED	29	/* Cache Attribute does not allow Store */
/* Reserved				30..31 */
#define EXCCAUSE_CP_DISABLED(n)		(32+(n))	/* Access to Coprocessor 'n' when disabled */
#define EXCCAUSE_CP0_DISABLED		32	/* Access to Coprocessor 0 when disabled */
#define EXCCAUSE_CP1_DISABLED		33	/* Access to Coprocessor 1 when disabled */
#define EXCCAUSE_CP2_DISABLED		34	/* Access to Coprocessor 2 when disabled */
#define EXCCAUSE_CP3_DISABLED		35	/* Access to Coprocessor 3 when disabled */
#define EXCCAUSE_CP4_DISABLED		36	/* Access to Coprocessor 4 when disabled */
#define EXCCAUSE_CP5_DISABLED		37	/* Access to Coprocessor 5 when disabled */
#define EXCCAUSE_CP6_DISABLED		38	/* Access to Coprocessor 6 when disabled */
#define EXCCAUSE_CP7_DISABLED		39	/* Access to Coprocessor 7 when disabled */
/* Reserved				40..63 */

/*  PS register fields:  */
#define PS_WOE_SHIFT		18
#define PS_WOE_MASK		0x00040000
#define PS_WOE			PS_WOE_MASK
#define PS_CALLINC_SHIFT	16
#define PS_CALLINC_MASK		0x00030000
#define PS_CALLINC(n)		(((n)&3)<<PS_CALLINC_SHIFT)	/* n = 0..3 */
#define PS_OWB_SHIFT		8
#define PS_OWB_MASK		0x00000F00
#define PS_OWB(n)		(((n)&15)<<PS_OWB_SHIFT)	/* n = 0..15 (or 0..7) */
#define PS_RING_SHIFT		6
#define PS_RING_MASK		0x000000C0
#define PS_RING(n)		(((n)&3)<<PS_RING_SHIFT)	/* n = 0..3 */
#define PS_UM_SHIFT		5
#define PS_UM_MASK		0x00000020
#define PS_UM			PS_UM_MASK
#define PS_EXCM_SHIFT		4
#define PS_EXCM_MASK		0x00000010
#define PS_EXCM			PS_EXCM_MASK
#define PS_INTLEVEL_SHIFT	0
#define PS_INTLEVEL_MASK	0x0000000F
#define PS_INTLEVEL(n)		((n)&PS_INTLEVEL_MASK)		/* n = 0..15 */
/*  Backward compatibility (deprecated):  */
#define PS_PROGSTACK_SHIFT	PS_UM_SHIFT
#define PS_PROGSTACK_MASK	PS_UM_MASK
#define PS_PROG_SHIFT		PS_UM_SHIFT
#define PS_PROG_MASK		PS_UM_MASK
#define PS_PROG			PS_UM

/*  DBREAKCn register fields:  */
#define DBREAKC_MASK_SHIFT		0
#define DBREAKC_MASK_MASK		0x0000003F
#define DBREAKC_LOADBREAK_SHIFT		30
#define DBREAKC_LOADBREAK_MASK		0x40000000
#define DBREAKC_STOREBREAK_SHIFT	31
#define DBREAKC_STOREBREAK_MASK		0x80000000

/*  DEBUGCAUSE register fields:  */
#define DEBUGCAUSE_DEBUGINT_SHIFT	5
#define DEBUGCAUSE_DEBUGINT_MASK	0x20	/* debug interrupt */
#define DEBUGCAUSE_BREAKN_SHIFT		4
#define DEBUGCAUSE_BREAKN_MASK		0x10	/* BREAK.N instruction */
#define DEBUGCAUSE_BREAK_SHIFT		3
#define DEBUGCAUSE_BREAK_MASK		0x08	/* BREAK instruction */
#define DEBUGCAUSE_DBREAK_SHIFT		2
#define DEBUGCAUSE_DBREAK_MASK		0x04	/* DBREAK match */
#define DEBUGCAUSE_IBREAK_SHIFT		1
#define DEBUGCAUSE_IBREAK_MASK		0x02	/* IBREAK match */
#define DEBUGCAUSE_ICOUNT_SHIFT		0
#define DEBUGCAUSE_ICOUNT_MASK		0x01	/* ICOUNT would increment to zero */

/*  MESR register fields:  */
#define MESR_MEME		0x00000001	/* memory error */
#define MESR_MEME_SHIFT		0
#define MESR_DME		0x00000002	/* double memory error */
#define MESR_DME_SHIFT		1
#define MESR_RCE		0x00000010	/* recorded memory error */
#define MESR_RCE_SHIFT		4
#define MESR_LCE		
#define MESR_LCE_SHIFT		?
#define MESR_LCE_L
#define MESR_ERRENAB		0x00000100
#define MESR_ERRENAB_SHIFT	8
#define MESR_ERRTEST		0x00000200
#define MESR_ERRTEST_SHIFT	9
#define MESR_DATEXC		0x00000400
#define MESR_DATEXC_SHIFT	10
#define MESR_INSEXC		0x00000800
#define MESR_INSEXC_SHIFT	11
#define MESR_WAYNUM_SHIFT	16
#define MESR_ACCTYPE_SHIFT	20
#define MESR_MEMTYPE_SHIFT	24
#define MESR_ERRTYPE_SHIFT	30

/*  MEMCTL register fields:  */
#define MEMCTL_SNOOP_EN_SHIFT	1
#define MEMCTL_SNOOP_EN		0x02	/* enable snoop responses (default 0) */
#define MEMCTL_L0IBUF_EN_SHIFT	0
#define MEMCTL_L0IBUF_EN	0x01	/* enable loop instr. buffer (default 1) */
#define MEMCTL_INV_EN_SHIFT	23
#define MEMCTL_INV_EN		0x00800000	/* invalidate cache ways being increased */
#define MEMCTL_DCWU_SHIFT	8
#define MEMCTL_DCWU_BITS	5
#define MEMCTL_DCWA_SHIFT	13
#define MEMCTL_DCWA_BITS	5
#define MEMCTL_ICWU_SHIFT	18
#define MEMCTL_ICWU_BITS	5
#define MEMCTL_DCWU_MASK	0x00001F00	/* Bits  8-12 dcache ways in use */
#define MEMCTL_DCWA_MASK	0x0003E000	/* Bits 13-17 dcache ways allocatable */
#define MEMCTL_ICWU_MASK	0x007C0000	/* Bits 18-22 icache ways in use */
#define MEMCTL_DCWU_CLR_MASK	~(MEMCTL_DCWU_MASK)
#define MEMCTL_DCWA_CLR_MASK	~(MEMCTL_DCWA_MASK)
#define MEMCTL_ICWU_CLR_MASK	~(MEMCTL_ICWU_MASK)
#define MEMCTL_DCW_CLR_MASK	(MEMCTL_DCWU_CLR_MASK | MEMCTL_DCWA_CLR_MASK)
#define MEMCTL_IDCW_CLR_MASK	(MEMCTL_DCW_CLR_MASK | MEMCTL_ICWU_CLR_MASK)


#endif /*XTENSA_COREBITS_H*/

