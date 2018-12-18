// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "tcpip_adapter.h"
#include "rom/queue.h"

struct tcpip_adapter_api_msg_s;

typedef int (*tcpip_adapter_api_fn)(struct tcpip_adapter_api_msg_s *msg);

typedef struct tcpip_adapter_api_msg_s {
    int type;  /**< The first field MUST be int */
    int ret;
    tcpip_adapter_api_fn api_fn;
    tcpip_adapter_if_t tcpip_if;
    tcpip_adapter_ip_info_t *ip_info;
    uint8_t *mac;
    void    *data;
} tcpip_adapter_api_msg_t;

typedef struct tcpip_adapter_dns_param_s {
    tcpip_adapter_dns_type_t dns_type;
    tcpip_adapter_dns_info_t *dns_info;
} tcpip_adapter_dns_param_t;

typedef struct tcpip_adapter_ip_lost_timer_s {
    bool timer_running;
} tcpip_adapter_ip_lost_timer_t;


#define TCPIP_ADAPTER_TRHEAD_SAFE 1
#define TCPIP_ADAPTER_IPC_LOCAL   0
#define TCPIP_ADAPTER_IPC_REMOTE  1

#define TCPIP_ADAPTER_IPC_CALL(_if, _mac, _ip, _data, _fn) do {\
    tcpip_adapter_api_msg_t msg;\
    if (tcpip_inited == false) {\
        ESP_LOGE(TAG, "tcpip_adapter is not initialized!");\
        abort();\
    }\
    memset(&msg, 0, sizeof(msg));\
    msg.tcpip_if = (_if);\
    msg.mac      = (uint8_t*)(_mac);\
    msg.ip_info  = (tcpip_adapter_ip_info_t*)(_ip);\
    msg.data     = (void*)(_data);\
    msg.api_fn   = (_fn);\
    if (TCPIP_ADAPTER_IPC_REMOTE == tcpip_adapter_ipc_check(&msg)) {\
        ESP_LOGV(TAG, "check: remote, if=%d fn=%p\n", (_if), (_fn));\
        return msg.ret;\
    } else {\
        ESP_LOGV(TAG, "check: local, if=%d fn=%p\n", (_if), (_fn));\
    }\
} while(0)
