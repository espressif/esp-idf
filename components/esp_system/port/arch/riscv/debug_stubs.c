/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This module implements debug/trace stubs. The stub is a piece of special code which can invoked by OpenOCD
// Currently one stub is used for GCOV functionality
//

#include "esp_cpu.h"
#include "esp_log.h"
#include "riscv/semihosting.h"

const static char *TAG = "esp_dbg_stubs";

/* Advertises apptrace control block address to host */
static int esp_dbg_stubs_advertise_table(void *stub_table_addr)
{
    if (!esp_cpu_dbgr_is_attached()) {
        return 0;
    }
    return (int) semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_DBG_STUBS_INIT, (long*)stub_table_addr);
}

void esp_dbg_stubs_ll_init(void *stub_table_addr)
{
    // notify host about control block address
    int res = esp_dbg_stubs_advertise_table(stub_table_addr);
    assert(res == 0 && "Failed to send debug stubs table address to host!");
    ESP_LOGV(TAG, "%s stubs %p", __func__, stub_table_addr);
}
