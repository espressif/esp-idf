// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "regi2c_ctrl.h"
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

uint8_t IRAM_ATTR regi2c_ctrl_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    portENTER_CRITICAL_SAFE(&mux);
    uint8_t value = i2c_read_reg_raw(block, host_id, reg_add);
    portEXIT_CRITICAL_SAFE(&mux);
    return value;
}

uint8_t IRAM_ATTR regi2c_ctrl_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    portENTER_CRITICAL_SAFE(&mux);
    uint8_t value = i2c_read_reg_mask_raw(block, host_id, reg_add, msb, lsb);
    portEXIT_CRITICAL_SAFE(&mux);
    return value;
}

void IRAM_ATTR regi2c_ctrl_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    portENTER_CRITICAL_SAFE(&mux);
    i2c_write_reg_raw(block, host_id, reg_add, data);
    portEXIT_CRITICAL_SAFE(&mux);
}

void IRAM_ATTR regi2c_ctrl_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    portENTER_CRITICAL_SAFE(&mux);
    i2c_write_reg_mask_raw(block, host_id, reg_add, msb, lsb, data);
    portEXIT_CRITICAL_SAFE(&mux);
}

void IRAM_ATTR regi2c_enter_critical(void)
{
    portENTER_CRITICAL_SAFE(&mux);
}

void IRAM_ATTR regi2c_exit_critical(void)
{
    portEXIT_CRITICAL_SAFE(&mux);
}
