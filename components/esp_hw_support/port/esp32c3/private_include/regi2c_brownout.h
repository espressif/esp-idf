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

#pragma once

/**
 * @file regi2c_brownout.h
 * @brief Register definitions for brownout detector
 *
 * This file lists register fields of the brownout detector, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h.
 */

#define I2C_BOD            0x61
#define I2C_BOD_HOSTID     0

#define I2C_BOD_THRESHOLD           0x5
#define I2C_BOD_THRESHOLD_MSB       2
#define I2C_BOD_THRESHOLD_LSB       0
