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


#ifndef __BTC_ALARM_H__
#define __BTC_ALARM_H__

#include <stdint.h>
#include "osi/alarm.h"

/* btc_alarm_args_t */
typedef struct {
    osi_alarm_callback_t cb;
    void *cb_data;
} btc_alarm_args_t;

void btc_alarm_handler(btc_msg_t *msg);

#endif /* __BTC_ALARM_H__ */
