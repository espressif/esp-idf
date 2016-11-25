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
#include "bt_defs.h"
    
/* BT APP Events */
#define BT_EVT_APP                     (0xB000)
#define BT_EVT_APP_CONTEXT_SWITCH      (0x0001 | BT_EVT_APP)

typedef void (tBTAPP_CBACK) (uint16_t event, char *p_param);
typedef void (tBTAPP_COPY_CBACK) (uint16_t event, char *p_dest, char *p_src);

typedef struct
{
    BT_HDR               hdr;
    tBTAPP_CBACK*       p_cb;    /* context switch callback */

    /* parameters passed to callback */
    UINT16               event;   /* message event id */
    char                 p_param[0]; /* parameter area needs to be last */
} tBTAPP_CONTEXT_SWITCH_CBACK;

bt_status_t bt_app_transfer_context (tBTAPP_CBACK *p_cback, UINT16 event, char* p_params, int param_len, tBTAPP_COPY_CBACK *p_copy_cback);

void bt_app_task_start_up(void);

#endif /* __BT_APP_COMMON_H__ */
