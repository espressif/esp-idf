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

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "nimble/profiles/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_PACS, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_PAC_SNK
static uint16_t pacs_snk_handle;
#if CONFIG_BT_PAC_SNK_LOC
static uint16_t pacs_snk_loc_handle;
#endif /* CONFIG_BT_PAC_SNK_LOC */
#endif /* CONFIG_BT_PAC_SNK */

#if CONFIG_BT_PAC_SRC
static uint16_t pacs_src_handle;
#if CONFIG_BT_PAC_SRC_LOC
static uint16_t pacs_src_loc_handle;
#endif /* CONFIG_BT_PAC_SRC_LOC */
#endif /* CONFIG_BT_PAC_SRC */

static uint16_t pacs_ava_ctx_handle;
static uint16_t pacs_sup_ctx_handle;

static const struct ble_gatt_svc_def gatt_svc_pacs[] = {
    {
        /* Published Audio Capabilities Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        {
#if CONFIG_BT_PAC_SNK
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_SNK_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_PAC_SNK_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
#else /* CONFIG_BT_PAC_SNK_NOTIFIABLE */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
#endif /* CONFIG_BT_PAC_SNK_NOTIFIABLE */
                .min_key_size = 16,
                .val_handle = &pacs_snk_handle,
            },
#if CONFIG_BT_PAC_SNK_LOC
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_SNK_LOC_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_PAC_SNK_LOC_WRITEABLE && CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | \
                BLE_GATT_CHR_F_WRITE_ENC,
#elif CONFIG_BT_PAC_SNK_LOC_WRITEABLE && !CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
#elif !CONFIG_BT_PAC_SNK_LOC_WRITEABLE && CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
#else /* !CONFIG_BT_PAC_SNK_LOC_WRITEABLE && !CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
#endif /* CONFIG_BT_PAC_SNK_LOC_WRITEABLE && CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE */
                .min_key_size = 16,
                .val_handle = &pacs_snk_loc_handle,
            },
#endif /* CONFIG_BT_PAC_SNK_LOC */
#endif /* CONFIG_BT_PAC_SNK */
#if CONFIG_BT_PAC_SRC
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_SRC_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_PAC_SRC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
#else /* CONFIG_BT_PAC_SRC_NOTIFIABLE */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
#endif /* CONFIG_BT_PAC_SRC_NOTIFIABLE */
                .min_key_size = 16,
                .val_handle = &pacs_src_handle,
            },
#if CONFIG_BT_PAC_SRC_LOC
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_SRC_LOC_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_PAC_SRC_LOC_WRITEABLE && CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC | \
                BLE_GATT_CHR_F_WRITE_ENC,
#elif CONFIG_BT_PAC_SRC_LOC_WRITEABLE && !CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | \
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC,
#elif !CONFIG_BT_PAC_SRC_LOC_WRITEABLE && CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
#else /* !CONFIG_BT_PAC_SRC_LOC_WRITEABLE && !CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
#endif /* CONFIG_BT_PAC_SRC_LOC_WRITEABLE && CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE */
                .min_key_size = 16,
                .val_handle = &pacs_src_loc_handle,
            },
#endif /* CONFIG_BT_PAC_SRC_LOC */
#endif /* CONFIG_BT_PAC_SRC */
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_AVAILABLE_CONTEXT_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
                .val_handle = &pacs_ava_ctx_handle,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_PACS_SUPPORTED_CONTEXT_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
#else /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
#endif /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */
                .min_key_size = 16,
                .val_handle = &pacs_sup_ctx_handle,
            }, {
                0, /* No more characteristics in this service. */
            }
        }
    },
    {
        0, /* No more services. */
    },
};

int bt_le_nimble_pacs_attr_handle_set(void)
{
    struct bt_gatt_service *pacs_svc;
    struct bt_gatt_attr *attr;
    uint16_t start_handle;
    uint16_t end_handle;

    pacs_svc = lib_pacs_svc_get();
    assert(pacs_svc);

#if CONFIG_BT_PAC_SNK
    assert(pacs_snk_handle >= 2);
    start_handle = pacs_snk_handle - 2;     /* server attr handle & char def handle */
#elif CONFIG_BT_PAC_SRC
    assert(pacs_src_handle >= 2);
    start_handle = pacs_src_handle - 2;     /* server attr handle & char def handle */
#else
    assert(pacs_ava_ctx_handle >= 2);
    start_handle = pacs_ava_ctx_handle - 2; /* server attr handle & char def handle */
#endif
#if CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE
    end_handle = pacs_sup_ctx_handle + 1;   /* cccd attr handle */
#else /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */
    end_handle = pacs_sup_ctx_handle;       /* char value attr handle */
#endif /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */

    LOG_DBG("[N]PacsAttrHdlSet[%u][%u][%u][%u]",
            start_handle, end_handle, pacs_sup_ctx_handle, pacs_svc->attr_count);

    for (size_t i = 0; i < pacs_svc->attr_count; i++) {
        (pacs_svc->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in PACS */
    attr = pacs_svc->attrs + pacs_svc->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]PacsMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, pacs_svc->attr_count);
        return -1;
    }

    return 0;
}

static int pacs_svc_check(void)
{
    struct bt_gatt_service *pacs_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    pacs_svc = lib_pacs_svc_get();
    assert(pacs_svc);

    LOG_DBG("[N]PacsSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_pacs[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < pacs_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(pacs_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]PacsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_pacs_init(void)
{
    int rc;

    LOG_DBG("[N]PacsInit");

    rc = ble_gatts_count_cfg(gatt_svc_pacs);
    if (rc) {
        LOG_ERR("[N]PacsCountCfgFail[%d]", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svc_pacs);
    if (rc) {
        LOG_ERR("[N]PacsAddSvcsFail[%d]", rc);
        return rc;
    }

    rc = pacs_svc_check();
    if (rc) {
        return rc;
    }

    return 0;
}
