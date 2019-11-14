// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SERVER_COMMON_H_
#define _SERVER_COMMON_H_

#include <string.h>
#include <stdint.h>

#include "mesh_buf.h"
#include "mesh_access.h"
#include "mesh_kernel.h"

#define BLE_MESH_SERVER_RSP_MAX_LEN         384

#define BLE_MESH_SERVER_TRANS_MIC_SIZE      4

#define BLE_MESH_CHECK_SEND_STATUS(_func) do {                      \
        int __status = (_func);                                     \
        if (__status) {                                             \
            BT_ERR("%s, Send failed, err %d", __func__, __status);  \
        }                                                           \
    } while(0);

#define BLE_MESH_STATE_OFF                      0x00
#define BLE_MESH_STATE_ON                       0x01
#define BLE_MESH_STATE_RESTORE                  0x02

/* Following 4 values are as per Mesh Model specification */
#define BLE_MESH_LIGHTNESS_MIN                  0x0001
#define BLE_MESH_LIGHTNESS_MAX                  0xFFFF
#define BLE_MESH_TEMPERATURE_MIN                0x0320
#define BLE_MESH_TEMPERATURE_MAX                0x4E20
#define BLE_MESH_TEMPERATURE_UNKNOWN            0xFFFF

/* Refer 7.2 of Mesh Model Specification */
#define BLE_MESH_RANGE_UPDATE_SUCCESS           0x00
#define BLE_MESH_CANNOT_SET_RANGE_MIN           0x01
#define BLE_MESH_CANNOT_SET_RANGE_MAX           0x02

#define BLE_MESH_UNKNOWN_REMAIN_TIME            0x3F
#define BLE_MESH_DEVICE_SPECIFIC_RESOLUTION     10

#define BLE_MESH_INVALID_DEVICE_PROPERTY_ID     0x0000

enum {
    BLE_MESH_TRANS_TIMER_START,  /* Proper transition timer has been started */
    BLE_MESH_TRANS_FLAG_MAX,
};

struct bt_mesh_state_transition {
    bool just_started;

    u8_t  trans_time;
    u8_t  remain_time;
    u8_t  delay;
    u32_t quo_tt;
    u32_t counter;
    u32_t total_duration;
    s64_t start_timestamp;

    BLE_MESH_ATOMIC_DEFINE(flag, BLE_MESH_TRANS_FLAG_MAX);
    struct k_delayed_work timer;
};

struct bt_mesh_last_msg_info {
    u8_t  tid;
    u16_t src;
    u16_t dst;
    s64_t timestamp;
};

#define BLE_MESH_SERVER_RSP_BY_APP  0
#define BLE_MESH_SERVER_AUTO_RSP    1

struct bt_mesh_server_rsp_ctrl {
    /**
     * @brief BLE Mesh Server Response Option
     *        1. If get_auto_rsp is set to BLE_MESH_SERVER_RSP_BY_APP, then the response
     *           of Client Get messages need to be replied by the application;
     *        2. If get_auto_rsp is set to BLE_MESH_SERVER_AUTO_RSP, then the response
     *           of Client Get messages will be replied by the server models;
     *        3. If set_auto_rsp is set to BLE_MESH_SERVER_RSP_BY_APP, then the response
     *           of Client Set messages need to be replied by the application;
     *        4. If set_auto_rsp is set to BLE_MESH_SERVER_AUTO_RSP, then the response
     *           of Client Set messages will be replied by the server models;
     *        5. If status_auto_rsp is set to BLE_MESH_SERVER_RSP_BY_APP, then the response
     *           of Server Status messages need to be replied by the application;
     *        6. If status_auto_rsp is set to BLE_MESH_SERVER_AUTO_RSP, then the response
     *           of Server status messages will be replied by the server models;
     */
    u8_t get_auto_rsp : 1, /* Response for Client Get messages */
         set_auto_rsp : 1, /* Response for Client Set messages */
         status_auto_rsp : 1; /* Response for Server Status messages */
};

u8_t bt_mesh_get_default_trans_time(struct bt_mesh_model *model);

int bt_mesh_get_light_lc_trans_time(struct bt_mesh_model *model, u8_t *trans_time);

int bt_mesh_server_get_optional(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf,
                                u8_t *trans_time, u8_t *delay,
                                bool *optional);

void bt_mesh_server_alloc_ctx(struct k_work *work);

bool bt_mesh_is_server_recv_last_msg(struct bt_mesh_last_msg_info *last,
                                     u8_t tid, u16_t src, u16_t dst, s64_t *now);

void bt_mesh_server_update_last_msg(struct bt_mesh_last_msg_info *last,
                                    u8_t tid, u16_t src, u16_t dst, s64_t *now);

struct net_buf_simple *bt_mesh_server_get_pub_msg(struct bt_mesh_model *model, u16_t msg_len);

#endif /* _SERVER_COMMON_H_ */
