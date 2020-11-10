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

#define SOC_ADC1_DATA_INVERT_DEFAULT    (0)
#define SOC_ADC2_DATA_INVERT_DEFAULT    (0)

#define SOC_ADC_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM) (0)

#define SOC_ADC_FSM_RSTB_WAIT_DEFAULT       (8)
#define SOC_ADC_FSM_START_WAIT_DEFAULT      (5)
#define SOC_ADC_FSM_STANDBY_WAIT_DEFAULT    (100)
#define ADC_FSM_SAMPLE_CYCLE_DEFAULT        (3)

#define SOC_ADC_PWDET_CCT_DEFAULT           (4)

#define SOC_ADC_SAR_CLK_DIV_DEFAULT(PERIPH_NUM) ((PERIPH_NUM==0)? 2 : 1)

#define SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT    (2)
