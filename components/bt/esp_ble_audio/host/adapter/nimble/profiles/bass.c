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

LOG_MODULE_REGISTER(LEA_BASS, CONFIG_BT_ISO_LOG_LEVEL);

static const ble_uuid16_t bass_uuid_control_point = BLE_UUID16_INIT(BT_UUID_BASS_CONTROL_POINT_VAL);
static const ble_uuid16_t bass_uuid_recv_state = BLE_UUID16_INIT(BT_UUID_BASS_RECV_STATE_VAL);

static uint16_t bass_control_point_handle;
static uint16_t bass_recv_state_handle[CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT];

static struct ble_gatt_svc_def gatt_svc_bass[] = {
    {
        /* Broadcast Audio Scan Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_BASS_VAL),
        .includes = NULL,
        .characteristics = NULL,
    },
    {
        0, /* No more services. */
    },
};

static void bass_svc_add_control_point_chr(struct ble_gatt_chr_def *chr)
{
    LOG_DBG("[N]BassSvcAddControlPointChr");

    chr->uuid = &bass_uuid_control_point.u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
    chr->flags = BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    chr->min_key_size = 16;
    chr->val_handle = &bass_control_point_handle;
}

static void bass_svc_add_recv_state_chr(struct ble_gatt_chr_def *chrs)
{
    for (size_t i = 0 ; i < CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT; i++) {
        struct ble_gatt_chr_def *chr = chrs + i;

        LOG_DBG("[N]BassSvcAddRecvStateChr[%u]", i);

        chr->uuid = &bass_uuid_recv_state.u;
        chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
        chr->arg = UINT_TO_POINTER(i);
        chr->descriptors = NULL;  /* NULL if no descriptors. Do not include CCCD */
        chr->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC;
        chr->min_key_size = 16;
        chr->val_handle = &bass_recv_state_handle[i];
    }
}

int bt_le_nimble_bass_attr_handle_set(void)
{
    struct bt_gatt_service *bass_svc;
    struct bt_gatt_attr *attr;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;
    int rc;

    /* App may not register this svc (e.g. CAP Acceptor single mode keeps
     * unused capability built). Skip rather than fail audio_start.
     */
    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_BASS_VAL), NULL);
    if (rc) {
        LOG_DBG("[N]BassNotInit");
        return 0;
    }

    bass_svc = lib_bap_bass_svc_get();
    if (!bass_svc) {
        LOG_ERR("[N]BassSvcGetFail");
        return -ENODEV;
    }

    assert(bass_control_point_handle >= 2);
    start_handle = bass_control_point_handle - 2;     /* server attr handle & char def handle */
    end_handle = bass_recv_state_handle[CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT - 1] + 1; /* cccd attr handle */

    LOG_DBG("[N]BassAttrHdlSet[%u][%u][%u]",
            start_handle, end_handle, bass_svc->attr_count);

    for (size_t i = 0; i < bass_svc->attr_count; i++) {
        (bass_svc->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in BASS */
    attr = bass_svc->attrs + bass_svc->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]BassMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, bass_svc->attr_count);
        return -1;
    }

    return 0;
}

static int bass_svc_check(void)
{
    struct bt_gatt_service *bass_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    bass_svc = lib_bap_bass_svc_get();
    if (!bass_svc) {
        LOG_ERR("[N]BassSvcGetFail");
        return -ENODEV;
    }

    LOG_DBG("[N]BassSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_bass->characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < bass_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(bass_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]BassChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_bass_init(void)
{
    uint16_t chr_count;
    int rc;

    /* An additional characteristic consist of all 0s indicating end of characteristics */
    chr_count = 1 + CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT + 1;

    LOG_DBG("[N]BassInit[%u]", chr_count);

    gatt_svc_bass->characteristics = calloc(chr_count, sizeof(struct ble_gatt_chr_def));
    assert(gatt_svc_bass->characteristics);

    bass_svc_add_control_point_chr((void *)(gatt_svc_bass->characteristics + 0));

    bass_svc_add_recv_state_chr((void *)(gatt_svc_bass->characteristics + 1));

    rc = ble_gatts_count_cfg(gatt_svc_bass);
    if (rc) {
        LOG_ERR("[N]BassCountCfgFail[%d]", rc);
        goto free;
    }

    rc = ble_gatts_add_svcs(gatt_svc_bass);
    if (rc) {
        LOG_ERR("[N]BassAddSvcsFail[%d]", rc);
        goto free;
    }

    rc = bass_svc_check();
    if (rc) {
        goto free;
    }

    return 0;

free:
    free((void *)gatt_svc_bass->characteristics);
    gatt_svc_bass->characteristics = NULL;
    return rc;
}
