/*
 * SPDX-FileCopyrightText: 2017, Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */

/* File adapted to use on IDF FreeRTOS component, extracted
 * originally from zephyr RTOS code base:
 * https://github.com/zephyrproject-rtos/zephyr/blob/dafd348/arch/xtensa/include/xtensa-asm2-s.h
 */

#ifndef __XT_ASM_UTILS_H
#define __XT_ASM_UTILS_H

/*
 * SPILL_ALL_WINDOWS
 *
 * Spills all windowed registers (i.e. registers not visible as
 * A0-A15) to their ABI-defined spill regions on the stack.
 *
 * Unlike the Xtensa HAL implementation, this code requires that the
 * EXCM and WOE bit be enabled in PS, and relies on repeated hardware
 * exception handling to do the register spills.  The trick is to do a
 * noop write to the high registers, which the hardware will trap
 * (into an overflow exception) in the case where those registers are
 * already used by an existing call frame.  Then it rotates the window
 * and repeats until all but the A0-A3 registers of the original frame
 * are guaranteed to be spilled, eventually rotating back around into
 * the original frame.  Advantages:
 *
 * - Vastly smaller code size
 *
 * - More easily maintained if changes are needed to window over/underflow
 *   exception handling.
 *
 * - Requires no scratch registers to do its work, so can be used safely in any
 *   context.
 *
 * - If the WOE bit is not enabled (for example, in code written for
 *   the CALL0 ABI), this becomes a silent noop and operates compatbily.
 *
 * - Hilariously it's ACTUALLY FASTER than the HAL routine.  And not
 *   just a little bit, it's MUCH faster.  With a mostly full register
 *   file on an LX6 core (ESP-32) I'm measuring 145 cycles to spill
 *   registers with this vs. 279 (!) to do it with
 *   xthal_spill_windows().
 */

.macro SPILL_ALL_WINDOWS
#if XCHAL_NUM_AREGS == 64
	and a12, a12, a12
	rotw 3
	and a12, a12, a12
	rotw 3
	and a12, a12, a12
	rotw 3
	and a12, a12, a12
	rotw 3
	and a12, a12, a12
	rotw 4
#elif XCHAL_NUM_AREGS == 32
	and a12, a12, a12
	rotw 3
	and a12, a12, a12
	rotw 3
	and a4, a4, a4
	rotw 2
#else
#error Unrecognized XCHAL_NUM_AREGS
#endif
.endm

/*
--------------------------------------------------------------------------------
  Macro spinlock_take

  This macro will repeatedley attempt to atomically set a spinlock variable
  using the s32c1i instruciton. A spinlock is considered free if its value is 0.

  Entry:
  - "reg_A/B" as scratch registers
  - "lock_var" spinlock variable's symbol
  - Interrupts must already be disabled by caller
  Exit:
  - Spinlock set to current core's ID (PRID)
  - "reg_A/B" clobbered
--------------------------------------------------------------------------------
*/

#if portNUM_PROCESSORS > 1

    .macro  spinlock_take       reg_A reg_B lock_var

    movi    \reg_A, \lock_var               /* reg_A = &lock_var */
.L_spinlock_loop:
    movi    \reg_B, 0                       /* Load spinlock free value (0) into SCOMPARE1 */
    wsr     \reg_B, SCOMPARE1
    rsync                                   /* Ensure that SCOMPARE1 is set before s32c1i executes */
    rsr     \reg_B, PRID                    /* Load the current core's ID into reg_B */
    s32c1i  \reg_B, \reg_A, 0               /* Attempt *lock_var = reg_B */
    bnez    \reg_B, .L_spinlock_loop        /* If the write was successful (i.e., lock was free), 0 will have been written back to reg_B */

    .endm

#endif /* portNUM_PROCESSORS > 1 */

/*
--------------------------------------------------------------------------------
  Macro spinlock_release

  This macro will release a spinlock variable previously taken by the
  spinlock_take macro.

  Entry:
  - "reg_A/B" as scratch registers
  - "lock_var" spinlock variable's symbol
  - Interrupts must already be disabled by caller
  Exit:
  - "reg_A/B" clobbered
--------------------------------------------------------------------------------
*/

#if portNUM_PROCESSORS > 1

    .macro spinlock_release     reg_A reg_B lock_var

    movi    \reg_A, \lock_var               /* reg_A = &lock_var */
    movi    \reg_B, 0
    s32i    \reg_B, \reg_A, 0               /* Release the spinlock (*reg_A = 0) */

    .endm

#endif /* portNUM_PROCESSORS > 1 */

#endif /* __XT_ASM_UTILS_H */
