/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sdkconfig.h>
#include "soc/dport_access.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BOOTLOADER_BUILD) || defined(CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) || !SOC_DPORT_WORKAROUND
#define DPORT_STALL_OTHER_CPU_START()
#define DPORT_STALL_OTHER_CPU_END()
#else
#include "esp_ipc_isr.h"
#define DPORT_STALL_OTHER_CPU_START()   esp_ipc_isr_stall_other_cpu()
#define DPORT_STALL_OTHER_CPU_END()     esp_ipc_isr_release_other_cpu()
#endif

#ifdef __cplusplus
}
#endif
