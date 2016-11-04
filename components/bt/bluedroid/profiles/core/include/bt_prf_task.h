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

#include <stddef.h> 
#include "bt_defs.h"
#include "fixed_queue.h"

#ifndef BT_PRF_TASK_H__
#define BT_PRF_TASK_H__

/* Functions provided by bt_prf_task.c
************************************
*/


void bt_prf_task_thread_handler(void *arg);

void  bt_prf_init_core(void);
void  bt_prf_free_core(void);

void bt_prf_task_post(uint32_t sig);


void bt_prf_StartUp(void);
void bt_prf_ShutDown(void);

void bt_prf_task_start_up(void);
void bt_prf_task_shut_down(void);

void bt_profile_msg_ready(fixed_queue_t *queue);








#endif		/// BT_PRF_TASK_H__
