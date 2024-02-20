/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "xtensa_context.h"
#include "esp_private/panic_reason.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

#ifndef CONFIG_FREERTOS_UNICORE
#ifndef CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5

#define L5_INTR_STACK_SIZE  12
#define L5_INTR_A2_OFFSET   0
#define L5_INTR_A3_OFFSET   4
#define L5_INTR_A4_OFFSET   8
    .data
_l5_intr_stack:
    .space      L5_INTR_STACK_SIZE

    .section .iram1,"ax"
    .global     xt_highint5
    .type       xt_highint5,@function
    .align      4
xt_highint5:

    movi    a0, xt_highint5_read_apb
    l32i    a0, a0, 0
    bnez    a0, .read_apb_reg

// Short interrupt
    movi a0, 0
    wsr a0, CCOMPARE2
    esync

    rsr     a0, EXCSAVE_5 // restore a0
    rfi     5



// read APB reg 10 time.
.read_apb_reg:
    movi    a0, _l5_intr_stack
    s32i    a2, a0, L5_INTR_A2_OFFSET
    s32i    a3, a0, L5_INTR_A3_OFFSET
    s32i    a4, a0, L5_INTR_A4_OFFSET

    movi    a4, 10         // count of reading
    movi    a0, 0x3ff40078 // read APB reg
    l32i    a2, a0, 0
.loop_read_apb_reg:
    l32i    a3, a0, 0
    bne     a3, a2, .need_set_apb_test_result
    addi    a4, a4, -1
    l32i    a2, a0, 0
    bnez    a4, .loop_read_apb_reg
    j       1f
.need_set_apb_test_result:
    movi    a0, apb_intr_test_result // set fail
    movi    a2, 0
    s32i    a2, a0, 0
    memw
1:
    movi    a0, _l5_intr_stack
    l32i    a2, a0, L5_INTR_A2_OFFSET
    l32i    a3, a0, L5_INTR_A3_OFFSET
    l32i    a4, a0, L5_INTR_A4_OFFSET
    rsync
.L_xt_highint5_exit:
    rsr     a0, EXCSAVE_5                   // restore a0
    rfi     5

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_test_dport_xt_highint5
ld_include_test_dport_xt_highint5:

#endif // CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
#endif // CONFIG_FREERTOS_UNICORE

#endif // CONFIG_IDF_TARGET_ESP32
