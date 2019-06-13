// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "temp_sensor.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S2BETA

#define TEMP_SENSOR_XPD_WAIT_DEFAULT 0xFF   /* Set wait cycle time(8MHz) from power up to reset enable. */

portMUX_TYPE rtc_temp_spinlock = portMUX_INITIALIZER_UNLOCKED;
static SemaphoreHandle_t rtc_touch_mux = NULL;

esp_err_t temp_sensor_set_config(temp_sensor_t temps)
{
    portENTER_CRITICAL(&rtc_temp_spinlock);
    SENS.sar_tctrl.tsens_dac = temps.dac_offset;
    SENS.sar_tctrl.tsens_clk_div = temps.clk_div;
    SENS.sar_tctrl.tsens_power_up_force = 1;
    SENS.sar_tctrl2.tsens_xpd_wait = TEMP_SENSOR_XPD_WAIT_DEFAULT;
    SENS.sar_tctrl2.tsens_xpd_force = 1;
    SENS.sar_tctrl2.tsens_reset = 1;// Reset the temp sensor.
    SENS.sar_tctrl2.tsens_reset = 0;// Clear the reset status.
    portEXIT_CRITICAL(&rtc_temp_spinlock);
    return ESP_OK;
}

esp_err_t temp_sensor_get_config(temp_sensor_t *temps)
{
    if(temps) {
        portENTER_CRITICAL(&rtc_temp_spinlock);
        temps->dac_offset = SENS.sar_tctrl.tsens_dac;
        temps->clk_div = SENS.sar_tctrl.tsens_clk_div;
        portEXIT_CRITICAL(&rtc_temp_spinlock);
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t temp_sensor_start(void)
{
    portENTER_CRITICAL(&rtc_temp_spinlock);
    SENS.sar_tctrl.tsens_dump_out = 0;
    SENS.sar_tctrl2.tsens_clkgate_en = 1;
    SENS.sar_tctrl.tsens_power_up = 1;
    portEXIT_CRITICAL(&rtc_temp_spinlock);
    return ESP_OK;
}

esp_err_t temp_sensor_stop(void)
{
    portENTER_CRITICAL(&rtc_temp_spinlock);
    SENS.sar_tctrl.tsens_power_up = 0;
    SENS.sar_tctrl2.tsens_clkgate_en = 0;
    portEXIT_CRITICAL(&rtc_temp_spinlock);
    return ESP_OK;
}

esp_err_t temp_sensor_read(uint8_t *temp_out)
{
    if(temp_out) {
        portENTER_CRITICAL(&rtc_temp_spinlock);
        SENS.sar_tctrl.tsens_dump_out = 1;
        while(!SENS.sar_tctrl.tsens_ready);
        *temp_out = SENS.sar_tctrl.tsens_out;
        SENS.sar_tctrl.tsens_dump_out = 0;
        portEXIT_CRITICAL(&rtc_temp_spinlock);
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}
#endif