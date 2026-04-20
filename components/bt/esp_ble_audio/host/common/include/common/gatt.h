/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_GATT_H_
#define HOST_COMMON_GATT_H_

#include <stdint.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/gatt.h>

#include "nimble/gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_gatt_mtu_change_event {
    uint16_t conn_handle;
    uint16_t mtu;
};

enum {
    GATTC_DISC_TYPE_DUMMY,
    GATTC_DISC_TYPE_SVC_BY_UUID,
    GATTC_DISC_TYPE_INC_SVCS,
    GATTC_DISC_TYPE_CHRS_BY_UUID,
    GATTC_DISC_TYPE_ALL_CHRS,
    GATTC_DISC_TYPE_ALL_DSCS,   /* Not used for now */

    GATTC_DISC_TYPE_MAX,
};

struct bt_le_gattc_discover_event {
    uint8_t type;
    uint16_t conn_handle;
    uint16_t uuid;
    struct bt_gatt_discover_params *params;
};

struct bt_le_gattc_disc_cmpl_event {
    uint8_t  status;
    uint16_t conn_handle;
};

struct bt_le_gatts_subscribe_event {
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint8_t  prev_notify : 1;
    uint8_t  cur_notify : 1;
    uint8_t  prev_indicate : 1;
    uint8_t  cur_indicate : 1;
    uint8_t  reason;
};

struct bt_le_gattc_notify_rx_event {
    bool     is_notify;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint16_t len;
    uint8_t *value;
};

struct bt_le_gatts_notify_tx_event {
    bool     is_notify;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint8_t  status;
};

struct bt_le_gatt_event_param {
    uint8_t type;

    union {
        struct bt_le_gatt_mtu_change_event gatt_mtu_change;
        struct bt_le_gattc_discover_event  gattc_discover;
        struct bt_le_gattc_disc_cmpl_event gattc_disc_cmpl;
        struct bt_le_gatts_subscribe_event gatts_subscribe;
        struct bt_le_gattc_notify_rx_event gattc_notify_rx;
        struct bt_le_gatts_notify_tx_event gatts_notify_tx;
    };
};

enum {
    BT_LE_GATT_MTU_CHANGE_EVENT,
    BT_LE_GATTC_DISCOVER_EVENT,
    BT_LE_GATTC_DISC_CMPL_EVENT,
    BT_LE_GATTS_SUBSCRIBE_EVENT,
    BT_LE_GATTC_NOTIFY_RX_EVENT,
    BT_LE_GATTS_NOTIFY_TX_EVENT,

    BT_LE_GATT_EVENT_MAX,
};

static inline char *audio_svc_uuid_to_str(uint16_t uuid)
{
    switch (uuid) {
    case BT_UUID_GAP_VAL:
        return "GAP";
    case BT_UUID_GATT_VAL:
        return "GATT";
    case BT_UUID_AICS_VAL:
        return "AICS";
    case BT_UUID_CAS_VAL:
        return "CAS";
    case BT_UUID_VCS_VAL:
        return "VCS";
    case BT_UUID_VOCS_VAL:
        return "VOCS";
    case BT_UUID_CSIS_VAL:
        return "CSIS";
    case BT_UUID_MCS_VAL:
        return "MCS";
    case BT_UUID_GMCS_VAL:
        return "GMCS";
    case BT_UUID_TBS_VAL:
        return "TBS";
    case BT_UUID_GTBS_VAL:
        return "GTBS";
    case BT_UUID_MICS_VAL:
        return "MICS";
    case BT_UUID_ASCS_VAL:
        return "ASCS";
    case BT_UUID_BASS_VAL:
        return "BASS";
    case BT_UUID_PACS_VAL:
        return "PACS";
    case BT_UUID_BASIC_AUDIO_VAL:
        return "BASIC_AUDIO";
    case BT_UUID_HAS_VAL:
        return "HAS";
    case BT_UUID_TMAS_VAL:
        return "TMAS";
    case BT_UUID_PBA_VAL:
        return "PBA";
    case BT_UUID_GMAS_VAL:
        return "GMAS";
    case BT_UUID_OTS_VAL:
        return "OTS";
    default:
        return "Unknown";
    }
}

static inline char *audio_chrc_uuid_to_str(uint16_t uuid)
{
    switch (uuid) {
    case BT_UUID_OTS_FEATURE_VAL:
        return "OTS_FEATURE";
    case BT_UUID_OTS_NAME_VAL:
        return "OTS_NAME";
    case BT_UUID_OTS_TYPE_VAL:
        return "OTS_TYPE";
    case BT_UUID_OTS_SIZE_VAL:
        return "OTS_SIZE";
    case BT_UUID_OTS_FIRST_CREATED_VAL:
        return "OTS_FIRST_CREATED";
    case BT_UUID_OTS_LAST_MODIFIED_VAL:
        return "OTS_LAST_MODIFIED";
    case BT_UUID_OTS_ID_VAL:
        return "OTS_ID";
    case BT_UUID_OTS_PROPERTIES_VAL:
        return "OTS_PROPERTIES";
    case BT_UUID_OTS_ACTION_CP_VAL:
        return "OTS_ACTION_CP";
    case BT_UUID_OTS_LIST_CP_VAL:
        return "OTS_LIST_CP";
    case BT_UUID_OTS_LIST_FILTER_VAL:
        return "OTS_LIST_FILTER";
    case BT_UUID_OTS_CHANGED_VAL:
        return "OTS_CHANGED";
    case BT_UUID_GATT_TMAPR_VAL:
        return "TMAP_ROLE";
    case BT_UUID_AICS_STATE_VAL:
        return "AICS_STATE";
    case BT_UUID_AICS_GAIN_SETTINGS_VAL:
        return "AICS_GAIN_SETTINGS";
    case BT_UUID_AICS_INPUT_TYPE_VAL:
        return "AICS_INPUT_TYPE";
    case BT_UUID_AICS_INPUT_STATUS_VAL:
        return "AICS_INPUT_STATUS";
    case BT_UUID_AICS_CONTROL_VAL:
        return "AICS_CONTROL";
    case BT_UUID_AICS_DESCRIPTION_VAL:
        return "AICS_DESCRIPTION";
    case BT_UUID_VCS_STATE_VAL:
        return "VCS_STATE";
    case BT_UUID_VCS_CONTROL_VAL:
        return "VCS_CONTROL";
    case BT_UUID_VCS_FLAGS_VAL:
        return "VCS_FLAGS";
    case BT_UUID_VOCS_STATE_VAL:
        return "VOCS_STATE";
    case BT_UUID_VOCS_LOCATION_VAL:
        return "VOCS_LOCATION";
    case BT_UUID_VOCS_CONTROL_VAL:
        return "VOCS_CONTROL";
    case BT_UUID_VOCS_DESCRIPTION_VAL:
        return "VOCS_DESCRIPTION";
    case BT_UUID_CSIS_SIRK_VAL:
        return "CSIS_SIRK";
    case BT_UUID_CSIS_SET_SIZE_VAL:
        return "CSIS_SET_SIZE";
    case BT_UUID_CSIS_SET_LOCK_VAL:
        return "CSIS_SET_LOCK";
    case BT_UUID_CSIS_RANK_VAL:
        return "CSIS_RANK";
    case BT_UUID_MCS_PLAYER_NAME_VAL:
        return "MCS_PLAYER_NAME";
    case BT_UUID_MCS_ICON_OBJ_ID_VAL:
        return "MCS_ICON_OBJ_ID";
    case BT_UUID_MCS_ICON_URL_VAL:
        return "MCS_ICON_URL";
    case BT_UUID_MCS_TRACK_CHANGED_VAL:
        return "MCS_TRACK_CHANGED";
    case BT_UUID_MCS_TRACK_TITLE_VAL:
        return "MCS_TRACK_TITLE";
    case BT_UUID_MCS_TRACK_DURATION_VAL:
        return "MCS_TRACK_DURATION";
    case BT_UUID_MCS_TRACK_POSITION_VAL:
        return "MCS_TRACK_POSITION";
    case BT_UUID_MCS_PLAYBACK_SPEED_VAL:
        return "MCS_PLAYBACK_SPEED";
    case BT_UUID_MCS_SEEKING_SPEED_VAL:
        return "MCS_SEEKING_SPEED";
    case BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL:
        return "MCS_TRACK_SEGMENTS_OBJ_ID";
    case BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL:
        return "MCS_CURRENT_TRACK_OBJ_ID";
    case BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL:
        return "MCS_NEXT_TRACK_OBJ_ID";
    case BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL:
        return "MCS_PARENT_GROUP_OBJ_ID";
    case BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL:
        return "MCS_CURRENT_GROUP_OBJ_ID";
    case BT_UUID_MCS_PLAYING_ORDER_VAL:
        return "MCS_PLAYING_ORDER";
    case BT_UUID_MCS_PLAYING_ORDERS_VAL:
        return "MCS_PLAYING_ORDERS";
    case BT_UUID_MCS_MEDIA_STATE_VAL:
        return "MCS_MEDIA_STATE";
    case BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL:
        return "MCS_MEDIA_CONTROL_POINT";
    case BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL:
        return "MCS_MEDIA_CONTROL_OPCODES";
    case BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL:
        return "MCS_SEARCH_RESULTS_OBJ_ID";
    case BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL:
        return "MCS_SEARCH_CONTROL_POINT";
    case BT_UUID_TBS_PROVIDER_NAME_VAL:
        return "TBS_PROVIDER_NAME";
    case BT_UUID_TBS_UCI_VAL:
        return "TBS_UCI";
    case BT_UUID_TBS_TECHNOLOGY_VAL:
        return "TBS_TECHNOLOGY";
    case BT_UUID_TBS_URI_LIST_VAL:
        return "TBS_URI_LIST";
    case BT_UUID_TBS_SIGNAL_STRENGTH_VAL:
        return "TBS_SIGNAL_STRENGTH";
    case BT_UUID_TBS_SIGNAL_INTERVAL_VAL:
        return "TBS_SIGNAL_INTERVAL";
    case BT_UUID_TBS_LIST_CURRENT_CALLS_VAL:
        return "TBS_LIST_CURRENT_CALLS";
    case BT_UUID_CCID_VAL:
        return "CCID";
    case BT_UUID_TBS_STATUS_FLAGS_VAL:
        return "TBS_STATUS_FLAGS";
    case BT_UUID_TBS_INCOMING_URI_VAL:
        return "TBS_INCOMING_URI";
    case BT_UUID_TBS_CALL_STATE_VAL:
        return "TBS_CALL_STATE";
    case BT_UUID_TBS_CALL_CONTROL_POINT_VAL:
        return "TBS_CALL_CONTROL_POINT";
    case BT_UUID_TBS_OPTIONAL_OPCODES_VAL:
        return "TBS_OPTIONAL_OPCODES";
    case BT_UUID_TBS_TERMINATE_REASON_VAL:
        return "TBS_TERMINATE_REASON";
    case BT_UUID_TBS_INCOMING_CALL_VAL:
        return "TBS_INCOMING_CALL";
    case BT_UUID_TBS_FRIENDLY_NAME_VAL:
        return "TBS_FRIENDLY_NAME";
    case BT_UUID_MICS_MUTE_VAL:
        return "MICS_MUTE";
    case BT_UUID_ASCS_ASE_SNK_VAL:
        return "ASCS_ASE_SNK";
    case BT_UUID_ASCS_ASE_SRC_VAL:
        return "ASCS_ASE_SRC";
    case BT_UUID_ASCS_ASE_CP_VAL:
        return "ASCS_ASE_CP";
    case BT_UUID_BASS_CONTROL_POINT_VAL:
        return "BASS_CP";
    case BT_UUID_BASS_RECV_STATE_VAL:
        return "BASS_RECV_STATE";
    case BT_UUID_PACS_SNK_VAL:
        return "PACS_SNK";
    case BT_UUID_PACS_SNK_LOC_VAL:
        return "PACS_SNK_LOC";
    case BT_UUID_PACS_SRC_VAL:
        return "PACS_SRC";
    case BT_UUID_PACS_SRC_LOC_VAL:
        return "PACS_SRC_LOC";
    case BT_UUID_PACS_AVAILABLE_CONTEXT_VAL:
        return "PACS_AVAILABLE_CONTEXT";
    case BT_UUID_PACS_SUPPORTED_CONTEXT_VAL:
        return "PACS_SUPPORTED_CONTEXT";
    case BT_UUID_HAS_HEARING_AID_FEATURES_VAL:
        return "HAS_HEARING_AID_FEATURES";
    case BT_UUID_HAS_PRESET_CONTROL_POINT_VAL:
        return "HAS_PRESET_CONTROL_POINT";
    case BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL:
        return "HAS_ACTIVE_PRESET_INDEX";
    default:
        return "Unknown";
    }
}

struct gattc_sub {
    uint8_t id;
    bt_addr_le_t peer;
    sys_slist_t list;
};

struct gatt_incl {
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid16;
} __attribute__((packed));

struct gatt_chrc {
    uint8_t  properties;
    uint16_t value_handle;
    union {
        uint16_t uuid16;
        uint8_t  uuid[16];
    };
} __attribute__((packed));

struct bt_att_write_req {
    uint16_t handle;
    uint8_t  value[0];
} __attribute__((packed));

struct notify_data {
    const struct bt_gatt_attr *attr;
    uint16_t handle;
    int err;
    uint16_t type;
    union {
        struct bt_gatt_notify_params *nfy_params;
        struct bt_gatt_indicate_params *ind_params;
    };
};

struct bt_gatt_attr *bt_gatts_find_attr_by_handle(uint16_t handle);

bool bt_gatts_find_attr_by_uuid(struct notify_data *found,
                                   const struct bt_uuid *uuid);

int bt_gatts_sub_changed(uint16_t conn_handle,
                         uint16_t ccc_handle,
                         uint8_t cur_notify,
                         uint8_t cur_indicate,
                         uint8_t reason);

int bt_gattc_disc_start_safe(uint16_t conn_handle);

struct gattc_sub *bt_gattc_sub_find(struct bt_conn *conn);

void bt_le_acl_conn_disconnected_gatt_listener(uint16_t conn_handle);

void bt_le_gatt_handle_event(uint8_t *data, size_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_GATT_H_ */
