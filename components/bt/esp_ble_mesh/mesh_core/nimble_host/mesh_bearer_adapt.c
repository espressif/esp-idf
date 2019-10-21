/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "sdkconfig.h"

#include "btc/btc_task.h"
#include "osi/alarm.h"

#include "mbedtls/aes.h"
#include "mbedtls/ecp.h"

#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "mesh_hci.h"
#include "mesh_aes_encrypt.h"
#include "mesh_bearer_adapt.h"
#include "mesh_trace.h"
#include "mesh_buf.h"
#include "mesh_atomic.h"

#include "esp_ble_mesh_defs.h"

#include "provisioner_prov.h"
#include "mesh_common.h"

/** @def BT_UUID_MESH_PROV
 *  @brief Mesh Provisioning Service
 */
#define BT_UUID_MESH_PROV_VAL             0x1827
/** @def BT_UUID_MESH_PROXY
 *  @brief Mesh Proxy Service
 */
#define BT_UUID_MESH_PROXY_VAL            0x1828
/** @def BT_UUID_GATT_CCC
 *  @brief GATT Client Characteristic Configuration
 */
#define BT_UUID_GATT_CCC_VAL              0x2902
/** @def BT_UUID_MESH_PROV_DATA_IN
 *  @brief Mesh Provisioning Data In
 */
#define BT_UUID_MESH_PROV_DATA_IN_VAL     0x2adb
/** @def BT_UUID_MESH_PROV_DATA_OUT
 *  @brief Mesh Provisioning Data Out
 */
#define BT_UUID_MESH_PROV_DATA_OUT_VAL    0x2adc
/** @def BT_UUID_MESH_PROXY_DATA_IN
 *  @brief Mesh Proxy Data In
 */
#define BT_UUID_MESH_PROXY_DATA_IN_VAL    0x2add
/** @def BT_UUID_MESH_PROXY_DATA_OUT
 *  @brief Mesh Proxy Data Out
 */
#define BT_UUID_MESH_PROXY_DATA_OUT_VAL   0x2ade

#define BLE_MESH_GATT_GET_CONN_ID(conn_id)     ((u16_t)(conn_id))
#define BLE_MESH_GATT_CREATE_CONN_ID(conn_id)  ((u16_t)(conn_id))

static uint16_t proxy_svc_start_handle, prov_svc_start_handle;
struct bt_mesh_dev bt_mesh_dev;

/* P-256 Variables */
static u8_t bt_mesh_public_key[64];
static BT_OCTET32 bt_mesh_private_key = {
    0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38,
    0x74, 0xc9, 0xb3, 0xe3, 0xd2, 0x10, 0x3f, 0x50,
    0x4a, 0xff, 0x60, 0x7b, 0xeb, 0x40, 0xb7, 0x99,
    0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
};

/* Scan related functions */
static bt_mesh_scan_cb_t *bt_mesh_scan_dev_found_cb;

#if defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE
/* the gatt database list to save the attribute table */
static sys_slist_t bt_mesh_gatts_db;

/* Static Variables */
static struct bt_mesh_conn bt_mesh_gatts_conn[BLE_MESH_MAX_CONN];
static struct bt_mesh_conn_cb *bt_mesh_gatts_conn_cb;

static BD_ADDR bt_mesh_gatts_addr;

#endif /* defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE */

esp_err_t bt_mesh_host_init(void)
{
    int rc;
    rc = btc_init();
    if (rc != 0) {
        return ESP_FAIL;
    }

    rc = osi_alarm_create_mux();
    if (rc != 0) {
        return ESP_FAIL;
    }

    osi_alarm_init();
    return ESP_OK;
}

uint8_t ble_hs_hci_get_hci_version(void);

void bt_mesh_hci_init(void)
{
    /**
     * Currently 20ms non-connectable adv interval is supported, and we need to add
     * a flag to indicate this support.
     */
#ifdef CONFIG_BLE_MESH_HCI_5_0
    bt_mesh_dev.hci_version = BLE_MESH_HCI_VERSION_5_0;
#else
    bt_mesh_dev.hci_version = ble_hs_hci_get_hci_version();
#endif
    return;
}

static struct ble_gap_disc_params scan_param;
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static struct gattc_prov_info {
    /* Service to be found depends on the type of adv pkt received */
    struct bt_mesh_conn conn;
    bt_mesh_addr_t addr;
    u16_t service_uuid;
    u16_t mtu;
    bool  wr_desc_done;    /* Indicate if write char descriptor event is received */
    u16_t start_handle;    /* Service attribute start handle */
    u16_t end_handle;      /* Service attribute end handle */
    u16_t data_in_handle;  /* Data In Characteristic attribute handle */
    u16_t data_out_handle; /* Data Out Characteristic attribute handle */
    u16_t ccc_handle;      /* Data Out Characteristic CCC attribute handle */
} bt_mesh_gattc_info[BLE_MESH_MAX_CONN];

static struct bt_mesh_prov_conn_cb *bt_mesh_gattc_conn_cb;

static int ble_on_subscribe(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            struct ble_gatt_attr *attr,
                            void *arg)
{
    struct bt_mesh_conn *conn = NULL;
    uint8_t value[2] = {0x01, 0x00};
    int i = (int)arg, j, len;
    MODLOG_DFLT(INFO, "Subscribe complete; status=%d conn_handle=%d "
                "attr_handle=%d\n",
                error->status, conn_handle, attr->handle);

    for (j = i + 1; j < ARRAY_SIZE(bt_mesh_gattc_info); j++) {
        if ((bt_mesh_gattc_info[j].conn.handle == conn_handle) && bt_mesh_gattc_info[j].ccc_handle) {
            break;
        }
    }
    if (j == ARRAY_SIZE(bt_mesh_gattc_info)) {

        conn = &bt_mesh_gattc_info[i].conn;

        if (bt_mesh_gattc_info[i].ccc_handle != attr->handle) {
            BT_WARN("%s, gattc ccc_handle is not matched", __func__);
            bt_mesh_gattc_disconnect(conn);
            return 0;
        }

        if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->prov_write_descr != NULL) {
                len = bt_mesh_gattc_conn_cb->prov_write_descr(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn);
                if (len < 0) {
                    BT_ERR("%s, prov_write_descr failed", __func__);
                    bt_mesh_gattc_disconnect(conn);
                    return 0;
                }
                bt_mesh_gattc_info[i].wr_desc_done = true;
            }
        } else if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROXY_VAL) {
            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->proxy_write_descr != NULL) {
                len = bt_mesh_gattc_conn_cb->proxy_write_descr(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn);
                if (len < 0) {
                    BT_ERR("%s, proxy_write_descr failed", __func__);
                    bt_mesh_gattc_disconnect(conn);
                    return 0;
                }
                bt_mesh_gattc_info[i].wr_desc_done = true;
            }
        }


        return 0;
    }

    ble_gattc_write_flat(conn_handle, bt_mesh_gattc_info[i].ccc_handle, value, sizeof(value), ble_on_subscribe, (void *)j);
    return 0;
}

static int dsc_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
                      uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                      void *arg)
{
    int rc = 0, j, i = (int)arg; /* char index */
    uint8_t value[2] = {0x01, 0x00};

    switch (error->status) {
    case 0:
        bt_mesh_gattc_info[i].ccc_handle = dsc->handle;
        break;

    case BLE_HS_EDONE:
        /* All descriptors in this characteristic discovered; start discovering
         * descriptors in the next characteristic.
         */
        for (j = i + 1; j < ARRAY_SIZE(bt_mesh_gattc_info); j++) {
            if ((bt_mesh_gattc_info[j].conn.handle == conn_handle) && bt_mesh_gattc_info[j].data_out_handle) {
                break;
            }
        }
        if (j == ARRAY_SIZE(bt_mesh_gattc_info)) {
            /* Register Notification for Mesh Provisioning/Proxy Data Out Characteristic */
            for (j = 0; j < ARRAY_SIZE(bt_mesh_gattc_info); j++) {
                if ((bt_mesh_gattc_info[j].conn.handle == conn_handle) && bt_mesh_gattc_info[j].ccc_handle) {
                    break;
                }
            }
            if (j == ARRAY_SIZE(bt_mesh_gattc_info)) {
                return 0;
            }
            ble_gattc_write_flat(conn_handle, bt_mesh_gattc_info[i].ccc_handle, value, sizeof(value), ble_on_subscribe, (void *)j);
        } else {
            ble_gattc_disc_all_dscs(conn_handle, bt_mesh_gattc_info[j].data_out_handle, 0xffff, dsc_disced, (void *)j);
        }
        rc = 0;
        break;

    default:
        /* Error; abort discovery. */
        rc = error->status;
        break;
    }

    return rc;
}


static int chr_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
                      const struct ble_gatt_chr *chr, void *arg)
{
    int rc = 0, j;
    uint16_t uuid16 = 0;
    int i = (int)arg; /* service index */
    struct bt_mesh_conn *conn = &bt_mesh_gattc_info[i].conn;
    const ble_uuid_any_t *uuid = &chr->uuid;
    if (chr) {
        uuid16 = (uint16_t) BLE_UUID16(uuid)->value;
    }
    switch (error->status) {
    case 0:
        /* Get Mesh Provisioning/Proxy Data In/Out Characteristic */
        if ((uuid16 == BLE_MESH_UUID_MESH_PROV_DATA_IN_VAL) || (uuid16 == BLE_MESH_UUID_MESH_PROXY_DATA_IN_VAL)) {
            if (!(chr->properties & BLE_MESH_GATT_CHRC_WRITE_WITHOUT_RESP)) {
                bt_mesh_gattc_disconnect(conn);
                BT_ERR("Write without response is not set for Data In characteristic");
                return 0;
            }
            bt_mesh_gattc_info[i].data_in_handle = chr->val_handle;
        } else if ((uuid16 == BLE_MESH_UUID_MESH_PROV_DATA_OUT_VAL) || (uuid16 == BLE_MESH_UUID_MESH_PROXY_DATA_OUT_VAL)) {
            if (!(chr->properties & BLE_MESH_GATT_CHRC_NOTIFY)) {
                bt_mesh_gattc_disconnect(conn);
                BT_ERR("Notify is not set for Data Out characteristic");
                return 0;
            }
            bt_mesh_gattc_info[i].data_out_handle = chr->val_handle;
        }
        break;
    case BLE_HS_EDONE:
        /* All characteristics in this service discovered; start discovering
         * characteristics in the next service.
         */
        for (j = i + 1; j < ARRAY_SIZE(bt_mesh_gattc_info); j++) {
            if ((bt_mesh_gattc_info[j].conn.handle == conn_handle) && (bt_mesh_gattc_info[j].start_handle > bt_mesh_gattc_info[j].end_handle)) {
                break;
            }
        }
        if (j == ARRAY_SIZE(bt_mesh_gattc_info)) {
            for (j = 0; j < ARRAY_SIZE(bt_mesh_gattc_info); j++) {
                if ((bt_mesh_gattc_info[j].conn.handle == conn_handle) && bt_mesh_gattc_info[j].data_out_handle) {
                    break;
                }
            }
            ble_gattc_disc_all_dscs(conn_handle, bt_mesh_gattc_info[j].data_out_handle, 0xffff, dsc_disced, (void *)j);
        } else {
            ble_gattc_disc_all_chrs(conn_handle, bt_mesh_gattc_info[j].start_handle, bt_mesh_gattc_info[j].end_handle,
                                    chr_disced, (void *)j);
        }
        break;

    default:
        rc = error->status;
        break;
    }

    return rc;
}


static int svc_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
                      const struct ble_gatt_svc *service, void *arg)
{
    struct bt_mesh_conn *conn = NULL;
    int rc = 0, i;
    const ble_uuid_any_t *uuid;
    uint8_t uuid_length;
    switch (error->status) {
    case 0:
        if (!service) {
            return 0;
        }
        uuid = &service->uuid;
        uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);
        if (uuid_length != 2) {
            return 0;
        }
        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].service_uuid == (uint16_t)BLE_UUID16(uuid)->value) {
                bt_mesh_gattc_info[i].start_handle = service->start_handle;
                bt_mesh_gattc_info[i].end_handle   = service->end_handle;
                break;
            }
        }

        break;
    case BLE_HS_EDONE:
        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].conn.handle == conn_handle) {
                break;
            }
        }

        if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
            BT_ERR("%s, Conn handle is not found", __func__);
            return 0;
        }
        conn = &bt_mesh_gattc_info[i].conn;
        if (bt_mesh_gattc_info[i].start_handle == 0x00 ||
                bt_mesh_gattc_info[i].end_handle   == 0x00 ||
                (bt_mesh_gattc_info[i].start_handle > bt_mesh_gattc_info[i].end_handle)) {
            bt_mesh_gattc_disconnect(conn);
            return 0;
        }

        /* Get the characteristic num within Mesh Provisioning/Proxy Service */
        ble_gattc_disc_all_chrs(conn_handle, bt_mesh_gattc_info[i].start_handle, bt_mesh_gattc_info[i].end_handle,
                                chr_disced, (void *)i);
        break;

    default:
        rc = error->status;
        break;
    }

    return rc;
}


#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

static int disc_cb(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_disc_desc *desc;

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    int rc, i;
    uint8_t notif_data[100];
    uint16_t notif_len;
    ssize_t len;
    struct ble_gap_conn_desc conn_desc;
    struct bt_mesh_conn *conn = NULL;
#endif

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        desc = &event->disc;

        struct net_buf_simple *buf = bt_mesh_alloc_buf(desc->length_data);
        if (!buf) {
            BT_ERR("%s, Failed to allocate memory", __func__);
            return 0;
        }
        net_buf_simple_add_mem(buf, desc->data, desc->length_data);

        if (bt_mesh_scan_dev_found_cb) {
            bt_mesh_scan_dev_found_cb((bt_mesh_addr_t *)&desc->addr, desc->rssi, desc->event_type, buf);
        }
        osi_free(buf);
        break;
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            /* Connection successfully established. */
            MODLOG_DFLT(INFO, "Connection established ");

            rc = ble_gap_conn_find(event->connect.conn_handle, &conn_desc);
            assert(rc == 0);

            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->connected != NULL) {
                for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                    if (!memcmp(bt_mesh_gattc_info[i].addr.val, conn_desc.peer_id_addr.val, BLE_MESH_ADDR_LEN)) {
                        bt_mesh_gattc_info[i].conn.handle = event->connect.conn_handle;
                        (bt_mesh_gattc_conn_cb->connected)(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn, i);
                        break;
                    }
                }
            }
        }
#if BLE_MESH_DEV
        if (!bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
            rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER, &scan_param, disc_cb, NULL);
            if (rc != 0) {
                BT_ERR("%s, Invalid status %d", __func__, rc);
                break;
            }
            bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
        }
#else
        rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER, &scan_param, disc_cb, NULL);
        if (rc != 0) {
            BT_ERR("%s, Invalid status %d", __func__, rc);
            break;
        }
#endif /* BLE_MESH_DEV */
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->disconnected != NULL) {
            for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                memcpy(&conn_desc, &event->disconnect.conn, sizeof(conn_desc));
                if (!memcmp(bt_mesh_gattc_info[i].addr.val, conn_desc.peer_ota_addr.val, BLE_MESH_ADDR_LEN)) {
                    if (bt_mesh_gattc_info[i].conn.handle == event->disconnect.conn.conn_handle) {
                        (bt_mesh_gattc_conn_cb->disconnected)(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn, event->disconnect.reason);
                        if (!bt_mesh_gattc_info[i].wr_desc_done) {
                            /* Add this in case connection is established, connected event comes, but
                             * connection is terminated before server->filter_type is set to PROV.
                             */
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
                            if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                                provisioner_clear_link_conn_info(bt_mesh_gattc_info[i].addr.val);
                            }
#endif
                        }
                    } else {
                        /* Add this in case connection is failed to be established, and here we
                         * need to clear some provision link info, like connecting flag, device
                         * uuid, address info, etc.
                         */
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
                        if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                            provisioner_clear_link_conn_info(bt_mesh_gattc_info[i].addr.val);
                        }
#endif
                    }
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
                    if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                        /* Decrease prov pbg_count */
                        provisioner_pbg_count_dec();
                    }
#endif
                    /* Reset corresponding gattc info */
                    memset(&bt_mesh_gattc_info[i], 0, sizeof(bt_mesh_gattc_info[i]));
                    bt_mesh_gattc_info[i].conn.handle = 0xFFFF;
                    bt_mesh_gattc_info[i].mtu = BLE_ATT_MTU_DFLT;
                    bt_mesh_gattc_info[i].wr_desc_done = false;
                    break;
                }
            }
        }
        break;
    case BLE_GAP_EVENT_MTU:
        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].conn.handle == event->mtu.conn_handle) {
                bt_mesh_gattc_info[i].mtu = event->mtu.value;
                break;
            }
        }
        /** Once mtu exchanged accomplished, start to find services, and here
         *  need a flag to indicate which service to find(Mesh Prov Service or
         *  Mesh Proxy Service)
         */
        ble_uuid_any_t bt_uuid;
        if (i != ARRAY_SIZE(bt_mesh_gattc_info)) {
            //service_uuid.len       = sizeof(bt_mesh_gattc_info[i].service_uuid);
            if (sizeof(bt_mesh_gattc_info[i].service_uuid) == 0x02) {
                bt_uuid.u16.u.type = BLE_UUID_TYPE_16;
                bt_uuid.u16.value = bt_mesh_gattc_info[i].service_uuid;

            } else if (sizeof(bt_mesh_gattc_info[i].service_uuid) == 0x10) {
                bt_uuid.u128.u.type = BLE_UUID_TYPE_128;
                memcpy(bt_uuid.u128.value, &bt_mesh_gattc_info[i].service_uuid, 16);
            }
            /* Search Mesh Provisioning Service or Mesh Proxy Service */
            ble_gattc_disc_all_svcs(bt_mesh_gattc_info[i].conn.handle, svc_disced, NULL);
        }
        break;
    case BLE_GAP_EVENT_NOTIFY_RX:
        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].conn.handle == event->notify_rx.conn_handle) {
                break;
            }
        }

        if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
            BT_ERR("%s, Conn handle is not found", __func__);
            return 0;
        }

        conn = &bt_mesh_gattc_info[i].conn;
        ble_gap_conn_find(event->notify_rx.conn_handle, &conn_desc);

        if (bt_mesh_gattc_info[i].data_out_handle != event->notify_rx.attr_handle) {
            /* Data isn't populated yet */
            return 0;
        }

        if (memcmp(bt_mesh_gattc_info[i].addr.val, conn_desc.peer_id_addr.val, BLE_MESH_ADDR_LEN) ||
                (bt_mesh_gattc_info[i].data_out_handle != event->notify_rx.attr_handle) ||
                (event->notify_rx.indication != 0)) {
            BT_ERR("%s, Notification error", __func__);
            bt_mesh_gattc_disconnect(conn);
            return 0;
        }

        notif_len = OS_MBUF_PKTLEN(event->notify_rx.om);
        rc = os_mbuf_copydata(event->notify_rx.om, 0, notif_len, notif_data);

        if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->prov_notify != NULL) {
                len = bt_mesh_gattc_conn_cb->prov_notify(&bt_mesh_gattc_info[i].conn,
                        notif_data, notif_len);
                if (len < 0) {
                    BT_ERR("%s, prov_notify failed", __func__);
                    bt_mesh_gattc_disconnect(conn);
                    return 0;
                }
            }
        } else if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROXY_VAL) {
            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->proxy_notify != NULL) {
                len = bt_mesh_gattc_conn_cb->proxy_notify(&bt_mesh_gattc_info[i].conn,
                        notif_data, notif_len);
                if (len < 0) {
                    BT_ERR("%s, proxy_notify failed", __func__);
                    bt_mesh_gattc_disconnect(conn);
                    return 0;
                }
            }
        }
        break;
#endif
    default:
        break;
    }

    return 0;
}

static int start_le_scan(u8_t scan_type, u16_t interval, u16_t window, u8_t filter_dup)
{

    scan_param.filter_duplicates = filter_dup;
    scan_param.itvl = interval;
    scan_param.window = window;

    if (scan_type == BLE_MESH_SCAN_PASSIVE) {
        scan_param.passive = 1;
    } else {
        scan_param.passive = 0;
    }
    ble_gap_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER, &scan_param, disc_cb, NULL);

#if BLE_MESH_DEV
    if (scan_type == BLE_MESH_SCAN_ACTIVE) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ACTIVE_SCAN);
    } else {
        bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ACTIVE_SCAN);
    }
#endif

    return 0;
}

static int set_ad(const struct bt_mesh_adv_data *ad, size_t ad_len, u8_t *buf, u8_t *buf_len)
{
    int i;

    for (i = 0; i < ad_len; i++) {
        buf[(*buf_len)++] = ad[i].data_len + 1;
        buf[(*buf_len)++] = ad[i].type;

        memcpy(&buf[*buf_len], ad[i].data,
               ad[i].data_len);
        *buf_len += ad[i].data_len;
    }

    return 0;
}

#if defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE
static struct bt_mesh_gatt_attr *bt_mesh_gatts_find_attr_by_handle(u16_t handle);

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        MODLOG_DFLT(INFO, "connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
        }
        MODLOG_DFLT(INFO, "\n");
#if BLE_MESH_DEV
        /* When connection is created, advertising will be stopped automatically. */
        bt_mesh_atomic_test_and_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif
        if (bt_mesh_gatts_conn_cb != NULL && bt_mesh_gatts_conn_cb->connected != NULL) {
            u8_t index = BLE_MESH_GATT_GET_CONN_ID(event->connect.conn_handle);
            if (index < BLE_MESH_MAX_CONN) {
                bt_mesh_gatts_conn[index].handle = BLE_MESH_GATT_GET_CONN_ID(event->connect.conn_handle);
                (bt_mesh_gatts_conn_cb->connected)(&bt_mesh_gatts_conn[index], 0);
            }
            memcpy(bt_mesh_gatts_addr, desc.peer_id_addr.val, BLE_MESH_ADDR_LEN);
            /* This is for EspBleMesh Android app. When it tries to connect with the
             * device at the first time and it fails due to some reason. And after
             * the second connection, the device needs to send GATT service change
             * indication to the phone manually to notify it dicovering service again.
             */
            ble_svc_gatt_changed(prov_svc_start_handle, 0xffff);

        }

        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        MODLOG_DFLT(INFO, "\n");
#if BLE_MESH_DEV
        bt_mesh_atomic_test_and_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif
        if (bt_mesh_gatts_conn_cb != NULL && bt_mesh_gatts_conn_cb->disconnected != NULL) {
            u8_t index = BLE_MESH_GATT_GET_CONN_ID(event->disconnect.conn.conn_handle);
            if (index < BLE_MESH_MAX_CONN) {
                bt_mesh_gatts_conn[index].handle = BLE_MESH_GATT_GET_CONN_ID(event->disconnect.conn.conn_handle);
                (bt_mesh_gatts_conn_cb->disconnected)(&bt_mesh_gatts_conn[index], event->disconnect.reason);
            }
            memset(bt_mesh_gatts_addr, 0x0, BLE_MESH_ADDR_LEN);
        }

        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        MODLOG_DFLT(INFO, "connection updated; status=%d ",
                    event->conn_update.status);
        rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
        assert(rc == 0);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                    event->adv_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
        assert(rc == 0);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        MODLOG_DFLT(INFO, "subscribe event; conn_handle=%d attr_handle=%d "
                    "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                    event->subscribe.conn_handle,
                    event->subscribe.attr_handle,
                    event->subscribe.reason,
                    event->subscribe.prev_notify,
                    event->subscribe.cur_notify,
                    event->subscribe.prev_indicate,
                    event->subscribe.cur_indicate);
        struct bt_mesh_gatt_attr *attr = bt_mesh_gatts_find_attr_by_handle(event->subscribe.attr_handle + 1);
        u8_t index = BLE_MESH_GATT_GET_CONN_ID(event->subscribe.conn_handle);
        u16_t len = 0;
        uint16_t ccc_val = 0;

        if (event->subscribe.prev_notify != event->subscribe.cur_notify) {
            ccc_val = event->subscribe.cur_notify;
        } else if (event->subscribe.prev_indicate != event->subscribe.cur_indicate) {
            if (event->subscribe.cur_indicate) {
                ccc_val = 2;
            } else {
                ccc_val = 0;
            }
        }

        if (attr != NULL && attr->write != NULL) {
            if ((len = attr->write(&bt_mesh_gatts_conn[index], attr,
                                   &ccc_val,
                                   sizeof(ccc_val),
                                   0 /* offset */, 0)) > 0) {
            }
        }

        return 0;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* We already have a bond with the peer, but it is attempting to
         * establish a new secure link.  This app sacrifices security for
         * convenience: just throw away the old bond and accept the new link.
         */

        /* Delete the old bond. */
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with the pairing operation.
         */
        return BLE_GAP_REPEAT_PAIRING_RETRY;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        MODLOG_DFLT(INFO, "PASSKEY_ACTION_EVENT started \n");
        return 0;
    }

    return 0;
}
#else

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    return 0;
}
#endif

/* APIs functions */
int bt_le_adv_start(const struct bt_mesh_adv_param *param,
                    const struct bt_mesh_adv_data *ad, size_t ad_len,
                    const struct bt_mesh_adv_data *sd, size_t sd_len)
{
#if BLE_MESH_DEV
    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        return -EALREADY;
    }
#endif
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    uint8_t buf_len = 0;
    int err;
    struct ble_gap_adv_params adv_params;

    err = set_ad(ad, ad_len, buf, &buf_len);
    if (err) {
        BT_ERR("set_ad failed: err %d", err);
        return err;
    }

    err = ble_gap_adv_set_data(buf, buf_len);
    if (err != 0) {
        BT_ERR("Advertising set failed: err %d", err);
        return err;
    }

    if (sd && (param->options & BLE_MESH_ADV_OPT_CONNECTABLE)) {
        buf_len = 0;

        err = set_ad(sd, sd_len, buf, &buf_len);
        if (err) {
            BT_ERR("set_ad failed: err %d", err);
            return err;
        }

        err = ble_gap_adv_rsp_set_data(buf, buf_len);
        if (err != 0) {
            BT_ERR("Scan rsp failed: err %d", err);
            return err;
        }
    }

    memset(&adv_params, 0, sizeof adv_params);
    adv_params.itvl_min = param->interval_min;
    adv_params.itvl_max = param->interval_max;

    if (param->options & BLE_MESH_ADV_OPT_CONNECTABLE) {
        adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
        adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    } else if (sd != NULL) {
        adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
        adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    } else {
        adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
        adv_params.disc_mode = BLE_GAP_DISC_MODE_NON;
    }

    err = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params,
                            gap_event_cb, NULL);
    if (err) {
        BT_ERR("Advertising start failed: err %d", err);
        return err;
    }

#if BLE_MESH_DEV
    bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);

    if (!(param->options & BLE_MESH_ADV_OPT_ONE_TIME)) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    }
#endif

    return 0;
}

int bt_le_adv_stop(void)
{
#if BLE_MESH_DEV
    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    if (!bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        return 0;
    }
#endif
    ble_gap_adv_stop();

#if BLE_MESH_DEV
    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif

    return 0;
}

int bt_le_scan_start(const struct bt_mesh_scan_param *param, bt_mesh_scan_cb_t cb)
{
    int err;

#if BLE_MESH_DEV
    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        return -EALREADY;
    }
#endif

#if BLE_MESH_DEV
    if (param->filter_dup) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCAN_FILTER_DUP);
    } else {
        bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCAN_FILTER_DUP);
    }
#endif

    err = start_le_scan(param->type, param->interval, param->window, param->filter_dup);
    if (err) {
        return err;
    }

#if BLE_MESH_DEV
    bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
#endif

    bt_mesh_scan_dev_found_cb = cb;
    return err;
}

int bt_le_scan_stop(void)
{
#if BLE_MESH_DEV
    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
        ble_gap_disc_cancel();
    }
#else
    ble_gap_disc_cancel();
#endif

    bt_mesh_scan_dev_found_cb = NULL;
    return 0;
}
#if defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE

void bt_mesh_gatts_conn_cb_register(struct bt_mesh_conn_cb *cb)
{
    bt_mesh_gatts_conn_cb = cb;
}

static struct bt_mesh_gatt_attr *bt_mesh_gatts_find_attr_by_handle(u16_t handle)
{
    struct bt_mesh_gatt_service *svc = NULL;
    struct bt_mesh_gatt_attr *attr = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER(&bt_mesh_gatts_db, svc, node) {
        int i;

        for (i = 0; i < svc->attr_count; i++) {
            attr = &svc->attrs[i];
            /* Check the attrs handle is equal to the handle or not */
            if (attr->handle == handle) {
                return attr;
            }
        }
    }

    return NULL;
}

static void bt_mesh_gatts_foreach_attr(u16_t start_handle, u16_t end_handle,
                                       bt_mesh_gatt_attr_func_t func, void *user_data)
{
    struct bt_mesh_gatt_service *svc = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER(&bt_mesh_gatts_db, svc, node) {
        int i;

        for (i = 0; i < svc->attr_count; i++) {
            struct bt_mesh_gatt_attr *attr = &svc->attrs[i];

            /* Check if attribute handle is within range */
            if (attr->handle < start_handle ||
                    attr->handle > end_handle) {
                continue;
            }

            if (func(attr, user_data) == BLE_MESH_GATT_ITER_STOP) {
                return;
            }
        }
    }
}

static u8_t find_next(const struct bt_mesh_gatt_attr *attr, void *user_data)
{
    struct bt_mesh_gatt_attr **next = user_data;

    *next = (struct bt_mesh_gatt_attr *)attr;

    return BLE_MESH_GATT_ITER_STOP;
}

static struct bt_mesh_gatt_attr *bt_mesh_gatts_attr_next(const struct bt_mesh_gatt_attr *attr)
{
    struct bt_mesh_gatt_attr *next = NULL;

    bt_mesh_gatts_foreach_attr(attr->handle + 1, attr->handle + 1, find_next, &next);

    return next;
}

ssize_t bt_mesh_gatts_attr_read(struct bt_mesh_conn *conn, const struct bt_mesh_gatt_attr *attr,
                                void *buf, u16_t buf_len, u16_t offset,
                                const void *value, u16_t value_len)
{
    u16_t len;

    if (offset > value_len) {
        return BLE_MESH_GATT_ERR(BLE_MESH_ATT_ERR_INVALID_OFFSET);
    }

    len = MIN(buf_len, value_len - offset);

    BT_DBG("handle 0x%04x offset %u length %u", attr->handle, offset, len);

    memcpy(buf, value + offset, len);

    return len;
}

struct gatts_incl {
    u16_t start_handle;
    u16_t end_handle;
    u16_t uuid16;
} __packed;

ssize_t bt_mesh_gatts_attr_read_included(struct bt_mesh_conn *conn,
        const struct bt_mesh_gatt_attr *attr,
        void *buf, u16_t len, u16_t offset)
{
    struct bt_mesh_gatt_attr *incl = attr->user_data;
    struct bt_mesh_uuid *uuid = incl->user_data;
    struct gatts_incl pdu = {0};
    u8_t value_len;

    /* First attr points to the start handle */
    pdu.start_handle = sys_cpu_to_le16(incl->handle);
    value_len = sizeof(pdu.start_handle) + sizeof(pdu.end_handle);

    /*
     * Core 4.2, Vol 3, Part G, 3.2,
     * The Service UUID shall only be present when the UUID is a 16-bit Bluetooth UUID.
     */
    if (uuid->type == BLE_MESH_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BLE_MESH_UUID_16(uuid)->val);
        value_len += sizeof(pdu.uuid16);
    }

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

ssize_t bt_mesh_gatts_attr_read_service(struct bt_mesh_conn *conn,
                                        const struct bt_mesh_gatt_attr *attr,
                                        void *buf, u16_t len, u16_t offset)
{
    struct bt_mesh_uuid *uuid = attr->user_data;

    if (uuid->type == BLE_MESH_UUID_TYPE_16) {
        u16_t uuid16 = sys_cpu_to_le16(BLE_MESH_UUID_16(uuid)->val);

        return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, &uuid16, 2);
    }

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset,
                                   BLE_MESH_UUID_128(uuid)->val, 16);
}

struct gatts_chrc {
    u8_t  properties;
    u16_t value_handle;
    union {
        u16_t uuid16;
        u8_t  uuid[16];
    };
} __packed;

ssize_t bt_mesh_gatts_attr_read_chrc(struct bt_mesh_conn *conn,
                                     const struct bt_mesh_gatt_attr *attr, void *buf,
                                     u16_t len, u16_t offset)
{
    struct bt_mesh_gatt_char *chrc = attr->user_data;
    const struct bt_mesh_gatt_attr *next = NULL;
    struct gatts_chrc pdu = {0};
    u8_t value_len;

    pdu.properties = chrc->properties;
    /* BLUETOOTH SPECIFICATION Version 4.2 [Vol 3, Part G] page 534:
     * 3.3.2 Characteristic Value Declaration
     * The Characteristic Value declaration contains the value of the
     * characteristic. It is the first Attribute after the characteristic
     * declaration. All characteristic definitions shall have a
     * Characteristic Value declaration.
     */
    next = bt_mesh_gatts_attr_next(attr);
    if (!next) {
        BT_WARN("%s, No value for characteristic at 0x%04x", __func__, attr->handle);
        pdu.value_handle = 0x0000;
    } else {
        pdu.value_handle = sys_cpu_to_le16(next->handle);
    }
    value_len = sizeof(pdu.properties) + sizeof(pdu.value_handle);

    if (chrc->uuid->type == BLE_MESH_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BLE_MESH_UUID_16(chrc->uuid)->val);
        value_len += 2;
    } else {
        memcpy(pdu.uuid, BLE_MESH_UUID_128(chrc->uuid)->val, 16);
        value_len += 16;
    }

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

static int gatts_register(struct bt_mesh_gatt_service *svc)
{
    struct bt_mesh_gatt_service *last;
    u16_t handle;

    if (sys_slist_is_empty(&bt_mesh_gatts_db)) {
        handle = 0;
        goto populate;
    }

    last = SYS_SLIST_PEEK_TAIL_CONTAINER(&bt_mesh_gatts_db, last, node);
    handle = last->attrs[last->attr_count - 1].handle;
    BT_DBG("%s, handle =  %d", __func__, handle);

populate:
    sys_slist_append(&bt_mesh_gatts_db, &svc->node);
    return 0;
}

int bt_mesh_gatts_service_register(struct bt_mesh_gatt_service *svc)
{
    uint16_t offset = 0;
    int i;
    if (BLE_MESH_UUID_16(svc->attrs[0].user_data)->val == BT_UUID_MESH_PROXY_VAL) {
        offset = proxy_svc_start_handle;
    } else if (BLE_MESH_UUID_16(svc->attrs[0].user_data)->val == BT_UUID_MESH_PROV_VAL) {
        offset = prov_svc_start_handle;
    }

    for (i = 0; i < svc->attr_count; i++) {
        svc->attrs[i].handle = offset + i;
    }
    gatts_register(svc);
    return 0;
}

int bt_mesh_gatts_disconnect(struct bt_mesh_conn *conn, u8_t reason)
{
    u16_t conn_id = BLE_MESH_GATT_CREATE_CONN_ID(conn->handle);
    ble_gap_terminate(conn_id, reason);
    return 0;
}

int bt_mesh_gatts_service_unregister(struct bt_mesh_gatt_service *svc)
{
    assert(svc != NULL);
    BT_ERR("%s, Unsupported for NimBLE host", __func__);
    return 0;
}

int bt_mesh_gatts_notify(struct bt_mesh_conn *conn, const struct bt_mesh_gatt_attr *attr,
                         const void *data, u16_t len)
{
    struct os_mbuf *om;
    u16_t conn_id = BLE_MESH_GATT_CREATE_CONN_ID(conn->handle);

    om = ble_hs_mbuf_from_flat(data, len);
    assert(om);
    ble_gattc_notify_custom(conn_id, attr->handle, om);

    return 0;
}

u16_t bt_mesh_gatt_get_mtu(struct bt_mesh_conn *conn)
{
    return ble_att_preferred_mtu();
}

/* APIs added by Espressif */
int bt_mesh_gatts_service_stop(struct bt_mesh_gatt_service *svc)
{
    int rc;
    uint16_t handle;
    if (!svc) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }
    const ble_uuid_t *uuid;
    if (BLE_MESH_UUID_16(svc->attrs[0].user_data)->val == BT_UUID_MESH_PROXY_VAL) {
        uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROXY_VAL);
    } else {
        uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROV_VAL);
    }

    rc = ble_gatts_find_svc(uuid, &handle);
    assert(rc == 0);
    ble_gatts_svc_set_visibility(handle, 0);

    /* FIXME: figure out end handle */
    ble_svc_gatt_changed(handle, 0xffff);

    return 0;
}

int bt_mesh_gatts_service_start(struct bt_mesh_gatt_service *svc)
{
    int rc;
    uint16_t handle;
    const ble_uuid_t *uuid;
    if (BLE_MESH_UUID_16(svc->attrs[0].user_data)->val == BT_UUID_MESH_PROXY_VAL) {
        uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROXY_VAL);
    } else {
        uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROV_VAL);
    }

    rc = ble_gatts_find_svc(uuid, &handle);
    assert(rc == 0);
    ble_gatts_svc_set_visibility(handle, 1);

    /* FIXME: figure out end handle */
    ble_svc_gatt_changed(handle, 0xffff);

    return 0;
}
#endif /* defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE */

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
void bt_mesh_gattc_conn_cb_register(struct bt_mesh_prov_conn_cb *cb)
{
    bt_mesh_gattc_conn_cb = cb;
}

u8_t bt_mesh_gattc_get_free_conn_count(void)
{
    u8_t count = 0;
    u8_t i;

    for (i = 0U; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (bt_mesh_gattc_info[i].conn.handle == 0xFFFF &&
                bt_mesh_gattc_info[i].service_uuid == 0x0000) {
            ++count;
        }
    }

    return count;
}

u16_t bt_mesh_gattc_get_service_uuid(struct bt_mesh_conn *conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            break;
        }
    }

    if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
        return 0;
    }

    return bt_mesh_gattc_info[i].service_uuid;
}

/** For provisioner acting as a GATT client, it may follow the procedures
 *  listed below.
 *  1. Create connection with the unprovisioned device
 *  2. Exchange MTU size
 *  3. Find Mesh Prov Service in the device's service database
 *  4. Find Mesh Prov Data In/Out characteristic within the service
 *  5. Get CCC of Mesh Prov Data Out Characteristic
 *  6. Set the Notification bit of CCC
 */

int bt_mesh_gattc_conn_create(const bt_mesh_addr_t *addr, u16_t service_uuid)
{
    u8_t zero[6] = {0};
    int i, rc;

    if (!addr || !memcmp(addr->val, zero, BLE_MESH_ADDR_LEN) ||
            (addr->type > BLE_ADDR_RANDOM)) {
        BT_ERR("%s, Invalid remote address", __func__);
        return -EINVAL;
    }

    if (service_uuid != BLE_MESH_UUID_MESH_PROV_VAL &&
            service_uuid != BLE_MESH_UUID_MESH_PROXY_VAL) {
        BT_ERR("%s, Invalid service uuid 0x%04x", __func__, service_uuid);
        return -EINVAL;
    }

    /* Check if already creating connection with the device */
    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (!memcmp(bt_mesh_gattc_info[i].addr.val, addr->val, BLE_MESH_ADDR_LEN)) {
            BT_WARN("%s, Already create connection with %s",
                    __func__, bt_hex(addr->val, BLE_MESH_ADDR_LEN));
            return -EALREADY;
        }
    }

    /* Find empty element in queue to store device info */
    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if ((bt_mesh_gattc_info[i].conn.handle == 0xFFFF) &&
                (bt_mesh_gattc_info[i].service_uuid == 0x0000)) {
            memcpy(bt_mesh_gattc_info[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);
            bt_mesh_gattc_info[i].addr.type = addr->type;
            /* Service to be found after exhanging mtu size */
            bt_mesh_gattc_info[i].service_uuid = service_uuid;
            break;
        }
    }

    if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
        BT_WARN("%s, gattc info is full", __func__);
        return -ENOMEM;
    }

#if BLE_MESH_DEV
    if (bt_mesh_atomic_test_and_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        rc = ble_gap_disc_cancel();
        if (rc != 0) {
            return -1;
        }
    }
#else
    rc = ble_gap_disc_cancel();
    if (rc != 0) {
        return -1;
    }
#endif /* BLE_MESH_DEV */

    BT_DBG("%s, create conn with %s", __func__, bt_hex(addr->val, BLE_MESH_ADDR_LEN));

    /* Min_interval: 250ms
     * Max_interval: 250ms
     * Slave_latency: 0x0
     * Supervision_timeout: 32 sec
     */
    struct ble_gap_conn_params conn_params = {0};
    conn_params.itvl_min = 0xC8; /* (250 * 1000) / 1250 = 200 = 0xC8 */
    conn_params.itvl_max = 0xC8; /* (250 * 1000) / 1250 = 200 = 0xC8 */
    conn_params.latency = 0;
    conn_params.supervision_timeout = 0xC80;
    conn_params.scan_itvl = 0x0020; //0x0010
    conn_params.scan_window = 0x0020; //0x0010
    conn_params.min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN;
    conn_params.max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN;


    ble_addr_t peer_addr;
    memcpy(peer_addr.val, addr->val, 6);
    peer_addr.type = addr->type;

    rc = ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &peer_addr, BLE_HS_FOREVER, &conn_params,
                         disc_cb, NULL);

    return i;
}

static int mtu_cb(uint16_t conn_handle,
                  const struct ble_gatt_error *error,
                  uint16_t mtu, void *arg)
{
    int i;
    if (error->status == 0) {

        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].conn.handle == conn_handle) {
                bt_mesh_gattc_info[i].mtu = mtu;
                break;
            }
        }
    }
    return 0;
}



void bt_mesh_gattc_exchange_mtu(u8_t index)
{
    /** Set local MTU and exchange with GATT server.
     *  ATT_MTU >= 69 for Mesh GATT Prov Service
     *  ATT_NTU >= 33 for Mesh GATT Proxy Service
    */

    ble_gattc_exchange_mtu(bt_mesh_gattc_info[index].conn.handle, mtu_cb, NULL);
}

u16_t bt_mesh_gattc_get_mtu_info(struct bt_mesh_conn *conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            return bt_mesh_gattc_info[i].mtu;
        }
    }

    return 0;
}

int bt_mesh_gattc_write_no_rsp(struct bt_mesh_conn *conn, const struct bt_mesh_gatt_attr *attr,
                               const void *data, u16_t len)
{
    u16_t conn_id;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            break;
        }
    }

    if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
        BT_ERR("%s, Conn is not found", __func__);
        /** Here we return 0 for prov_send() return value check in provisioner.c
         */
        return 0;
    }

    conn_id = BLE_MESH_GATT_CREATE_CONN_ID(bt_mesh_gattc_info[i].conn.handle);

    struct os_mbuf *om;
    int rc;

    om = ble_hs_mbuf_from_flat(data, len);
    if (om == NULL) {
        return -1;
    }

    rc = ble_gattc_write_no_rsp(conn_id, bt_mesh_gattc_info[i].data_in_handle, om);
    if (rc != 0) {
        return -1;
    }

    return 0;
}

void bt_mesh_gattc_disconnect(struct bt_mesh_conn *conn)
{
    /** Disconnect
     *  Clear proper proxy server information
     *  Clear proper prov_link information
     *  Clear proper bt_mesh_gattc_info information
     *  Here in adapter, we just clear proper bt_mesh_gattc_info, and
     *  when proxy_disconnected callback comes, the proxy server
     *  information and prov_link information should be cleared.
     */
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            break;
        }
    }

    if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
        BT_ERR("%s, Conn is not found", __func__);
        return;
    }
    ble_gap_terminate(bt_mesh_gattc_info[i].conn.handle, BLE_ERR_REM_USER_CONN_TERM);
}

/** Mesh Provisioning Service:  0x1827
 *  Mesh Provisioning Data In:  0x2ADB
 *  Mesh Provisioning Data Out: 0x2ADC
 *  Mesh Proxy Service:  0x1828
 *  Mesh Proxy Data In:  0x2ADD
 *  Mesh PROXY Data Out: 0x2ADE
 */
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

struct bt_mesh_conn *bt_mesh_conn_ref(struct bt_mesh_conn *conn)
{
    bt_mesh_atomic_inc(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, bt_mesh_atomic_get(&conn->ref));

    return conn;
}

void bt_mesh_conn_unref(struct bt_mesh_conn *conn)
{
    bt_mesh_atomic_dec(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, bt_mesh_atomic_get(&conn->ref));
}

#if defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE
static int proxy_char_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR || ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC) {
        struct bt_mesh_gatt_attr *attr = bt_mesh_gatts_find_attr_by_handle(attr_handle);
        u8_t index = BLE_MESH_GATT_GET_CONN_ID(conn_handle);
        u16_t len = 0;

        BT_DBG("%s, write: handle = %d, len = %d, data = %s", __func__, attr_handle,
               ctxt->om->om_len,
               bt_hex(ctxt->om->om_data, ctxt->om->om_len));

        if (attr != NULL && attr->write != NULL) {
            if ((len = attr->write(&bt_mesh_gatts_conn[index], attr,
                                   ctxt->om->om_data,
                                   ctxt->om->om_len,
                                   0 /* offset */, 0)) > 0) {
            }
        }
    } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR || ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
        BT_ERR("%s, Unhandled read request for chr and dsc: opcode - %d", __func__, ctxt->op);
    }
    return 0;
}

static int dummy_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /*
     * We should never never enter this callback - it's attached to notify-only
     * characteristic which are notified directly from mbuf. And we can't pass
     * NULL as access_cb because gatts will assert on init...
     */
    assert(0);
    return 0;
}

static const struct ble_gatt_svc_def svc_defs [] = {
#ifdef CONFIG_BLE_MESH_GATT_PROXY_SERVER
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROXY_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROXY_DATA_IN_VAL),
                .access_cb = proxy_char_access_cb,
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROXY_DATA_OUT_VAL),
                .access_cb = dummy_access_cb,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },
#endif
#ifdef CONFIG_BLE_MESH_PB_GATT
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROV_VAL),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROV_DATA_IN_VAL),
                .access_cb = proxy_char_access_cb,
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
            }, {
                .uuid = BLE_UUID16_DECLARE(BT_UUID_MESH_PROV_DATA_OUT_VAL),
                .access_cb = dummy_access_cb,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },
#endif
    {
        0, /* No more services. */
    },
};
#endif

void gatt_register_cb(struct ble_gatt_register_ctxt *ctxt,
                      void *arg )
{
    if (ctxt->op == BLE_GATT_REGISTER_OP_SVC) {
        if (ble_uuid_cmp(ctxt->svc.svc_def->uuid, BLE_UUID16_DECLARE(BT_UUID_MESH_PROXY_VAL)) == 0) {
            proxy_svc_start_handle = ctxt->svc.handle;
        } else if (ble_uuid_cmp(ctxt->svc.svc_def->uuid, BLE_UUID16_DECLARE(BT_UUID_MESH_PROV_VAL)) == 0) {
            prov_svc_start_handle = ctxt->svc.handle;
        }
    }
}

void bt_mesh_gatt_init(void)
{
    ble_att_set_preferred_mtu(BLE_ATT_MTU_DFLT);

    ble_hs_cfg.gatts_register_cb = gatt_register_cb;

#if defined(CONFIG_BLE_MESH_NODE) && CONFIG_BLE_MESH_NODE
    int rc;
    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(svc_defs);
    assert(rc == 0);

    rc = ble_gatts_add_svcs(svc_defs);
    assert(rc == 0);

    ble_gatts_start();

    ble_gatts_svc_set_visibility(prov_svc_start_handle, 1);
    ble_gatts_svc_set_visibility(proxy_svc_start_handle, 0);
#endif

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    for (int i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        bt_mesh_gattc_info[i].conn.handle = 0xFFFF;
        bt_mesh_gattc_info[i].mtu = BLE_ATT_MTU_DFLT;
        bt_mesh_gattc_info[i].wr_desc_done = false;
    }
#endif
}

void ble_sm_alg_ecc_init(void);

void bt_mesh_adapt_init(void)
{
    BT_DBG("%s", __func__);
    /* initialization of P-256 parameters */
    ble_sm_alg_ecc_init();
}

int bt_mesh_rand(void *buf, size_t len)
{
    int i;

    if (buf == NULL || len == 0) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EAGAIN;
    }

    for (i = 0; i < (int)(len / sizeof(u32_t)); i++) {
        u32_t rand = esp_random();
        memcpy(buf + i * sizeof(u32_t), &rand, sizeof(u32_t));
    }

    BT_DBG("%s, rand: %s", __func__, bt_hex(buf, len));
    return 0;
}

void bt_mesh_set_private_key(const u8_t pri_key[32])
{
    memcpy(bt_mesh_private_key, pri_key, 32);
}

int ble_sm_alg_gen_key_pair(uint8_t *pub, uint8_t *priv);

const u8_t *bt_mesh_pub_key_get(void)
{
    uint8_t pri_key[32] = {0};

#if 1
    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_HAS_PUB_KEY)) {
        return bt_mesh_public_key;
    }
#else
    /* BLE Mesh BQB test case MESH/NODE/PROV/UPD/BV-12-C requires
     * different public key for each provisioning procedure.
     * Note: if enabled, when Provisioner provision multiple devices
     * at the same time, this may cause invalid confirmation value.
     */
    if (bt_mesh_rand(bt_mesh_private_key, 32)) {
        BT_ERR("%s, Unable to generate bt_mesh_private_key", __func__);
        return NULL;
    }
#endif

    int rc = ble_sm_alg_gen_key_pair(bt_mesh_public_key, pri_key);
    if (rc != 0) {
        BT_ERR("%s, Failed to generate the key pair", __func__);
        return NULL;
    }
    memcpy(bt_mesh_private_key, pri_key, 32);

    bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_HAS_PUB_KEY);
    BT_DBG("gen the bt_mesh_public_key:%s", bt_hex(bt_mesh_public_key, sizeof(bt_mesh_public_key)));

    return bt_mesh_public_key;
}

bool bt_mesh_check_public_key(const u8_t key[64])
{
    struct mbedtls_ecp_point pt = {0};
    mbedtls_ecp_group grp = {0};
    bool rc = false;

    uint8_t pub[65] = {0};
    /* Hardcoded first byte of pub key for MBEDTLS_ECP_PF_UNCOMPRESSED */
    pub[0] = 0x04;
    memcpy(&pub[1], key, 64);

    /* Initialize the required structures here */
    mbedtls_ecp_point_init(&pt);
    mbedtls_ecp_group_init(&grp);

    /* Below 3 steps are to validate public key on curve secp256r1 */
    if (mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1) != 0) {
        goto exit;
    }

    if (mbedtls_ecp_point_read_binary(&grp, &pt, pub, 65) != 0) {
        goto exit;
    }

    if (mbedtls_ecp_check_pubkey(&grp, &pt) != 0) {
        goto exit;
    }

    rc = true;

exit:
    mbedtls_ecp_point_free(&pt);
    mbedtls_ecp_group_free(&grp);
    return rc;

}

int ble_sm_alg_gen_dhkey(uint8_t *peer_pub_key_x, uint8_t *peer_pub_key_y,
                         uint8_t *our_priv_key, uint8_t *out_dhkey);

int bt_mesh_dh_key_gen(const u8_t remote_pk[64], bt_mesh_dh_key_cb_t cb, const u8_t idx)
{
    uint8_t dhkey[32];

    BT_DBG("private key = %s", bt_hex(bt_mesh_private_key, BT_OCTET32_LEN));

    ble_sm_alg_gen_dhkey((uint8_t *)&remote_pk[0], (uint8_t *)&remote_pk[32], bt_mesh_private_key, dhkey);

    if (cb != NULL) {
        cb((const u8_t *)dhkey, idx);
    }
    return 0;
}

#if CONFIG_MBEDTLS_HARDWARE_AES
static void ecb_encrypt(u8_t const *const key_le, u8_t const *const clear_text_le,
                        u8_t *const cipher_text_le, u8_t *const cipher_text_be)
{
    struct bt_mesh_ecb_param ecb;
    mbedtls_aes_context aes_ctx = {0};

    aes_ctx.key_bytes = 16;
    mem_rcopy(&aes_ctx.key[0], key_le, 16);
    mem_rcopy(&ecb.clear_text[0], clear_text_le, sizeof(ecb.clear_text));
    mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, &ecb.clear_text[0], &ecb.cipher_text[0]);

    if (cipher_text_le) {
        mem_rcopy(cipher_text_le, &ecb.cipher_text[0],
                  sizeof(ecb.cipher_text));
    }

    if (cipher_text_be) {
        memcpy(cipher_text_be, &ecb.cipher_text[0],
               sizeof(ecb.cipher_text));
    }
}

static void ecb_encrypt_be(u8_t const *const key_be, u8_t const *const clear_text_be,
                           u8_t *const cipher_text_be)
{
    struct bt_mesh_ecb_param ecb;
    mbedtls_aes_context aes_ctx = {0};

    aes_ctx.key_bytes = 16;
    memcpy(&aes_ctx.key[0], key_be, 16);
    memcpy(&ecb.clear_text[0], clear_text_be, sizeof(ecb.clear_text));
    mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, &ecb.clear_text[0], &ecb.cipher_text[0]);

    memcpy(cipher_text_be, &ecb.cipher_text[0], sizeof(ecb.cipher_text));
}
#endif /* CONFIG_MBEDTLS_HARDWARE_AES */

int bt_mesh_encrypt_le(const u8_t key[16], const u8_t plaintext[16],
                       u8_t enc_data[16])
{
#if CONFIG_MBEDTLS_HARDWARE_AES
    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    ecb_encrypt(key, plaintext, enc_data, NULL);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));
    return 0;
#else /* CONFIG_MBEDTLS_HARDWARE_AES */
    struct tc_aes_key_sched_struct s;
    u8_t tmp[16];

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    sys_memcpy_swap(tmp, key, 16);

    if (tc_aes128_set_encrypt_key(&s, tmp) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    sys_memcpy_swap(tmp, plaintext, 16);

    if (tc_aes_encrypt(enc_data, tmp, &s) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    sys_mem_swap(enc_data, 16);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));

    return 0;
#endif /* CONFIG_MBEDTLS_HARDWARE_AES */
}

int bt_mesh_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
                       u8_t enc_data[16])
{
#if CONFIG_MBEDTLS_HARDWARE_AES
    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    ecb_encrypt_be(key, plaintext, enc_data);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));

    return 0;
#else /* CONFIG_MBEDTLS_HARDWARE_AES */
    struct tc_aes_key_sched_struct s;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    if (tc_aes128_set_encrypt_key(&s, key) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    if (tc_aes_encrypt(enc_data, plaintext, &s) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));

    return 0;
#endif /* CONFIG_MBEDTLS_HARDWARE_AES */
}

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
int bt_mesh_update_exceptional_list(u8_t sub_code, u8_t type, void *info)
{
    BT_ERR("%s, Unsupported for NimBLE host", __func__);
    return 0;
}
#endif
