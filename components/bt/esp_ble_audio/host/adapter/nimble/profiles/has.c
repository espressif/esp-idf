/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "nimble/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_HAS, CONFIG_BT_ISO_LOG_LEVEL);

static const ble_uuid16_t has_uuid_features = BLE_UUID16_INIT(BT_UUID_HAS_HEARING_AID_FEATURES_VAL);
static const ble_uuid16_t has_uuid_control_point = BLE_UUID16_INIT(BT_UUID_HAS_PRESET_CONTROL_POINT_VAL);
static const ble_uuid16_t has_uuid_preset_index = BLE_UUID16_INIT(BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL);

static uint16_t has_features_handle;
static uint16_t has_control_point_handle;
static uint16_t has_preset_index_handle;

static struct ble_gatt_svc_def gatt_svc_has[] = {
    {
        /* Hearing Access Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_HAS_VAL),
        .includes = NULL,
        .characteristics = NULL,
    },
    {
        0, /* No more services. */
    },
};

static void has_svc_add_features_chr(struct ble_gatt_chr_def *chr)
{
    chr->uuid = &has_uuid_features.u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_HAS_FEATURES_NOTIFIABLE
    chr->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC;
#else /* CONFIG_BT_HAS_FEATURES_NOTIFIABLE */
    chr->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
#endif /* CONFIG_BT_HAS_FEATURES_NOTIFIABLE */
    chr->min_key_size = 16;
    chr->val_handle = &has_features_handle;

    LOG_DBG("[N]HasSvcAddFeatChr[%04x]", chr->flags);
}

static void has_svc_add_control_point_chr(struct ble_gatt_chr_def *chr)
{
    chr->uuid = &has_uuid_control_point.u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE
    chr->flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE |
                 BLE_GATT_CHR_F_WRITE_ENC | BLE_GATT_CHR_F_NOTIFY;
#else /* CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE */
    chr->flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE | BLE_GATT_CHR_F_WRITE_ENC;
#endif /* CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE */
    chr->min_key_size = 16;
    chr->val_handle = &has_control_point_handle;

    LOG_DBG("[N]HasSvcAddControlPointChr[%04x]", chr->flags);
}

#if CONFIG_BT_HAS_ACTIVE_PRESET_INDEX
static void has_svc_add_preset_index_chr(struct ble_gatt_chr_def *chr)
{
    LOG_DBG("[N]HasSvcAddPresetIndexChr");

    chr->uuid = &has_uuid_preset_index.u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
    chr->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC;
    chr->min_key_size = 16;
    chr->val_handle = &has_preset_index_handle;
}
#endif /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */

int bt_le_nimble_has_attr_handle_set(void)
{
    struct bt_gatt_service *has_svc;
    struct bt_gatt_attr *attr;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;
    int rc;

    /* App may not register this svc (e.g. CAP Acceptor single mode keeps
     * unused capability built). Skip rather than fail audio_start.
     */
    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_HAS_VAL), &start_handle);
    if (rc) {
        LOG_DBG("[N]HasNotInit");
        return 0;
    }

    has_svc = lib_has_svc_get();
    if (!has_svc) {
        LOG_ERR("[N]HasSvcGetFail");
        return -ENODEV;
    }
    assert(has_svc->attr_count > 0);

    end_handle = start_handle + has_svc->attr_count - 1;

    LOG_DBG("[N]HasAttrHdlSet[%u][%u][%u]",
            start_handle, end_handle, has_svc->attr_count);

    for (size_t i = 0; i < has_svc->attr_count; i++) {
        (has_svc->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in HAS */
    attr = has_svc->attrs + has_svc->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]HasMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, has_svc->attr_count);
        return -1;
    }

    return 0;
}

static int has_svc_check(void)
{
    struct bt_gatt_service *has_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    has_svc = lib_has_svc_get();
    if (!has_svc) {
        LOG_ERR("[N]HasSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]HasSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_has[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < has_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(has_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]HasChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_has_init(void)
{
    uint8_t chr_count;
    int rc;

    /* An additional characteristic consist of all 0s indicating end of characteristics */
#if CONFIG_BT_HAS_ACTIVE_PRESET_INDEX
    chr_count = 1 + 1 + 1 + 1;
#else /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */
    chr_count = 1 + 1 + 1;
#endif /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */

    LOG_DBG("[N]HasInit[%u]", chr_count);

    gatt_svc_has->characteristics = calloc(chr_count, sizeof(struct ble_gatt_chr_def));
    assert(gatt_svc_has->characteristics);

    has_svc_add_features_chr((void *)(gatt_svc_has->characteristics + 0));

    has_svc_add_control_point_chr((void *)(gatt_svc_has->characteristics + 1));

#if CONFIG_BT_HAS_ACTIVE_PRESET_INDEX
    has_svc_add_preset_index_chr((void *)(gatt_svc_has->characteristics + 2));
#endif /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */

    rc = ble_gatts_count_cfg(gatt_svc_has);
    if (rc) {
        LOG_ERR("[N]HasCountCfgFail[%d]", rc);
        goto free;
    }

    rc = ble_gatts_add_svcs(gatt_svc_has);
    if (rc) {
        LOG_ERR("[N]HasAddSvcsFail[%d]", rc);
        goto free;
    }

    rc = has_svc_check();
    if (rc) {
        goto free;
    }

    return 0;

free:
    free((void *)gatt_svc_has->characteristics);
    gatt_svc_has->characteristics = NULL;
    return rc;
}
