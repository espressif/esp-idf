/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "freertos/xtensa_context.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

/* High-priority interrupt - IPC_ISR handler */

#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
#define LX_INTR_STACK_SIZE  16
#define LX_INTR_A0_OFFSET   0
#define LX_INTR_A2_OFFSET   4
#define LX_INTR_A3_OFFSET   8
#define LX_INTR_A4_OFFSET   12
#define EXCSAVE_X           EXCSAVE_5
#define RFI_X               5

#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4

#define LX_INTR_STACK_SIZE  16
#define LX_INTR_A0_OFFSET   0
#define LX_INTR_A2_OFFSET   4
#define LX_INTR_A3_OFFSET   8
#define LX_INTR_A4_OFFSET   12
#define EXCSAVE_X           EXCSAVE_4
#define RFI_X               4

#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */

    .data
_lx_intr_stack:
    .space      LX_INTR_STACK_SIZE
    .section .iram1,"ax"
    .global     esp_ipc_isr_handler
    .type       esp_ipc_isr_handler,@function
    .align      4
esp_ipc_isr_handler:
    /* Allocate exception frame and save minimal context. */
    /* Because the interrupt cause code has protection that only
       allows one cpu to enter in the IPC_ISR section of the LX
       interrupt at one time, there's no need to have two
       _lx_intr_stack for each cpu */

    /* Save A0, A2, A3, A4 so we can use those registers further*/
    movi    a0, _lx_intr_stack
    s32i    a2, a0, LX_INTR_A2_OFFSET
    s32i    a3, a0, LX_INTR_A3_OFFSET
    s32i    a4, a0, LX_INTR_A4_OFFSET
    rsr     a2, EXCSAVE_X
    s32i    a2, a0, LX_INTR_A0_OFFSET

    /* disable nested iterrupts */
    /* PS.EXCM is changed from 1 to 0 . It allows using usually exception handler instead of the Double exception handler. */
    /* PS_UM = 1 */
    movi    a0, PS_INTLEVEL(5) | PS_UM
    wsr     a0, PS
    rsync
    /* restore PS will be done by rfi the end */

    /*
    * Reset isr interrupt flags
    */
#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
    /* This int is level-triggered and doesn't need clearing.
       Do nothing here and clear int status by peripheral register later.*/
#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4
    /* This int is edge-triggered and needs clearing. */
    movi    a3, (1 << ETS_IPC_ISR_INUM)
    wsr     a3, INTCLEAR
#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */

    /* get CORE_ID */
    getcoreid   a3
    beqz    a3, 1f

    /* current cpu is 1 */
    movi    a3, SYSTEM_CPU_INTR_FROM_CPU_3_REG
    movi    a4, 0
    s32i    a4, a3, 0   /* clear intr */
    j       2f
1:
    /* current cpu is 0 */
    movi    a3, SYSTEM_CPU_INTR_FROM_CPU_2_REG
    movi    a4, 0
    s32i    a4, a3, 0   /* clear intr */
2:

    /* set the start flag */
    movi    a0, esp_ipc_isr_start_fl
    memw
    s32i    a0, a0, 0

    /* Call the esp_ipc_function(void* arg) */
    movi    a0, esp_ipc_func
    l32i    a0, a0, 0
    movi    a2, esp_ipc_func_arg
    l32i    a2, a2, 0
    callx0  a0

    /* Done. Restore registers and return. */
    movi    a0, _lx_intr_stack
    l32i    a2, a0, LX_INTR_A2_OFFSET
    l32i    a3, a0, LX_INTR_A3_OFFSET
    l32i    a4, a0, LX_INTR_A4_OFFSET

    /* set the end flag */
    movi    a0, esp_ipc_isr_end_fl
    memw
    s32i    a0, a0, 0

    /* restore a0 */
    rsr     a0, EXCSAVE_X
    /* restores PS from EPS[X] and jumps to the address in EPC[X] */
    rfi     RFI_X
