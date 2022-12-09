/*
 * xtensa/coreasm.h -- assembler-specific definitions that depend on CORE configuration
 *
 *  Source for configuration-independent binaries (which link in a
 *  configuration-specific HAL library) must NEVER include this file.
 *  It is perfectly normal, however, for the HAL itself to include this file.
 *
 *  This file must NOT include xtensa/config/system.h.  Any assembler
 *  header file that depends on system information should likely go
 *  in a new systemasm.h (or sysasm.h) header file.
 *
 *  NOTE: macro beqi32 is NOT configuration-dependent, and is placed
 *        here until we have a proper configuration-independent header file.
 */

/* $Id: //depot/rel/Foxhill/dot.9/Xtensa/OS/include/xtensa/coreasm.h#1 $ */

/*
 * Copyright (c) 2000-2014 Tensilica Inc.
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

#ifndef XTENSA_COREASM_H
#define XTENSA_COREASM_H

/*
 *  Tell header files this is assembly source, so they can avoid non-assembler
 *  definitions (eg. C types etc):
 */
#ifndef _ASMLANGUAGE	/* conditionalize to avoid cpp warnings (3rd parties might use same macro) */
#define _ASMLANGUAGE
#endif

#include <xtensa/config/core.h>
#include <xtensa/config/specreg.h>
#include <xtensa/config/system.h>

/*
 *  Assembly-language specific definitions (assembly macros, etc.).
 */

/*----------------------------------------------------------------------
 *  find_ms_setbit
 *
 *  This macro finds the most significant bit that is set in <as>
 *  and return its index + <base> in <ad>, or <base> - 1 if <as> is zero.
 *  The index counts starting at zero for the lsbit, so the return
 *  value ranges from <base>-1 (no bit set) to <base>+31 (msbit set).
 *
 *  Parameters:
 *	<ad>	destination address register (any register)
 *	<as>	source address register
 *	<at>	temporary address register (must be different than <as>)
 *	<base>	constant value added to result (usually 0 or 1)
 *  On entry:
 *	<ad> = undefined if different than <as>
 *	<as> = value whose most significant set bit is to be found
 *	<at> = undefined
 *	no other registers are used by this macro.
 *  On exit:
 *	<ad> = <base> + index of msbit set in original <as>,
 *	     = <base> - 1 if original <as> was zero.
 *	<as> clobbered (if not <ad>)
 *	<at> clobbered (if not <ad>)
 *  Example:
 *	find_ms_setbit a0, a4, a0, 0		-- return in a0 index of msbit set in a4
 */

	.macro	find_ms_setbit ad, as, at, base
#if XCHAL_HAVE_NSA
	movi	\at, 31+\base
	nsau	\as, \as	// get index of \as, numbered from msbit (32 if absent)
	sub	\ad, \at, \as	// get numbering from lsbit (0..31, -1 if absent)
#else /* XCHAL_HAVE_NSA */
	movi	\at, \base	// start with result of 0 (point to lsbit of 32)

	beqz	\as, 2f		// special case for zero argument: return -1
	bltui	\as, 0x10000, 1f	// is it one of the 16 lsbits? (if so, check lower 16 bits)
	addi	\at, \at, 16	// no, increment result to upper 16 bits (of 32)
	//srli	\as, \as, 16	// check upper half (shift right 16 bits)
	extui	\as, \as, 16, 16	// check upper half (shift right 16 bits)
1:	bltui	\as, 0x100, 1f	// is it one of the 8 lsbits? (if so, check lower 8 bits)
	addi	\at, \at, 8	// no, increment result to upper 8 bits (of 16)
	srli	\as, \as, 8	// shift right to check upper 8 bits
1:	bltui	\as, 0x10, 1f	// is it one of the 4 lsbits? (if so, check lower 4 bits)
	addi	\at, \at, 4	// no, increment result to upper 4 bits (of 8)
	srli	\as, \as, 4	// shift right 4 bits to check upper half
1:	bltui	\as, 0x4, 1f	// is it one of the 2 lsbits? (if so, check lower 2 bits)
	addi	\at, \at, 2	// no, increment result to upper 2 bits (of 4)
	srli	\as, \as, 2	// shift right 2 bits to check upper half
1:	bltui	\as, 0x2, 1f	// is it the lsbit?
	addi	\at, \at, 2	// no, increment result to upper bit (of 2)
2:	addi	\at, \at, -1	// (from just above: add 1;  from beqz: return -1)
	//srli	\as, \as, 1
1:				// done! \at contains index of msbit set (or -1 if none set)
	.if	0x\ad - 0x\at	// destination different than \at ? (works because regs are a0-a15)
	mov	\ad, \at	// then move result to \ad
	.endif
#endif /* XCHAL_HAVE_NSA */
	.endm	// find_ms_setbit

/*----------------------------------------------------------------------
 *  find_ls_setbit
 *
 *  This macro finds the least significant bit that is set in <as>,
 *  and return its index in <ad>.
 *  Usage is the same as for the find_ms_setbit macro.
 *  Example:
 *	find_ls_setbit a0, a4, a0, 0	-- return in a0 index of lsbit set in a4
 */

	.macro	find_ls_setbit ad, as, at, base
	neg	\at, \as	// keep only the least-significant bit that is set...
	and	\as, \at, \as	// ... in \as
	find_ms_setbit	\ad, \as, \at, \base
	.endm	// find_ls_setbit

/*----------------------------------------------------------------------
 *  find_ls_one
 *
 *  Same as find_ls_setbit with base zero.
 *  Source (as) and destination (ad) registers must be different.
 *  Provided for backward compatibility.
 */

	.macro	find_ls_one ad, as
	find_ls_setbit	\ad, \as, \ad, 0
	.endm	// find_ls_one

/*----------------------------------------------------------------------
 *  floop, floopnez, floopgtz, floopend
 *
 *  These macros are used for fast inner loops that
 *  work whether or not the Loops options is configured.
 *  If the Loops option is configured, they simply use
 *  the zero-overhead LOOP instructions; otherwise
 *  they use explicit decrement and branch instructions.
 *
 *  They are used in pairs, with floop, floopnez or floopgtz
 *  at the beginning of the loop, and floopend at the end.
 *
 *  Each pair of loop macro calls must be given the loop count
 *  address register and a unique label for that loop.
 *
 *  Example:
 *
 *	movi	 a3, 16     // loop 16 times
 *	floop    a3, myloop1
 *	:
 *	bnez     a7, end1	// exit loop if a7 != 0
 *	:
 *	floopend a3, myloop1
 *  end1:
 *
 *  Like the LOOP instructions, these macros cannot be
 *  nested, must include at least one instruction,
 *  cannot call functions inside the loop, etc.
 *  The loop can be exited by jumping to the instruction
 *  following floopend (or elsewhere outside the loop),
 *  or continued by jumping to a NOP instruction placed
 *  immediately before floopend.
 *
 *  Unlike LOOP instructions, the register passed to floop*
 *  cannot be used inside the loop, because it is used as
 *  the loop counter if the Loops option is not configured.
 *  And its value is undefined after exiting the loop.
 *  And because the loop counter register is active inside
 *  the loop, you can't easily use this construct to loop
 *  across a register file using ROTW as you might with LOOP
 *  instructions, unless you copy the loop register along.
 */

	/*  Named label version of the macros:  */

	.macro	floop		ar, endlabel
	floop_		\ar, .Lfloopstart_\endlabel, .Lfloopend_\endlabel
	.endm

	.macro	floopnez	ar, endlabel
	floopnez_	\ar, .Lfloopstart_\endlabel, .Lfloopend_\endlabel
	.endm

	.macro	floopgtz	ar, endlabel
	floopgtz_	\ar, .Lfloopstart_\endlabel, .Lfloopend_\endlabel
	.endm

	.macro	floopend	ar, endlabel
	floopend_	\ar, .Lfloopstart_\endlabel, .Lfloopend_\endlabel
	.endm

	/*  Numbered local label version of the macros:  */
#if 0 /*UNTESTED*/
	.macro	floop89		ar
	floop_		\ar, 8, 9f
	.endm

	.macro	floopnez89	ar
	floopnez_	\ar, 8, 9f
	.endm

	.macro	floopgtz89	ar
	floopgtz_	\ar, 8, 9f
	.endm

	.macro	floopend89	ar
	floopend_	\ar, 8b, 9
	.endm
#endif /*0*/

	/*  Underlying version of the macros:  */

	.macro	floop_	ar, startlabel, endlabelref
	.ifdef	_infloop_
	.if	_infloop_
	.err	// Error: floop cannot be nested
	.endif
	.endif
	.set	_infloop_, 1
#if XCHAL_HAVE_LOOPS
	loop	\ar, \endlabelref
#else /* XCHAL_HAVE_LOOPS */
\startlabel:
	addi	\ar, \ar, -1
#endif /* XCHAL_HAVE_LOOPS */
	.endm	// floop_

	.macro	floopnez_	ar, startlabel, endlabelref
	.ifdef	_infloop_
	.if	_infloop_
	.err	// Error: floopnez cannot be nested
	.endif
	.endif
	.set	_infloop_, 1
#if XCHAL_HAVE_LOOPS
	loopnez	\ar, \endlabelref
#else /* XCHAL_HAVE_LOOPS */
	beqz	\ar, \endlabelref
\startlabel:
	addi	\ar, \ar, -1
#endif /* XCHAL_HAVE_LOOPS */
	.endm	// floopnez_

	.macro	floopgtz_	ar, startlabel, endlabelref
	.ifdef	_infloop_
	.if	_infloop_
	.err	// Error: floopgtz cannot be nested
	.endif
	.endif
	.set	_infloop_, 1
#if XCHAL_HAVE_LOOPS
	loopgtz	\ar, \endlabelref
#else /* XCHAL_HAVE_LOOPS */
	bltz	\ar, \endlabelref
	beqz	\ar, \endlabelref
\startlabel:
	addi	\ar, \ar, -1
#endif /* XCHAL_HAVE_LOOPS */
	.endm	// floopgtz_


	.macro	floopend_	ar, startlabelref, endlabel
	.ifndef	_infloop_
	.err	// Error: floopend without matching floopXXX
	.endif
	.ifeq	_infloop_
	.err	// Error: floopend without matching floopXXX
	.endif
	.set	_infloop_, 0
#if ! XCHAL_HAVE_LOOPS
	bnez	\ar, \startlabelref
#endif /* XCHAL_HAVE_LOOPS */
\endlabel:
	.endm	// floopend_

/*----------------------------------------------------------------------
 *  crsil  --  conditional RSIL (read/set interrupt level)
 *
 *  Executes the RSIL instruction if it exists, else just reads PS.
 *  The RSIL instruction does not exist in the new exception architecture
 *  if the interrupt option is not selected.
 */

	.macro	crsil	ar, newlevel
#if XCHAL_HAVE_OLD_EXC_ARCH || XCHAL_HAVE_INTERRUPTS
	rsil	\ar, \newlevel
#else
	rsr.ps	\ar
#endif
	.endm	// crsil

/*----------------------------------------------------------------------
 *  safe_movi_a0  --  move constant into a0 when L32R is not safe
 *
 *  This macro is typically used by interrupt/exception handlers.
 *  Loads a 32-bit constant in a0, without using any other register,
 *  and without corrupting the LITBASE register, even when the
 *  value of the LITBASE register is unknown (eg. when application
 *  code and interrupt/exception handling code are built independently,
 *  and thus with independent values of the LITBASE register;
 *  debug monitors are one example of this).
 *
 *  Worst-case size of resulting code:  17 bytes.
 */

	.macro	safe_movi_a0	constant
#if XCHAL_HAVE_ABSOLUTE_LITERALS
	/*  Contort a PC-relative literal load even though we may be in litbase-relative mode: */
	j	1f
	.begin	no-transform			// ensure what follows is assembled exactly as-is
	.align	4				// ensure constant and call0 target ...
	.byte	0				// ... are 4-byte aligned (call0 instruction is 3 bytes long)
1:	call0	2f				// read PC (that follows call0) in a0
	.long	\constant			// 32-bit constant to load into a0
2:
	.end	no-transform
	l32i	a0, a0, 0			// load constant
#else
	movi	a0, \constant			// no LITBASE, can assume PC-relative L32R
#endif
	.endm




/*----------------------------------------------------------------------
 *  window_spill{4,8,12}
 *
 *  These macros spill callers' register windows to the stack.
 *  They work for both privileged and non-privileged tasks.
 *  Must be called from a windowed ABI context, eg. within
 *  a windowed ABI function (ie. valid stack frame, window
 *  exceptions enabled, not in exception mode, etc).
 *
 *  This macro requires a single invocation of the window_spill_common
 *  macro in the same assembly unit and section.
 *
 *  Note that using window_spill{4,8,12} macros is more efficient
 *  than calling a function implemented using window_spill_function,
 *  because the latter needs extra code to figure out the size of
 *  the call to the spilling function.
 *
 *  Example usage:
 *
 *		.text
 *		.align	4
 *		.global	some_function
 *		.type	some_function,@function
 *	some_function:
 *		entry	a1, 16
 *		:
 *		:
 *
 *		window_spill4	// Spill windows of some_function's callers; preserves a0..a3 only;
 *				// to use window_spill{8,12} in this example function we'd have
 *				// to increase space allocated by the entry instruction, because
 *				// 16 bytes only allows call4; 32 or 48 bytes (+locals) are needed
 *				// for call8/window_spill8 or call12/window_spill12 respectively.
 *
 *		:
 *
 *		retw
 *
 *		window_spill_common	// instantiates code used by window_spill4
 *
 *
 *  On entry:
 *	none (if window_spill4)
 *	stack frame has enough space allocated for call8 (if window_spill8)
 *	stack frame has enough space allocated for call12 (if window_spill12)
 *  On exit:
 *	 a4..a15 clobbered (if window_spill4)
 *	 a8..a15 clobbered (if window_spill8)
 *	a12..a15 clobbered (if window_spill12)
 *	no caller windows are in live registers
 */

	.macro	window_spill4
#if XCHAL_HAVE_WINDOWED
# if XCHAL_NUM_AREGS == 16
	movi	a15, 0			// for 16-register files, no need to call to reach the end
# elif XCHAL_NUM_AREGS == 32
	call4	.L__wdwspill_assist28	// call deep enough to clear out any live callers
# elif XCHAL_NUM_AREGS == 64
	call4	.L__wdwspill_assist60	// call deep enough to clear out any live callers
# endif
#endif
	.endm	// window_spill4

	.macro	window_spill8
#if XCHAL_HAVE_WINDOWED
# if XCHAL_NUM_AREGS == 16
	movi	a15, 0			// for 16-register files, no need to call to reach the end
# elif XCHAL_NUM_AREGS == 32
	call8	.L__wdwspill_assist24	// call deep enough to clear out any live callers
# elif XCHAL_NUM_AREGS == 64
	call8	.L__wdwspill_assist56	// call deep enough to clear out any live callers
# endif
#endif
	.endm	// window_spill8

	.macro	window_spill12
#if XCHAL_HAVE_WINDOWED
# if XCHAL_NUM_AREGS == 16
	movi	a15, 0			// for 16-register files, no need to call to reach the end
# elif XCHAL_NUM_AREGS == 32
	call12	.L__wdwspill_assist20	// call deep enough to clear out any live callers
# elif XCHAL_NUM_AREGS == 64
	call12	.L__wdwspill_assist52	// call deep enough to clear out any live callers
# endif
#endif
	.endm	// window_spill12


/*----------------------------------------------------------------------
 *  window_spill_function
 *
 *  This macro outputs a function that will spill its caller's callers'
 *  register windows to the stack.  Eg. it could be used to implement
 *  a version of xthal_window_spill() that works in non-privileged tasks.
 *  This works for both privileged and non-privileged tasks.
 *
 *  Typical usage:
 *
 *		.text
 *		.align	4
 *		.global	my_spill_function
 *		.type	my_spill_function,@function
 *	my_spill_function:
 *		window_spill_function
 *
 *  On entry to resulting function:
 *	none
 *  On exit from resulting function:
 *	none (no caller windows are in live registers)
 */

	.macro	window_spill_function
#if XCHAL_HAVE_WINDOWED
#  if XCHAL_NUM_AREGS == 32
	entry	sp, 48
	bbci.l	a0, 31, 1f		// branch if called with call4
	bbsi.l	a0, 30, 2f		// branch if called with call12
	call8	.L__wdwspill_assist16	// called with call8, only need another 8
	retw
1:	call12	.L__wdwspill_assist16	// called with call4, only need another 12
	retw
2:	call4	.L__wdwspill_assist16	// called with call12, only need another 4
	retw
#  elif XCHAL_NUM_AREGS == 64
	entry	sp, 48
	bbci.l	a0, 31, 1f		// branch if called with call4
	bbsi.l	a0, 30, 2f		// branch if called with call12
	call4	.L__wdwspill_assist52	// called with call8, only need a call4
	retw
1:	call8	.L__wdwspill_assist52	// called with call4, only need a call8
	retw
2:	call12	.L__wdwspill_assist40	// called with call12, can skip a call12
	retw
#  elif XCHAL_NUM_AREGS == 16
	entry	sp, 16
	bbci.l	a0, 31, 1f	// branch if called with call4
	bbsi.l	a0, 30, 2f	// branch if called with call12
	movi	a7, 0		// called with call8
	retw
1:	movi	a11, 0		// called with call4
2:	retw			// if called with call12, everything already spilled

//	movi	a15, 0		// trick to spill all but the direct caller
//	j	1f
//	//  The entry instruction is magical in the assembler (gets auto-aligned)
//	//  so we have to jump to it to avoid falling through the padding.
//	//  We need entry/retw to know where to return.
//1:	entry	sp, 16
//	retw
#  else
#   error "unrecognized address register file size"
#  endif

#endif /* XCHAL_HAVE_WINDOWED */
	window_spill_common
	.endm	// window_spill_function

/*----------------------------------------------------------------------
 *  window_spill_common
 *
 *  Common code used by any number of invocations of the window_spill##
 *  and window_spill_function macros.
 *
 *  Must be instantiated exactly once within a given assembly unit,
 *  within call/j range of and same section as window_spill##
 *  macro invocations for that assembly unit.
 *  (Is automatically instantiated by the window_spill_function macro.)
 */

	.macro	window_spill_common
#if XCHAL_HAVE_WINDOWED && (XCHAL_NUM_AREGS == 32 || XCHAL_NUM_AREGS == 64)
	.ifndef	.L__wdwspill_defined
#  if XCHAL_NUM_AREGS >= 64
.L__wdwspill_assist60:
	entry	sp, 32
	call8	.L__wdwspill_assist52
	retw
.L__wdwspill_assist56:
	entry	sp, 16
	call4	.L__wdwspill_assist52
	retw
.L__wdwspill_assist52:
	entry	sp, 48
	call12	.L__wdwspill_assist40
	retw
.L__wdwspill_assist40:
	entry	sp, 48
	call12	.L__wdwspill_assist28
	retw
#  endif
.L__wdwspill_assist28:
	entry	sp, 48
	call12	.L__wdwspill_assist16
	retw
.L__wdwspill_assist24:
	entry	sp, 32
	call8	.L__wdwspill_assist16
	retw
.L__wdwspill_assist20:
	entry	sp, 16
	call4	.L__wdwspill_assist16
	retw
.L__wdwspill_assist16:
	entry	sp, 16
	movi	a15, 0
	retw
	.set	.L__wdwspill_defined, 1
	.endif
#endif /* XCHAL_HAVE_WINDOWED with 32 or 64 aregs */
	.endm	// window_spill_common

/*----------------------------------------------------------------------
 *  beqi32
 *
 *  macro implements version of beqi for arbitrary 32-bit immediate value
 *
 *     beqi32 ax, ay, imm32, label
 *
 *  Compares value in register ax with imm32 value and jumps to label if
 *  equal. Clobbers register ay if needed
 *
 */
   .macro beqi32	ax, ay, imm, label
    .ifeq ((\imm-1) & ~7)	// 1..8 ?
		beqi	\ax, \imm, \label
    .else
      .ifeq (\imm+1)		// -1 ?
		beqi	\ax, \imm, \label
      .else
        .ifeq (\imm)		// 0 ?
		beqz	\ax, \label
        .else
		//  We could also handle immediates 10,12,16,32,64,128,256
		//  but it would be a long macro...
		movi	\ay, \imm
		beq	\ax, \ay, \label
        .endif
      .endif
    .endif
   .endm // beqi32

/*----------------------------------------------------------------------
 *  isync_retw_nop
 *
 *  This macro must be invoked immediately after ISYNC if ISYNC
 *  would otherwise be immediately followed by RETW (or other instruction
 *  modifying WindowBase or WindowStart), in a context where
 *  kernel vector mode may be selected, and level-one interrupts
 *  and window overflows may be enabled, on an XEA1 configuration.
 *
 *  On hardware with erratum "XEA1KWIN" (see <xtensa/core.h> for details),
 *  XEA1 code must have at least one instruction between ISYNC and RETW if
 *  run in kernel vector mode with interrupts and window overflows enabled.
 */
	.macro	isync_retw_nop
#if XCHAL_MAYHAVE_ERRATUM_XEA1KWIN
	nop
#endif
	.endm

/*----------------------------------------------------------------------
 *  isync_return_nop
 *
 *  This macro should be used instead of isync_retw_nop in code that is
 *  intended to run on both the windowed and call0 ABIs
 */
        .macro  isync_return_nop
#ifdef __XTENSA_WINDOWED_ABI__
        isync_retw_nop
#endif
        .endm

/*----------------------------------------------------------------------
 *  isync_erratum453
 *
 *  This macro must be invoked at certain points in the code,
 *  such as in exception and interrupt vectors in particular,
 *  to work around erratum 453.
 */
	.macro	isync_erratum453
#if XCHAL_ERRATUM_453
	isync
#endif
	.endm


/*----------------------------------------------------------------------
 *  readsr
 *
 *  wrapper for 'rsr' that constructs register names that involve levels
 *  e.g. EPCn etc. Use like so:
 *      readsr epc XCHAL_DEBUGLEVEL a2
 */
	.macro	readsr  reg suf ar
	rsr.\reg\suf	\ar
	.endm

/*----------------------------------------------------------------------
 *  writesr
 *
 *  wrapper for 'wsr' that constructs register names that involve levels
 *  e.g. EPCn etc. Use like so:
 *      writesr epc XCHAL_DEBUGLEVEL a2
 */
	.macro	writesr  reg suf ar
	wsr.\reg\suf	\ar
	.endm

/*----------------------------------------------------------------------
 *  xchgsr
 *
 *  wrapper for 'xsr' that constructs register names that involve levels
 *  e.g. EPCn etc. Use like so:
 *      xchgsr epc XCHAL_DEBUGLEVEL a2
 */
	.macro	xchgsr  reg suf ar
	xsr.\reg\suf	\ar
	.endm

/*----------------------------------------------------------------------
 * INDEX_SR
 *
 * indexing wrapper for rsr/wsr/xsr that constructs register names from
 * the provided base name and the current index. Use like so:
 *     .set _idx, 0
 *     INDEX_SR rsr.ccompare a2
 *
 * this yields: rsr.ccompare0 a2
 */
	.macro	INDEX_SR  instr ar
.ifeq (_idx)
	&instr&0	\ar
.endif
.ifeq (_idx-1)
	&instr&1	\ar
.endif
.ifeq (_idx-2)
	&instr&2	\ar
.endif
.ifeq (_idx-3)
	&instr&3	\ar
.endif
.ifeq (_idx-4)
	&instr&4	\ar
.endif
.ifeq (_idx-5)
	&instr&5	\ar
.endif
.ifeq (_idx-6)
	&instr&6	\ar
.endif
.ifeq (_idx-7)
	&instr&7	\ar
.endif
	.endm


/*----------------------------------------------------------------------
 *  abs
 *
 *  implements abs on machines that do not have it configured
 */

#if !XCHAL_HAVE_ABS
	.macro abs arr, ars
	.ifc \arr, \ars
	//src equal dest is less efficient
	bgez \arr, 1f
	neg \arr, \arr
1:
	.else
	neg \arr, \ars
	movgez \arr, \ars, \ars
	.endif
	.endm
#endif /* !XCHAL_HAVE_ABS */


/*----------------------------------------------------------------------
 *  addx2
 *
 *  implements addx2 on machines that do not have it configured
 *
 */

#if !XCHAL_HAVE_ADDX
	.macro addx2 arr, ars, art
	.ifc \arr, \art
	.ifc \arr, \ars
	// addx2 a, a, a     (not common)
	.err
	.else
	add \arr, \ars, \art
	add \arr, \ars, \art
	.endif
	.else
	//addx2 a, b, c
	//addx2 a, a, b
	//addx2 a, b, b
	slli \arr, \ars, 1
	add  \arr, \arr, \art
	.endif
	.endm
#endif /* !XCHAL_HAVE_ADDX */

/*----------------------------------------------------------------------
 *  addx4
 *
 *  implements addx4 on machines that do not have it configured
 *
 */

#if !XCHAL_HAVE_ADDX
	.macro addx4 arr, ars, art
	.ifc \arr, \art
	.ifc \arr, \ars
	// addx4 a, a, a     (not common)
	 .err
	 .else
	//# addx4 a, b, a
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	.endif
	.else
	//addx4 a, b, c
	//addx4 a, a, b
	//addx4 a, b, b
	slli \arr, \ars, 2
	add  \arr, \arr, \art
	.endif
	.endm
#endif /* !XCHAL_HAVE_ADDX */

/*----------------------------------------------------------------------
 *  addx8
 *
 *  implements addx8 on machines that do not have it configured
 *
 */

#if !XCHAL_HAVE_ADDX
	.macro addx8 arr, ars, art
	.ifc \arr, \art
	.ifc \arr, \ars
	//addx8 a, a, a     (not common)
	.err
	.else
	//addx8 a, b, a
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	add \arr, \ars, \art
	.endif
	.else
	//addx8 a, b, c
	//addx8 a, a, b
	//addx8 a, b, b
	slli \arr, \ars, 3
	add  \arr, \arr, \art
	.endif
	.endm
#endif /* !XCHAL_HAVE_ADDX */


/*----------------------------------------------------------------------
 *  rfe_rfue
 *
 *  Maps to RFUE on XEA1, and RFE on XEA2.  No mapping on XEAX.
 */

#if XCHAL_HAVE_XEA1
	.macro	rfe_rfue
	rfue
	.endm
#elif XCHAL_HAVE_XEA2
	.macro	rfe_rfue
	rfe
	.endm
#endif


/*----------------------------------------------------------------------
 *  abi_entry
 *
 *  Generate proper function entry sequence for the current ABI
 *  (windowed or call0).  Takes care of allocating stack space (up to 1kB)
 *  and saving the return PC, if necessary.  The corresponding abi_return
 *  macro does the corresponding stack deallocation and restoring return PC.
 *
 *  Parameters are:
 *
 *	locsize		Number of bytes to allocate on the stack
 *			for local variables (and for args to pass to
 *			callees, if any calls are made).  Defaults to zero.
 *			The macro rounds this up to a multiple of 16.
 *			NOTE:  large values are allowed (e.g. up to 1 GB).
 *
 *	callsize	Maximum call size made by this function.
 *			Leave zero (default) for leaf functions, i.e. if
 *			this function makes no calls to other functions.
 *			Otherwise must be set to 4, 8, or 12 according
 *			to whether the "largest" call made is a call[x]4,
 *			call[x]8, or call[x]12 (for call0 ABI, it makes
 *			no difference whether this is set to 4, 8 or 12,
 *			but it must be set to one of these values).
 *
 *  NOTE:  It is up to the caller to align the entry point, declare the
 *  function symbol, make it global, etc.
 *
 *  NOTE:  This macro relies on assembler relaxation for large values
 *  of locsize.  It might not work with the no-transform directive.
 *  NOTE:  For the call0 ABI, this macro ensures SP is allocated or
 *  de-allocated cleanly, i.e. without temporarily allocating too much
 *  (or allocating negatively!) due to addi relaxation.
 *
 *  NOTE:  Generating the proper sequence and register allocation for
 *  making calls in an ABI independent manner is a separate topic not
 *  covered by this macro.
 *
 *  NOTE:  To access arguments, you can't use a fixed offset from SP.
 *  The offset depends on the ABI, whether the function is leaf, etc.
 *  The simplest method is probably to use the .locsz symbol, which
 *  is set by this macro to the actual number of bytes allocated on
 *  the stack, in other words, to the offset from SP to the arguments.
 *  E.g. for a function whose arguments are all 32-bit integers, you
 *  can get the 7th and 8th arguments (1st and 2nd args stored on stack)
 *  using:
 *	l32i	a2, sp, .locsz
 *	l32i	a3, sp, .locsz+4
 *  (this example works as long as locsize is under L32I's offset limit
 *   of 1020 minus up to 48 bytes of ABI-specific stack usage;
 *   otherwise you might first need to do "addi a?, sp, .locsz"
 *   or similar sequence).
 *
 *  NOTE:  For call0 ABI, this macro (and abi_return) may clobber a9
 *  (a caller-saved register).
 *
 *  Examples:
 *		abi_entry
 *		abi_entry  5
 *		abi_entry  22, 8
 *		abi_entry  0, 4
 */

	/*
	 *  Compute .locsz and .callsz without emitting any instructions.
	 *  Used by both abi_entry and abi_return.
	 *  Assumes locsize >= 0.
	 */
	.macro	abi_entry_size locsize=0, callsize=0
#if XCHAL_HAVE_WINDOWED && !__XTENSA_CALL0_ABI__
	.ifeq	\callsize
	 .set	.callsz, 16
	.else
	 .ifeq	\callsize-4
	  .set	.callsz, 16
	 .else
	  .ifeq	\callsize-8
	   .set	.callsz, 32
	  .else
	   .ifeq \callsize-12
	    .set .callsz, 48
	   .else
	    .error	"abi_entry: invalid call size \callsize"
	   .endif
	  .endif
	 .endif
	.endif
	.set	.locsz, .callsz + ((\locsize + 15) & -16)
#else
	.set	.callsz, \callsize
	.if	.callsz		/* if calls, need space for return PC */
	 .set	.locsz, (\locsize + 4 + 15) & -16
	.else
	 .set	.locsz, (\locsize + 15) & -16
	.endif
#endif
	.endm

	.macro abi_entry locsize=0, callsize=0
	.iflt	\locsize
	 .error	"abi_entry: invalid negative size of locals (\locsize)"
	.endif
	abi_entry_size	\locsize, \callsize
#if XCHAL_HAVE_WINDOWED && !__XTENSA_CALL0_ABI__
# define ABI_ENTRY_MINSIZE	3	/* size of abi_entry (no arguments) instructions in bytes */
	.ifgt	.locsz - 32760	/* .locsz > 32760 (ENTRY's max range)? */
	/*  Funky computation to try to have assembler use addmi efficiently if possible:  */
	entry	sp, 0x7F00 + (.locsz & 0xF0)
	addi	a12, sp, - ((.locsz & -0x100) - 0x7F00)
	movsp	sp, a12
	.else
	entry	sp, .locsz
	.endif
#else
# define ABI_ENTRY_MINSIZE	0	/* size of abi_entry (no arguments) instructions in bytes */
	.if	.locsz
	 .ifle	.locsz - 128	/* if locsz <= 128 */
	addi	sp, sp, -.locsz
	  .if	.callsz
	s32i	a0, sp, .locsz - 4
	  .endif
	 .elseif  .callsz	/* locsz > 128, with calls: */
	movi	a9, .locsz - 16		/* note: a9 is caller-saved */
	addi	sp, sp, -16
	s32i	a0, sp, 12
	sub	sp, sp, a9
	 .else			/* locsz > 128, no calls: */
	movi	a9, .locsz
	sub	sp, sp, a9
	 .endif			/* end */
	.endif
#endif
	.endm



/*----------------------------------------------------------------------
 *  abi_return
 *
 *  Generate proper function exit sequence for the current ABI
 *  (windowed or call0).  Takes care of freeing stack space and
 *  restoring the return PC, if necessary.
 *  NOTE:  This macro MUST be invoked following a corresponding
 *  abi_entry macro invocation.  For call0 ABI in particular,
 *  all stack and PC restoration are done according to the last
 *  abi_entry macro invoked before this macro in the assembly file.
 *
 *  Normally this macro takes no arguments.  However to allow
 *  for placing abi_return *before* abi_entry (as must be done
 *  for some highly optimized assembly), it optionally takes
 *  exactly the same arguments as abi_entry.
 */

	.macro abi_return	locsize=-1, callsize=0
	.ifge	\locsize
	abi_entry_size	\locsize, \callsize
	.endif
#if XCHAL_HAVE_WINDOWED && !__XTENSA_CALL0_ABI__
	retw
#else
	.if	.locsz
	 .iflt	.locsz - 128	/* if locsz < 128 */
	  .if	.callsz
	l32i	a0, sp, .locsz - 4
	  .endif
	addi	sp, sp, .locsz
	 .elseif  .callsz	/* locsz >= 128, with calls: */
	addi	a9, sp, .locsz - 16
	l32i	a0, a9, 12
	addi	sp, a9, 16
	 .else			/* locsz >= 128, no calls: */
	movi	a9, .locsz
	add	sp, sp, a9
	 .endif			/* end */
	.endif
	ret
#endif
	.endm


/*
 * HW erratum fixes.
 */

	.macro hw_erratum_487_fix
#if defined XSHAL_ERRATUM_487_FIX
	isync
#endif
	.endm

/*
 * These macros are internal, subject to change, and should not be used in
 * any new code.
 */

#define _GBL(x)    .global x
#define _TYP(x)    .type x,@function
#define _ALN(x)    .align x
#define _SIZ(x)    .size x, . - x
#define _MKEND(x)  .purgem endfunc ; .macro endfunc ; _SIZ(x) ; .purgem endfunc ; .macro endfunc ; .endm ; .endm
#define _SYMT(x)   _GBL(x); _MKEND(x); _TYP(x); _ALN(4); x:
#define _SYM2(x)   _GBL(x); _TYP(x); x:
#define _SYM(x)   _GBL(x); _MKEND(x); _ALN(4); x:
.macro endfunc ; .endm

/*
 * the DECLFUNC() macro provides a mechanism for implementing both the
 * standard and _nw interface with a single copy of the code.
 *
 * For Call0 ABI there is one function definition which is labeled with
 * both the xthal_..._nw and xthal_... symbols.
 *
 * For windowed ABI, two compilations are involved (one with the __NW_FUNCTION__
 * symbol defined) resulting in two separate functions (the _nw one without
 * the window adjustments).
*/

#if defined(__NW_FUNCTION__)
# define DECLFUNC(x) _SYMT(x ## _nw)
#else
# if defined (__XTENSA_CALL0_ABI__)
#  define DECLFUNC(x)  _SYMT(x); _SYM2(x ## _nw)
# else
#  define DECLFUNC(x)  _SYMT(x)
# endif
#endif

#endif /*XTENSA_COREASM_H*/

