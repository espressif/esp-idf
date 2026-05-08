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
#include <zephyr/bluetooth/audio/vcp.h>

#include <../host/conn_internal.h>

#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"

#include "nimble/profiles/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_VCS, CONFIG_BT_ISO_LOG_LEVEL);

#define VOCS_INST_COUNT     CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT

#define INC_VOCS_CHR_COUNT  (4 + 1)

#define INC_VOCS_CHR_FLAGS_STATE \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC)

#define INC_VOCS_CHR_FLAGS_LOCATION \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_NO_RSP | \
     BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC)

#define INC_VOCS_CHR_FLAGS_CONTROL \
    (BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC)

#define INC_VOCS_CHR_FLAGS_DESCRIPTION \
    (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_NO_RSP | \
     BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC)

static uint8_t inc_vocs_svc_count;

static struct inc_vocs_inst {
    struct bt_gatt_service *svc_p;
    uint16_t state_handle;
    uint16_t location_handle;
    uint16_t control_handle;
    uint16_t description_handle;
} inc_vocs_insts[VOCS_INST_COUNT];

static struct ble_gatt_svc_def *gatt_svc_inc_vocs;

static const ble_uuid16_t inc_vocs_uuid_svc = BLE_UUID16_INIT(BT_UUID_VOCS_VAL);
static const ble_uuid16_t inc_vocs_uuid_state = BLE_UUID16_INIT(BT_UUID_VOCS_STATE_VAL);
static const ble_uuid16_t inc_vocs_uuid_location = BLE_UUID16_INIT(BT_UUID_VOCS_LOCATION_VAL);
static const ble_uuid16_t inc_vocs_uuid_control = BLE_UUID16_INIT(BT_UUID_VOCS_CONTROL_VAL);
static const ble_uuid16_t inc_vocs_uuid_description = BLE_UUID16_INIT(BT_UUID_VOCS_DESCRIPTION_VAL);

#define AICS_INST_COUNT     CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT

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
} inc_aics_insts[AICS_INST_COUNT];

static struct ble_gatt_svc_def *gatt_svc_inc_aics;

static const ble_uuid16_t inc_aics_uuid_svc = BLE_UUID16_INIT(BT_UUID_AICS_VAL);
static const ble_uuid16_t inc_aics_uuid_state = BLE_UUID16_INIT(BT_UUID_AICS_STATE_VAL);
static const ble_uuid16_t inc_aics_uuid_gain = BLE_UUID16_INIT(BT_UUID_AICS_GAIN_SETTINGS_VAL);
static const ble_uuid16_t inc_aics_uuid_type = BLE_UUID16_INIT(BT_UUID_AICS_INPUT_TYPE_VAL);
static const ble_uuid16_t inc_aics_uuid_status = BLE_UUID16_INIT(BT_UUID_AICS_INPUT_STATUS_VAL);
static const ble_uuid16_t inc_aics_uuid_control = BLE_UUID16_INIT(BT_UUID_AICS_CONTROL_VAL);
static const ble_uuid16_t inc_aics_uuid_description = BLE_UUID16_INIT(BT_UUID_AICS_DESCRIPTION_VAL);

static struct ble_gatt_svc_def **vcs_inc_svcs;

static struct ble_gatt_svc_def gatt_svc_vcs[] = {
    {
        /* Volume Control Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_VCS_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                /* Volume Control Service -- Volume state characteristic */
                .uuid = BLE_UUID16_DECLARE(BT_UUID_VCS_STATE_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
                .min_key_size = 16,
            }, {
                /* Volume Control Service -- Volume COntrol Point characteristic */
                .uuid = BLE_UUID16_DECLARE(BT_UUID_VCS_CONTROL_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC,
                .min_key_size = 16,
            }, {
                /* Volume Control Service -- Volume Flags characteristic */
                .uuid = BLE_UUID16_DECLARE(BT_UUID_VCS_FLAGS_VAL),
                .access_cb = bt_le_nimble_gatts_access_cb_safe,
                .arg = NULL,
                .descriptors = NULL,    /* NULL if no descriptors. Do not include CCCD */
#if CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ_ENC,
#else /* CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE */
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC,
#endif /* CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE */
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

static int inc_vocs_svc_check(void)
{
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    assert(gatt_svc_inc_vocs);

    LOG_DBG("[N]IncVocsSvcCheck[%u]", inc_vocs_svc_count);

    for (size_t i = 0; i < inc_vocs_svc_count; i++) {
        struct ble_gatt_svc_def *vocs = &gatt_svc_inc_vocs[i];
        struct bt_gatt_service *svc = inc_vocs_insts[i].svc_p;

        assert(svc);

        for (const struct ble_gatt_chr_def *chr = vocs->characteristics;
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
                LOG_ERR("[N]IncVocsChrNotFound[%u][%04x]", i, check->value);
                return -1;
            }
        }
    }

    return 0;
}

static int inc_aics_svc_check(void)
{
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    assert(gatt_svc_inc_aics);

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

static int vcs_svc_check(void)
{
    struct bt_gatt_service *vcs_svc;
    const struct bt_uuid_16 *uuid;
    bool chr_found;

    /* This function is used to make sure the characteristics within
     * the service exist in the service defined by Zephyr.
     */

    vcs_svc = lib_vcs_svc_get();
    assert(vcs_svc);

    LOG_DBG("[N]VCSSvcCheck");

    for (const struct ble_gatt_chr_def *chr = gatt_svc_vcs[0].characteristics;
            chr && chr->uuid; chr++) {
        const ble_uuid16_t *check = (const ble_uuid16_t *)chr->uuid;

        chr_found = false;

        for (size_t i = 0; i < vcs_svc->attr_count; i++) {
            uuid = (const struct bt_uuid_16 *)(vcs_svc->attrs + i)->uuid;

            if (uuid->uuid.type == BT_LE_NIMBLE_GATT_UUID_TO_Z(check->u.type) &&
                    uuid->val == check->value) {
                chr_found = true;
                break;
            }
        }

        if (chr_found == false) {
            LOG_ERR("[N]VcsChrNotFound[%04x]", check->value);
            return -1;
        }
    }

    return 0;
}

static int inc_vocs_attr_handle_set(void)
{
    struct bt_gatt_attr *attr;
    uint16_t start_handle;
    uint16_t end_handle;

    LOG_DBG("[N]IncVocsAttrHdlSet[%u]", inc_vocs_svc_count);

    for (size_t i = 0; i < inc_vocs_svc_count; i++) {
        assert(inc_vocs_insts[i].svc_p);

        assert(inc_vocs_insts[i].state_handle >= 2);
        start_handle = inc_vocs_insts[i].state_handle - 2;      /* server attr handle & char def handle */
        end_handle = inc_vocs_insts[i].description_handle + 1;  /* cccd for chr Audio Output Description */

        LOG_DBG("[N]IncVocsInst[%u][%u][%u][%u]",
                i, start_handle, end_handle, inc_vocs_insts[i].svc_p->attr_count);

        for (size_t j = 0; j < inc_vocs_insts[i].svc_p->attr_count; j++) {
            (inc_vocs_insts[i].svc_p->attrs + j)->handle = start_handle + j;
        }

        /* Last attribute in included VOCS */
        attr = inc_vocs_insts[i].svc_p->attrs + inc_vocs_insts[i].svc_p->attr_count - 1;

        if (attr->handle != end_handle) {
            LOG_ERR("[N]IncVocsMismatchAttrHdl[%u][%u][%u][%u]",
                    start_handle, end_handle, attr->handle,
                    inc_vocs_insts[i].svc_p->attr_count);
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

int bt_le_nimble_vcs_attr_handle_set(void)
{
    struct bt_gatt_service *vcs_svc;
    struct bt_gatt_attr *attr;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;
    int rc;

    vcs_svc = lib_vcs_svc_get();
    assert(vcs_svc);

    LOG_DBG("[N]VcsAttrHdlSet[%u]", vcs_svc->attr_count);

    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_VCS_VAL), &start_handle);
    if (rc) {
        LOG_ERR("[N]VcsNotFound[%d]", rc);
        return rc;
    }

    end_handle = start_handle + vcs_svc->attr_count - 1;

    LOG_DBG("[N]Hdl[%u][%u]", start_handle, end_handle);

    for (size_t i = 0; i < vcs_svc->attr_count; i++) {
        (vcs_svc->attrs + i)->handle = start_handle + i;
    }

    /* Last attribute in VCS */
    attr = vcs_svc->attrs + vcs_svc->attr_count - 1;

    if (attr->handle != end_handle) {
        LOG_ERR("[N]VcsMismatchAttrHdl[%u][%u][%u][%u]",
                start_handle, end_handle, attr->handle, vcs_svc->attr_count);
        return -1;
    }

    if (inc_vocs_svc_count) {
        rc = inc_vocs_attr_handle_set();
        if (rc) {
            return rc;
        }
    }

    if (inc_aics_svc_count) {
        rc = inc_aics_attr_handle_set();
        if (rc) {
            return rc;
        }
    }

    return 0;
}

static inline void inc_vocs_chr_init(struct ble_gatt_chr_def *chr,
                                     const ble_uuid16_t *uuid,
                                     uint16_t *val_handle,
                                     ble_gatt_chr_flags flags)
{
    LOG_DBG("[N]IncVocsChrInit[%04x]", uuid->value);

    chr->uuid = &uuid->u;
    chr->access_cb = bt_le_nimble_gatts_access_cb_safe;
    chr->arg = NULL;
    chr->descriptors = NULL;    /* NULL if no descriptors. Do not include CCCD */
    chr->flags = flags;
    chr->min_key_size = 16;
    chr->val_handle = val_handle;
}

static void inc_vocs_svc_init(struct inc_vocs_inst *inst,
                              struct ble_gatt_svc_def *svc)
{
    LOG_DBG("[N]IncVocsSvcInit");

    svc->type = BLE_GATT_SVC_TYPE_SECONDARY;
    svc->uuid = &inc_vocs_uuid_svc.u;
    svc->includes = NULL;

    svc->characteristics = calloc(INC_VOCS_CHR_COUNT, sizeof(struct ble_gatt_chr_def));
    assert(svc->characteristics);

    /* Characteristic - Volume Offset State */
    inc_vocs_chr_init((void *)&svc->characteristics[0],
                      &inc_vocs_uuid_state,
                      &inst->state_handle,
                      INC_VOCS_CHR_FLAGS_STATE);

    /* Characteristic - Audio Location */
    inc_vocs_chr_init((void *)&svc->characteristics[1],
                      &inc_vocs_uuid_location,
                      &inst->location_handle,
                      INC_VOCS_CHR_FLAGS_LOCATION);

    /* Characteristic - Volume Offset Control Point */
    inc_vocs_chr_init((void *)&svc->characteristics[2],
                      &inc_vocs_uuid_control,
                      &inst->control_handle,
                      INC_VOCS_CHR_FLAGS_CONTROL);

    /* Characteristic - Audio Output Description */
    inc_vocs_chr_init((void *)&svc->characteristics[3],
                      &inc_vocs_uuid_description,
                      &inst->description_handle,
                      INC_VOCS_CHR_FLAGS_DESCRIPTION);
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

int bt_le_nimble_vcs_init(void *vcp_inc)
{
    struct bt_vcp_included *vcp_included;
    uint8_t inc_count;
    int rc;

    LOG_DBG("[N]VcsInit[%p]", vcp_inc);

    vcp_included = vcp_inc;

    if (vcp_included) {
        LOG_DBG("[N]VcpIncCount[%u][%u]",
                vcp_included->vocs_cnt, vcp_included->aics_cnt);

        if (vcp_included->vocs_cnt > VOCS_INST_COUNT ||
                vcp_included->aics_cnt > AICS_INST_COUNT) {
            LOG_ERR("[N]InvVcpIncCount[%u][%u][%u][%u]",
                    vcp_included->vocs_cnt, VOCS_INST_COUNT,
                    vcp_included->aics_cnt, AICS_INST_COUNT);
            return -1;
        }

        inc_vocs_svc_count = vcp_included->vocs_cnt;
        inc_aics_svc_count = vcp_included->aics_cnt;

        /* Extra one for terminating the included service array with NULL */
        inc_count = inc_vocs_svc_count + inc_aics_svc_count + 1;

        vcs_inc_svcs = calloc(inc_count, sizeof(struct ble_gatt_svc_def *));
        assert(vcs_inc_svcs);

        /* VCS may include zero or more instances of VOCS */
        if (inc_vocs_svc_count) {
            /* Extra one for terminating the VOCS service array */
            gatt_svc_inc_vocs = calloc(inc_vocs_svc_count + 1, sizeof(struct ble_gatt_svc_def));
            assert(gatt_svc_inc_vocs);

            for (size_t i = 0; i < inc_vocs_svc_count; i++) {
                inc_vocs_svc_init(&inc_vocs_insts[i], &gatt_svc_inc_vocs[i]);

                inc_vocs_insts[i].svc_p = lib_vocs_svc_get(vcp_included->vocs[i]);

                vcs_inc_svcs[i] = &gatt_svc_inc_vocs[i];
            }

            rc = ble_gatts_count_cfg(gatt_svc_inc_vocs);
            if (rc) {
                LOG_ERR("[N]IncVocsCountCfgFail[%d]", rc);
                goto free;
            }

            rc = ble_gatts_add_svcs(gatt_svc_inc_vocs);
            if (rc) {
                LOG_ERR("[N]IncVocsAddSvcsFail[%d]", rc);
                goto free;
            }

            rc = inc_vocs_svc_check();
            if (rc) {
                goto free;
            }
        }

        /* VCS may include zero or more instances of AICS */
        if (inc_aics_svc_count) {
            /* Extra one for terminating the AICS service array */
            gatt_svc_inc_aics = calloc(inc_aics_svc_count + 1, sizeof(struct ble_gatt_svc_def));
            assert(gatt_svc_inc_aics);

            for (size_t i = 0; i < inc_aics_svc_count; i++) {
                inc_aics_svc_init(&inc_aics_insts[i], &gatt_svc_inc_aics[i]);

                inc_aics_insts[i].svc_p = lib_aics_svc_get(vcp_included->aics[i]);

                vcs_inc_svcs[inc_vocs_svc_count + i] = &gatt_svc_inc_aics[i];
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
        vcs_inc_svcs[inc_count - 1] = NULL;
    }

    gatt_svc_vcs[0].includes = (const struct ble_gatt_svc_def **)vcs_inc_svcs;

    rc = ble_gatts_count_cfg(gatt_svc_vcs);
    if (rc) {
        LOG_ERR("[N]VcsCountCfgFail[%d]", rc);
        goto free;
    }

    rc = ble_gatts_add_svcs(gatt_svc_vcs);
    if (rc) {
        LOG_ERR("[N]VcsAddSvcsFail[%d]", rc);
        goto free;
    }

    rc = vcs_svc_check();
    if (rc) {
        goto free;
    }

    return 0;

free:
    if (vcp_included) {
        if (inc_vocs_svc_count) {
            for (size_t i = 0; i < inc_vocs_svc_count; i++) {
                free((void *)gatt_svc_inc_vocs[i].characteristics);
                gatt_svc_inc_vocs[i].characteristics = NULL;
            }

            free(gatt_svc_inc_vocs);
            gatt_svc_inc_vocs = NULL;

            inc_vocs_svc_count = 0;
        }

        if (inc_aics_svc_count) {
            for (size_t i = 0; i < inc_aics_svc_count; i++) {
                free((void *)gatt_svc_inc_aics[i].characteristics);
                gatt_svc_inc_aics[i].characteristics = NULL;
            }

            free(gatt_svc_inc_aics);
            gatt_svc_inc_aics = NULL;

            inc_aics_svc_count = 0;
        }

        free(vcs_inc_svcs);
        vcs_inc_svcs = NULL;
    }
    gatt_svc_vcs[0].includes = NULL;
    return rc;
}
