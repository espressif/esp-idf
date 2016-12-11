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

#ifndef __BTC_MANAGE_H__
#define __BTC_MANAGE_H__

#include "bta_api.h"
#include "btc_task.h"
#include "esp_bt_defs.h"

/* reset gatt callback table */
void esp_profile_cb_reset(void);

int btc_profile_cb_set(btc_pid_t profile_id, void *cb);
void *btc_profile_cb_get(btc_pid_t profile_id);

#endif /* __BTC_MANAGE_H__ */
