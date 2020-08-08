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

// The HAL layer for SIGMADELTA (common part)

#include <stdio.h>
#include "soc/soc.h"
#include "hal/sigmadelta_hal.h"

void sigmadelta_hal_init(sigmadelta_hal_context_t *hal, int sigmadelta_num)
{
    //Get hardware instance.
    hal->dev = SIGMADELTA_LL_GET_HW(sigmadelta_num);
    sigmadelta_ll_set_en(hal->dev, 1);
}