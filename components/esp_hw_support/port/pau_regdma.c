/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_pau.h"
#include "esp_private/periph_ctrl.h"
#if SOC_PAU_IN_TOP_DOMAIN
#include "hal/lp_sys_ll.h"
#endif

#define PAU_REGDMA_LINK_LOOP                (0x3FF)
#define PAU_REGDMA_REG_ACCESS_TIME          (0x3FF)
#define PAU_REGDMA_LINK_WAIT_RETRY_COUNT    (1000)
#define PAU_REGDMA_LINK_WAIT_READ_INTERNAL  (32)

static __attribute__((unused)) const char *TAG = "pau_regdma";

#if !SOC_RCC_IS_INDEPENDENT
// Reset and Clock Control registers are mixing with other peripherals, so we need to use a critical section
#define PAU_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define PAU_RCC_ATOMIC()
#endif

typedef struct {
    pau_hal_context_t *hal;
} pau_context_t;

pau_context_t * __attribute__((weak)) IRAM_ATTR PAU_instance(void)
{
    static pau_hal_context_t pau_hal = { .dev = NULL };
    static pau_context_t pau_context = { .hal = &pau_hal };

    /* periph_module_enable don not need to be put in iram because it is
     * called before the flash is powered off and will not be called again. */

    if (pau_hal.dev == NULL) {
        pau_hal.dev = &PAU;
        PAU_RCC_ATOMIC() {
            pau_hal_enable_bus_clock(true);
        }
        pau_hal_set_regdma_wait_timeout(&pau_hal, PAU_REGDMA_LINK_WAIT_RETRY_COUNT, PAU_REGDMA_LINK_WAIT_READ_INTERNAL);
        pau_hal_set_regdma_work_timeout(&pau_hal, PAU_REGDMA_LINK_LOOP, PAU_REGDMA_REG_ACCESS_TIME);
#if SOC_PM_PAU_REGDMA_LINK_CONFIGURABLE
        pau_hal_regdma_link_count_config(&pau_hal, SOC_PM_PAU_LINK_NUM);
#endif
#if SOC_PAU_IN_TOP_DOMAIN
        pau_hal_lp_sys_initialize();
#endif
    }

    return &pau_context;
}

void pau_regdma_set_entry_link_addr(pau_regdma_link_addr_t *link_entries)
{
    ESP_LOGD(TAG, "All link addresses %p,%p,%p,%p", (*link_entries)[0], (*link_entries)[1], (*link_entries)[2], (*link_entries)[3]);
    pau_hal_set_regdma_entry_link_addr(PAU_instance()->hal, link_entries);
}

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
#if SOC_PM_PAU_REGDMA_LINK_WIFIMAC
void pau_regdma_set_modem_link_addr(void *link_addr)
{
    pau_hal_set_regdma_modem_link_addr(PAU_instance()->hal, link_addr);
}
#endif

void IRAM_ATTR pau_regdma_trigger_modem_link_backup(void)
{
    pau_hal_start_regdma_modem_link(PAU_instance()->hal, true);
    pau_hal_stop_regdma_modem_link(PAU_instance()->hal);
}

void IRAM_ATTR pau_regdma_trigger_modem_link_restore(void)
{
    pau_hal_start_regdma_modem_link(PAU_instance()->hal, false);
    pau_hal_stop_regdma_modem_link(PAU_instance()->hal);
}
#endif

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
void IRAM_ATTR pau_regdma_set_system_link_addr(void *link_addr)
{
    /* ESP32H2 use software to trigger REGDMA to restore instead of PMU,
     * because regdma has power bug, so we need to manually set the clock
     * for regdma before using it after the chip wakes up. We use
     * pau_hal_clock_configure because periph_module_enable will consume
     * a relatively large amount of memory space. */

    pau_hal_regdma_clock_configure(PAU_instance()->hal, true);
#if SOC_PM_PAU_REGDMA_LINK_MULTI_ADDR
    pau_hal_set_regdma_system_link_addr(PAU_instance()->hal, link_addr);
#endif
}

void IRAM_ATTR pau_regdma_trigger_system_link_backup(void)
{
    pau_hal_start_regdma_system_link(PAU_instance()->hal, true);
    pau_hal_stop_regdma_system_link(PAU_instance()->hal);
}

void IRAM_ATTR pau_regdma_trigger_system_link_restore(void)
{
    pau_hal_start_regdma_system_link(PAU_instance()->hal, false);
    pau_hal_stop_regdma_system_link(PAU_instance()->hal);
}
#endif

void IRAM_ATTR pau_regdma_set_extra_link_addr(void *link_addr)
{
#if SOC_PM_PAU_REGDMA_LINK_MULTI_ADDR
    pau_hal_set_regdma_extra_link_addr(PAU_instance()->hal, link_addr);
#endif
}

void IRAM_ATTR pau_regdma_trigger_extra_link_backup(void)
{
    pau_hal_start_regdma_extra_link(PAU_instance()->hal, true);
    pau_hal_stop_regdma_extra_link(PAU_instance()->hal);
}

void IRAM_ATTR pau_regdma_trigger_extra_link_restore(void)
{
    pau_hal_start_regdma_extra_link(PAU_instance()->hal, false);
    pau_hal_stop_regdma_extra_link(PAU_instance()->hal);
}

#if SOC_PAU_IN_TOP_DOMAIN
void pau_regdma_enable_aon_link_entry(bool enable)
{
    lp_sys_ll_set_pau_aon_bypass(enable);
}
#endif
