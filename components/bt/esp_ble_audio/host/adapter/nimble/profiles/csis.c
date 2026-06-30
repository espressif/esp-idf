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

#include <zephyr/bluetooth/audio/csip.h>

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "nimble/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_CSIS, CONFIG_BT_ISO_LOG_LEVEL);

#define CSIS_SVC_COUNT      CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT

#define CSIS_CHR_COUNT      (4 + 1)

#if CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE
#define CSIS_CHR_FLAGS_SIRK \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC)
#else /* CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE */
#define CSIS_CHR_FLAGS_SIRK \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE */

#if CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE
#define CSIS_CHR_FLAGS_SET_SIZE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC)
#else /* CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE */
#define CSIS_CHR_FLAGS_SET_SIZE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE */

#define CSIS_CHR_FLAGS_SET_LOCK \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE | \
     BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC)

#define CSIS_CHR_FLAGS_RANK \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

static const ble_uuid16_t csis_uuid_svc = BLE_UUID16_INIT(BT_UUID_CSIS_VAL);
static const ble_uuid16_t csis_uuid_sirk = BLE_UUID16_INIT(BT_UUID_CSIS_SIRK_VAL);
static const ble_uuid16_t csis_uuid_set_size = BLE_UUID16_INIT(BT_UUID_CSIS_SET_SIZE_VAL);
static const ble_uuid16_t csis_uuid_set_lock = BLE_UUID16_INIT(BT_UUID_CSIS_SET_LOCK_VAL);
static const ble_uuid16_t csis_uuid_rank = BLE_UUID16_INIT(BT_UUID_CSIS_RANK_VAL);

static struct csis_inst {
    struct bt_gatt_service *svc_p;
    uint16_t sirk_handle;
    uint16_t set_size_handle;
    uint16_t set_lock_handle;
    uint16_t rank_handle;
} csis_insts[CSIS_SVC_COUNT];

static uint8_t csis_svc_count;

/* Extra one for terminating the CSIS service array */
static struct ble_gatt_svc_def gatt_svc_csis[CSIS_SVC_COUNT + 1];

struct ble_gatt_svc_def *cas_get_included_csis(void *csis_svc_p)
{
    size_t i;

    LOG_DBG("[N]CasGetIncCsis[%u]", csis_svc_count);

    for (i = 0; i < csis_svc_count; i++) {
        if (csis_insts[i].svc_p == csis_svc_p) {
            break;
        }
    }

    if (i == csis_svc_count) {
        return NULL;
    }

    return &gatt_svc_csis[i];
}

static int csis_svc_check(void)
{
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    LOG_DBG("[N]CsisSvcCheck[%u]", csis_svc_count);

    for (size_t i = 0; i < csis_svc_count; i++) {
        struct ble_gatt_svc_def *csis = &gatt_svc_csis[i];
        struct bt_gatt_service *svc = csis_insts[i].svc_p;

        assert(svc);

        for (const struct ble_gatt_chr_def *chr = csis->characteristics;
                chr && chr->uuid; chr++) {
            const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

            chr_found = false;

            for (size_t j = 0; j < svc->attr_count; j++) {
                uuid = (const struct bt_uuid_16 *)(svc->attrs + j)->uuid;

                if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                        uuid->val == check->value) {
                    chr_found = true;
                    break;
                }
            }

            if (chr_found == false) {
                LOG_ERR("[N]CsisChrNotFound[%u][%04x]", i, check->value);
                return -1;
            }
        }
    }

    return 0;
}

int bt_le_nimble_csis_attr_handle_set(void)
{
    uint16_t start_handle = 0;

    LOG_DBG("[N]CsisAttrHdlSet[%u]", csis_svc_count);

    for (size_t i = 0; i < csis_svc_count; i++) {
        struct bt_gatt_service *zsvc = csis_insts[i].svc_p;

        assert(zsvc);
        assert(csis_insts[i].sirk_handle >= 2);

        /* SIRK is always the first characteristic, so its value handle anchors the range. */
        start_handle = csis_insts[i].sirk_handle - 2;    /* server attr handle & char def handle */

        LOG_DBG("[N]CsisInst[%u][%u][%u]", i, start_handle, zsvc->attr_count);

        for (size_t j = 0; j < zsvc->attr_count; j++) {
            (zsvc->attrs + j)->handle = start_handle + j;
        }

        /* Cross-check the last characteristic value against the handle NimBLE assigned, to
         * catch divergence between the lib's attribute layout and NimBLE's registration. The
         * last characteristic is no longer necessarily Rank, as optional ones may be absent.
         */
        for (size_t j = zsvc->attr_count; j-- > 0;) {
            const struct bt_uuid_16 *uuid = (const struct bt_uuid_16 *)(zsvc->attrs + j)->uuid;
            uint16_t chr_handle = 0;

            if (uuid->uuid.type != BT_UUID_TYPE_16) {
                continue;
            }

            switch (uuid->val) {
            case BT_UUID_CSIS_SIRK_VAL:
                chr_handle = csis_insts[i].sirk_handle;
                break;
            case BT_UUID_CSIS_SET_SIZE_VAL:
                chr_handle = csis_insts[i].set_size_handle;
                break;
            case BT_UUID_CSIS_SET_LOCK_VAL:
                chr_handle = csis_insts[i].set_lock_handle;
                break;
            case BT_UUID_CSIS_RANK_VAL:
                chr_handle = csis_insts[i].rank_handle;
                break;
            default:
                continue;
            }

            if ((zsvc->attrs + j)->handle != chr_handle) {
                LOG_ERR("[N]CsisMismatchAttrHdl[%u][%u][%u]",
                        i, (zsvc->attrs + j)->handle, chr_handle);
                return -1;
            }
            break;
        }
    }

    return 0;
}

static inline void csis_chr_init(struct ble_gatt_chr_def *chr,
                                 const ble_uuid16_t *uuid,
                                 uint16_t *val_handle,
                                 ble_gatt_chr_flags flags)
{
    LOG_DBG("[N]CsisChrInit[%04x]", uuid->value);

    chr->uuid = &uuid->u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
    chr->flags = flags;
    chr->min_key_size = 16;
    chr->val_handle = val_handle;
}

static void csis_svc_init(struct csis_inst *inst,
                          struct ble_gatt_svc_def *svc,
                          const struct bt_gatt_service *zsvc)
{
    size_t chr_cnt = 0U;

    LOG_DBG("[N]CsisSvcInit");

    svc->type = BLE_GATT_SVC_TYPE_PRIMARY;
    svc->uuid = &csis_uuid_svc.u;
    svc->includes = NULL;

    svc->characteristics = calloc(CSIS_CHR_COUNT, sizeof(struct ble_gatt_chr_def));
    assert(svc->characteristics);

    /* Build the NimBLE characteristics from the ones actually present in the Zephyr
     * service. Optional characteristics (set size, lock, rank) may be absent depending
     * on the registration parameters, so the layout is data-driven rather than fixed.
     */
    for (size_t i = 0; i < zsvc->attr_count; i++) {
        const struct bt_uuid_16 *uuid = (const struct bt_uuid_16 *)zsvc->attrs[i].uuid;

        if (uuid->uuid.type != BT_UUID_TYPE_16) {
            continue;
        }

        switch (uuid->val) {
        case BT_UUID_CSIS_SIRK_VAL:
            csis_chr_init((void *)&svc->characteristics[chr_cnt++], &csis_uuid_sirk,
                          &inst->sirk_handle, CSIS_CHR_FLAGS_SIRK);
            break;
        case BT_UUID_CSIS_SET_SIZE_VAL:
            csis_chr_init((void *)&svc->characteristics[chr_cnt++], &csis_uuid_set_size,
                          &inst->set_size_handle, CSIS_CHR_FLAGS_SET_SIZE);
            break;
        case BT_UUID_CSIS_SET_LOCK_VAL:
            csis_chr_init((void *)&svc->characteristics[chr_cnt++], &csis_uuid_set_lock,
                          &inst->set_lock_handle, CSIS_CHR_FLAGS_SET_LOCK);
            break;
        case BT_UUID_CSIS_RANK_VAL:
            csis_chr_init((void *)&svc->characteristics[chr_cnt++], &csis_uuid_rank,
                          &inst->rank_handle, CSIS_CHR_FLAGS_RANK);
            break;
        default:
            break;
        }
    }

    /* svc->characteristics has CSIS_CHR_COUNT (= 4 + 1) slots: up to 4 real chars plus
     * a trailing zeroed slot that NimBLE requires as the NULL-uuid array terminator.
     * chr_cnt is the real-char count written via [chr_cnt++], so it must stay strictly
     * below CSIS_CHR_COUNT — chr_cnt == CSIS_CHR_COUNT means a write already clobbered
     * the terminator slot. Trips if the switch matches a 5th char: a new CSIS case added
     * without bumping the (4 + 1), or a duplicate UUID in the Zephyr service table.
     */
    assert(chr_cnt < CSIS_CHR_COUNT);
}

int bt_le_nimble_csis_init(void *svc, uint8_t count)
{
    int rc;

    LOG_DBG("[N]CsisInit[%u]", count);

    if (count > CSIS_SVC_COUNT) {
        LOG_ERR("[N]InvCsisCount[%u][%u]", count, CSIS_SVC_COUNT);
        return -1;
    }

    csis_svc_count = count;

    for (size_t i = 0; i < csis_svc_count; i++) {
        csis_insts[i].svc_p = ((struct bt_gatt_service **)svc)[i];

        csis_svc_init(&csis_insts[i], &gatt_svc_csis[i], csis_insts[i].svc_p);
    }

    rc = ble_gatts_count_cfg(gatt_svc_csis);
    if (rc) {
        LOG_ERR("[N]CsisCountCfgFail[%d]", rc);
        goto free;
    }

    rc = ble_gatts_add_svcs(gatt_svc_csis);
    if (rc) {
        LOG_ERR("[N]CsisAddSvcsFail[%d]", rc);
        goto free;
    }

    rc = csis_svc_check();
    if (rc) {
        goto free;
    }

    return 0;

free:
    for (size_t i = 0; i < csis_svc_count; i++) {
        free((void *)gatt_svc_csis[i].characteristics);
        gatt_svc_csis[i].characteristics = NULL;
    }
    csis_svc_count = 0;
    return rc;
}
