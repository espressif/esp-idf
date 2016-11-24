#include "prf_defs.h"
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


#if (BUT_PROFILE_CFG)
#include "bt_target.h"
#include "gatt_api.h"
#include "gattdefs.h"
#include "esp_gatt_api.h"

#define KEY_SUCCESS             GATT_SUCCESS
#define KEY_ILLEGAL_PARAM       GATT_ILLEGAL_PARAMETER
#define KEY_NO_RESOURCES        GATT_NO_RESOURCES

//define the key serivce uuid
#define ATT_SVC_BUTTON  0xFFFF
//define the key Char uuid
#define ATT_CHAR_BUTTON_WIT 0xFF01
#define ATT_CHAR_BUTTON_NTF 0xFF02

#define BUTTON_PRESS_NTF_CFG    0x01

#define BUTTON_VAL_MAX_LEN  (10)

#define BUTT_MAX_APPS                  GATT_CL_MAX_LCB

#define BUT_MAX_STRING_DATA     7

typedef void (*but_prf_cb_t)(uint8_t app_id, uint8_t event, uint16_t len, uint8_t *value);

#ifndef BUT_MAX_INT_NUM
#define BUT_MAX_INT_NUM     4
#endif

enum {
    RECEIVE_NET_PASSWD_EVT,
    RECEIVE_NET_SSD_EVT,
    RECEIVE_EVT_MAX
};

/// button Service Attributes Indexes
enum {
    KEY_IDX_SVC,
    KEY_IDX_BUTTON_WIT_CHAR,
    KEY_IDX_BUTTON_WIT_VAL,
    KEY_IDX_BUTTON_NTF_CHAR,
    KEY_IDX_BUTTON_NTF_VAL,
    KEY_IDX_BUTTON_NTF_CFG,

    KEY_IDX_NB,
};

typedef struct {
    BD_ADDR     remote_bda;
    BOOLEAN     need_rsp;
    uint16_t    clt_cfg;
} but_write_data_t;

typedef struct {
    BOOLEAN         in_use;
    BOOLEAN         congest;
    uint16_t        conn_id;
    BOOLEAN         connected;
    BD_ADDR         remote_bda;
    uint32_t        trans_id;
    uint8_t         cur_srvc_id;

} but_clcb_t;


typedef struct {
    uint8_t           app_id;
    uint16_t          but_wirt_hdl;
    uint16_t          but_ntf_hdl;
    uint16_t          but_cfg_hdl;

    but_prf_cb_t    p_cback;

} but_inst_t;


/* service engine control block */
typedef struct {
    but_clcb_t              clcb;           /* connection link*/
    esp_gatt_if_t           gatt_if;
    BOOLEAN                 enabled;
    BOOLEAN                 is_primery;
    but_inst_t              button_inst;
    uint8_t                 inst_id;
} button_env_cb_t;

void Button_CreateService(void);

but_clcb_t *button_env_clcb_alloc(uint16_t conn_id, BD_ADDR bda);

uint16_t button_env_find_conn_id_by_bd_adddr(BD_ADDR bda);

BOOLEAN button_env_clcb_dealloc(uint16_t conn_id);

esp_gatt_status_t button_init(but_prf_cb_t call_back);

void button_disable(uint16_t connid);

void button_msg_notify(uint16_t len, uint8_t *button_msg);

extern button_env_cb_t button_cb_env;

#endif ///BUT_PROFILE_CFG
