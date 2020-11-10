/*
 * tie-asm.h -- compile-time HAL assembler definitions dependent on CORE & TIE
 *
 *  NOTE:  This header file is not meant to be included directly.
 */

/* This header file contains assembly-language definitions (assembly
   macros, etc.) for this specific Xtensa processor's TIE extensions
   and options.  It is customized to this Xtensa processor configuration.

   Customer ID=11657; Build=0x5fe96; Copyright (c) 1999-2016 Cadence Design Systems Inc.

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

#ifndef _XTENSA_CORE_TIE_ASM_H
#define _XTENSA_CORE_TIE_ASM_H

/*  Selection parameter values for save-area save/restore macros:  */
/*  Option vs. TIE:  */
#define XTHAL_SAS_TIE	0x0001	/* custom extension or coprocessor */
#define XTHAL_SAS_OPT	0x0002	/* optional (and not a coprocessor) */
#define XTHAL_SAS_ANYOT	0x0003	/* both of the above */
/*  Whether used automatically by compiler:  */
#define XTHAL_SAS_NOCC	0x0004	/* not used by compiler w/o special opts/code */
#define XTHAL_SAS_CC	0x0008	/* used by compiler without special opts/code */
#define XTHAL_SAS_ANYCC	0x000C	/* both of the above */
/*  ABI handling across function calls:  */
#define XTHAL_SAS_CALR	0x0010	/* caller-saved */
#define XTHAL_SAS_CALE	0x0020	/* callee-saved */
#define XTHAL_SAS_GLOB	0x0040	/* global across function calls (in thread) */
#define XTHAL_SAS_ANYABI	0x0070	/* all of the above three */
/*  Misc  */
#define XTHAL_SAS_ALL	0xFFFF	/* include all default NCP contents */
#define XTHAL_SAS3(optie,ccuse,abi)	( ((optie) & XTHAL_SAS_ANYOT)  \
					| ((ccuse) & XTHAL_SAS_ANYCC)  \
					| ((abi)   & XTHAL_SAS_ANYABI) )


    /*
      *  Macro to store all non-coprocessor (extra) custom TIE and optional state
      *  (not including zero-overhead loop registers).
      *  Required parameters:
      *      ptr         Save area pointer address register (clobbered)
      *                  (register must contain a 4 byte aligned address).
      *      at1..at4    Four temporary address registers (first XCHAL_NCP_NUM_ATMPS
      *                  registers are clobbered, the remaining are unused).
      *  Optional parameters:
      *      continue    If macro invoked as part of a larger store sequence, set to 1
      *                  if this is not the first in the sequence.  Defaults to 0.
      *      ofs         Offset from start of larger sequence (from value of first ptr
      *                  in sequence) at which to store.  Defaults to next available space
      *                  (or 0 if <continue> is 0).
      *      select      Select what category(ies) of registers to store, as a bitmask
      *                  (see XTHAL_SAS_xxx constants).  Defaults to all registers.
      *      alloc       Select what category(ies) of registers to allocate; if any
      *                  category is selected here that is not in <select>, space for
      *                  the corresponding registers is skipped without doing any store.
      */
    .macro xchal_ncp_store  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start	\continue, \ofs
	// Optional global registers used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	rur.THREADPTR	\at1		// threadptr option
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	// Optional caller-saved registers used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1012, 4, 4
	rsr.ACCLO	\at1		// MAC16 option
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	rsr.ACCHI	\at1		// MAC16 option
	s32i	\at1, \ptr, .Lxchal_ofs_+4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1012, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
	// Optional caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 996, 4, 4
	rsr.BR	\at1		// boolean option
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	rsr.SCOMPARE1	\at1		// conditional store option
	s32i	\at1, \ptr, .Lxchal_ofs_+4
	rsr.M0	\at1		// MAC16 option
	s32i	\at1, \ptr, .Lxchal_ofs_+8
	rsr.M1	\at1		// MAC16 option
	s32i	\at1, \ptr, .Lxchal_ofs_+12
	rsr.M2	\at1		// MAC16 option
	s32i	\at1, \ptr, .Lxchal_ofs_+16
	rsr.M3	\at1		// MAC16 option
	s32i	\at1, \ptr, .Lxchal_ofs_+20
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 24
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 996, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 24
	.endif
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1008, 4, 4
	rur.F64R_LO	\at1		// ureg 234
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	rur.F64R_HI	\at1		// ureg 235
	s32i	\at1, \ptr, .Lxchal_ofs_+4
	rur.F64S	\at1		// ureg 236
	s32i	\at1, \ptr, .Lxchal_ofs_+8
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 12
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1008, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 12
	.endif
    .endm	// xchal_ncp_store

    /*
      *  Macro to load all non-coprocessor (extra) custom TIE and optional state
      *  (not including zero-overhead loop registers).
      *  Required parameters:
      *      ptr         Save area pointer address register (clobbered)
      *                  (register must contain a 4 byte aligned address).
      *      at1..at4    Four temporary address registers (first XCHAL_NCP_NUM_ATMPS
      *                  registers are clobbered, the remaining are unused).
      *  Optional parameters:
      *      continue    If macro invoked as part of a larger load sequence, set to 1
      *                  if this is not the first in the sequence.  Defaults to 0.
      *      ofs         Offset from start of larger sequence (from value of first ptr
      *                  in sequence) at which to load.  Defaults to next available space
      *                  (or 0 if <continue> is 0).
      *      select      Select what category(ies) of registers to load, as a bitmask
      *                  (see XTHAL_SAS_xxx constants).  Defaults to all registers.
      *      alloc       Select what category(ies) of registers to allocate; if any
      *                  category is selected here that is not in <select>, space for
      *                  the corresponding registers is skipped without doing any load.
      */
    .macro xchal_ncp_load  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start	\continue, \ofs
	// Optional global registers used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wur.THREADPTR	\at1		// threadptr option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	// Optional caller-saved registers used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1012, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wsr.ACCLO	\at1		// MAC16 option
	l32i	\at1, \ptr, .Lxchal_ofs_+4
	wsr.ACCHI	\at1		// MAC16 option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1012, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
	// Optional caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 996, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wsr.BR	\at1		// boolean option
	l32i	\at1, \ptr, .Lxchal_ofs_+4
	wsr.SCOMPARE1	\at1		// conditional store option
	l32i	\at1, \ptr, .Lxchal_ofs_+8
	wsr.M0	\at1		// MAC16 option
	l32i	\at1, \ptr, .Lxchal_ofs_+12
	wsr.M1	\at1		// MAC16 option
	l32i	\at1, \ptr, .Lxchal_ofs_+16
	wsr.M2	\at1		// MAC16 option
	l32i	\at1, \ptr, .Lxchal_ofs_+20
	wsr.M3	\at1		// MAC16 option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 24
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 996, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 24
	.endif
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1008, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wur.F64R_LO	\at1		// ureg 234
	l32i	\at1, \ptr, .Lxchal_ofs_+4
	wur.F64R_HI	\at1		// ureg 235
	l32i	\at1, \ptr, .Lxchal_ofs_+8
	wur.F64S	\at1		// ureg 236
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 12
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1008, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 12
	.endif
    .endm	// xchal_ncp_load


#define XCHAL_NCP_NUM_ATMPS	1

    /*
     *  Macro to store the state of TIE coprocessor FPU.
     *  Required parameters:
     *      ptr         Save area pointer address register (clobbered)
     *                  (register must contain a 4 byte aligned address).
     *      at1..at4    Four temporary address registers (first XCHAL_CP0_NUM_ATMPS
     *                  registers are clobbered, the remaining are unused).
     *  Optional parameters are the same as for xchal_ncp_store.
     */
#define xchal_cp_FPU_store	xchal_cp0_store
    .macro	xchal_cp0_store  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start \continue, \ofs
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 948, 4, 4
	rur.FCR	\at1		// ureg 232
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	rur.FSR	\at1		// ureg 233
	s32i	\at1, \ptr, .Lxchal_ofs_+4
	ssi	f0, \ptr, .Lxchal_ofs_+8
	ssi	f1, \ptr, .Lxchal_ofs_+12
	ssi	f2, \ptr, .Lxchal_ofs_+16
	ssi	f3, \ptr, .Lxchal_ofs_+20
	ssi	f4, \ptr, .Lxchal_ofs_+24
	ssi	f5, \ptr, .Lxchal_ofs_+28
	ssi	f6, \ptr, .Lxchal_ofs_+32
	ssi	f7, \ptr, .Lxchal_ofs_+36
	ssi	f8, \ptr, .Lxchal_ofs_+40
	ssi	f9, \ptr, .Lxchal_ofs_+44
	ssi	f10, \ptr, .Lxchal_ofs_+48
	ssi	f11, \ptr, .Lxchal_ofs_+52
	ssi	f12, \ptr, .Lxchal_ofs_+56
	ssi	f13, \ptr, .Lxchal_ofs_+60
	ssi	f14, \ptr, .Lxchal_ofs_+64
	ssi	f15, \ptr, .Lxchal_ofs_+68
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 72
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 948, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 72
	.endif
    .endm	// xchal_cp0_store

    /*
     *  Macro to load the state of TIE coprocessor FPU.
     *  Required parameters:
     *      ptr         Save area pointer address register (clobbered)
     *                  (register must contain a 4 byte aligned address).
     *      at1..at4    Four temporary address registers (first XCHAL_CP0_NUM_ATMPS
     *                  registers are clobbered, the remaining are unused).
     *  Optional parameters are the same as for xchal_ncp_load.
     */
#define xchal_cp_FPU_load	xchal_cp0_load
    .macro	xchal_cp0_load  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start \continue, \ofs
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 948, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wur.FCR	\at1		// ureg 232
	l32i	\at1, \ptr, .Lxchal_ofs_+4
	wur.FSR	\at1		// ureg 233
	lsi	f0, \ptr, .Lxchal_ofs_+8
	lsi	f1, \ptr, .Lxchal_ofs_+12
	lsi	f2, \ptr, .Lxchal_ofs_+16
	lsi	f3, \ptr, .Lxchal_ofs_+20
	lsi	f4, \ptr, .Lxchal_ofs_+24
	lsi	f5, \ptr, .Lxchal_ofs_+28
	lsi	f6, \ptr, .Lxchal_ofs_+32
	lsi	f7, \ptr, .Lxchal_ofs_+36
	lsi	f8, \ptr, .Lxchal_ofs_+40
	lsi	f9, \ptr, .Lxchal_ofs_+44
	lsi	f10, \ptr, .Lxchal_ofs_+48
	lsi	f11, \ptr, .Lxchal_ofs_+52
	lsi	f12, \ptr, .Lxchal_ofs_+56
	lsi	f13, \ptr, .Lxchal_ofs_+60
	lsi	f14, \ptr, .Lxchal_ofs_+64
	lsi	f15, \ptr, .Lxchal_ofs_+68
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 72
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 948, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 72
	.endif
    .endm	// xchal_cp0_load

#define XCHAL_CP0_NUM_ATMPS	1
#define XCHAL_SA_NUM_ATMPS	1

	/*  Empty macros for unconfigured coprocessors:  */
	.macro xchal_cp1_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp1_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp2_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp2_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp3_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp3_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp4_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp4_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp5_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp5_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp6_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp6_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp7_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp7_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm

#endif /*_XTENSA_CORE_TIE_ASM_H*/
