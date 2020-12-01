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

#define SOC_SYSTIMER_COUNTER_NUM (2)   // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM (3)     // Number of alarm units

#define SOC_SYSTIMER_BIT_WIDTH_LO (32) // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI (20) // Bit width of systimer high part

#define SOC_SYSTIMER_FIXED_TICKS_US (16)  // If defined, number of ticks per microsecond is fixed
#define SOC_SYSTIMER_INT_LEVEL      (1)   // Systimer peripheral uses level interrupt
