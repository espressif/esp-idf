// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
 * @file regi2c_bias.h
 * @brief Register definitions for bias
 *
 * This file lists register fields of BIAS. These definitions are used via macros defined in regi2c_ctrl.h, by
 * bootloader_hardware_init function in bootloader_esp32c3.c.
 */

#define I2C_BIAS            0X6A
#define I2C_BIAS_HOSTID     0

#define I2C_BIAS_DREG_1P1_PVT 1
#define I2C_BIAS_DREG_1P1_PVT_MSB 3
#define I2C_BIAS_DREG_1P1_PVT_LSB 0
