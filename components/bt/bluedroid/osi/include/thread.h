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

#ifndef __THREAD_H__
#define __THREAD_H__

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "bt_defs.h"

#define portBASE_TYPE int

struct bt_task_evt {
    uint32_t    sig;    //task sig
    void       *par;    //point to task param
    void       *cb;     //point to function cb
    void       *arg;    //point to function arg
};
typedef struct bt_task_evt BtTaskEvt_t;

typedef bt_status_t (* BtTaskCb_t)(void *arg);

enum {
    SIG_PRF_START_UP = 0xfc,
    SIG_PRF_WORK = 0xfd,
    SIG_BTU_START_UP = 0xfe,
    SIG_BTU_WORK = 0xff,
    SIG_BTIF_WORK = 0xff
};

void btu_task_post(uint32_t sig);
void hci_host_task_post(void);
void hci_hal_h4_task_post(void);
void hci_drv_task_post(void);
void bt_alarm_task_post(void);


#endif /* __THREAD_H__ */
