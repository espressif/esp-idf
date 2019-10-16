/* ESP BLE Mesh Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __RUN_TC_H__
#define __RUN_TC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define TC_NAME_LEN     63
typedef void (*tc_func_start_t)(void *param);
typedef void (*tc_func_end_t)(void);


typedef struct tc {
    char name[TC_NAME_LEN + 1];
    uint8_t case_id;
    tc_func_start_t func_start;
    tc_func_end_t func_stop;
    void *param_list;
} tc_t;


#define TC_WIFI_COEX_TCP_TX_THROUGHPUT       "TCP_COEX_TX_THROUGHPUT"
#define TC_WIFI_COEX_TCP_RX_THROUGHPUT       "TCP_COEX_RX_THROUGHPUT"
#define TC_BLE_COEX_ADV                      "BLE_COEX_ADVERTISING"
#define TC_BLE_COEX_SCAN                     "BLE_COEX_SCAN"

// run_task queue size
#define RUN_TASK_QUEUE_LEN   6
#define RUN_TASK_PRIORITY    18

#define DECLARE_TC(name, id, start_func, stop_func, param_list) \
    {name, id, start_func, stop_func, param_list}

typedef struct run_task_msg {
    uint8_t case_id;
} run_task_msg_t;

extern xQueueHandle xTaskQueue ;

void run_tc_init(void);

#endif /* __RUN_TC_H__ */