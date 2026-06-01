/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/ots.h>
#include <zephyr/bluetooth/audio/media_proxy.h>

#if CONFIG_BT_OTS
#include "ots_internal.h"
#endif /* CONFIG_BT_OTS */

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "nimble/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_MCS, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_OTS
#define INC_OTS_CHR_COUNT   8   /* - OTS Feature (M)
                                 * - Object Name (M)
                                 * - Object Type (M)
                                 * - Object Size (M)
                                 * - Object ID (C.1)
                                 * - Object Properties (M)
                                 * - Object Action Control Point (M)
                                 * - Object List Control Point (C.1)
                                 */

#define INC_OTS_CHR_FLAGS_FEATURE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#if CONFIG_BT_OTS_OBJ_NAME_WRITE_SUPPORT
#define INC_OTS_CHR_FLAGS_NAME \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC)
#else /* CONFIG_BT_OTS_OBJ_NAME_WRITE_SUPPORT */
#define INC_OTS_CHR_FLAGS_NAME \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)
#endif /* CONFIG_BT_OTS_OBJ_NAME_WRITE_SUPPORT */

#define INC_OTS_CHR_FLAGS_TYPE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#define INC_OTS_CHR_FLAGS_SIZE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#define INC_OTS_CHR_FLAGS_ID \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#define INC_OTS_CHR_FLAGS_PROPS \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#define INC_OTS_CHR_FLAGS_ACTION_CP \
    (BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE | BLE_GATT_CHR_F_WRITE_ENC)

#define INC_OTS_CHR_FLAGS_LIST_CP \
    (BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE | BLE_GATT_CHR_F_WRITE_ENC)

static uint8_t inc_ots_svc_count;

static struct bt_ots *ots;
static uint16_t inc_ots_chr_feature_handle;
static uint16_t inc_ots_chr_name_handle;
static uint16_t inc_ots_chr_type_handle;
static uint16_t inc_ots_chr_size_handle;
static uint16_t inc_ots_chr_id_handle;
static uint16_t inc_ots_chr_props_handle;
static uint16_t inc_ots_chr_action_cp_handle;
static uint16_t inc_ots_chr_list_cp_handle;

static struct ble_gatt_svc_def *gatt_svc_inc_ots;

static struct ble_gatt_svc_def **gmcs_inc_svcs;

static const ble_uuid16_t inc_ots_uuid_svc = BLE_UUID16_INIT(BT_UUID_OTS_VAL);
static const ble_uuid16_t inc_ots_uuid_feature = BLE_UUID16_INIT(BT_UUID_OTS_FEATURE_VAL);
static const ble_uuid16_t inc_ots_uuid_name = BLE_UUID16_INIT(BT_UUID_OTS_NAME_VAL);
static const ble_uuid16_t inc_ots_uuid_type = BLE_UUID16_INIT(BT_UUID_OTS_TYPE_VAL);
static const ble_uuid16_t inc_ots_uuid_size = BLE_UUID16_INIT(BT_UUID_OTS_SIZE_VAL);
static const ble_uuid16_t inc_ots_uuid_id = BLE_UUID16_INIT(BT_UUID_OTS_ID_VAL);
static const ble_uuid16_t inc_ots_uuid_props = BLE_UUID16_INIT(BT_UUID_OTS_PROPERTIES_VAL);
static const ble_uuid16_t inc_ots_uuid_action_cp = BLE_UUID16_INIT(BT_UUID_OTS_ACTION_CP_VAL);
static const ble_uuid16_t inc_ots_uuid_list_cp = BLE_UUID16_INIT(BT_UUID_OTS_LIST_CP_VAL);
#endif /* CONFIG_BT_OTS */

static uint16_t mcs_player_name_handle;
#if CONFIG_BT_OTS
static uint16_t mcs_icon_obj_id_handle;
#endif /* CONFIG_BT_OTS */
static uint16_t mcs_icon_url_handle;
static uint16_t mcs_track_changed_handle;
static uint16_t mcs_track_title_handle;
static uint16_t mcs_track_duration_handle;
static uint16_t mcs_track_position_handle;
static uint16_t mcs_playback_speed_handle;
static uint16_t mcs_seeking_speed_handle;
#if CONFIG_BT_OTS
static uint16_t mcs_track_segments_obj_id_handle;
static uint16_t mcs_current_track_obj_id_handle;
static uint16_t mcs_next_track_obj_id_handle;
static uint16_t mcs_parent_group_obj_id_handle;
static uint16_t mcs_current_group_obj_id_handle;
#endif /* CONFIG_BT_OTS */
static uint16_t mcs_playing_order_handle;
static uint16_t mcs_playing_orders_handle;
static uint16_t mcs_media_state_handle;
static uint16_t mcs_media_control_point_handle;
static uint16_t mcs_media_control_opcodes_handle;
#if CONFIG_BT_OTS
static uint16_t mcs_search_control_point_handle;
static uint16_t mcs_search_results_obj_id_handle;
#endif /* CONFIG_BT_OTS */
static uint16_t ccid_handle;

static struct ble_gatt_svc_def gatt_svc_gmcs[] = {
    {
        /* Published Audio Capabilities Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_GMCS_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_PLAYER_NAME_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_player_name_handle,
            },
#if CONFIG_BT_OTS
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_ICON_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_icon_obj_id_handle,
            },
#endif /* CONFIG_BT_OTS */
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_ICON_URL_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_icon_url_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_TRACK_CHANGED_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_NOTIFY,
                .min_key_size = 16,
                .val_handle = &mcs_track_changed_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_TRACK_TITLE_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_track_title_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_TRACK_DURATION_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_track_duration_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_TRACK_POSITION_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_track_position_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_PLAYBACK_SPEED_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_playback_speed_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_SEEKING_SPEED_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_seeking_speed_handle,
            },
#if CONFIG_BT_OTS
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_track_segments_obj_id_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_current_track_obj_id_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_next_track_obj_id_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_parent_group_obj_id_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_current_group_obj_id_handle,
            },
#endif /* CONFIG_BT_OTS */
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_PLAYING_ORDER_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_playing_order_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_PLAYING_ORDERS_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_playing_orders_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_MEDIA_STATE_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_media_state_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP | \
                BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_media_control_point_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_media_control_opcodes_handle,
            },
#if CONFIG_BT_OTS
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP | \
                BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_search_control_point_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &mcs_search_results_obj_id_handle,
            },
#endif /* CONFIG_BT_OTS */
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_CCID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &ccid_handle,
            }, {
                0, /* No more characteristics in this service. */
            }
        }
    },
    {
        0, /* No more services. */
    },
};

#if CONFIG_BT_OTS
static int inc_ots_attr_handle_set(void)
{
    struct bt_gatt_attr *attr;
    uint16_t start_handle;
    uint16_t end_handle;

    assert(ots && ots->service);

    assert(inc_ots_chr_feature_handle >= 2);
    start_handle = inc_ots_chr_feature_handle - 2;  /* server attr handle & char def handle */
    end_handle = inc_ots_chr_list_cp_handle + 1;    /* cccd for chr Object List Control Point */

    LOG_DBG("[N]IncOtsAttrHdlSet[%u][%u][%u]",
            start_handle, end_handle, ots->service->attr_count);

    for (size_t i = 0; i < ots->service->attr_count; i++) {
        (ots->service->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in included OTS */
    attr = ots->service->attrs + ots->service->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]IncOtsMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, ots->service->attr_count);
        return -1;
    }

    return 0;
}
#endif /* CONFIG_BT_OTS */

int bt_le_nimble_gmcs_attr_handle_set(void)
{
    struct bt_gatt_service *gmcs_svc;
    struct bt_gatt_attr *attr;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;
    int rc;

    /* App may not register this svc (e.g. CAP Acceptor single mode keeps
     * unused capability built). Skip rather than fail audio_start.
     */
    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_GMCS_VAL), &start_handle);
    if (rc) {
        LOG_DBG("[N]GmcsNotInit");
        return 0;
    }

    gmcs_svc = lib_mcs_svc_get();
    if (!gmcs_svc) {
        LOG_ERR("[N]GmcsSvcGetFail");
        return -ENODEV;
    }

    end_handle = start_handle + gmcs_svc->attr_count - 1;

    LOG_DBG("[N]GmcsAttrHdlSet[%u][%u][%u]",
            start_handle, end_handle, gmcs_svc->attr_count);

    for (size_t i = 0; i < gmcs_svc->attr_count; i++) {
        (gmcs_svc->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in MCS */
    attr = gmcs_svc->attrs + gmcs_svc->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]GmcsMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, gmcs_svc->attr_count);
        return -1;
    }

#if CONFIG_BT_OTS
    if (inc_ots_svc_count) {
        rc = inc_ots_attr_handle_set();
        if (rc) {
            return rc;
        }
    }
#endif /* CONFIG_BT_OTS */

    return 0;
}

static int gmcs_svc_check(void)
{
    struct bt_gatt_service *gmcs_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    gmcs_svc = lib_mcs_svc_get();
    if (!gmcs_svc) {
        LOG_ERR("[N]GmcsSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]GmcsSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_gmcs[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < gmcs_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(gmcs_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]GmcsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

#if CONFIG_BT_OTS
static int inc_ots_svc_check(void)
{
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    assert(gatt_svc_inc_ots);
    assert(ots && ots->service);

    LOG_DBG("[N]IncOtsSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_inc_ots[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < ots->service->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(ots->service->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]IncOtsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

static inline void inc_ots_chr_init(struct ble_gatt_chr_def *chr,
                                    const ble_uuid16_t *uuid,
                                    uint16_t *val_handle,
                                    ble_gatt_chr_flags flags)
{
    LOG_DBG("[N]IncOtsChrInit[%04x]", uuid->value);

    chr->uuid = &uuid->u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
    chr->flags = flags;
    chr->min_key_size = 16;
    chr->val_handle = val_handle;
}

static int inc_ots_svc_init(void)
{
    struct ble_gatt_svc_def *svc;
    struct bt_gatt_attr *attrs;
    uint8_t chr_count;

    attrs = bt_ots_svc_decl_get(ots);
    assert(attrs);

    chr_count = 0;

    for (size_t i = 0; i < ots->service->attr_count; i++) {
        if (bt_uuid_cmp(attrs[i].uuid, BT_UUID_GATT_CHRC) == 0) {
            chr_count++;
        }
    }

    LOG_DBG("[N]IncOtsSvcInit[%u]", chr_count);

    if (chr_count != INC_OTS_CHR_COUNT) {
        LOG_ERR("[N]InvOtsChrCount[%u][%u]", chr_count, INC_OTS_CHR_COUNT);
        return -EINVAL;
    }

    svc = &gatt_svc_inc_ots[0];

    svc->type = BLE_GATT_SVC_TYPE_SECONDARY;
    svc->uuid = &inc_ots_uuid_svc.u;
    svc->includes = NULL;

    /* An additional characteristic consist of all 0s indicating end of characteristics */
    svc->characteristics = calloc(INC_OTS_CHR_COUNT + 1, sizeof(struct ble_gatt_chr_def));
    assert(svc->characteristics);

    /* Characteristic - OTS Feature */
    inc_ots_chr_init((void *)&svc->characteristics[0],
                     &inc_ots_uuid_feature,
                     &inc_ots_chr_feature_handle,
                     INC_OTS_CHR_FLAGS_FEATURE);

    /* Characteristic - Object Name */
    inc_ots_chr_init((void *)&svc->characteristics[1],
                     &inc_ots_uuid_name,
                     &inc_ots_chr_name_handle,
                     INC_OTS_CHR_FLAGS_NAME);

    /* Characteristic - Object Type */
    inc_ots_chr_init((void *)&svc->characteristics[2],
                     &inc_ots_uuid_type,
                     &inc_ots_chr_type_handle,
                     INC_OTS_CHR_FLAGS_TYPE);

    /* Characteristic - Object Size */
    inc_ots_chr_init((void *)&svc->characteristics[3],
                     &inc_ots_uuid_size,
                     &inc_ots_chr_size_handle,
                     INC_OTS_CHR_FLAGS_SIZE);

    /* Characteristic - Object ID */
    inc_ots_chr_init((void *)&svc->characteristics[4],
                     &inc_ots_uuid_id,
                     &inc_ots_chr_id_handle,
                     INC_OTS_CHR_FLAGS_ID);

    /* Characteristic - Object Properties */
    inc_ots_chr_init((void *)&svc->characteristics[5],
                     &inc_ots_uuid_props,
                     &inc_ots_chr_props_handle,
                     INC_OTS_CHR_FLAGS_PROPS);

    /* Characteristic - Object Action Control Point */
    inc_ots_chr_init((void *)&svc->characteristics[6],
                     &inc_ots_uuid_action_cp,
                     &inc_ots_chr_action_cp_handle,
                     INC_OTS_CHR_FLAGS_ACTION_CP);

    /* Characteristic - Object List Control Point */
    inc_ots_chr_init((void *)&svc->characteristics[7],
                     &inc_ots_uuid_list_cp,
                     &inc_ots_chr_list_cp_handle,
                     INC_OTS_CHR_FLAGS_LIST_CP);

    return 0;
}
#endif /* CONFIG_BT_OTS */

int bt_le_nimble_gmcs_init(bool ots_included)
{
    int rc;

    LOG_DBG("[N]GmcsInit[%u]", ots_included);

#if CONFIG_BT_OTS
    if (ots_included) {
        inc_ots_svc_count = 1;

        /* Extra one for terminating the included service array with NULL */
        gmcs_inc_svcs = calloc(2, sizeof(struct ble_gatt_svc_def *));
        assert(gmcs_inc_svcs);

        /* Extra one for terminating the OTS service array */
        gatt_svc_inc_ots = calloc(2, sizeof(struct ble_gatt_svc_def));
        assert(gatt_svc_inc_ots);

        ots = lib_mcs_get_ots();
        assert(ots && ots->service);

        rc = inc_ots_svc_init();
        if (rc) {
            goto free;
        }

        gmcs_inc_svcs[0] = &gatt_svc_inc_ots[0];

        rc = ble_gatts_count_cfg(gatt_svc_inc_ots);
        if (rc) {
            LOG_ERR("[N]IncOtsCountCfgFail[%d]", rc);
            goto free;
        }

        rc = ble_gatts_add_svcs(gatt_svc_inc_ots);
        if (rc) {
            LOG_ERR("[N]IncOtsAddSvcsFail[%d]", rc);
            goto free;
        }

        rc = inc_ots_svc_check();
        if (rc) {
            goto free;
        }

        /* Terminate the included service array with NULL */
        gmcs_inc_svcs[1] = NULL;
    }

    gatt_svc_gmcs[0].includes = (const struct ble_gatt_svc_def **)gmcs_inc_svcs;
#endif /* CONFIG_BT_OTS */

    rc = ble_gatts_count_cfg(gatt_svc_gmcs);
    if (rc) {
        LOG_ERR("[N]GmcsCountCfgFail[%d]", rc);
        goto free;
    }

    rc = ble_gatts_add_svcs(gatt_svc_gmcs);
    if (rc) {
        LOG_ERR("[N]GmcsAddSvcsFail[%d]", rc);
        goto free;
    }

    rc = gmcs_svc_check();
    if (rc) {
        goto free;
    }

    return 0;

free:
#if CONFIG_BT_OTS
    if (ots_included) {
        inc_ots_svc_count = 0;

        free(gmcs_inc_svcs);
        gmcs_inc_svcs = NULL;

        if (gatt_svc_inc_ots[0].characteristics) {
            free((void *)gatt_svc_inc_ots[0].characteristics);
            gatt_svc_inc_ots[0].characteristics = NULL;
        }
        free(gatt_svc_inc_ots);
        gatt_svc_inc_ots = NULL;
    }
    gatt_svc_gmcs[0].includes = NULL;
#endif /* CONFIG_BT_OTS */
    return rc;
}
