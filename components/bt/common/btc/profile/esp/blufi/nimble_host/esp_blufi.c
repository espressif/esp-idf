/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "btc_blufi_prf.h"
#include "blufi_int.h"
#include "esp_log.h"
#include "esp_blufi_api.h"
#include "esp_err.h"
#include "btc/btc_task.h"
#include "esp_blufi.h"
#include "osi/allocator.h"
#include "console/console.h"

/*nimBLE Host*/
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#if (BLUFI_INCLUDED == TRUE)

static uint8_t own_addr_type;

struct gatt_value gatt_values[SERVER_MAX_VALUES];
const static char *TAG = "BLUFI_EXAMPLE";

enum {
    GATT_VALUE_TYPE_CHR,
    GATT_VALUE_TYPE_DSC,
};

static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt,
                              void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service: Blufi */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLUFI_SERVICE_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /*** Characteristic: P2E */
                .uuid = BLE_UUID16_DECLARE(BLUFI_CHAR_P2E_UUID),
                .access_cb = gatt_svr_access_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
                .arg = &gatt_values[0],
                .val_handle = &gatt_values[0].val_handle,
            }, {
                /*** Characteristic: E2P  */
                .uuid = BLE_UUID16_DECLARE(BLUFI_CHAR_E2P_UUID),
                .access_cb = gatt_svr_access_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .arg = &gatt_values[1],
                .val_handle = &gatt_values[1].val_handle,
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        0, /* No more services. */
    },
};

void esp_blufi_gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];
    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGI(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGI(TAG, "registering characteristic %s with "
                 "def_handle=%d val_handle=%d\n",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle,
                 ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGI(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

static size_t write_value(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt,
                          void *arg)
{
    struct gatt_value *value = (struct gatt_value *)arg;
    uint16_t len;
    int rc;
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        if (ctxt->chr->flags & BLE_GATT_CHR_F_WRITE_AUTHOR) {
            return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
        }
    } else {
        if (ctxt->dsc->att_flags & BLE_ATT_F_WRITE_AUTHOR) {
            return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
        }
    }

    /* Data may come in linked om. So retrieve all data */
    if (SLIST_NEXT(ctxt->om, om_next) != NULL) {
	uint8_t *fw_buf = (uint8_t *)malloc(517 * sizeof(uint8_t));
	memset(fw_buf, 0x0, 517);

        memcpy(fw_buf, &ctxt->om->om_data[0], ctxt->om->om_len);
        struct os_mbuf *last;
        last = ctxt->om;
        uint32_t offset = ctxt->om->om_len;

        while (SLIST_NEXT(last, om_next) != NULL) {
              struct os_mbuf *temp = SLIST_NEXT(last, om_next);
	      memcpy(fw_buf + offset  , &temp->om_data[0], temp->om_len);
	      offset += temp->om_len;
	      last = SLIST_NEXT(last, om_next);
              temp = NULL;
        }
	btc_blufi_recv_handler(fw_buf, offset);

	free(fw_buf);
    }
    else {
        btc_blufi_recv_handler(&ctxt->om->om_data[0], ctxt->om->om_len);
    }

    rc = ble_hs_mbuf_to_flat(ctxt->om, value->buf->om_data,
                             value->buf->om_len, &len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    /* Maximum attribute value size is 512 bytes */
    assert(value->buf->om_len < MAX_VAL_SIZE);

    return 0;
}

static size_t read_value(uint16_t conn_handle, uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt,
                         void *arg)
{
    const struct gatt_value *value = (const struct gatt_value *) arg;
    char str[BLE_UUID_STR_LEN];
    int rc;

    memset(str, '\0', sizeof(str));

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        if (ctxt->chr->flags & BLE_GATT_CHR_F_READ_AUTHOR) {
            return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
        }

        ble_uuid_to_str(ctxt->chr->uuid, str);
    } else {
        if (ctxt->dsc->att_flags & BLE_ATT_F_READ_AUTHOR) {
            return BLE_ATT_ERR_INSUFFICIENT_AUTHOR;
        }

        ble_uuid_to_str(ctxt->dsc->uuid, str);
    }

    rc = os_mbuf_append(ctxt->om, value->buf->om_data, value->buf->om_len);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt,
                              void *arg)
{
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        return read_value(conn_handle, attr_handle,
                          ctxt, arg);
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        return write_value(conn_handle, attr_handle,
                           ctxt, arg);
    default:
        assert(0);
        return BLE_ATT_ERR_UNLIKELY;
    }

    /* Unknown characteristic; the nimble stack should not have called this
     * function.
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static void init_gatt_values(void)
{
    int i = 0;
    const struct ble_gatt_svc_def *svc;
    const struct ble_gatt_chr_def *chr;
    const struct ble_gatt_dsc_def *dsc;

    for (svc = gatt_svr_svcs; svc && svc->uuid; svc++) {
        for (chr = svc->characteristics; chr && chr->uuid; chr++) {
            assert(i < SERVER_MAX_VALUES);
            gatt_values[i].type = GATT_VALUE_TYPE_CHR;
            gatt_values[i].ptr = (void *)chr;
            gatt_values[i].buf = os_msys_get(0, 0);
            os_mbuf_extend(gatt_values[i].buf, 1);
            ++i;

            for (dsc = chr->descriptors; dsc && dsc->uuid; dsc++) {
                assert(i < SERVER_MAX_VALUES);
                gatt_values[i].type = GATT_VALUE_TYPE_DSC;
                gatt_values[i].ptr = (void *)dsc;
                gatt_values[i].buf = os_msys_get(0, 0);
                os_mbuf_extend(gatt_values[i].buf, 1);
                ++i;
            }
        }
    }

}

int esp_blufi_gatt_svr_init(void)
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

    init_gatt_values();
    return 0;
}

static int
esp_blufi_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        ESP_LOGI(TAG, "connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);
        if (event->connect.status == 0) {

            blufi_env.is_connected = true;
            blufi_env.recv_seq = blufi_env.send_seq = 0;
            btc_msg_t msg;
            esp_blufi_cb_param_t param;
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_BLE_CONNECT;

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            memcpy(param.connect.remote_bda, desc.peer_id_addr.val, ESP_BLUFI_BD_ADDR_LEN);

            param.connect.conn_id = event->connect.conn_handle;
            /* save connection handle */
            blufi_env.conn_id = event->connect.conn_handle;
            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
        }
        if (event->connect.status != 0) {
            /* Connection failed; resume advertising. */
            esp_blufi_adv_start();
        }
        return 0;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "disconnect; reason=%d", event->disconnect.reason);
        memcpy(blufi_env.remote_bda, event->disconnect.conn.peer_id_addr.val, ESP_BLUFI_BD_ADDR_LEN);
        blufi_env.is_connected = false;
        blufi_env.recv_seq = blufi_env.send_seq = 0;
        blufi_env.sec_mode = 0x0;
        blufi_env.offset = 0;

        if (blufi_env.aggr_buf != NULL) {
            osi_free(blufi_env.aggr_buf);
            blufi_env.aggr_buf = NULL;
        }

        btc_msg_t msg;
        esp_blufi_cb_param_t param;

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_BLE_DISCONNECT;
        memcpy(param.disconnect.remote_bda, event->disconnect.conn.peer_id_addr.val, ESP_BLUFI_BD_ADDR_LEN);
        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);

        return 0;
    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        ESP_LOGI(TAG, "connection updated; status=%d",
                 event->conn_update.status);
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "advertise complete; reason=%d",
                 event->adv_complete.reason);
        esp_blufi_adv_start();
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d "
                 "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                 event->subscribe.conn_handle,
                 event->subscribe.attr_handle,
                 event->subscribe.reason,
                 event->subscribe.prev_notify,
                 event->subscribe.cur_notify,
                 event->subscribe.prev_indicate,
                 event->subscribe.cur_indicate);
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle,
                 event->mtu.channel_id,
                 event->mtu.value);
        blufi_env.frag_size = (event->mtu.value < BLUFI_MAX_DATA_LEN ? event->mtu.value : BLUFI_MAX_DATA_LEN) - BLUFI_MTU_RESERVED_SIZE;
        return 0;

    }
    return 0;
}

void esp_blufi_adv_start(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGI(TAG, "error determining address type; rc=%d ", rc);
        return;
    }

    /* Printing ADDR */
    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

    /* Begin advertising. */
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;

    /**
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info).
     *     o Advertising tx power.
     *     o Device name.
     *     o 16-bit service UUIDs (alert notifications).
     */

    memset(&fields, 0, sizeof fields);

    /* Advertise two flags:
    *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(BLUFI_APP_UUID)
    };
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;
    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "error setting advertisement data; rc=%d", rc);
        return;
    }

    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, esp_blufi_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "error enabling advertisement; rc=%d", rc);
        return;
    }
}

uint8_t esp_blufi_init(void)
{
    blufi_env.enabled = true;
    esp_blufi_cb_param_t param;
    param.init_finish.state = ESP_BLUFI_INIT_OK;
    btc_blufi_cb_to_app(ESP_BLUFI_EVENT_INIT_FINISH, &param);
    return ESP_BLUFI_ERROR;
}

void esp_blufi_deinit(void)
{
    blufi_env.enabled = false;
    btc_msg_t msg;
    esp_blufi_cb_param_t param;
    msg.pid = BTC_PID_BLUFI;
    msg.act = ESP_BLUFI_EVENT_DEINIT_FINISH;
    param.deinit_finish.state = ESP_BLUFI_DEINIT_OK;
    btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
}

void esp_blufi_send_notify(void *arg)
{
    struct pkt_info *pkts = (struct pkt_info *) arg;
    struct os_mbuf *om;
    om = ble_hs_mbuf_from_flat(pkts->pkt, pkts->pkt_len);
    if (om == NULL) {
        ESP_LOGE(TAG, "Error in allocating memory");
        return;
    }
    int rc = 0;
    rc = ble_gatts_notify_custom(blufi_env.conn_id, gatt_values[1].val_handle, om);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error in sending notification");
    }
}

void esp_blufi_disconnect(void)
{
    ble_gap_terminate(blufi_env.conn_id, BLE_ERR_REM_USER_CONN_TERM);
}

void esp_blufi_adv_stop(void) {}

void esp_blufi_send_encap(void *arg)
{
    struct blufi_hdr *hdr = (struct blufi_hdr *)arg;
    if (blufi_env.is_connected == false) {
        BTC_TRACE_WARNING("%s ble connection is broken\n", __func__);
        return;
    }
    btc_blufi_send_notify((uint8_t *)hdr,
                          ((hdr->fc & BLUFI_FC_CHECK) ?
                           hdr->data_len + sizeof(struct blufi_hdr) + 2 :
                           hdr->data_len + sizeof(struct blufi_hdr)));
}

void esp_blufi_btc_init(void)
{
    int rc;
    rc = btc_init();
    assert(rc == 0);
}

void esp_blufi_btc_deinit(void)
{
    btc_deinit();
}

int esp_blufi_handle_gap_events(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    if (event != NULL) {
        switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                btc_msg_t msg;
                esp_blufi_cb_param_t param;

                blufi_env.is_connected = true;
                blufi_env.recv_seq = blufi_env.send_seq = 0;
                blufi_env.conn_id  = event->connect.conn_handle;

                msg.sig = BTC_SIG_API_CB;
                msg.pid = BTC_PID_BLUFI;
                msg.act = ESP_BLUFI_EVENT_BLE_CONNECT;

                rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
                assert(rc == 0);
                memcpy(param.connect.remote_bda, desc.peer_id_addr.val, ESP_BLUFI_BD_ADDR_LEN);

                param.connect.conn_id = event->connect.conn_handle;
                btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
            }
            return 0;
        case BLE_GAP_EVENT_DISCONNECT: {
            btc_msg_t msg;
            esp_blufi_cb_param_t param;

            blufi_env.is_connected = false;
            blufi_env.recv_seq = blufi_env.send_seq = 0;
            blufi_env.sec_mode = 0x0;
            blufi_env.offset = 0;

            memcpy(blufi_env.remote_bda,
                   event->disconnect.conn.peer_id_addr.val,
                   ESP_BLUFI_BD_ADDR_LEN);

            if (blufi_env.aggr_buf != NULL) {
                osi_free(blufi_env.aggr_buf);
                blufi_env.aggr_buf = NULL;
            }

            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_BLE_DISCONNECT;
            memcpy(param.disconnect.remote_bda,
                   event->disconnect.conn.peer_id_addr.val,
                   ESP_BLUFI_BD_ADDR_LEN);
            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
            return 0;
        }

        case BLE_GAP_EVENT_MTU:
            blufi_env.frag_size = (event->mtu.value < BLUFI_MAX_DATA_LEN ? event->mtu.value :
                                   BLUFI_MAX_DATA_LEN) - BLUFI_MTU_RESERVED_SIZE;
            return 0;
        }
    }

    return 0;
}

#endif
