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

#include "prf_defs.h"

#if (WX_AIRSYNC_CFG)

#include "bt_target.h"
#include "gatt_api.h"
#include "gattdefs.h"
#include "bt_app_api.h"


/// Maximum Transmission Unit
#define ATT_DEFAULT_MTU                                 (23)

#define BLE_WECHAT_MAX_DATA_LEN            (ATT_DEFAULT_MTU - 3)


//define the key serivce uuid
#define ATT_SVC_AIRSYNC 0xFEE7
//define the airsync Char uuid
#define ATT_CHAR_AIRSYNC_WIT    0xFEC7
#define ATT_CHAR_AIRSYBC_NTF    0xFEC8
#define ATT_CHAR_AIRSYNC_READ   0xFEC9


typedef void (tAIRSYNC_CBACK)(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *data);


/// WX AirSync Service Attributes Indexes
enum {
    WX_IDX_SVC,
    WX_IDX_AIRSYNC_WIT_CHAR,
    WX_IDX_AIRSYNC_WIT_VAL,
    WX_IDX_AIRSYNC_NTF_CHAR,
    WX_IDX_AIRSYNC_NTF_VAL,
    WX_IDX_AIRSYNC_READ_CHAR,
    WX_IDX_AIRSYNC_READ_VAL,
    WX_IDX_AIRSYNC_NTF_CFG,

    WX_IDX_NB,
};

typedef struct {
    BD_ADDR remote_bda;
    BOOLEAN need_rsp;
    UINT16  clt_cfg;
} tAirSync_WRITE_DATA;

typedef struct {
    BOOLEAN      in_use;
    BOOLEAN      congest;
    UINT16       conn_id;
    BOOLEAN      connected;
    BD_ADDR      remote_bda;
    UINT32       trans_id;
    UINT8            cur_srvc_id;

} tAirSync_CLCB;


typedef struct {
    UINT8            app_id;
    UINT16       airsync_wirt_hdl;
    UINT16       airsync_ntf_hdl;
    UINT16           airsync_read_hdl;
    UINT16       airsync_cfg_hdl;

    tAIRSYNC_CBACK       *p_cback;

} tAirSync_INST;


/* service engine control block */
typedef struct {
    tAirSync_CLCB            clcb;           /* connection link*/
    tGATT_IF                 gatt_if;
    BOOLEAN              enabled;
    BOOLEAN              is_primery;
    tAirSync_INST            airsync_inst;
    UINT8                    inst_id;
} tAIRSYNC_CB_ENV;

void AirSync_CreateService(void);

tAirSync_CLCB *airsync_env_clcb_alloc (UINT16 conn_id, BD_ADDR remote_bda);

UINT16 AirSync_env_find_conn_id_by_bd_adddr(BD_ADDR bda);

BOOLEAN AirSync_env_clcb_dealloc(UINT16 conn_id);

tGATT_STATUS AirSync_Init(tAIRSYNC_CBACK *call_back);

void AirSync_msg_notify(UINT8 len, UINT8 *button_msg);

extern tAIRSYNC_CB_ENV airsync_cb_env;

#endif  ///WX_AIRSYNC_CFG
