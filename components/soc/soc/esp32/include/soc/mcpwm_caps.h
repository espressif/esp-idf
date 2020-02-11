// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#define SOC_MCPWM_PERIPH_NUM        2   ///< MCPWM peripheral number
#define SOC_MCPWM_TIMER_NUM         3   ///< Timer that each peripheral has
#define SOC_MCPWM_OP_NUM            3   ///< Operator that each peripheral has
#define SOC_MCPWM_COMPARATOR_NUM    2   ///< Comparator that each operator has
#define SOC_MCPWM_GENERATOR_NUM     2   ///< Generator that each operator has
#define SOC_MCPWM_FAULT_SIG_NUM     3   ///< Fault signal number that each peripheral has