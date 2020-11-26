// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ESP32-C3 have 2 I2C.
#define SOC_I2C_NUM            (1)

#define SOC_I2C_FIFO_LEN       (32) /*!< I2C hardware FIFO depth */
#define I2C_INTR_MASK          (0x3ffff) /*!< I2C all interrupt bitmap */

//ESP32-C3 support hardware FSM reset
#define I2C_SUPPORT_HW_FSM_RST  (1)
//ESP32-C3 support hardware clear bus
#define I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_XTAL       (1)
#define SOC_I2C_SUPPORT_RTC        (1)

#ifdef __cplusplus
}
#endif
