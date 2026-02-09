/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "gdma_test_utils.h"
#include "esp_private/sleep_retention.h"
#include "hal/gdma_ll.h"

void test_gdma_trigger_retention_backup(gdma_channel_handle_t chan, ...)
{
#if SOC_PAU_SUPPORTED && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // trigger a software retention to test GDMA retention correctnesss
    // 1. backup gdma register context
    sleep_retention_do_extra_retention(true);

    // 2. reset gdma registers to default value
    gdma_channel_handle_t chan_itor = chan;
    va_list args;
    int group_id = -1;
    va_start(args, chan);
    while (chan_itor) {
        gdma_get_group_channel_id(chan_itor, &group_id, NULL);
        _gdma_ll_reset_register(group_id);
        chan_itor = va_arg(args, gdma_channel_handle_t);
    }
    va_end(args);

    // 3. restore gdma register context
    sleep_retention_do_extra_retention(false);
#endif
    vTaskDelay(pdMS_TO_TICKS(10));
}
