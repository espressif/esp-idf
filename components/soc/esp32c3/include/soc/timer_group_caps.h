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

#define SOC_TIMER_GROUP_SUPPORT_XTAL (1)
#define SOC_TIMER_GROUP_XTAL_MHZ (40)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH  (54)
#define SOC_TIMER_GROUP_PRESCALE_BIT_WIDTH (16)
#define SOC_TIMER_GROUPS (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS (SOC_TIMER_GROUPS * SOC_TIMER_GROUP_TIMERS_PER_GROUP)
#define SOC_TIMER_GROUP_LAYOUT {1,1}
