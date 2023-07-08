/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/pcr_reg.h"
#include "esp_private/esp_pau.h"
#include "esp_private/periph_ctrl.h"

static __attribute__((unused)) const char *TAG = "pau_regdma";

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
        periph_module_enable(PERIPH_REGDMA_MODULE);
    }

    return &pau_context;
}

void pau_regdma_set_entry_link_addr(pau_regdma_link_addr_t *link_entries)
{
    ESP_LOGD(TAG, "All link addresses %p,%p,%p,%p", (*link_entries)[0], (*link_entries)[1], (*link_entries)[2], (*link_entries)[3]);
    pau_hal_set_regdma_entry_link_addr(PAU_instance()->hal, link_entries);
}

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
void pau_regdma_set_modem_link_addr(void *link_addr)
{
    pau_hal_set_regdma_modem_link_addr(PAU_instance()->hal, link_addr);
}

void pau_regdma_trigger_modem_link_backup(void)
{
    pau_hal_start_regdma_modem_link(PAU_instance()->hal, true);
    pau_hal_stop_regdma_modem_link(PAU_instance()->hal);
}

void pau_regdma_trigger_modem_link_restore(void)
{
    pau_hal_start_regdma_modem_link(PAU_instance()->hal, false);
    pau_hal_stop_regdma_modem_link(PAU_instance()->hal);
}
#endif

#if SOC_PM_RETENTION_HAS_REGDMA_POWER_BUG
void IRAM_ATTR pau_regdma_set_system_link_addr(void *link_addr)
{
    /* ESP32H2 use software to trigger REGDMA to restore instead of PMU,
     * because regdma has power bug, so we need to manually set the clock
     * for regdma before using it after the chip wakes up. We use
     * pau_hal_clock_configure because periph_module_enable will consume
     * a relatively large amount of memory space. */

    pau_hal_regdma_clock_configure(PAU_instance()->hal, true);
    pau_hal_set_regdma_system_link_addr(PAU_instance()->hal, link_addr);
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

void pau_regdma_set_extra_link_addr(void *link_addr)
{
    pau_hal_set_regdma_extra_link_addr(PAU_instance()->hal, link_addr);
}

void pau_regdma_trigger_extra_link_backup(void)
{
    pau_hal_start_regdma_extra_link(PAU_instance()->hal, true);
    pau_hal_stop_regdma_extra_link(PAU_instance()->hal);
}

void pau_regdma_trigger_extra_link_restore(void)
{
    pau_hal_start_regdma_extra_link(PAU_instance()->hal, false);
    pau_hal_stop_regdma_extra_link(PAU_instance()->hal);
}
