/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/audio/micp.h>
#include <zephyr/bluetooth/audio/vcp.h>

#include <../host/conn_internal.h>

#include "bta/bta_gatt_api.h"

#include "bluedroid/gatt.h"
#include "bluedroid/server.h"

#include "common/host.h"

LOG_MODULE_REGISTER(LEA_GSRV, CONFIG_BT_ISO_LOG_LEVEL);

static uint8_t svc_in_progress;

static uint16_t inc_svc_handle;
static uint16_t svc_handle;
static uint16_t chrc_handle;

static bool is_primary_svc(void)
{
    if (svc_in_progress == ASCS_IN_PROGRESS ||
            svc_in_progress == BASS_IN_PROGRESS ||
            svc_in_progress == CAS_IN_PROGRESS ||
            svc_in_progress == CSIS_IN_PROGRESS ||
            svc_in_progress == HAS_IN_PROGRESS ||
            svc_in_progress == GMCS_IN_PROGRESS ||
            svc_in_progress == MCS_IN_PROGRESS ||
            svc_in_progress == MICS_IN_PROGRESS ||
            svc_in_progress == PACS_IN_PROGRESS ||
            svc_in_progress == GTBS_IN_PROGRESS ||
            svc_in_progress == TBS_IN_PROGRESS ||
            svc_in_progress == TMAS_IN_PROGRESS ||
            svc_in_progress == VCS_IN_PROGRESS) {
        return true;
    }

    return false;
}

static bool is_secondary_svc(void)
{
    if (svc_in_progress == AICS_IN_PROGRESS ||
            svc_in_progress == VOCS_IN_PROGRESS) {
        return true;
    }

    return false;
}

static bool any_included_svc(void)
{
    if (svc_in_progress == CAS_IN_PROGRESS ||
            svc_in_progress == MICS_IN_PROGRESS ||
            svc_in_progress == VCS_IN_PROGRESS) {
        return true;
    }

    return false;
}

static bool is_svc_uuid_valid(uint16_t uuid)
{
    switch (svc_in_progress) {
    case AICS_IN_PROGRESS:
        return (uuid == BT_UUID_AICS_VAL);

    case ASCS_IN_PROGRESS:
        return (uuid == BT_UUID_ASCS_VAL);

    case BASS_IN_PROGRESS:
        return (uuid == BT_UUID_BASS_VAL);

    case CAS_IN_PROGRESS:
        return (uuid == BT_UUID_CAS_VAL);

    case CSIS_IN_PROGRESS:
        return (uuid == BT_UUID_CSIS_VAL);

    case HAS_IN_PROGRESS:
        return (uuid == BT_UUID_HAS_VAL);

    case GMCS_IN_PROGRESS:
        return (uuid == BT_UUID_GMCS_VAL);

    case MCS_IN_PROGRESS:
        return (uuid == BT_UUID_MCS_VAL);

    case MICS_IN_PROGRESS:
        return (uuid == BT_UUID_MICS_VAL);

    case PACS_IN_PROGRESS:
        return (uuid == BT_UUID_PACS_VAL);

    case GTBS_IN_PROGRESS:
        return (uuid == BT_UUID_GTBS_VAL);

    case TBS_IN_PROGRESS:
        return (uuid == BT_UUID_TBS_VAL);

    case TMAS_IN_PROGRESS:
        return (uuid == BT_UUID_TMAS_VAL);

    case VCS_IN_PROGRESS:
        return (uuid == BT_UUID_VCS_VAL);

    case VOCS_IN_PROGRESS:
        return (uuid == BT_UUID_VOCS_VAL);

    default:
        return false;
    }
}

static void svc_create_cb(uint16_t service_id, uint16_t svc_instance,
                          bool is_primary, uint8_t status, void *uuid)
{
    int result = status;

    if ((is_primary_svc() && is_primary == false) ||
            (is_secondary_svc() && is_primary == true) ||
            ((tBT_UUID *)uuid)->len != 2 ||
            is_svc_uuid_valid(((tBT_UUID *)uuid)->uu.uuid16) == false) {
        /* uuid16 is only meaningful when len == 2; for 32/128-bit UUIDs the
         * union access would log the first 2 bytes of a wider value. */
        if (((tBT_UUID *)uuid)->len == 2) {
            LOG_ERR("[B]SvcCreateMismatch[%u][%u][2][0x%04x]",
                    svc_in_progress, is_primary,
                    ((tBT_UUID *)uuid)->uu.uuid16);
        } else {
            LOG_ERR("[B]SvcCreateMismatch[%u][%u][%u][nonU16]",
                    svc_in_progress, is_primary,
                    ((tBT_UUID *)uuid)->len);
        }
        result = -1;
        goto end;
    }

    svc_handle = service_id;

end:
    bt_le_bluedroid_gatts_sem_give(result);
}

static void inc_svc_add_cb(uint16_t service_id, uint16_t attr_id, uint8_t status)
{
    int result = status;

    if (service_id != svc_handle) {
        LOG_ERR("[B]IncSvcAddMismatch[%u][%u][%u]",
                svc_in_progress, service_id, svc_handle);
        result = -1;
        goto end;
    }

    inc_svc_handle = attr_id;

end:
    bt_le_bluedroid_gatts_sem_give(result);
}

static void chrc_add_cb(uint16_t service_id, uint16_t attr_id,
                        uint8_t status, void *uuid)
{
    int result = status;

    if (service_id != svc_handle || ((tBT_UUID *)uuid)->len != 2) {
        /* uuid16 is only meaningful when len == 2; for 32/128-bit UUIDs the
         * union access would log the first 2 bytes of a wider value. */
        if (((tBT_UUID *)uuid)->len == 2) {
            LOG_ERR("[B]ChrcAddMismatch[%u][%u][%u][2][0x%04x]",
                    svc_in_progress, service_id, svc_handle,
                    ((tBT_UUID *)uuid)->uu.uuid16);
        } else {
            LOG_ERR("[B]ChrcAddMismatch[%u][%u][%u][%u][nonU16]",
                    svc_in_progress, service_id, svc_handle,
                    ((tBT_UUID *)uuid)->len);
        }
        result = -1;
        goto end;
    }

    chrc_handle = attr_id;

end:
    bt_le_bluedroid_gatts_sem_give(result);
}

static void svc_start_cb(uint16_t service_id, uint8_t status)
{
    int result = status;

    if (service_id != svc_handle) {
        LOG_ERR("[B]SvcStartMismatch[%u][%u]", service_id, svc_handle);
        result = -1;
    }

    bt_le_bluedroid_gatts_sem_give(result);
}

static struct gatts_svc_cb svc_cb = {
    .svc_create_cb  = svc_create_cb,
    .inc_svc_add_cb = inc_svc_add_cb,
    .chrc_add_cb    = chrc_add_cb,
    .svc_start_cb   = svc_start_cb,
};

void bt_le_bluedroid_audio_gatts_init(void)
{
    bt_le_bluedroid_gatts_svc_cb_register(&svc_cb);
}

int bt_le_bluedroid_set_svc_in_progress(uint8_t value)
{
    if (value >= MAX_IN_PROGRESS) {
        LOG_ERR("[B]SvcInProgressInv[%u]", value);
        return -1;
    }

    svc_in_progress = value;
    return 0;
}

static bool is_svc_attr_uuid_valid(uint16_t attr_uuid)
{
    switch (svc_in_progress) {
    case AICS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_AICS_STATE_VAL ||
                attr_uuid == BT_UUID_AICS_GAIN_SETTINGS_VAL ||
                attr_uuid == BT_UUID_AICS_INPUT_TYPE_VAL ||
                attr_uuid == BT_UUID_AICS_INPUT_STATUS_VAL ||
                attr_uuid == BT_UUID_AICS_CONTROL_VAL ||
                attr_uuid == BT_UUID_AICS_DESCRIPTION_VAL);

    case ASCS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_ASCS_ASE_SNK_VAL ||
                attr_uuid == BT_UUID_ASCS_ASE_SRC_VAL ||
                attr_uuid == BT_UUID_ASCS_ASE_CP_VAL);

    case BASS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_BASS_CONTROL_POINT_VAL ||
                attr_uuid == BT_UUID_BASS_RECV_STATE_VAL);

    case CAS_IN_PROGRESS:
        /* No characteristic in CAS */
        return false;

    case CSIS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_CSIS_SIRK_VAL ||
                attr_uuid == BT_UUID_CSIS_SET_SIZE_VAL ||
                attr_uuid == BT_UUID_CSIS_SET_LOCK_VAL ||
                attr_uuid == BT_UUID_CSIS_RANK_VAL);

    case HAS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_HAS_HEARING_AID_FEATURES_VAL ||
                attr_uuid == BT_UUID_HAS_PRESET_CONTROL_POINT_VAL ||
                attr_uuid == BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL);

    case GMCS_IN_PROGRESS:
    case MCS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_MCS_PLAYER_NAME_VAL ||
                attr_uuid == BT_UUID_MCS_ICON_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_ICON_URL_VAL ||
                attr_uuid == BT_UUID_MCS_TRACK_CHANGED_VAL ||
                attr_uuid == BT_UUID_MCS_TRACK_TITLE_VAL ||
                attr_uuid == BT_UUID_MCS_TRACK_DURATION_VAL ||
                attr_uuid == BT_UUID_MCS_TRACK_POSITION_VAL ||
                attr_uuid == BT_UUID_MCS_PLAYBACK_SPEED_VAL ||
                attr_uuid == BT_UUID_MCS_SEEKING_SPEED_VAL ||
                attr_uuid == BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_PLAYING_ORDER_VAL ||
                attr_uuid == BT_UUID_MCS_PLAYING_ORDERS_VAL ||
                attr_uuid == BT_UUID_MCS_MEDIA_STATE_VAL ||
                attr_uuid == BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL ||
                attr_uuid == BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL ||
                attr_uuid == BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL ||
                attr_uuid == BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL ||
                attr_uuid == BT_UUID_CCID_VAL);

    case MICS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_MICS_MUTE_VAL);

    case PACS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_PACS_SNK_VAL ||
                attr_uuid == BT_UUID_PACS_SNK_LOC_VAL ||
                attr_uuid == BT_UUID_PACS_SRC_VAL ||
                attr_uuid == BT_UUID_PACS_SRC_LOC_VAL ||
                attr_uuid == BT_UUID_PACS_AVAILABLE_CONTEXT_VAL ||
                attr_uuid == BT_UUID_PACS_SUPPORTED_CONTEXT_VAL);

    case GTBS_IN_PROGRESS:
    case TBS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_TBS_PROVIDER_NAME_VAL ||
                attr_uuid == BT_UUID_TBS_UCI_VAL ||
                attr_uuid == BT_UUID_TBS_TECHNOLOGY_VAL ||
                attr_uuid == BT_UUID_TBS_URI_LIST_VAL ||
                attr_uuid == BT_UUID_TBS_SIGNAL_STRENGTH_VAL ||
                attr_uuid == BT_UUID_TBS_SIGNAL_INTERVAL_VAL ||
                attr_uuid == BT_UUID_TBS_LIST_CURRENT_CALLS_VAL ||
                attr_uuid == BT_UUID_CCID_VAL ||
                attr_uuid == BT_UUID_TBS_STATUS_FLAGS_VAL ||
                attr_uuid == BT_UUID_TBS_INCOMING_URI_VAL ||
                attr_uuid == BT_UUID_TBS_CALL_STATE_VAL ||
                attr_uuid == BT_UUID_TBS_CALL_CONTROL_POINT_VAL ||
                attr_uuid == BT_UUID_TBS_OPTIONAL_OPCODES_VAL ||
                attr_uuid == BT_UUID_TBS_TERMINATE_REASON_VAL ||
                attr_uuid == BT_UUID_TBS_INCOMING_CALL_VAL ||
                attr_uuid == BT_UUID_TBS_FRIENDLY_NAME_VAL);

    case TMAS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_GATT_TMAPR_VAL);

    case VCS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_VCS_STATE_VAL ||
                attr_uuid == BT_UUID_VCS_CONTROL_VAL ||
                attr_uuid == BT_UUID_VCS_FLAGS_VAL);

    case VOCS_IN_PROGRESS:
        return (attr_uuid == BT_UUID_VOCS_STATE_VAL ||
                attr_uuid == BT_UUID_VOCS_LOCATION_VAL ||
                attr_uuid == BT_UUID_VOCS_CONTROL_VAL ||
                attr_uuid == BT_UUID_VOCS_DESCRIPTION_VAL);

    default:
        return false;
    }
}

static struct inc_svc_inst *get_not_included_inst(void)
{
    switch (svc_in_progress) {
#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
    case CAS_IN_PROGRESS:
        extern struct inc_svc_inst *cas_not_included_inst(void);
        return cas_not_included_inst();
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

#if CONFIG_BT_MICP_MIC_DEV
    case MICS_IN_PROGRESS:
        extern struct inc_svc_inst *mics_not_included_inst(void);
        return mics_not_included_inst();
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_VCP_VOL_REND
    case VCS_IN_PROGRESS:
        extern struct inc_svc_inst *vcs_not_included_inst(void);
        return vcs_not_included_inst();
#endif /* CONFIG_BT_VCP_VOL_REND */

    default:
        return NULL;
    }
}

static uint8_t get_svc_inst_id(uint16_t svc_uuid)
{
    /* Note:
     * For LE Audio, some service could have multiple instances.
     */

    static uint8_t aics_count;
    static uint8_t csis_count;
    static uint8_t vocs_count;
    static uint8_t mcs_count;
    static uint8_t ots_count;
    static uint8_t tbs_count;

    switch (svc_uuid) {
    case BT_UUID_AICS_VAL:
        return aics_count++;

    case BT_UUID_CSIS_VAL:
        return csis_count++;

    case BT_UUID_VOCS_VAL:
        return vocs_count++;

    case BT_UUID_MCS_VAL:
        return mcs_count++;

    case BT_UUID_OTS_VAL:
        return ots_count++;

    case BT_UUID_TBS_VAL:
        return tbs_count++;

    default:
        return 0;
    }
}

int bt_le_bluedroid_svc_init(struct bt_gatt_service *svc)
{
    struct bt_gatt_attr *curr_attr;
    struct bt_gatt_attr *next_attr;
    struct inc_svc_inst *inc_inst;
    struct bt_gatt_chrc *chrc;
    tBTA_GATT_PERM perm;
    uint16_t attr_uuid;
    uint8_t inst_id;
    tBT_UUID uuid;

    assert(svc);

    for (size_t i = 0; i < svc->attr_count; i++) {
        curr_attr = &svc->attrs[i];

        attr_uuid = BT_UUID_16(curr_attr->uuid)->val;

        switch (attr_uuid) {
        case BT_UUID_GATT_PRIMARY_VAL:
            if (is_primary_svc() == false) {
                LOG_ERR("[B]NotCreatingPrimary");
                return -1;
            }

            assert(curr_attr->user_data);

            bt_le_bluedroid_gatt_uuid_convert(curr_attr->user_data, &uuid);
            inst_id = get_svc_inst_id(uuid.uu.uuid16);

            bt_le_bluedroid_gatts_sem_reset();
            BTA_GATTS_CreateService(bt_le_bluedroid_gatts_get_if(), &uuid, inst_id, svc->attr_count, true);

            if (bt_le_bluedroid_gatts_sem_take()) {
                LOG_ERR("[B]PrimaryCreateFail");
                return -1;
            }

            curr_attr->handle = svc_handle;

            LOG_INF("[B]PrimarySvc[%s][0x%04x][%u][0x%04x][%d]",
                    audio_svc_uuid_to_str(uuid.uu.uuid16), uuid.uu.uuid16,
                    inst_id, curr_attr->perm, svc_handle);
            break;

        case BT_UUID_GATT_SECONDARY_VAL:
            if (is_secondary_svc() == false) {
                LOG_ERR("[B]NotCreatingSecondary");
                return -1;
            }

            assert(curr_attr->user_data);

            bt_le_bluedroid_gatt_uuid_convert(curr_attr->user_data, &uuid);
            inst_id = get_svc_inst_id(uuid.uu.uuid16);

            bt_le_bluedroid_gatts_sem_reset();
            BTA_GATTS_CreateService(bt_le_bluedroid_gatts_get_if(), &uuid, inst_id, svc->attr_count, false);

            if (bt_le_bluedroid_gatts_sem_take()) {
                LOG_ERR("[B]SecondaryCreateFail");
                return -1;
            }

            curr_attr->handle = svc_handle;

            LOG_INF("[B]SecondarySvc[%s][0x%04x][%u][0x%04x][%d]",
                    audio_svc_uuid_to_str(uuid.uu.uuid16), uuid.uu.uuid16,
                    inst_id, curr_attr->perm, svc_handle);
            break;

        case BT_UUID_GATT_INCLUDE_VAL:
            if (any_included_svc() == false) {
                LOG_ERR("[B]NoSvcToInc[%u]", svc_in_progress);
                return -1;
            }

            inc_inst = get_not_included_inst();

            if (inc_inst == NULL) {
                LOG_ERR("[B]IncSvcNotFound[%u]", svc_in_progress);
                return -1;
            }

            bt_le_bluedroid_gatts_sem_reset();
            BTA_GATTS_AddIncludeService(svc_handle, inc_inst->svc_p->attrs[0].handle);

            if (bt_le_bluedroid_gatts_sem_take()) {
                LOG_ERR("[B]IncSvcAddFail");
                return -1;
            }

            curr_attr->handle = inc_svc_handle;
            inc_inst->included = true; /* Mark the svc as included */

            LOG_INF("[B]IncSvc[%s][0x%04x][0x%04x][%d]",
                    audio_svc_uuid_to_str(BT_UUID_16(inc_inst->svc_p->attrs[0].user_data)->val),
                    BT_UUID_16(inc_inst->svc_p->attrs[0].user_data)->val,
                    curr_attr->perm, inc_svc_handle);
            break;

        case BT_UUID_GATT_CHRC_VAL:
            /* Chrc declaration must be followed by its value attribute.
             * Standard BT_GATT_CHARACTERISTIC pairs them, but guard against
             * a malformed table where it is the last entry. */
            if (i + 1 >= svc->attr_count) {
                LOG_ERR("[B]ChrcDeclLastAttr[%u]", svc_in_progress);
                return -1;
            }

            next_attr = &svc->attrs[i + 1];
            perm = bt_le_bluedroid_gatt_perm_convert(next_attr->perm);

            assert(curr_attr->user_data);

            chrc = curr_attr->user_data;
            bt_le_bluedroid_gatt_uuid_convert(chrc->uuid, &uuid);
            bt_le_bluedroid_gatts_sem_reset();
            BTA_GATTS_AddCharacteristic(svc_handle, &uuid, perm, chrc->properties, NULL, NULL);

            if (bt_le_bluedroid_gatts_sem_take()) {
                LOG_ERR("[B]ChrcAddFail");
                return -1;
            }

            /* Characteristic declaration handle and Characteristic value handle */
            curr_attr->handle = chrc_handle - 1;
            next_attr->handle = chrc_handle;

            LOG_INF("[B]Chrc[%s][0x%04x][%u][%u][0x%04x][0x%02x]",
                    audio_chrc_uuid_to_str(uuid.uu.uuid16), uuid.uu.uuid16,
                    chrc_handle - 1, chrc_handle, next_attr->perm, chrc->properties);
            break;

        case BT_UUID_GATT_CCC_VAL:
            perm = bt_le_bluedroid_gatt_perm_convert(curr_attr->perm);
            bt_le_bluedroid_gatt_uuid_convert(curr_attr->uuid, &uuid);
            bt_le_bluedroid_gatts_sem_reset();
            BTA_GATTS_AddCharDescriptor(svc_handle, perm, &uuid, NULL, NULL);

            if (bt_le_bluedroid_gatts_sem_take()) {
                LOG_ERR("[B]ChrcCccdAddFail");
                return -1;
            }

            curr_attr->handle = chrc_handle;

            LOG_INF("[B]ChrcCccd[%u][0x%04x]", chrc_handle, curr_attr->perm);
            break;

        default:
            if (is_svc_attr_uuid_valid(attr_uuid) == false) {
                LOG_ERR("[B]InvSvcAttrUuid[0x%04x][%u]", attr_uuid, svc_in_progress);
                return -1;
            }
            break;
        }
    }

    return 0;
}

int bt_le_bluedroid_svc_start(struct bt_gatt_service *svc)
{
    assert(svc);

    svc_handle = svc->attrs[0].handle;

    /* App may not register this svc (e.g. CAP Acceptor single mode keeps
     * unused capability built). Skip rather than fail audio_start.
     */
    if (svc_handle == 0) {
        LOG_DBG("[B]SvcNotInit[%u]", svc_in_progress);
        return 0;
    }

    bt_le_bluedroid_gatts_sem_reset();
    BTA_GATTS_StartService(svc_handle, BTA_GATT_TRANSPORT_LE);

    if (bt_le_bluedroid_gatts_sem_take()) {
        LOG_ERR("[B]SvcStartFail");
        return -1;
    }

    return 0;
}
