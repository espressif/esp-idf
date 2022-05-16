/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This module implements debug/trace stubs. The stub is a piece of special code which can invoked by OpenOCD
// Currently one stub is used for GCOV functionality
//

#include "esp_cpu.h"
#include "hal/cpu_hal.h"

#include "esp_log.h"
const static char *TAG = "esp_dbg_stubs";

#define RISCV_DBG_STUBS_SYSNR    0x65

/* Advertises apptrace control block address to host */
static int esp_dbg_stubs_advertise_table(void *stub_table_addr)
{
    if (!esp_cpu_in_ocd_debug_mode()) {
        return 0;
    }
    return cpu_hal_syscall(RISCV_DBG_STUBS_SYSNR, (int)stub_table_addr, 0, 0, 0, NULL);
}

void esp_dbg_stubs_ll_init(void *stub_table_addr)
{
    // notify host about control block address
    int res = esp_dbg_stubs_advertise_table(stub_table_addr);
    assert(res == 0 && "Falied to send debug stubs table address to host!");
    ESP_LOGV(TAG, "%s stubs %x", __func__, stub_table_addr);
}
