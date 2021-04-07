// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/lcd_cam_struct.h"

typedef struct {
    lcd_cam_dev_t *dev;
} lcd_hal_context_t;

void lcd_hal_init(lcd_hal_context_t *hal, int id);

#ifdef __cplusplus
}
#endif
