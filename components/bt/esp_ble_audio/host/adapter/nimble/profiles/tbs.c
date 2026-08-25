/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
#include "common/audio_attr.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_TBS, CONFIG_BT_ISO_LOG_LEVEL);

static const struct ble_gatt_svc_def gatt_svc_gtbs[] =  {
    {
        /* Generic Telephone Bearer Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_GTBS_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_PROVIDER_NAME_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_UCI_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_TECHNOLOGY_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_URI_LIST_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_SIGNAL_STRENGTH_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_SIGNAL_INTERVAL_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_READ_ENC | \
                BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_LIST_CURRENT_CALLS_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_CCID_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_STATUS_FLAGS_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_INCOMING_URI_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_CALL_STATE_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_CALL_CONTROL_POINT_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_OPTIONAL_OPCODES_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_TERMINATE_REASON_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_INCOMING_CALL_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_TBS_FRIENDLY_NAME_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_NOTIFY_INDICATE_ENC,
                .min_key_size = 16,
            }, {
                0, /* No more characteristics in this service. */
            }
        }
    },
    {
        0, /* No more services. */
    },
};

#if CONFIG_BT_TBS_BEARER_COUNT > 0
static const ble_uuid16_t tbs_uuid_svc = BLE_UUID16_INIT(BT_UUID_TBS_VAL);

/* A def and a characteristic table per bearer: val_handle lives in the table,
 * so a shared one would only ever hold the last bearer's handles. */
static BT_AUDIO_EXT_RAM_BSS_ATTR struct ble_gatt_svc_def *tbs_svc_defs[CONFIG_BT_TBS_BEARER_COUNT];
static BT_AUDIO_EXT_RAM_BSS_ATTR uint16_t tbs_anchor_handle[CONFIG_BT_TBS_BEARER_COUNT];

static struct ble_gatt_svc_def *tbs_svc_def_new(uint16_t *anchor)
{
    const struct ble_gatt_chr_def *src = gatt_svc_gtbs[0].characteristics;
    struct ble_gatt_chr_def *chrs;
    struct ble_gatt_svc_def *def;
    size_t count = 0;

    while (src[count].uuid) {
        count++;
    }

    def = bt_le_ext_calloc(2, sizeof(*def));
    chrs = bt_le_ext_calloc(count + 1, sizeof(*chrs));
    if (!def || !chrs) {
        free(def);
        free(chrs);
        return NULL;
    }

    memcpy(chrs, src, count * sizeof(*chrs));
    chrs[0].val_handle = anchor;

    def[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    def[0].uuid = &tbs_uuid_svc.u;
    def[0].characteristics = chrs;

    return def;
}
#endif /* CONFIG_BT_TBS_BEARER_COUNT > 0 */

int bt_le_nimble_gtbs_attr_handle_set(void)
{
    struct bt_gatt_service *gtbs_svc;
    uint16_t handle = 0;
    int rc;

    /* App may not register this svc (e.g. CAP Acceptor single mode keeps
     * unused capability built). Skip rather than fail audio_start.
     */
    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_GTBS_VAL), &handle);
    if (rc) {
        LOG_DBG("[N]GtbsNotInit");
        return 0;
    }

    gtbs_svc = lib_gtbs_svc_get();
    if (!gtbs_svc) {
        LOG_ERR("[N]GtbsSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]GtbsAttrHdlSet[%u][%u]", handle, gtbs_svc->attr_count);

    for (size_t i = 0; i < gtbs_svc->attr_count; i++) {
        (gtbs_svc->attrs + i)->handle = handle + i;
    }

    return 0;
}

static int gtbs_svc_check(void)
{
    struct bt_gatt_service *gtbs_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    gtbs_svc = lib_gtbs_svc_get();
    if (!gtbs_svc) {
        LOG_ERR("[N]GtbsSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]GtbsSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_gtbs[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < gtbs_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(gtbs_svc->attrs + i)->uuid;

            if (uuid && uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]GtbsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_gtbs_init(void)
{
    int rc;

    LOG_DBG("[N]GtbsInit");

    rc = ble_gatts_count_cfg(gatt_svc_gtbs);
    if (rc) {
        LOG_ERR("[N]GtbsCountCfgFail[%d]", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svc_gtbs);
    if (rc) {
        LOG_ERR("[N]GtbsAddSvcsFail[%d]", rc);
        return rc;
    }

    rc = gtbs_svc_check();
    if (rc) {
        return rc;
    }

    return 0;
}

int bt_le_nimble_tbs_attr_handle_set(void)
{
#if CONFIG_BT_TBS_BEARER_COUNT > 0
    struct bt_gatt_service *tbs_list;
    const struct bt_uuid_16 *uuid;
    uint16_t base;

    tbs_list = lib_tbs_server_list_get();
    if (!tbs_list) {
        LOG_DBG("[N]TbsNotInit");
        return 0;
    }

    for (int i = 0; i < CONFIG_BT_TBS_BEARER_COUNT; i++) {
        struct bt_gatt_service *svc = &tbs_list[i];

        /* Zero when the bearer was registered after the boot's ble_gatts_start():
         * it never reached the ATT database, so there is no range to anchor. */
        if (!tbs_svc_defs[i] || tbs_anchor_handle[i] < 2) {
            continue;
        }

        base = tbs_anchor_handle[i] - 2;    /* service decl & char def handle */

        /* Holds only while the lib orders this bearer the way NimBLE registered it. */
        uuid = svc->attr_count > 2 ? (const struct bt_uuid_16 *)svc->attrs[2].uuid : NULL;

        if (!uuid || uuid->uuid.type != BT_UUID_TYPE_16 ||
                uuid->val != BT_UUID_TBS_PROVIDER_NAME_VAL) {
            LOG_ERR("[N]TbsAnchorMismatch[%d][%u]", i, tbs_anchor_handle[i]);
            return -1;
        }

        LOG_DBG("[N]TbsAttrHdlSet[%d][%u][%u]", i, base, svc->attr_count);

        for (size_t j = 0; j < svc->attr_count; j++) {
            (svc->attrs + j)->handle = base + j;
        }
    }
#endif /* CONFIG_BT_TBS_BEARER_COUNT > 0 */

    return 0;
}

#if CONFIG_BT_TBS_BEARER_COUNT > 0
static int tbs_svc_check(void)
{
    struct bt_gatt_service *tbs_list;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    tbs_list = lib_tbs_server_list_get();
    if (!tbs_list) {
        LOG_ERR("[N]TbsSvcListGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]TbsSvcCheck");

    /* Every bearer's table is a copy of this one. */
    for (const struct ble_gatt_chr_def *chr = gatt_svc_gtbs[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < tbs_list[0].attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(tbs_list[0].attrs + i)->uuid;

            if (uuid && uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]TbsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}
#endif /* CONFIG_BT_TBS_BEARER_COUNT > 0 */

int bt_le_nimble_tbs_init(void)
{
#if CONFIG_BT_TBS_BEARER_COUNT > 0
    struct bt_gatt_service *tbs_list;
    int rc;
    int i;

    tbs_list = lib_tbs_server_list_get();
    if (!tbs_list) {
        return 0;
    }

    /* Runs once per bearer registration and gets no index, so sweep for the slot
     * that is registered but not yet added - as bt_le_bluedroid_tbs_init() does. */
    for (i = 0; i < CONFIG_BT_TBS_BEARER_COUNT; i++) {
        if (tbs_list[i].attr_count == 0 || tbs_svc_defs[i]) {
            continue;
        }

        LOG_DBG("[N]TbsInit[%d]", i);

        tbs_svc_defs[i] = tbs_svc_def_new(&tbs_anchor_handle[i]);
        if (!tbs_svc_defs[i]) {
            LOG_ERR("[N]TbsSvcDefAllocFail[%d]", i);
            return -ENOMEM;
        }

        rc = ble_gatts_count_cfg(tbs_svc_defs[i]);
        if (rc) {
            LOG_ERR("[N]TbsCountCfgFail[%d]", rc);
            goto free;
        }

        rc = ble_gatts_add_svcs(tbs_svc_defs[i]);
        if (rc) {
            LOG_ERR("[N]TbsAddSvcsFail[%d]", rc);
            goto free;
        }
    }

    return tbs_svc_check();

free:
    /* Only reachable before ble_gatts_add_svcs() succeeded; after that NimBLE
     * holds the def and it must be leaked instead. */
    free((void *)tbs_svc_defs[i]->characteristics);
    free(tbs_svc_defs[i]);
    tbs_svc_defs[i] = NULL;
    return rc;
#else
    return 0;
#endif /* CONFIG_BT_TBS_BEARER_COUNT > 0 */
}

int bt_le_nimble_gtbs_deinit(void)
{
    LOG_DBG("[N]GtbsDeinit");
    return 0;
}

int bt_le_nimble_tbs_deinit(uint8_t bearer_index)
{
    LOG_DBG("[N]TbsDeinit[%u]", bearer_index);
    return 0;
}
