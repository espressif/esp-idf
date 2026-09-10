/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_phy_init.h"

void app_main(void)
{
    /* Whether the bin comes from the phy partition or from the application binary is
       only observable in the log emitted while it is loaded, so trigger the load once
       here to keep that visible in the boot output. */
    const esp_phy_init_data_t *init_data = esp_phy_get_init_data();
    if (init_data != NULL) {
        esp_phy_release_init_data(init_data);
    }

    unity_run_menu();
}
