// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __BT_APP_COMMON_H__
#define __BT_APP_COMMON_H__

#include <stdint.h>
#include "osi.h"
#include "bt_common_types.h"
#include "esp_err.h"

enum BLUFI_SIG {
    BLUFI_SIG_SWITCH_CONTEXT = 0,
    BLUFI_SIG_ENABLE,
    BLUFI_SIG_DISABLE,
};

typedef esp_err_t (*blufi_task_cb_t)(void *arg);

void blufi_init(void);
void blufi_deinit(void);

esp_err_t blufi_enable(void *arg);
esp_err_t blufi_disable(void *arg);

esp_err_t blufi_transfer_context(blufi_task_cb_t cb, void *arg);

#endif /* __BT_APP_COMMON_H__ */
