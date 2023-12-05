/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "xtensa_context.h"
#include "sdkconfig.h"
#include "soc/soc.h"

#if CONFIG_BTDM_CTRL_HLI

/* Interrupt stack size, for C code.
 * TODO: reduce and make configurable.
 */
#define L4_INTR_STACK_SIZE  4096

/* Save area for the CPU state:
 * - 64 words for the general purpose registers
 * - 7 words for some of the special registers:
 *   - WINDOWBASE, WINDOWSTART — only WINDOWSTART is truly needed
 *   - SAR, LBEG, LEND, LCOUNT — since the C code might use these
 *   - EPC1 — since the C code might cause window overflow exceptions
 * This is not laid out as standard exception frame structure
 * for simplicity of the save/restore code.
 */
#define REG_FILE_SIZE         (64 * 4)
#define SPECREG_OFFSET        REG_FILE_SIZE
#define SPECREG_SIZE          (7 * 4)
#define REG_SAVE_AREA_SIZE    (SPECREG_OFFSET + SPECREG_SIZE)

    .data
_l4_intr_stack:
    .space      L4_INTR_STACK_SIZE
_l4_save_ctx:
    .space      REG_SAVE_AREA_SIZE

    .section .iram1,"ax"
    .global     xt_highint4
    .type       xt_highint4,@function
    .align      4

xt_highint4:

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    /*
    Here, Timer2 is used to count a little time(50us).
    The subsequent dram0 write operation is blocked due to live lock, which will
    cause timer2 to timeout and trigger a level 5 interrupt.
    */
    rsr.ccount  a0
    addmi   a0, a0, (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ*50)
    wsr     a0, CCOMPARE2

    /* Enable Timer 2 interrupt */
    rsr     a0, INTENABLE
    extui   a0, a0, 16, 1
    bnez    a0, 1f
    movi    a0, 0
    xsr     a0, INTENABLE     /* disable all interrupts */
    /* And a0 with (1 << 16) for Timer 2 interrupt mask */
    addmi   a0, a0, (1<<14)
    addmi   a0, a0, (1<<14)
    addmi   a0, a0, (1<<14)
    addmi   a0, a0, (1<<14)
    wsr     a0, INTENABLE    /* Enable Timer 2 */
1:
#endif

    movi    a0, _l4_save_ctx
    /* save 4 lower registers */
    s32i    a1, a0, 4
    s32i    a2, a0, 8
    s32i    a3, a0, 12
    rsr     a2, EXCSAVE_4  /* holds the value of a0 */
    s32i    a2, a0, 0

    /* Save special registers */
    addi    a0, a0, SPECREG_OFFSET
    rsr     a2, WINDOWBASE
    s32i    a2, a0, 0
    rsr     a2, WINDOWSTART
    s32i    a2, a0, 4
    rsr     a2, SAR
    s32i    a2, a0, 8
    rsr     a2, LBEG
    s32i    a2, a0, 12
    rsr     a2, LEND
    s32i    a2, a0, 16
    rsr     a2, LCOUNT
    s32i    a2, a0, 20
    rsr     a2, EPC1
    s32i    a2, a0, 24

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    movi    a0, 0
    xsr     a0, INTENABLE     /* disable all interrupts */
    movi    a2, ~(1<<16)
    and     a0, a2, a0
    wsr     a0, INTENABLE
#endif

    /* disable exception mode, window overflow */
    movi    a0, PS_INTLEVEL(5) | PS_EXCM
    wsr     a0, PS
    rsync

    /* Save the remaining physical registers.
     * 4 registers are already saved, which leaves 60 registers to save.
     * (FIXME: consider the case when the CPU is configured with physical 32 registers)
     * These 60 registers are saved in 5 iterations, 12 registers at a time.
     */
    movi    a1, 5
    movi    a3, _l4_save_ctx + 4 * 4

    /* This is repeated 5 times, each time the window is shifted by 12 registers.
     * We come here with a1 = downcounter, a3 = save pointer, a2 and a0 unused.
     */
1:
    s32i    a4, a3, 0
    s32i    a5, a3, 4
    s32i    a6, a3, 8
    s32i    a7, a3, 12
    s32i    a8, a3, 16
    s32i    a9, a3, 20
    s32i    a10, a3, 24
    s32i    a11, a3, 28
    s32i    a12, a3, 32
    s32i    a13, a3, 36
    s32i    a14, a3, 40
    s32i    a15, a3, 44

    /* We are about to rotate the window, so that a12-a15 will become the new a0-a3.
     * Copy a0-a3 to a12-15 to still have access to these values.
     * At the same time we can decrement the counter and adjust the save area pointer
     */

    /* a0 is constant (_l4_save_ctx), no need to copy */
    addi    a13, a1, -1  /* copy and decrement the downcounter */
    /* a2 is scratch so no need to copy */
    addi    a15, a3, 48  /* copy and adjust the save area pointer */
    beqz    a13, 2f      /* have saved all registers ? */
    rotw    3            /* rotate the window and go back */
    j       1b

    /* the loop is complete */
2:
    rotw 4      /* this brings us back to the original window */
    /* a0 still points to _l4_save_ctx */

    /* Can clear WINDOWSTART now, all registers are saved */
    rsr     a2, WINDOWBASE
    /* WINDOWSTART = (1 << WINDOWBASE) */
    movi    a3, 1
    ssl     a2
    sll     a3, a3
    wsr     a3, WINDOWSTART

_highint4_stack_switch:
    movi    a0, 0
    movi    sp, _l4_intr_stack + L4_INTR_STACK_SIZE - 16
    s32e    a0, sp, -12         /* For GDB: set null SP */
    s32e    a0, sp, -16         /* For GDB: set null PC */
    movi    a0, _highint4_stack_switch     /* For GDB: cosmetics, for the frame where stack switch happened */

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a6, PS_INTLEVEL(4) | PS_UM | PS_WOE
    wsr     a6, PS
    rsync

    /* Call C handler */
    mov     a6, sp
    call4   hli_c_handler

    l32e    sp, sp, -12                     /* switch back to the original stack */

    /* Done with C handler; re-enable exception mode, disabling window overflow */
    movi    a2, PS_INTLEVEL(5) | PS_EXCM    /* TOCHECK */
    wsr     a2, PS
    rsync

    /* Restore the special registers.
     * WINDOWSTART will be restored near the end.
     */
    movi    a0, _l4_save_ctx + SPECREG_OFFSET
    l32i    a2, a0, 8
    wsr     a2, SAR
    l32i    a2, a0, 12
    wsr     a2, LBEG
    l32i    a2, a0, 16
    wsr     a2, LEND
    l32i    a2, a0, 20
    wsr     a2, LCOUNT
    l32i    a2, a0, 24
    wsr     a2, EPC1

    /* Restoring the physical registers.
     * This is the reverse to the saving process above.
     */

    /* Rotate back to the final window, then start loading 12 registers at a time,
     * in 5 iterations.
     * Again, a1 is the downcounter and a3 is the save area pointer.
     * After each rotation, a1 and a3 are copied from a13 and a15.
     * To simplify the loop, we put the initial values into a13 and a15.
     */
    rotw     -4
    movi    a15, _l4_save_ctx + 64 * 4  /* point to the end of the save area */
    movi    a13, 5

1:
    /* Copy a1 and a3 from their previous location,
     * at the same time decrementing and adjusting the save area pointer.
     */
    addi    a1, a13, -1
    addi    a3, a15, -48

    /* Load 12 registers */
    l32i    a4, a3, 0
    l32i    a5, a3, 4
    l32i    a6, a3, 8
    l32i    a7, a3, 12
    l32i    a8, a3, 16
    l32i    a9, a3, 20
    l32i    a10, a3, 24
    l32i    a11, a3, 28                                /* ensure PS and EPC written */
    l32i    a12, a3, 32
    l32i    a13, a3, 36
    l32i    a14, a3, 40
    l32i    a15, a3, 44

    /* Done with the loop? */
    beqz    a1, 2f
    /* If no, rotate the window and repeat */
    rotw    -3
    j       1b

2:
    /* Done with the loop. Only 4 registers (a0-a3 in the original window) remain
     * to be restored. Also need to restore WINDOWSTART, since all the general
     * registers are now in place.
     */
    movi    a0, _l4_save_ctx

    l32i    a2, a0, SPECREG_OFFSET + 4
    wsr     a2, WINDOWSTART

    l32i    a1, a0, 4
    l32i    a2, a0, 8
    l32i    a3, a0, 12
    rsr     a0, EXCSAVE_4  /* holds the value of a0 before the interrupt handler */

    /* Return from the interrupt, restoring PS from EPS_4 */
    rfi     4

#endif /* CONFIG_BTDM_CTRL_HLI */

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_hli_vectors_bt
ld_include_hli_vectors_bt:
