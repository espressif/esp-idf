/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "multi_adv.h"

static uint8_t gatt_svr_chr_val = 0x01;  /* Example characteristic value */
#define GATT_SVR_UUID16_1 (0xCDAB)
#define GATT_SVR_UUID16_2 (0x1118)

static const uint16_t gatt_svr_adv_uuids16[] = {
    GATT_SVR_UUID16_1,
    GATT_SVR_UUID16_2,
};
static const ble_uuid16_t gatt_svr_svc16_uuid_1 = BLE_UUID16_INIT(GATT_SVR_UUID16_1);
static const ble_uuid16_t gatt_svr_svc16_uuid_2 = BLE_UUID16_INIT(GATT_SVR_UUID16_2);

static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                     0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

/* A characteristic that can be subscribed to */
static uint16_t gatt_svr_chr_val_handle;
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
                     0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33);

/* A custom descriptor */
static uint8_t gatt_svr_dsc_val;
static const ble_uuid128_t gatt_svr_dsc_uuid =
    BLE_UUID128_INIT(0x01, 0x01, 0x01, 0x01, 0x12, 0x12, 0x12, 0x12,
                     0x23, 0x23, 0x23, 0x23, 0x34, 0x34, 0x34, 0x34);

static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt,
                void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc16_uuid_1.u,
    },
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc16_uuid_2.u,
    },
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /*** This characteristic can be subscribed to by writing 0x00 and 0x01 to the CCCD ***/
                .uuid = &gatt_svr_chr_uuid.u,
                .access_cb = gatt_svc_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                .val_handle = &gatt_svr_chr_val_handle,
                .descriptors = (struct ble_gatt_dsc_def[]) {
                    {
                        .uuid = &gatt_svr_dsc_uuid.u,
                        .att_flags = BLE_ATT_F_READ,
                        .access_cb = gatt_svc_access,
                    }, {
                        0, /* No more descriptors in this characteristic */
                    }
                },
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        0, /* No more services. */
    },
};

/**
 * Access callback whenever a characteristic/descriptor is read or written to.
 * Here reads and writes need to be handled.
 * ctxt->op tells weather the operation is read or write and
 * weather it is on a characteristic or descriptor,
 * ctxt->dsc->uuid tells which characteristic/descriptor is accessed.
 * attr_handle give the value handle of the attribute being accessed.
 * Accordingly do:
 *     Append the value to ctxt->om if the operation is READ
 *     Write ctxt->om to the value if the operation is WRITE
 **/
static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc = 0;
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        MODLOG_DFLT(INFO, "Characteristic read; conn_handle=%d attr_handle=%d\n",
                    conn_handle, attr_handle);
        rc = os_mbuf_append(ctxt->om,
                            &gatt_svr_chr_val,
                            sizeof(gatt_svr_chr_val));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        MODLOG_DFLT(INFO, "Characteristic write; conn_handle=%d attr_handle=%d",
                    conn_handle, attr_handle);
        rc = os_mbuf_copydata(ctxt->om,
                              0,
                              sizeof(gatt_svr_chr_val),
                              &gatt_svr_chr_val);
        return rc == 0 ? 0 : BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        break;

    case BLE_GATT_ACCESS_OP_READ_DSC:
        MODLOG_DFLT(INFO, "Descriptor read; conn_handle=%d attr_handle=%d\n",
                    conn_handle, attr_handle);
        rc = os_mbuf_append(ctxt->om,
                            &gatt_svr_dsc_val,
                            sizeof(gatt_svr_dsc_val));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        break;

    case BLE_GATT_ACCESS_OP_WRITE_DSC:
        goto unknown;
        break;

    default:
        goto unknown;
    }

unknown:
    /* Unknown characteristic/descriptor;
     * The NimBLE host should not have called this function;
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                    "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

int
gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    /* Setting a value for the read-only descriptor */
    gatt_svr_dsc_val = 0x99;

    return 0;
}

const uint8_t *
gatt_svr_service_uuid128(void)
{
    return gatt_svr_svc_uuid.value;
}

const uint16_t *
gatt_svr_service_uuids16(size_t *count)
{
    if (count) {
        *count = sizeof(gatt_svr_adv_uuids16) / sizeof(gatt_svr_adv_uuids16[0]);
    }

    return gatt_svr_adv_uuids16;
}
