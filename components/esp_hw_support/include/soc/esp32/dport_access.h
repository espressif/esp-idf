/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <sdkconfig.h>
#include "xtensa/xtruntime.h"

#ifdef __cplusplus
extern "C" {
#endif

void esp_dport_access_stall_other_cpu_start(void) __attribute__ ((deprecated));
void esp_dport_access_stall_other_cpu_end(void) __attribute__ ((deprecated));
void esp_dport_access_int_init(void) __attribute__ ((deprecated));
void esp_dport_access_int_pause(void) __attribute__ ((deprecated));
void esp_dport_access_int_resume(void) __attribute__ ((deprecated));
void esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words);
uint32_t esp_dport_access_reg_read(uint32_t reg);
uint32_t esp_dport_access_sequence_reg_read(uint32_t reg);
//This routine does not stop the dport routines in any way that is recoverable. Please
//only call in case of panic().
void esp_dport_access_int_abort(void) __attribute__ ((deprecated));

#if defined(BOOTLOADER_BUILD) || !defined(CONFIG_ESP32_DPORT_WORKAROUND) || !defined(ESP_PLATFORM)
#define DPORT_STALL_OTHER_CPU_START()
#define DPORT_STALL_OTHER_CPU_END()
#define DPORT_INTERRUPT_DISABLE()
#define DPORT_INTERRUPT_RESTORE()
#else
#include "esp_ipc_isr.h"
#define DPORT_STALL_OTHER_CPU_START()   esp_ipc_isr_stall_other_cpu()
#define DPORT_STALL_OTHER_CPU_END()     esp_ipc_isr_release_other_cpu()
#define DPORT_INTERRUPT_DISABLE()       unsigned int intLvl = XTOS_SET_INTLEVEL(CONFIG_ESP32_DPORT_DIS_INTERRUPT_LVL)
#define DPORT_INTERRUPT_RESTORE()       XTOS_RESTORE_JUST_INTLEVEL(intLvl)
#endif

#ifdef __cplusplus
}
#endif
