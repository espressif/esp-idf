/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_CPU_HAS_HWLOOP

    .text
    .align      4

/**
 * @brief Perform a hardware loop with a given number of iterations
 *
 * @param a0 Number of iterations
 */
    .global use_hwlp
    .type use_hwlp, @function
    /* norvc keeps every loop instr 32-bit; .balign 4 (kept outside norvc) 4-byte
     * aligns the loop setup and last instruction per the hardware-loop placement
     * constraints listed on use_hwlp_broken below. */
    .balign 4
    .option push
    .option norvc
use_hwlp:
    li   a1, 0
    mv   t0, a0
    esp.lp.setup 0, t0, end
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
    addi a1, a1, 1
end:
    addi a1, a1, 1
    mv   a0, a1
    ret
    .size  use_hwlp, .-use_hwlp
    .option pop

/**
 * @brief Runs a hardware loop and pends a from-CPU software interrupt from
 *        INSIDE the loop body at the two critical boundaries on every iteration:
 *        right after esp.lp.setup (loop entry) and just before the last loop
 *        instruction (loop exit).
 *
 *        The store only requests the interrupt; the ISR is not expected to fire
 *        on the next cycle. Delivery takes a few cycles, so the handler typically
 *        preempts a few instructions later (the exit-boundary trigger often lands
 *        near the start of the next iteration). Dual triggers still produce
 *        reproducible preemption around the loop entry/exit windows and catch
 *        the HWLOOP state erratum 100% of the time.
 *
 * @param a0 Number of iterations (>= 2)
 * @param a1 Address of the from-CPU trigger register
 * @param a2 Value to write to the trigger register
 * @return   a0 = count * 16 (must stay correct if the context is preserved)
 */
    .global use_hwlp_irq
    .type use_hwlp_irq, @function
    /* norvc + 4-byte aligned loop setup/last instruction so preemption at the
     * boundaries is safe (see use_hwlp above). */
    .balign 4
    .option push
    .option norvc
use_hwlp_irq:
    li    t1, 0                     /* result accumulator */
    mv    t0, a0                    /* loop count */
    esp.lp.setup 0, t0, .Lirq_end
    sw    a2, 0(a1)                 /* pend IRQ #1 at loop-entry boundary (ISR a few cycles later) */
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    addi  t1, t1, 1
    sw    a2, 0(a1)                 /* pend IRQ #2 at loop-exit boundary (ISR a few cycles later) */
.Lirq_end:
    addi  t1, t1, 1
    mv    a0, t1
    ret
    .size use_hwlp_irq, .-use_hwlp_irq
    .option pop

/**
 * @brief Hardware loop that DELIBERATELY violates the esp.lp.setup
 *        placement constraints, used only by a manual [ignore] test.
 *
 *        The full set of constraints for the hwloop is:
 *          1. the loop start (esp.lp.setup / first instruction) is 4-byte aligned;
 *          2. the last executed loop instruction is 4-byte aligned and NOT a
 *             compressed (16-bit) instruction;
 *          3. the loop body has at least six 32-bit instructions or twelve
 *             16-bit ones; for a mix, the body must exceed 6*4 = 24 bytes;
 *          4. the last loop instruction is not a coprocessor (FPU/PIE/AI) instr.
 *
 *        This routine VIOLATES (1) and (2): a c.nop before esp.lp.setup shifts
 *        the whole loop by 2 bytes, so both the first and the last loop
 *        instruction are NOT 4-byte aligned. Constraints (3) and (4) are kept
 *        satisfied (>6 plain 32-bit instructions, no coprocessor instruction),
 *        to isolate misalignment as the sole cause. The loop also pends a
 *        from-CPU interrupt at its entry and exit boundaries; the ISR arrives a
 *        few cycles later (see use_hwlp_irq).
 *
 * @param a0 Number of iterations (>= 2)
 * @param a1 Address of the from-CPU trigger register
 * @param a2 Value to write to the trigger register
 * @return   a0 = count * 9
 */
    .balign 4
    .option push
    .option norvc
    .option norelax
    .global use_hwlp_broken
    .type use_hwlp_broken, @function
use_hwlp_broken:
    li     t1, 0                    /* result accumulator */
    mv     t0, a0                   /* loop count */
    .short 0x0001                   /* c.nop: 2-byte shift -> loop start (and all body) NOT 4-aligned */
    esp.lp.setup 0, t0, .Lbroken_end
    sw     a2, 0(a1)                /* pend IRQ at loop entry (ISR a few cycles later) */
    addi   t1, t1, 1
    addi   t1, t1, 1
    addi   t1, t1, 1
    addi   t1, t1, 1
    addi   t1, t1, 1
    addi   t1, t1, 1
    addi   t1, t1, 1
    addi   t1, t1, 1
    sw     a2, 0(a1)                /* pend IRQ at loop-exit boundary (ISR a few cycles later) */
.Lbroken_end:
    addi   t1, t1, 1               /* last loop instruction, deliberately NOT 4-byte aligned */
    mv     a0, t1
    ret
    .size use_hwlp_broken, .-use_hwlp_broken
    .option pop

#endif /* SOC_CPU_HAS_HWLOOP */
