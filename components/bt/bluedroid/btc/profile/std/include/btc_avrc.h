/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BTC_AVRC_H__
#define __BTC_AVRC_H__

#include <stdint.h>
#include <stdbool.h>
#include "bt_defs.h"
#include "bt_types.h"
#include "bta_av_api.h"

#if (BTC_AV_INCLUDED == TRUE)
#ifndef BTC_AVRC_TGT_INCLUDED
#define BTC_AVRC_TGT_INCLUDED      FALSE
#endif

/* Macros */
typedef enum {
    BTRC_FEAT_NONE = 0x00,    /* AVRCP 1.0 */
    BTRC_FEAT_METADATA = 0x01,    /* AVRCP 1.3 */
    BTRC_FEAT_ABSOLUTE_VOLUME = 0x02,    /* Supports TG role and volume sync */
    BTRC_FEAT_BROWSE = 0x04,    /* AVRCP 1.4 and up, with Browsing support */
} btrc_remote_features_t;

typedef enum {
    BTC_AVRC_CTRL_API_INIT_EVT = 0,
    BTC_AVRC_CTRL_API_DEINIT_EVT,
    BTC_AVRC_CTRL_API_SND_PTCMD_EVT
} btc_avrc_act_t;

typedef struct {
    uint8_t tl; /* transaction label */
    uint8_t key_code;
    uint8_t key_state;
} pt_cmd_t;

/* btc_avrc_args_t */
typedef union {
    // BTC_AVRC_CTRL_API_SND_PT_CMD_EVT
    struct {
        uint8_t tl;
        uint8_t key_code;
        uint8_t key_state;
    } pt_cmd;
} btc_avrc_args_t;

/** BT-RC Controller callback structure. */
typedef void (* btrc_passthrough_rsp_callback) (int id, int key_state);

typedef void (* btrc_connection_state_callback) (bool state, bt_bdaddr_t *bd_addr);

void btc_rc_handler(tBTA_AV_EVT event, tBTA_AV *p_data);

BOOLEAN btc_rc_get_connected_peer(BD_ADDR peer_addr);

/*******************************************************************************
**  BTC AVRC API
********************************************************************************/
void btc_avrc_call_handler(btc_msg_t *msg);

#endif  ///BTC_AV_INCLUDED == TRUE

#endif /* __BTC_AVRC_H__ */
