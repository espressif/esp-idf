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

#include "btc/btc_task.h"
#include "btc/btc_alarm.h"
#include "esp_log.h"

void btc_alarm_handler(btc_msg_t *msg)
{
    btc_alarm_args_t *arg = (btc_alarm_args_t *)msg->arg;

    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    if (arg->cb) {
        arg->cb(arg->cb_data);
    }
}
