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
#include <zephyr/bluetooth/audio/micp.h>

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "nimble/profiles/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_MICS, CONFIG_BT_ISO_LOG_LEVEL);

#define INC_AICS_CHR_COUNT  (6 + 1)

#define INC_AICS_CHR_FLAGS_STATE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC)

#define INC_AICS_CHR_FLAGS_GAIN \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#define INC_AICS_CHR_FLAGS_TYPE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC)

#define INC_AICS_CHR_FLAGS_STATUS \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC)

#define INC_AICS_CHR_FLAGS_CONTROL \
    (BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC)

#define INC_AICS_CHR_FLAGS_DESCRIPTION \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_NO_RSP | \
     BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC)

static uint8_t inc_aics_svc_count;

static struct inc_aics_inst {
    struct bt_gatt_service *svc_p;
    uint16_t state_handle;
    uint16_t gain_handle;
    uint16_t type_handle;
    uint16_t status_handle;
    uint16_t control_handle;
    uint16_t description_handle;
} *inc_aics_insts;

static struct ble_gatt_svc_def *gatt_svc_inc_aics;

static const ble_uuid16_t inc_aics_uuid_svc = BLE_UUID16_INIT(BT_UUID_AICS_VAL);
static const ble_uuid16_t inc_aics_uuid_state = BLE_UUID16_INIT(BT_UUID_AICS_STATE_VAL);
static const ble_uuid16_t inc_aics_uuid_gain = BLE_UUID16_INIT(BT_UUID_AICS_GAIN_SETTINGS_VAL);
static const ble_uuid16_t inc_aics_uuid_type = BLE_UUID16_INIT(BT_UUID_AICS_INPUT_TYPE_VAL);
static const ble_uuid16_t inc_aics_uuid_status = BLE_UUID16_INIT(BT_UUID_AICS_INPUT_STATUS_VAL);
static const ble_uuid16_t inc_aics_uuid_control = BLE_UUID16_INIT(BT_UUID_AICS_CONTROL_VAL);
static const ble_uuid16_t inc_aics_uuid_description = BLE_UUID16_INIT(BT_UUID_AICS_DESCRIPTION_VAL);

static struct ble_gatt_svc_def **mics_inc_svcs;

static uint16_t mics_mute_handle;

static struct ble_gatt_svc_def gatt_svc_mics[] = {
    {
        /* Microphone Control Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_MICS_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MICS_MUTE_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | \
                         BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ_ENC | \
                         BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
                .val_handle = &mics_mute_handle,
            }, {
                0, /* No more characteristics in this service. */
            }
        }
    },
    {
        0, /* No more services. */
    }
};

static int inc_aics_svc_check(void)
{
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    assert(gatt_svc_inc_aics);
    assert(inc_aics_insts);

    LOG_DBG("[N]IncAicsSvcCheck[%u]", inc_aics_svc_count);

    for (size_t i = 0; i < inc_aics_svc_count; i++) {
        struct ble_gatt_svc_def *aics = &gatt_svc_inc_aics[i];
        struct bt_gatt_service *svc = inc_aics_insts[i].svc_p;

        assert(svc);

        for (const struct ble_gatt_chr_def *chr = aics->characteristics;
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
                LOG_ERR("[N]IncAicsChrNotFound[%u][%04x]", i, check->value);
                return -1;
            }
        }
    }

    return 0;
}

static int mics_svc_check(void)
{
    struct bt_gatt_service *mics_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    mics_svc = lib_mics_svc_get();
    assert(mics_svc);

    LOG_DBG("[N]MicsSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_mics->characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < mics_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(mics_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]MicsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

static int inc_aics_attr_handle_set(void)
{
    struct bt_gatt_attr *attr;
    uint16_t start_handle;
    uint16_t end_handle;

    assert(inc_aics_insts);

    LOG_DBG("[N]IncAicsAttrHdlSet[%u]", inc_aics_svc_count);

    for (size_t i = 0; i < inc_aics_svc_count; i++) {
        assert(inc_aics_insts[i].svc_p);

        assert(inc_aics_insts[i].state_handle >= 2);
        start_handle = inc_aics_insts[i].state_handle - 2;      /* server attr handle & char def handle */
        end_handle = inc_aics_insts[i].description_handle + 1;  /* cccd for chr Audio Input Description */

        LOG_DBG("[N]IncAicsInst[%u][%u][%u][%u]",
                i, start_handle, end_handle, inc_aics_insts[i].svc_p->attr_count);

        for (size_t j = 0; j < inc_aics_insts[i].svc_p->attr_count; j++) {
            (inc_aics_insts[i].svc_p->attrs + j)->handle = start_handle + j;
        }

        /* Last attribute in included AICS */
        attr = inc_aics_insts[i].svc_p->attrs + inc_aics_insts[i].svc_p->attr_count - 1;

        if (attr->handle != end_handle) {
            LOG_ERR("[N]IncAicsMismatchAttrHdl[%u][%u][%u][%u]",
                    start_handle, end_handle, attr->handle,
                    inc_aics_insts[i].svc_p->attr_count);
            return -1;
        }
    }

    return 0;
}

int bt_le_nimble_mics_attr_handle_set(void)
{
    struct bt_gatt_service *mics_svc;
    struct bt_gatt_attr *attr;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;
    int rc;

    mics_svc = lib_mics_svc_get();
    assert(mics_svc);

    LOG_DBG("[N]MicsAttrHdlSet[%u]", mics_svc->attr_count);

    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_MICS_VAL), &start_handle);
    if (rc) {
        LOG_ERR("[N]MicsNotFound[%d]", rc);
        return rc;
    }

    end_handle = start_handle + mics_svc->attr_count - 1;

    LOG_DBG("[N]Hdl[%u][%u]", start_handle, end_handle);

    for (size_t i = 0; i < mics_svc->attr_count; i++) {
        (mics_svc->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in MICS */
    attr = mics_svc->attrs + mics_svc->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]MicsMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, mics_svc->attr_count);
        return -1;
    }

    if (inc_aics_svc_count) {
        rc = inc_aics_attr_handle_set();
        if (rc) {
            return rc;
        }
    }

    return 0;
}

static inline void inc_aics_chr_init(struct ble_gatt_chr_def *chr,
                                     const ble_uuid16_t *uuid,
                                     uint16_t *val_handle,
                                     ble_gatt_chr_flags flags)
{
    LOG_DBG("[N]IncAicsChrInit[%04x]", uuid->value);

    chr->uuid = &uuid->u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
    chr->flags = flags;
    chr->min_key_size = 16;
    chr->val_handle = val_handle;
}

static void inc_aics_svc_init(struct inc_aics_inst *inst,
                              struct ble_gatt_svc_def *svc)
{
    LOG_DBG("[N]IncAicsSvcInit");

    svc->type = BLE_GATT_SVC_TYPE_SECONDARY;
    svc->uuid = &inc_aics_uuid_svc.u;
    svc->includes = NULL;

    svc->characteristics = calloc(INC_AICS_CHR_COUNT, sizeof(struct ble_gatt_chr_def));
    assert(svc->characteristics);

    /* Characteristic - Audio Input State */
    inc_aics_chr_init((void *)&svc->characteristics[0],
                      &inc_aics_uuid_state,
                      &inst->state_handle,
                      INC_AICS_CHR_FLAGS_STATE);

    /* Characteristic - Gain Setting Properties */
    inc_aics_chr_init((void *)&svc->characteristics[1],
                      &inc_aics_uuid_gain,
                      &inst->gain_handle,
                      INC_AICS_CHR_FLAGS_GAIN);

    /* Characteristic - Audio Input Type */
    inc_aics_chr_init((void *)&svc->characteristics[2],
                      &inc_aics_uuid_type,
                      &inst->type_handle,
                      INC_AICS_CHR_FLAGS_TYPE);

    /* Characteristic - Audio Input Status */
    inc_aics_chr_init((void *)&svc->characteristics[3],
                      &inc_aics_uuid_status,
                      &inst->status_handle,
                      INC_AICS_CHR_FLAGS_STATUS);

    /* Characteristic - Audio Input Control Point */
    inc_aics_chr_init((void *)&svc->characteristics[4],
                      &inc_aics_uuid_control,
                      &inst->control_handle,
                      INC_AICS_CHR_FLAGS_CONTROL);

    /* Characteristic - Audio Input Description */
    inc_aics_chr_init((void *)&svc->characteristics[5],
                      &inc_aics_uuid_description,
                      &inst->description_handle,
                      INC_AICS_CHR_FLAGS_DESCRIPTION);
}

int bt_le_nimble_mics_init(void *micp_inc)
{
    struct bt_micp_included *micp_included;
    uint8_t inc_count;
    int rc;

    LOG_DBG("[N]MicsInit[%p]", micp_inc);

    micp_included = micp_inc;

    if (micp_included) {
        LOG_DBG("[N]MicpIncCount[%u]", micp_included->aics_cnt);

        if (micp_included->aics_cnt > CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT) {
            LOG_ERR("[N]InvMicpIncCount[%u][%u]",
                    micp_included->aics_cnt, CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT);
            return -1;
        }

        inc_aics_svc_count = micp_included->aics_cnt;

        /* Extra one for terminating the included service array with NULL */
        inc_count = inc_aics_svc_count + 1;

        mics_inc_svcs = calloc(inc_count, sizeof(struct ble_gatt_svc_def *));
        assert(mics_inc_svcs);

        /* MICS may include zero or more instances of AICS */
        if (inc_aics_svc_count) {
            inc_aics_insts = calloc(inc_aics_svc_count, sizeof(struct inc_aics_inst));
            assert(inc_aics_insts);

            /* Extra one for terminating the AICS service array */
            gatt_svc_inc_aics = calloc(inc_aics_svc_count + 1, sizeof(struct ble_gatt_svc_def));
            assert(gatt_svc_inc_aics);

            for (size_t i = 0; i < inc_aics_svc_count; i++) {
                inc_aics_svc_init(&inc_aics_insts[i], &gatt_svc_inc_aics[i]);

                inc_aics_insts[i].svc_p = lib_aics_svc_get(micp_included->aics[i]);

                mics_inc_svcs[i] = &gatt_svc_inc_aics[i];
            }

            rc = ble_gatts_count_cfg(gatt_svc_inc_aics);
            if (rc) {
                LOG_ERR("[N]IncAicsCountCfgFail[%d]", rc);
                goto free;
            }

            rc = ble_gatts_add_svcs(gatt_svc_inc_aics);
            if (rc) {
                LOG_ERR("[N]IncAicsAddSvcsFail[%d]", rc);
                goto free;
            }

            rc = inc_aics_svc_check();
            if (rc) {
                goto free;
            }
        }

        /* Terminate the included service array with NULL */
        mics_inc_svcs[inc_count - 1] = NULL;
    }

    gatt_svc_mics[0].includes = (const struct ble_gatt_svc_def **)mics_inc_svcs;

    rc = ble_gatts_count_cfg(gatt_svc_mics);
    if (rc) {
        LOG_ERR("[N]MicsCountCfgFail[%d]", rc);
        goto free;
    }

    rc = ble_gatts_add_svcs(gatt_svc_mics);
    if (rc) {
        LOG_ERR("[N]MicsAddSvcsFail[%d]", rc);
        goto free;
    }

    rc = mics_svc_check();
    if (rc) {
        goto free;
    }

    return 0;

free:
    if (micp_included) {
        free(mics_inc_svcs);
        mics_inc_svcs = NULL;

        if (inc_aics_svc_count) {
            free(inc_aics_insts);
            inc_aics_insts = NULL;

            for (size_t i = 0; i < inc_aics_svc_count; i++) {
                free((void *)gatt_svc_inc_aics[i].characteristics);
                gatt_svc_inc_aics[i].characteristics = NULL;
            }

            free(gatt_svc_inc_aics);
            gatt_svc_inc_aics = NULL;

            inc_aics_svc_count = 0;
        }
    }
    gatt_svc_mics[0].includes = NULL;
    return rc;
}
