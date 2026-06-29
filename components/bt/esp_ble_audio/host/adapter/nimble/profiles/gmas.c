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

#include "nimble/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_GMAS, CONFIG_BT_ISO_LOG_LEVEL);

/* Gaming Audio Service: GMAP Role + one feature char per registered role. The char
 * list is built at init from the lib's runtime table (gmas_build_svc) so the NimBLE DB
 * matches it 1:1 — handles map by index, no phantom chars. All chars encrypted reads. */
#define GMAS_MAX_CHRS  (1 + 4)   /* GMAP Role + up to one feature char per role */

static const ble_uuid16_t gmas_svc_uuid = BLE_UUID16_INIT(BT_UUID_GMAS_VAL);
static ble_uuid16_t gmas_chr_uuids[GMAS_MAX_CHRS];
static struct ble_gatt_chr_def gmas_chrs[GMAS_MAX_CHRS + 1];
static struct ble_gatt_svc_def gatt_svc_gmas[2];

/* Mirror the lib's GMAS table (primary + role char + registered feature chars,
   each value attr following a 0x2803 declaration) into the NimBLE service def. */
static int gmas_build_svc(struct bt_gatt_service *gmas_svc)
{
    bool prev_decl = false;
    size_t count = 0;

    for (size_t i = 0; i < gmas_svc->attr_count; i++) {
        const struct bt_uuid_16 *u = (const struct bt_uuid_16 *)gmas_svc->attrs[i].uuid;

        if (u->uuid.type != BT_UUID_TYPE_16) {
            prev_decl = false;
            continue;
        }

        if (u->val == BT_UUID_GATT_CHRC_VAL) {
            prev_decl = true;   /* characteristic declaration; value is next */
            continue;
        }

        if (!prev_decl) {
            continue;           /* primary service declaration */
        }

        prev_decl = false;

        if (count >= GMAS_MAX_CHRS) {
            LOG_ERR("[N]GmasTooManyChr[%u]", (unsigned)gmas_svc->attr_count);
            return -ENOMEM;
        }

        gmas_chr_uuids[count] = (ble_uuid16_t)BLE_UUID16_INIT(u->val);
        gmas_chrs[count] = (struct ble_gatt_chr_def) {
            .uuid = &gmas_chr_uuids[count].u,
            .access_cb = bt_le_nimble_gatts_access_cb_safe,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
            .min_key_size = 16,
        };
        count++;
    }

    gmas_chrs[count] = (struct ble_gatt_chr_def){0};

    gatt_svc_gmas[0] = (struct ble_gatt_svc_def) {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gmas_svc_uuid.u,
        .includes = NULL,
        .characteristics = gmas_chrs,
    };
    gatt_svc_gmas[1] = (struct ble_gatt_svc_def){0};

    return 0;
}

int bt_le_nimble_gmas_attr_handle_set(void)
{
    struct bt_gatt_service *gmas_svc;
    uint16_t handle = 0;
    int rc;

    /* App may not register this svc; skip rather than fail audio_start. */
    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_GMAS_VAL), &handle);
    if (rc) {
        LOG_DBG("[N]GmasNotInit");
        return 0;
    }

    gmas_svc = lib_gmas_svc_get();
    if (!gmas_svc) {
        LOG_ERR("[N]GmasSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]GmasAttrHdlSet[%u][%u]", handle, gmas_svc->attr_count);

    for (size_t i = 0; i < gmas_svc->attr_count; i++) {
        (gmas_svc->attrs + i)->handle = handle + i;
    }

    return 0;
}

static int gmas_svc_check(void)
{
    struct bt_gatt_service *gmas_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* Post-add integrity check: every char in the built def must exist in the
     * lib table (holds by construction now the def is built from it). */
    gmas_svc = lib_gmas_svc_get();
    if (!gmas_svc) {
        LOG_ERR("[N]GmasSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]GmasSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_gmas[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < gmas_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(gmas_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]GmasChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_gmas_init(void)
{
    struct bt_gatt_service *gmas_svc;
    int rc;

    LOG_DBG("[N]GmasInit");

    /* GMAP compiled but no role registered (attr_count 0): skip adding GMAS
       rather than failing here and blocking all audio startup. */
    gmas_svc = lib_gmas_svc_get();
    if (!gmas_svc || gmas_svc->attr_count == 0) {
        LOG_DBG("[N]GmasNotReg");
        return 0;
    }

    rc = gmas_build_svc(gmas_svc);
    if (rc) {
        return rc;
    }

    rc = ble_gatts_count_cfg(gatt_svc_gmas);
    if (rc) {
        LOG_ERR("[N]GmasCountCfgFail[%d]", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svc_gmas);
    if (rc) {
        LOG_ERR("[N]GmasAddSvcsFail[%d]", rc);
        return rc;
    }

    rc = gmas_svc_check();
    if (rc) {
        return rc;
    }

    return 0;
}
