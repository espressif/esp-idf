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

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void soc_ll_reset_core(int core)
{
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_PROCPU_RST_M);
}

#ifdef __cplusplus
}
#endif