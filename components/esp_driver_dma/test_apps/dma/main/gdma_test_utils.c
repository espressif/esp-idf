/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "gdma_test_utils.h"
#include "esp_private/sleep_retention.h"
#include "hal/gdma_ll.h"

void test_gdma_trigger_retention_backup(size_t chan_num, ...)
{
#if SOC_PAU_SUPPORTED && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // trigger a software retention to test GDMA retention correctnesss
    // 1. backup gdma register context
    sleep_retention_do_extra_retention(true);

    // 2. reset gdma registers to default value
    va_list args;
    int group_id = -1;
    va_start(args, chan_num);
    for (size_t i = 0; i < chan_num; i++) {
        gdma_channel_handle_t chan = va_arg(args, gdma_channel_handle_t);
        gdma_get_group_channel_id(chan, &group_id, NULL);
        _gdma_ll_reset_register(group_id);
    }
    va_end(args);

    // 3. restore gdma register context
    sleep_retention_do_extra_retention(false);
#endif
    vTaskDelay(pdMS_TO_TICKS(10));
}
