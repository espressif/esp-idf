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
#include "esp_task.h"
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
};

#define HCI_HOST_TASK_STACK_SIZE        (2048 + BT_TASK_EXTRA_STACK_SIZE)
#define HCI_HOST_TASK_PRIO              (configMAX_PRIORITIES - 3)
#define HCI_HOST_TASK_NAME              "hciHostT"
#define HCI_HOST_QUEUE_NUM              40

#define HCI_H4_TASK_STACK_SIZE          (2048 + BT_TASK_EXTRA_STACK_SIZE)
#define HCI_H4_TASK_PRIO                (configMAX_PRIORITIES - 4)
#define HCI_H4_TASK_NAME                "hciH4T"
#define HCI_H4_QUEUE_NUM                60

#define BTU_TASK_STACK_SIZE             (4096 + BT_TASK_EXTRA_STACK_SIZE)
#define BTU_TASK_PRIO                   (configMAX_PRIORITIES - 5)
#define BTU_TASK_NAME                   "btuT"
#define BTU_QUEUE_NUM                   50

#define BTC_TASK_STACK_SIZE             (CONFIG_BTC_TASK_STACK_SIZE + BT_TASK_EXTRA_STACK_SIZE)	//by menuconfig
#define BTC_TASK_NAME                   "btcT"
#define BTC_TASK_PRIO                   (configMAX_PRIORITIES - 6)
#define BTC_TASK_QUEUE_NUM              60


#define TASK_POST_NON_BLOCKING          (0)
#define TASK_POST_BLOCKING              (portMAX_DELAY)
typedef uint32_t task_post_t;           /* Timeout of task post return, unit TICK */

void btu_task_post(uint32_t sig, task_post_t timeout);
void hci_host_task_post(task_post_t timeout);
void hci_hal_h4_task_post(task_post_t timeout);


#endif /* __THREAD_H__ */
