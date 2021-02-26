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

#ifdef __cplusplus
extern "C" {
#endif

// ESP32-C3 has 1 GPIO peripheral
#define SOC_GPIO_PORT           (1)
#define SOC_GPIO_PIN_COUNT          (22)

// Target has no full RTC IO subsystem, so GPIO is 100% "independent" of RTC
// On ESP32-C3, Digital IOs have their own registers to control pullup/down/capability, independent with RTC registers.
#define GPIO_SUPPORTS_RTC_INDEPENDENT (1)
// Force hold is a new function of ESP32-C3
#define SOC_GPIO_SUPPORT_FORCE_HOLD      (1)
// GPIO0~5 on ESP32C3 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP      (1)

#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)
#define GPIO_MODE_DEF_OUTPUT          (BIT1)
#define GPIO_MODE_DEF_OD              (BIT2)

#define SOC_GPIO_VALID_GPIO_MASK        ((1U<<SOC_GPIO_PIN_COUNT) - 1)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK
#define SOC_GPIO_DEEP_SLEEP_WAKEUP_VALID_GPIO_MASK        (0ULL | BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5)

// Support to configure slept status
#define SOC_GPIO_SUPPORT_SLP_SWITCH  (1)

#ifdef __cplusplus
}
#endif
