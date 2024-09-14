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
#include "common/bt_defs.h"
#include "stack/bt_types.h"
#include "bta/bta_av_api.h"
#include "btc/btc_task.h"
#include "esp_avrc_api.h"

#if (BTC_AV_INCLUDED == TRUE)
#ifndef BTC_AVRC_TGT_INCLUDED
#define BTC_AVRC_TGT_INCLUDED      FALSE
#endif

typedef enum {
    BTC_AVRC_CT_API_INIT_EVT = 0,
    BTC_AVRC_CT_API_DEINIT_EVT,
    BTC_AVRC_CTRL_API_SND_PTCMD_EVT,
    BTC_AVRC_STATUS_API_SND_META_EVT,
    BTC_AVRC_STATUS_API_SND_PLAY_STATUS_EVT,
    BTC_AVRC_STATUS_API_SND_GET_RN_CAPS_EVT,
    BTC_AVRC_NOTIFY_API_SND_REG_NOTIFY_EVT,
    BTC_AVRC_CTRL_API_SND_SET_PLAYER_SETTING_EVT,
    BTC_AVRC_CTRL_API_SND_SET_ABSOLUTE_VOLUME_EVT,
#if BTC_AV_CA_INCLUDED
    BTC_AVRC_CT_API_COVER_ART_CONNECT_EVT,
    BTC_AVRC_CT_API_COVER_ART_DISCONNECT_EVT,
    BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_PROPERTIES_EVT,
    BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_EVT,
    BTC_AVRC_CT_API_COVER_ART_GET_LINKED_THUMBNAIL_EVT,
#endif
} btc_avrc_act_t;

typedef struct {
    uint8_t tl; /* transaction label */
    uint8_t key_code;
    uint8_t key_state;
} pt_cmd_t;

typedef struct {
    uint8_t tl;
    uint8_t attr_mask;
} md_cmd_t;

typedef struct {
    uint8_t tl;
    uint8_t event_id;
    uint32_t event_parameter;
} rn_cmd_t;

typedef struct {
    uint8_t tl;
    uint8_t attr_id;
    uint8_t value_id;
} ps_cmd_t;

typedef struct {
    uint8_t tl;
} get_caps_cmd_t;

#define BTC_AVRC_MIN_VOLUME 0x00
#define BTC_AVRC_MAX_VOLUME 0x7f

typedef struct {
    uint8_t tl;
    uint8_t volume;
} set_abs_vol_cmd_t;

#if BTC_AV_CA_INCLUDED

typedef struct {
    uint16_t mtu;
} ca_conn_t;

typedef struct {
    uint8_t image_handle[7];
} ca_get_img_prop_t;

typedef struct {
    uint8_t image_handle[7];
    uint16_t image_descriptor_len;
    uint8_t *image_descriptor;
} ca_get_img_t;

typedef struct {
    uint8_t image_handle[7];
} ca_get_lk_thn_t;

#endif /* BTC_AV_CA_INCLUDED */

/* btc_avrc_args_t */
typedef union {
    pt_cmd_t pt_cmd;
    md_cmd_t md_cmd;
    rn_cmd_t rn_cmd;
    ps_cmd_t ps_cmd;
    get_caps_cmd_t get_caps_cmd;
    set_abs_vol_cmd_t set_abs_vol_cmd;
#if BTC_AV_CA_INCLUDED
    ca_conn_t ca_conn;
    ca_get_img_prop_t ca_get_img_prop;
    ca_get_img_t ca_get_img;
    ca_get_lk_thn_t ca_get_lk_thn;
#endif
} btc_avrc_args_t;

/* btc_avrc_tg_act_t */
typedef enum {
    BTC_AVRC_TG_API_INIT_EVT = 0,
    BTC_AVRC_TG_API_DEINIT_EVT,
    BTC_AVRC_TG_API_SET_RN_SUPPORTED_EVT,
    BTC_AVRC_TG_API_SET_PSTH_SUPPORTED_CMD_EVT,
    BTC_AVRC_TG_API_SEND_RN_RSP_EVT,
} btc_avrc_tg_act_t;

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
/* for AVRC 1.4 need to change this */
#define BTC_RC_CT_INIT_MAGIC            0x20181128
#define BTC_RC_TG_INIT_MAGIC            0x20181129

#define MAX_RC_NOTIFICATIONS            (13)  // refer to ESP_AVRC_RN_MAX_EVT


#define CHECK_ESP_RC_CONNECTED       do { \
        BTC_TRACE_DEBUG("## %s ##", __FUNCTION__); \
        if (btc_rc_cb.rc_connected == FALSE) { \
            BTC_TRACE_WARNING("Function %s() called when RC is not connected", __FUNCTION__); \
        return ESP_ERR_INVALID_STATE; \
        } \
    } while (0)

/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    BOOLEAN registered;
    UINT8 label;
} btc_rc_reg_ntf_t;

typedef struct {
    BOOLEAN                     rc_connected;
#if BTC_AV_CA_INCLUDED
    BOOLEAN                     rc_cover_art_connected;
#endif
    UINT8                       rc_handle;
    tBTA_AV_FEAT                rc_features;
    UINT16                      rc_ct_features;
    UINT16                      rc_tg_features;
    BD_ADDR                     rc_addr;
    btc_rc_reg_ntf_t            rc_ntf[MAX_RC_NOTIFICATIONS];
} btc_rc_cb_t;

/*****************************************************************************
**  Static variables
******************************************************************************/
#if AVRC_DYNAMIC_MEMORY == TRUE
extern btc_rc_cb_t *btc_rc_cb_ptr;
#define btc_rc_cb (*btc_rc_cb_ptr)
#endif ///AVRC_DYNAMIC_MEMORY == FALSE

typedef struct {
    esp_avrc_rn_event_ids_t event_id;
    esp_avrc_rn_rsp_t rsp;
    esp_avrc_rn_param_t param;
} rn_rsp_t;

/* btc_avrc_tg_args_t */
typedef union {
    rn_rsp_t rn_rsp;         /* BTC_AVRC_TG_API_SEND_RN_RSP_EVT */
    uint16_t set_rn_evt;     /* BTC_AVRC_TG_API_SET_RN_SUPPORTED_EVT */
    uint16_t *set_psth_cmd;   /* BTC_AVRC_TG_API_SET_PSTH_SUPPORTED_CMD_EVT */
} btc_avrc_tg_args_t;

void btc_rc_handler(tBTA_AV_EVT event, tBTA_AV *p_data);

BOOLEAN btc_rc_get_connected_peer(BD_ADDR peer_addr);

/*******************************************************************************
**  BTC AVRC API
********************************************************************************/
void btc_avrc_ct_call_handler(btc_msg_t *msg);
void btc_avrc_tg_call_handler(btc_msg_t *msg);
void btc_avrc_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_avrc_arg_deep_free(btc_msg_t *msg);
void btc_avrc_tg_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_avrc_tg_arg_deep_free(btc_msg_t *msg);

bool btc_avrc_tg_init_p(void);
bool btc_avrc_ct_init_p(void);
bool btc_avrc_tg_connected_p(void);
bool btc_avrc_ct_connected_p(void);

const uint16_t *btc_avrc_tg_get_supported_command(void);
const uint16_t *btc_avrc_tg_get_allowed_command(void);
bool btc_avrc_tg_check_supported_command(const uint16_t *cmd_set);

uint16_t btc_avrc_tg_get_rn_allowed_evt(void);
uint16_t btc_avrc_tg_get_rn_supported_evt(void);
bool btc_avrc_tg_check_rn_supported_evt(uint16_t evt_set);
bool btc_avrc_tg_rn_evt_supported(uint8_t event_id);
bool btc_avrc_ct_rn_evt_supported(uint8_t event_id);
bool btc_avrc_ct_check_cover_art_support(void);

#endif  ///BTC_AV_INCLUDED == TRUE

#endif /* __BTC_AVRC_H__ */
