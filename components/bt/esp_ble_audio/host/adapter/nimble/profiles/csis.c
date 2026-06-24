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

#define CSIS_CHR_FLAGS_SET_SIZE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC)

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
    struct bt_gatt_attr *attr;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;

    LOG_DBG("[N]CsisAttrHdlSet[%u]", csis_svc_count);

    for (size_t i = 0; i < csis_svc_count; i++) {
        assert(csis_insts[i].svc_p);

        assert(csis_insts[i].sirk_handle >= 2);
        start_handle = csis_insts[i].sirk_handle - 2;    /* server attr handle & char def handle */
        end_handle = csis_insts[i].rank_handle;          /* no cccd for chr Set Member Rank */

        LOG_DBG("[N]CsisInst[%u][%u][%u][%u]",
                i, start_handle, end_handle, csis_insts[i].svc_p->attr_count);

        for (size_t j = 0; j < csis_insts[i].svc_p->attr_count; j++) {
            (csis_insts[i].svc_p->attrs + j)->handle = start_handle + j;
        }

        /* Last attribute in CSIS */
        attr = csis_insts[i].svc_p->attrs + csis_insts[i].svc_p->attr_count - 1;

        if (attr->handle != end_handle) {
            LOG_ERR("[N]CsisMismatchAttrHdl[%u][%u][%u][%u][%u]",
                    i, start_handle, end_handle, attr->handle,
                    csis_insts[i].svc_p->attr_count);
            return -1;
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
                          struct ble_gatt_svc_def *svc)
{
    LOG_DBG("[N]CsisSvcInit");

    svc->type = BLE_GATT_SVC_TYPE_PRIMARY;
    svc->uuid = &csis_uuid_svc.u;
    svc->includes = NULL;

    svc->characteristics = calloc(CSIS_CHR_COUNT, sizeof(struct ble_gatt_chr_def));
    assert(svc->characteristics);

    /* Characteristic - Set Identity Resolving Key */
    csis_chr_init((void *)&svc->characteristics[0],
                  &csis_uuid_sirk,
                  &inst->sirk_handle,
                  CSIS_CHR_FLAGS_SIRK);

    /* Characteristic - Coordinated Set Size */
    csis_chr_init((void *)&svc->characteristics[1],
                  &csis_uuid_set_size,
                  &inst->set_size_handle,
                  CSIS_CHR_FLAGS_SET_SIZE);

    /* Characteristic - Set Member Lock */
    csis_chr_init((void *)&svc->characteristics[2],
                  &csis_uuid_set_lock,
                  &inst->set_lock_handle,
                  CSIS_CHR_FLAGS_SET_LOCK);

    /* Characteristic - Set Member Rank */
    csis_chr_init((void *)&svc->characteristics[3],
                  &csis_uuid_rank,
                  &inst->rank_handle,
                  CSIS_CHR_FLAGS_RANK);
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
        csis_svc_init(&csis_insts[i], &gatt_svc_csis[i]);

        csis_insts[i].svc_p = ((struct bt_gatt_service **)svc)[i];
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
