/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_SERVER_H_
#define HOST_BLUEDROID_SERVER_H_

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline char *audio_svc_uuid_to_str(uint16_t uuid)
{
    switch (uuid) {
    case BT_UUID_AICS_VAL:          return "AICS";
    case BT_UUID_CAS_VAL:           return "CAS";
    case BT_UUID_VCS_VAL:           return "VCS";
    case BT_UUID_VOCS_VAL:          return "VOCS";
    case BT_UUID_CSIS_VAL:          return "CSIS";
    case BT_UUID_MCS_VAL:           return "MCS";
    case BT_UUID_GMCS_VAL:          return "GMCS";
    case BT_UUID_TBS_VAL:           return "TBS";
    case BT_UUID_GTBS_VAL:          return "GTBS";
    case BT_UUID_MICS_VAL:          return "MICS";
    case BT_UUID_ASCS_VAL:          return "ASCS";
    case BT_UUID_BASS_VAL:          return "BASS";
    case BT_UUID_PACS_VAL:          return "PACS";
    case BT_UUID_BASIC_AUDIO_VAL:   return "BASIC_AUDIO";
    case BT_UUID_HAS_VAL:           return "HAS";
    case BT_UUID_TMAS_VAL:          return "TMAS";
    case BT_UUID_PBA_VAL:           return "PBA";
    case BT_UUID_GMAS_VAL:          return "GMAS";
    case BT_UUID_OTS_VAL:           return "OTS";
    default:                        return "UnknownSvc";
    }
}

static inline char *audio_chrc_uuid_to_str(uint16_t uuid)
{
    switch (uuid) {
    case BT_UUID_OTS_FEATURE_VAL:               return "OTS_FEATURE";
    case BT_UUID_OTS_NAME_VAL:                  return "OTS_NAME";
    case BT_UUID_OTS_TYPE_VAL:                  return "OTS_TYPE";
    case BT_UUID_OTS_SIZE_VAL:                  return "OTS_SIZE";
    case BT_UUID_OTS_FIRST_CREATED_VAL:         return "OTS_FIRST_CREATED";
    case BT_UUID_OTS_LAST_MODIFIED_VAL:         return "OTS_LAST_MODIFIED";
    case BT_UUID_OTS_ID_VAL:                    return "OTS_ID";
    case BT_UUID_OTS_PROPERTIES_VAL:            return "OTS_PROPERTIES";
    case BT_UUID_OTS_ACTION_CP_VAL:             return "OTS_ACTION_CP";
    case BT_UUID_OTS_LIST_CP_VAL:               return "OTS_LIST_CP";
    case BT_UUID_OTS_LIST_FILTER_VAL:           return "OTS_LIST_FILTER";
    case BT_UUID_OTS_CHANGED_VAL:               return "OTS_CHANGED";
    case BT_UUID_GATT_TMAPR_VAL:                return "TMAP_ROLE";
    case BT_UUID_AICS_STATE_VAL:                return "AICS_STATE";
    case BT_UUID_AICS_GAIN_SETTINGS_VAL:        return "AICS_GAIN_SETTINGS";
    case BT_UUID_AICS_INPUT_TYPE_VAL:           return "AICS_INPUT_TYPE";
    case BT_UUID_AICS_INPUT_STATUS_VAL:         return "AICS_INPUT_STATUS";
    case BT_UUID_AICS_CONTROL_VAL:              return "AICS_CONTROL";
    case BT_UUID_AICS_DESCRIPTION_VAL:          return "AICS_DESCRIPTION";
    case BT_UUID_VCS_STATE_VAL:                 return "VCS_STATE";
    case BT_UUID_VCS_CONTROL_VAL:               return "VCS_CONTROL";
    case BT_UUID_VCS_FLAGS_VAL:                 return "VCS_FLAGS";
    case BT_UUID_VOCS_STATE_VAL:                return "VOCS_STATE";
    case BT_UUID_VOCS_LOCATION_VAL:             return "VOCS_LOCATION";
    case BT_UUID_VOCS_CONTROL_VAL:              return "VOCS_CONTROL";
    case BT_UUID_VOCS_DESCRIPTION_VAL:          return "VOCS_DESCRIPTION";
    case BT_UUID_CSIS_SIRK_VAL:                 return "CSIS_SIRK";
    case BT_UUID_CSIS_SET_SIZE_VAL:             return "CSIS_SET_SIZE";
    case BT_UUID_CSIS_SET_LOCK_VAL:             return "CSIS_SET_LOCK";
    case BT_UUID_CSIS_RANK_VAL:                 return "CSIS_RANK";
    case BT_UUID_MCS_PLAYER_NAME_VAL:           return "MCS_PLAYER_NAME";
    case BT_UUID_MCS_ICON_OBJ_ID_VAL:           return "MCS_ICON_OBJ_ID";
    case BT_UUID_MCS_ICON_URL_VAL:              return "MCS_ICON_URL";
    case BT_UUID_MCS_TRACK_CHANGED_VAL:         return "MCS_TRACK_CHANGED";
    case BT_UUID_MCS_TRACK_TITLE_VAL:           return "MCS_TRACK_TITLE";
    case BT_UUID_MCS_TRACK_DURATION_VAL:        return "MCS_TRACK_DURATION";
    case BT_UUID_MCS_TRACK_POSITION_VAL:        return "MCS_TRACK_POSITION";
    case BT_UUID_MCS_PLAYBACK_SPEED_VAL:        return "MCS_PLAYBACK_SPEED";
    case BT_UUID_MCS_SEEKING_SPEED_VAL:         return "MCS_SEEKING_SPEED";
    case BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL: return "MCS_TRACK_SEGMENTS_OBJ_ID";
    case BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL:  return "MCS_CURRENT_TRACK_OBJ_ID";
    case BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL:     return "MCS_NEXT_TRACK_OBJ_ID";
    case BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL:   return "MCS_PARENT_GROUP_OBJ_ID";
    case BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL:  return "MCS_CURRENT_GROUP_OBJ_ID";
    case BT_UUID_MCS_PLAYING_ORDER_VAL:         return "MCS_PLAYING_ORDER";
    case BT_UUID_MCS_PLAYING_ORDERS_VAL:        return "MCS_PLAYING_ORDERS";
    case BT_UUID_MCS_MEDIA_STATE_VAL:           return "MCS_MEDIA_STATE";
    case BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL:   return "MCS_MEDIA_CONTROL_POINT";
    case BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL: return "MCS_MEDIA_CONTROL_OPCODES";
    case BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL: return "MCS_SEARCH_RESULTS_OBJ_ID";
    case BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL:  return "MCS_SEARCH_CONTROL_POINT";
    case BT_UUID_TBS_PROVIDER_NAME_VAL:         return "TBS_PROVIDER_NAME";
    case BT_UUID_TBS_UCI_VAL:                   return "TBS_UCI";
    case BT_UUID_TBS_TECHNOLOGY_VAL:            return "TBS_TECHNOLOGY";
    case BT_UUID_TBS_URI_LIST_VAL:              return "TBS_URI_LIST";
    case BT_UUID_TBS_SIGNAL_STRENGTH_VAL:       return "TBS_SIGNAL_STRENGTH";
    case BT_UUID_TBS_SIGNAL_INTERVAL_VAL:       return "TBS_SIGNAL_INTERVAL";
    case BT_UUID_TBS_LIST_CURRENT_CALLS_VAL:    return "TBS_LIST_CURRENT_CALLS";
    case BT_UUID_CCID_VAL:                      return "CCID";
    case BT_UUID_TBS_STATUS_FLAGS_VAL:          return "TBS_STATUS_FLAGS";
    case BT_UUID_TBS_INCOMING_URI_VAL:          return "TBS_INCOMING_URI";
    case BT_UUID_TBS_CALL_STATE_VAL:            return "TBS_CALL_STATE";
    case BT_UUID_TBS_CALL_CONTROL_POINT_VAL:    return "TBS_CALL_CONTROL_POINT";
    case BT_UUID_TBS_OPTIONAL_OPCODES_VAL:      return "TBS_OPTIONAL_OPCODES";
    case BT_UUID_TBS_TERMINATE_REASON_VAL:      return "TBS_TERMINATE_REASON";
    case BT_UUID_TBS_INCOMING_CALL_VAL:         return "TBS_INCOMING_CALL";
    case BT_UUID_TBS_FRIENDLY_NAME_VAL:         return "TBS_FRIENDLY_NAME";
    case BT_UUID_MICS_MUTE_VAL:                 return "MICS_MUTE";
    case BT_UUID_ASCS_ASE_SNK_VAL:              return "ASCS_ASE_SNK";
    case BT_UUID_ASCS_ASE_SRC_VAL:              return "ASCS_ASE_SRC";
    case BT_UUID_ASCS_ASE_CP_VAL:               return "ASCS_ASE_CP";
    case BT_UUID_BASS_CONTROL_POINT_VAL:        return "BASS_CP";
    case BT_UUID_BASS_RECV_STATE_VAL:           return "BASS_RECV_STATE";
    case BT_UUID_PACS_SNK_VAL:                  return "PACS_SNK";
    case BT_UUID_PACS_SNK_LOC_VAL:              return "PACS_SNK_LOC";
    case BT_UUID_PACS_SRC_VAL:                  return "PACS_SRC";
    case BT_UUID_PACS_SRC_LOC_VAL:              return "PACS_SRC_LOC";
    case BT_UUID_PACS_AVAILABLE_CONTEXT_VAL:    return "PACS_AVAILABLE_CONTEXT";
    case BT_UUID_PACS_SUPPORTED_CONTEXT_VAL:    return "PACS_SUPPORTED_CONTEXT";
    case BT_UUID_HAS_HEARING_AID_FEATURES_VAL:  return "HAS_HEARING_AID_FEATURES";
    case BT_UUID_HAS_PRESET_CONTROL_POINT_VAL:  return "HAS_PRESET_CONTROL_POINT";
    case BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL:   return "HAS_ACTIVE_PRESET_INDEX";
    case BT_UUID_GMAP_ROLE_VAL:                 return "GMAP_ROLE";
    case BT_UUID_GMAP_UGG_FEAT_VAL:             return "GMAP_UGG_FEAT";
    case BT_UUID_GMAP_UGT_FEAT_VAL:             return "GMAP_UGT_FEAT";
    case BT_UUID_GMAP_BGS_FEAT_VAL:             return "GMAP_BGS_FEAT";
    case BT_UUID_GMAP_BGR_FEAT_VAL:             return "GMAP_BGR_FEAT";
    default:                                    return "UnknownChrc";
    }
}

enum {
    AICS_IN_PROGRESS,
    ASCS_IN_PROGRESS,
    BASS_IN_PROGRESS,
    CAS_IN_PROGRESS,
    CSIS_IN_PROGRESS,
    HAS_IN_PROGRESS,
    GMCS_IN_PROGRESS,
    MCS_IN_PROGRESS,
    MICS_IN_PROGRESS,
    PACS_IN_PROGRESS,
    GTBS_IN_PROGRESS,
    TBS_IN_PROGRESS,
    TMAS_IN_PROGRESS,
    VCS_IN_PROGRESS,
    VOCS_IN_PROGRESS,

    MAX_IN_PROGRESS,
};

void bt_le_bluedroid_audio_gatts_init(void);

int bt_le_bluedroid_set_svc_in_progress(uint8_t value);

int bt_le_bluedroid_svc_init(struct bt_gatt_service *svc);

int bt_le_bluedroid_svc_start(struct bt_gatt_service *svc);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_SERVER_H_ */
