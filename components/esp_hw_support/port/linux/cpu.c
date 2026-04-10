/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Stub implementations of non-inline esp_cpu_* functions for Linux/macOS
 * host builds. These provide no-op or minimal behaviour since hardware
 * CPU control, interrupt descriptors, breakpoints, watchpoints, and memory
 * region protection don't exist on the host.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "esp_cpu.h"
#include "esp_err.h"

/* --------------------------------------------------- CPU Control ----------------------------------------------------- */

void esp_cpu_stall(int core_id)
{
    (void)core_id;
}

void esp_cpu_unstall(int core_id)
{
    (void)core_id;
}

void esp_cpu_reset(int core_id)
{
    (void)core_id;
}

void esp_cpu_wait_for_intr(void)
{
    /* No WFI equivalent on host; just return */
}

/* ------------------------------------------------- CPU Interrupts ---------------------------------------------------- */

void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    (void)core_id;
    (void)intr_num;
    if (intr_desc_ret) {
        intr_desc_ret->priority = -1;
        intr_desc_ret->type = ESP_CPU_INTR_TYPE_NA;
        intr_desc_ret->flags = 0;
    }
}

/* -------------------------------------------------- Memory Ports ----------------------------------------------------- */

void esp_cpu_configure_region_protection(void)
{
    /* No memory protection unit on host */
}

/* ---------------------------------------------------- Debugging ------------------------------------------------------ */

esp_err_t esp_cpu_set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_cpu_watchpoint_trigger_t trigger)
{
    (void)wp_num;
    (void)wp_addr;
    (void)size;
    (void)trigger;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_cpu_clear_watchpoint(int wp_num)
{
    (void)wp_num;
    return ESP_ERR_NOT_SUPPORTED;
}

/* ------------------------------------------------------ Misc --------------------------------------------------------- */

bool esp_cpu_compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value)
{
    return atomic_compare_exchange_strong((_Atomic uint32_t *)addr, &compare_value, new_value);
}
