/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "ble_multi_conn_cent.h"

#define TAG     "BLE_MUTLI_CONN_CENT_SVC"

static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                     0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

static uint16_t gatt_svr_chr_val_handle;
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
                     0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33);

static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt,
                void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                .uuid = &gatt_svr_chr_uuid.u,
                .access_cb = gatt_svc_access,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &gatt_svr_chr_val_handle,
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
gatt_svc_send_to_peers(const struct peer *peer, void *arg)
{
    int rc;
    const struct peer_chr *chr;
    struct os_mbuf *src_om = arg;
    struct os_mbuf *dst_om = NULL;

    chr = peer_chr_find_uuid(peer, (const ble_uuid_t *)&gatt_svr_svc_uuid,
                             (const ble_uuid_t *)&gatt_svr_chr_uuid);
    if (!chr) {
        ESP_LOGE(TAG, "Didn't find the characteristic, handle:%d", peer->conn_handle);
        goto failed_to_send;
    }

    dst_om = ble_hs_mbuf_att_pkt();
    if (!dst_om) {
        ESP_LOGE(TAG, "No enough buffer, handle:%d", peer->conn_handle);
        goto failed_to_send;
    }

    rc = os_mbuf_appendfrom(dst_om, src_om, 0, os_mbuf_len(src_om));
    if (rc) {
        ESP_LOGE(TAG, "Failed to copy data, handle:%d, rc:%d", peer->conn_handle, rc);
        goto failed_to_send;
    }

    rc = ble_gattc_write(peer->conn_handle, chr->chr.val_handle, dst_om, NULL, NULL);
    if (rc) {
        ESP_LOGE(TAG, "Failed to write, handle:%d, rc:%d", peer->conn_handle, rc);
        /* The dst_om has already been freed in ble_gattc_write. */
        dst_om = NULL;
        goto failed_to_send;
    }

    return 0;

failed_to_send:
    if (dst_om) {
        os_mbuf_free_chain(dst_om);
    }

    return -1;
}

static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t data[10];
    uint8_t len;
    struct os_mbuf *om;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        ESP_LOGI(TAG, "Characteristic write; conn_handle=%d", conn_handle);
        if (attr_handle == gatt_svr_chr_val_handle) {
            om = ctxt->om;
            len = os_mbuf_len(om);
            len = len < sizeof(data) ? len : sizeof(data);
            assert(os_mbuf_copydata(om, 0, len, data) == 0);
            ESP_LOG_BUFFER_HEX(TAG, data, len);
            /* Send the received data to all of peers. */
            peer_traverse_all(gatt_svc_send_to_peers, om);
            return 0;
        }
        goto unknown;

    default:
        goto unknown;
    }

unknown:
    return BLE_ATT_ERR_UNLIKELY;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s with handle=%d\n",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf), ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG, "registering characteristic %s with def_handle=%d val_handle=%d\n",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf), ctxt->chr.def_handle,
                 ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d\n",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf), ctxt->dsc.handle);
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

    return 0;
}
