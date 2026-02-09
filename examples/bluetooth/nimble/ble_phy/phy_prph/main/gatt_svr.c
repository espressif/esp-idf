/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "phy_prph.h"

static uint8_t *le_phy_val;
static uint16_t gatt_svr_chr_val_handle;
static int
gatt_svr_chr_access_le_phy(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt,
                           void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs_le_phy[] = {
    {
        /*** Service: LE PHY. */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(LE_PHY_UUID16),
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /*** Characteristic */
                .uuid = BLE_UUID16_DECLARE(LE_PHY_CHR_UUID16),
                .access_cb = gatt_svr_chr_access_le_phy,
                .val_handle = &gatt_svr_chr_val_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE
                | BLE_GATT_CHR_F_WRITE_ENC,
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        0, /* No more services. */
    },
};

static int
gatt_svr_chr_access_le_phy(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt,
                           void *arg)
{
    const ble_uuid_t *uuid;
    int rand_num;
    int rc;
    int len;
    uint16_t copied_len;
    uuid = ctxt->chr->uuid;

    /* Determine which characteristic is being accessed by examining its
     * 128-bit UUID.
     */

    if (ble_uuid_cmp(uuid, BLE_UUID16_DECLARE(LE_PHY_CHR_UUID16)) == 0) {
        switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            rand_num = rand();
            rc = os_mbuf_append(ctxt->om, &rand_num, sizeof rand_num);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            len = OS_MBUF_PKTLEN(ctxt->om);
            if (len == 0) {
                return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
            }
            le_phy_val = (uint8_t *)malloc(len);
            if (le_phy_val == NULL) {
                return BLE_ATT_ERR_INSUFFICIENT_RES;
            }

            rc = ble_hs_mbuf_to_flat(ctxt->om, le_phy_val, len, &copied_len);
            free(le_phy_val);
            if (rc == 0) {
                MODLOG_DFLT(INFO, "Write received of len = %d", (int)copied_len);
                return 0;
            }

            MODLOG_DFLT(ERROR, "Failed to receive write characteristic");
            return BLE_ATT_ERR_INSUFFICIENT_RES;

        default:
            return BLE_ATT_ERR_UNLIKELY;
        }
    }

    /* Unknown characteristic; the nimble stack should not have called this
     * function.
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
gatt_svr_init_le_phy(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs_le_phy);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs_le_phy);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
