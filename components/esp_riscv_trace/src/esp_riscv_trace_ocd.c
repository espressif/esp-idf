/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"
#include "esp_dbg_stubs.h"
#include "esp_private/startup_internal.h"
#include "esp_riscv_trace_ocd.h"

esp_riscv_trace_ocd_t g_esp_riscv_trace_ocd = {
    .magic = ESP_RISCV_TRACE_OCD_MAGIC,
    .abi_version = ESP_RISCV_TRACE_OCD_ABI_VERSION,
    .hdr_size = ESP_RISCV_TRACE_OCD_HDR_SIZE,
    .core_size = ESP_RISCV_TRACE_OCD_CORE_SIZE,
    .core_count = SOC_CPU_CORES_NUM,
    .snapshot = &g_esp_riscv_trace_snapshot,
};

/* Referenced with -u so the linker keeps this object. */
void esp_riscv_trace_include_ocd(void)
{
}

ESP_SYSTEM_INIT_FN(esp_riscv_trace_ocd_init, SECONDARY, BIT(0), 139)
{
    return esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_RISCV_TRACE, (uint32_t)&g_esp_riscv_trace_ocd);
}
