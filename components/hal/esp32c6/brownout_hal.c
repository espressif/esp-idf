
/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/brownout_hal.h"
#include "esp_attr.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_brownout.h"

void brownout_hal_config(const brownout_hal_config_t *cfg)
{
    // TODO: IDF-5711
}

void brownout_hal_intr_enable(bool enable)
{
    // TODO: IDF-5711
}

IRAM_ATTR void brownout_hal_intr_clear(void)
{
    // TODO: IDF-5711
}
