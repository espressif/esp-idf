// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

/**
 * @file regi2c_ulp.h
 * @brief Register definitions for analog to calibrate o_code for getting a more precise voltage.
 *
 * This file lists register fields of ULP, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h, by
 * rtc_init function in rtc_init.c.
 */

#define I2C_ULP 0x61
#define I2C_ULP_HOSTID 1

#define I2C_ULP_IR_RESETB 0
#define I2C_ULP_IR_RESETB_MSB 0
#define I2C_ULP_IR_RESETB_LSB 0

#define I2C_ULP_O_DONE_FLAG 3
#define I2C_ULP_O_DONE_FLAG_MSB 0
#define I2C_ULP_O_DONE_FLAG_LSB 0

#define I2C_ULP_BG_O_DONE_FLAG 3
#define I2C_ULP_BG_O_DONE_FLAG_MSB 3
#define I2C_ULP_BG_O_DONE_FLAG_LSB 3