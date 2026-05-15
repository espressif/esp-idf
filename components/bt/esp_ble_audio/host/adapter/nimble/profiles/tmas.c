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

LOG_MODULE_REGISTER(LEA_TMAS, CONFIG_BT_ISO_LOG_LEVEL);

static const struct ble_gatt_svc_def gatt_svc_tmas[] = {
    {
        /* Telephony and Media Audio Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_TMAS_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_GATT_TMAPR_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ,
                .min_key_size = 0,
                .val_handle = NULL,
            }, {
                0, /* No more characteristics in this service. */
            }
        }
    },
    {
        0, /* No more services. */
    },
};

int bt_le_nimble_tmas_attr_handle_set(void)
{
    struct bt_gatt_service *tmas_svc;
    uint16_t handle = 0;
    int rc;

    /* App may not register this svc (e.g. CAP Acceptor single mode keeps
     * unused capability built). Skip rather than fail audio_start.
     */
    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_TMAS_VAL), &handle);
    if (rc) {
        LOG_DBG("[N]TmasNotInit");
        return 0;
    }

    tmas_svc = lib_tmas_svc_get();
    assert(tmas_svc);

    LOG_DBG("[N]TmasAttrHdlSet[%u][%u]", handle, tmas_svc->attr_count);

    for (size_t i = 0; i < tmas_svc->attr_count; i++) {
        (tmas_svc->attrs + i)->handle = handle + i;
    }

    return 0;
}

static int tmas_svc_check(void)
{
    struct bt_gatt_service *tmas_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    tmas_svc = lib_tmas_svc_get();
    assert(tmas_svc);

    LOG_DBG("[N]TmasSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_tmas[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < tmas_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(tmas_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]TmasChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_tmas_init(void)
{
    int rc;

    LOG_DBG("[N]TmasInit");

    rc = ble_gatts_count_cfg(gatt_svc_tmas);
    if (rc) {
        LOG_ERR("[N]TmasCountCfgFail[%d]", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svc_tmas);
    if (rc) {
        LOG_ERR("[N]TmasAddSvcsFail[%d]", rc);
        return rc;
    }

    rc = tmas_svc_check();
    if (rc) {
        return rc;
    }

    return 0;
}
