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

#define SOC_GPIO_PORT           (1)
#define SOC_GPIO_PIN_COUNT          (22)

// Target has no full RTC IO subsystem, so GPIO is 100% "independent" of RTC
#define GPIO_SUPPORTS_RTC_INDEPENDENT (1)

#define GPIO_SUPPORTS_FORCE_HOLD      (1)

#define GPIO_PRO_CPU_INTR_ENA      (BIT(0))
#define GPIO_PRO_CPU_NMI_INTR_ENA  (BIT(1))

#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)
#define GPIO_MODE_DEF_OUTPUT          (BIT1)
#define GPIO_MODE_DEF_OD              (BIT2)

// TODO ESP32-C3 IDF-2119 - check if any IOs are not full featured GPIO
#define SOC_GPIO_VALID_GPIO_MASK        ((1U<<SOC_GPIO_PIN_COUNT) - 1)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#ifdef __cplusplus
}
#endif
