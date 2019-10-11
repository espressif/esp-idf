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

#ifndef __SPP_TASK_H__
#define __SPP_TASK_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SPP_TASK_TAG                   "SPP_TASK"

#define SPP_TASK_SIG_WORK_DISPATCH          (0x01)

/**
 * @brief     handler for the dispatched work
 */
typedef void (* spp_task_cb_t) (uint16_t event, void *param);

/* message to be sent */
typedef struct {
    uint16_t             sig;      /*!< signal to spp_task_task */
    uint16_t             event;    /*!< message event id */
    spp_task_cb_t        cb;       /*!< context switch callback */
    void                 *param;   /*!< parameter area needs to be last */
} spp_task_msg_t;

/**
 * @brief     parameter deep-copy function to be customized
 */
typedef void (* spp_task_copy_cb_t) (spp_task_msg_t *msg, void *p_dest, void *p_src);

/**
 * @brief     work dispatcher for the application task
 */
bool spp_task_work_dispatch(spp_task_cb_t p_cback, uint16_t event, void *p_params, int param_len, spp_task_copy_cb_t p_copy_cback);

void spp_task_task_start_up(void);

void spp_task_task_shut_down(void);


/**
 * @brief     handler for write and read
 */
typedef void (* spp_wr_task_cb_t) (void *fd);

void spp_wr_task_start_up(spp_wr_task_cb_t p_cback, int fd);

void spp_wr_task_shut_down(void);

#endif ///__SPP_TASK_H__