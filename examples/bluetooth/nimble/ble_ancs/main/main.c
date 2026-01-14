/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "nimble/ble.h"
#include "esp_peripheral.h"
#include "ble_ancs.h"
#include "esp_timer.h"

uint16_t notification_source_handle;
uint16_t data_source_handle;
uint16_t control_point_handle;
uint16_t svc_e_handle;

static void periodic_timer_callback(void* arg);
esp_timer_handle_t periodic_timer;

const esp_timer_create_args_t periodic_timer_args = {
    .callback = &periodic_timer_callback,
    /* name is optional, but may help identify the timer when debugging */
    .name = "periodic"
};

struct data_source_buffer {
    uint8_t buffer[1024];
    uint16_t len;
};

static struct data_source_buffer data_buffer = {0};
static uint16_t MTU_size = 23;
//In its basic form, the ANCS exposes three characteristics:
// service UUID: 7905F431-B5CE-4E99-A40F-4B1E122D00D0
static  ble_uuid128_t APPLE_NC_UUID = BLE_UUID128_INIT(
            0xD0, 0x00, 0x2D, 0x12, 0x1E, 0x4B, 0x0F, 0xA4, 0x99, 0x4E, 0xCE, 0xB5, 0x31, 0xF4, 0x05, 0x79);

// Notification Source UUID: 9FBF120D-6301-42D9-8C58-25E699A21DBD(notifiable)
static   ble_uuid128_t notification_source = BLE_UUID128_INIT(
        0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f);
// Control Point UUID:69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9(writeable with response)
static   ble_uuid128_t control_point = BLE_UUID128_INIT(
        0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98, 0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69);
// Data Source UUID:22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB(notifiable)
static   ble_uuid128_t data_source = BLE_UUID128_INIT(
        0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22);

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t ext_adv_pattern_1[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0xab, 0xcd,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x18, 0x11,
    0x0e, BLE_HS_ADV_TYPE_COMP_NAME, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'a', 'n', 'c', 's', '-', 'e',
};
#endif

static const char *tag = "NimBLE_ANCS";
static int ble_ancs_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t own_addr_type;

void ble_store_config_init(void);

static ble_noti_attr_list_t p_attr[8] = {
    [attr_appidentifier_index] = {
        .noti_attribute_id = NotificationAttributeIDAppIdentifier,
        .attribute_len = 0,
    },
    [attr_title_index] = {
        .noti_attribute_id = NotificationAttributeIDTitle,
        .attribute_len = 0xFFFF,
    },
    [attr_subtitle_index] = {
        .noti_attribute_id = NotificationAttributeIDSubtitle,
        .attribute_len = 0xFFFF,
    },
    [attr_message_index] = {
        .noti_attribute_id = NotificationAttributeIDMessage,
        .attribute_len = 0xFFFF,
    },
    [attr_messagesize_index] = {
        .noti_attribute_id = NotificationAttributeIDMessageSize,
        .attribute_len = 0,
    },
    [attr_date_index] = {
        .noti_attribute_id = NotificationAttributeIDDate,
        .attribute_len = 0,
    },
    [attr_positiveactionlabel_index] = {
        .noti_attribute_id = NotificationAttributeIDPositiveActionLabel,
        .attribute_len = 0,
    },
    [attr_negativeactionlabel_index] = {
        .noti_attribute_id = NotificationAttributeIDNegativeActionLabel,
        .attribute_len = 0,
    },
};


void ble_get_notification_attributes(uint16_t conn_handle,
                                     uint16_t control_point_handle,
                                     uint8_t *notificationUID,
                                     uint8_t num_attr,
                                     ble_noti_attr_list_t *p_attr)
{
    uint8_t cmd[600] = {0};

    uint32_t index = 0;
    cmd[index++] = CommandIDGetNotificationAttributes;

    memcpy(&cmd[index], notificationUID, ESP_NOTIFICATIONUID_LEN);
    index += ESP_NOTIFICATIONUID_LEN;

    while (num_attr > 0) {
        cmd[index++] = p_attr->noti_attribute_id;

        if (p_attr->attribute_len > 0) {
            cmd[index++] = (uint8_t)(p_attr->attribute_len & 0xFF);
            cmd[index++] = (uint8_t)((p_attr->attribute_len >> 8) & 0xFF);
        }

        p_attr++;
        num_attr--;
    }

    int rc = ble_gattc_write_flat(conn_handle,
                                  control_point_handle,
                                  cmd,
                                  index,
                                  NULL,  // no callback
                                  NULL); // no argument

    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to write notification attribute request: rc=%d\n", rc);

    }
}
void ble_perform_notification_action(uint8_t *notificationUID,
                                     uint8_t ActionID,
                                     uint16_t conn_handle,
                                     uint16_t control_point_handle)
{
    uint8_t buffer[600] = {0};

    uint16_t index = 0;
    buffer[index++] = CommandIDPerformNotificationAction;  // define this as needed

    memcpy(&buffer[index], notificationUID, ESP_NOTIFICATIONUID_LEN);
    index += ESP_NOTIFICATIONUID_LEN;

    buffer[index++] = ActionID;

    int rc = ble_gattc_write_flat(conn_handle,
                                  control_point_handle,
                                  buffer,
                                  index,
                                  NULL, // no callback
                                  NULL); // no callback argument

    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to write perform action command: rc=%d\n", rc);
    }
}

static void periodic_timer_callback(void* arg)
{
    esp_timer_stop(periodic_timer);
    if (data_buffer.len > 0) {
        ble_receive_apple_data_source(data_buffer.buffer, data_buffer.len);
        memset(data_buffer.buffer, 0, data_buffer.len);
        data_buffer.len = 0;
    }
}

static int
ancs_on_custom_subscribe(uint16_t conn_handle,
                        const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr,
                        void *arg)
{

    MODLOG_DFLT(INFO,
                "Subscribe to the custom subscribable characteristic complete; "
                "status=%d conn_handle=%d",
                error->status, conn_handle);

    return 0;
}
int ancs_dsc_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                                uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                                void *arg)
{
    int rc = 0;
    uint8_t cccd_val[2] = {0x01, 0x00}; // Enable notification
    if (error->status == 0) {
        if (ble_uuid_cmp(&dsc->uuid.u, BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16)) == 0) {
            rc = ble_gattc_write_flat(conn_handle,
                                    dsc->handle,
                                    cccd_val, sizeof(cccd_val),
                                    ancs_on_custom_subscribe, NULL);

            if (rc != 0) {
                MODLOG_DFLT(WARN, "Failed to subscribe to (rc=%d)", rc);
                return rc;
            }
        }
    } else {
        rc = error->status;
    }

    return rc;
}

static int
ancs_chr_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                    const struct ble_gatt_chr *chr, void *arg)
{
    int rc = 0;
    if (error->status == 0) {
        /* Notification Source */
        if ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) &&
             ble_uuid_cmp(&chr->uuid.u, &notification_source.u) == 0) {

            notification_source_handle = chr->val_handle;
            rc = ble_gattc_disc_all_dscs(conn_handle, chr->val_handle, svc_e_handle, ancs_dsc_discovered_cb, NULL);
            MODLOG_DFLT(INFO, "Found Notification Source characnteristic");

        }
        /* Data Source */
        else if ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) &&
                 ble_uuid_cmp(&chr->uuid.u, &data_source.u) == 0) {

            data_source_handle = chr->val_handle;
            rc = ble_gattc_disc_all_dscs(conn_handle, chr->val_handle, svc_e_handle, ancs_dsc_discovered_cb, NULL);
            MODLOG_DFLT(INFO, "Found Data Source characteristic");

        }
        /* Control Point */
        else if ((chr->properties & BLE_GATT_CHR_PROP_WRITE) &&
                 ble_uuid_cmp(&chr->uuid.u, &control_point.u) == 0) {

            control_point_handle = chr->val_handle;
            MODLOG_DFLT(INFO, "Found Control Point characteristic");
        }
    } else{
        rc = error->status;
    }

    return rc;
}

static int ancs_service_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                                    const struct ble_gatt_svc *svc, void *arg)
{
    int rc ;
    if (error->status == 0) {
        if (svc == NULL) {
            MODLOG_DFLT(WARN, "ANCS service not found");
            return -1;
        }

        svc_e_handle = svc->end_handle;
        /* discover characteristics within this service range */
        rc = ble_gattc_disc_all_chrs(conn_handle, svc->start_handle, svc->end_handle, ancs_chr_discovered_cb, NULL);
    } else {
        rc = error->status;
    }
    return rc;
}

#if NIMBLE_BLE_CONNECT
/**
 * Logs information about a connection to the console.
 */
static void
ble_ancs_print_conn_desc(struct ble_gap_conn_desc *desc)
{
    MODLOG_DFLT(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
                desc->conn_handle, desc->our_ota_addr.type);
    print_addr(desc->our_ota_addr.val);
    MODLOG_DFLT(INFO, " our_id_addr_type=%d our_id_addr=",
                desc->our_id_addr.type);
    print_addr(desc->our_id_addr.val);
    MODLOG_DFLT(INFO, " peer_ota_addr_type=%d peer_ota_addr=",
                desc->peer_ota_addr.type);
    print_addr(desc->peer_ota_addr.val);
    MODLOG_DFLT(INFO, " peer_id_addr_type=%d peer_id_addr=",
                desc->peer_id_addr.type);
    print_addr(desc->peer_id_addr.val);
    MODLOG_DFLT(INFO, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                "encrypted=%d authenticated=%d bonded=%d\n",
                desc->conn_itvl, desc->conn_latency,
                desc->supervision_timeout,
                desc->sec_state.encrypted,
                desc->sec_state.authenticated,
                desc->sec_state.bonded);
}
#endif

#if CONFIG_EXAMPLE_EXTENDED_ADV
/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
ext_ble_ancs_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;

    /* First check if any instance is already active */
    if(ble_gap_ext_adv_active(instance)) {
        return;
    }

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    /* enable connectable advertising */
    params.connectable = 1;

    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_PUBLIC;

    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.tx_power = 127;
    params.sid = 1;

    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   ble_ancs_gap_event, NULL);
    assert (rc == 0);

    /* in this case only scan response is allowed */

    /* get mbuf for scan rsp data */
    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);

    /* fill mbuf with scan rsp data */
    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);
}
#else
/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
ble_ancs_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
#if CONFIG_BT_NIMBLE_GAP_SERVICE
    const char *name;
#endif
    int rc;

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

    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_ancs_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}
#endif

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * ble_ancs uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                                  ble_ancs.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
ble_ancs_gap_event(struct ble_gap_event *event, void *arg)
{
#if NIMBLE_BLE_CONNECT
    struct ble_gap_conn_desc desc;
    int rc;
#endif

    switch (event->type) {

#if NIMBLE_BLE_CONNECT
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        MODLOG_DFLT(INFO, "connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            ble_ancs_print_conn_desc(&desc);
        }
        MODLOG_DFLT(INFO, "\n");

        if (event->connect.status != 0) {
            /* Connection failed; resume advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
            ext_ble_ancs_advertise();
#else
            ble_ancs_advertise();
#endif
        }

        /** Initiate security - It will perform
         * Pairing (Exchange keys)
         * Bonding (Store keys)
         * Encryption (Enable encryption)
         * Will invoke event BLE_GAP_EVENT_ENC_CHANGE
         **/
        rc = ble_gap_security_initiate(event->connect.conn_handle);
        if (rc != 0) {
            MODLOG_DFLT(INFO, "Security could not be initiated, rc = %d\n", rc);
            return ble_gap_terminate(event->connect.conn_handle,
                                    BLE_ERR_REM_USER_CONN_TERM);
        } else {
            MODLOG_DFLT(INFO, "Connection secured\n");
        }
    return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        ble_ancs_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        /* Connection terminated; resume advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_ble_ancs_advertise();
#else
        ble_ancs_advertise();
#endif
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        MODLOG_DFLT(INFO, "connection updated; status=%d ",
                    event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        ble_ancs_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                    event->adv_complete.reason);
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_ble_ancs_advertise();
#else
        ble_ancs_advertise();
#endif
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        ble_ancs_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        rc = ble_gattc_disc_svc_by_uuid(event->connect.conn_handle, &APPLE_NC_UUID.u,
                                    ancs_service_discovered_cb, NULL);
        if (rc != 0) {
            return rc;
        }
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        MODLOG_DFLT(INFO, "notify_tx event; conn_handle=%d attr_handle=%d "
                    "status=%d is_indication=%d",
                    event->notify_tx.conn_handle,
                    event->notify_tx.attr_handle,
                    event->notify_tx.status,
                    event->notify_tx.indication);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
    /* Peer sent us a notification or indication. */
        if (event->notify_rx.attr_handle == notification_source_handle) {
            ble_receive_apple_notification_source(event->notify_rx.om->om_data, event->notify_rx.om->om_len);
            uint8_t *notificationUID = &event->notify_rx.om->om_data[4];
            if (event->notify_rx.om->om_data[0] == EventIDNotificationAdded &&
                event->notify_rx.om->om_data[2] == CategoryIDIncomingCall) {
                MODLOG_DFLT(INFO, "IncomingCall, reject");
                ble_perform_notification_action(notificationUID, ActionIDNegative, event->notify_rx.conn_handle, control_point_handle);
            } else if (event->notify_rx.om->om_data[0] == EventIDNotificationAdded) {
                MODLOG_DFLT(INFO, "Get detailed information");
                ble_get_notification_attributes(event->notify_rx.conn_handle, control_point_handle,
                                                notificationUID, sizeof(p_attr)/sizeof(ble_noti_attr_list_t), p_attr);
            }
        } else if (event->notify_rx.attr_handle == data_source_handle) {
            memcpy(&data_buffer.buffer[data_buffer.len],
            event->notify_rx.om->om_data,
            event->notify_rx.om->om_len);
            data_buffer.len += event->notify_rx.om->om_len;

            if (event->notify_rx.om->om_len == (MTU_size - 3)) {
                esp_timer_start_periodic(periodic_timer, 500000);
            } else {
                esp_timer_stop(periodic_timer);
                ble_receive_apple_data_source(data_buffer.buffer, data_buffer.len);
                memset(data_buffer.buffer, 0, data_buffer.len);
                data_buffer.len = 0;
            }

        } else {
            MODLOG_DFLT(INFO, "unknown handle, receive notify value:" );
        }
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
        return 0;

    case BLE_GAP_EVENT_MTU:
        MTU_size = event->mtu.value;
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
        ESP_LOGI(tag, "PASSKEY_ACTION_EVENT started");
        struct ble_sm_io pkey = {0};
        int key = 0;

        if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
            pkey.action = event->passkey.params.action;
            pkey.passkey = 123456; // This is the passkey to be entered on peer
            ESP_LOGI(tag, "Enter passkey %" PRIu32 "on the peer side", pkey.passkey);
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
            ESP_LOGI(tag, "Passkey on device's display: %" PRIu32 , event->passkey.params.numcmp);
            ESP_LOGI(tag, "Accept or reject the passkey through console in this format -> key Y or key N");
            pkey.action = event->passkey.params.action;
            if (scli_receive_key(&key)) {
                pkey.numcmp_accept = key;
            } else {
                pkey.numcmp_accept = 0;
                ESP_LOGE(tag, "Timeout! Rejecting the key");
            }
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_OOB) {
            static uint8_t tem_oob[16] = {0};
            pkey.action = event->passkey.params.action;
            for (int i = 0; i < 16; i++) {
                pkey.oob[i] = tem_oob[i];
            }
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_INPUT) {
            ESP_LOGI(tag, "Enter the passkey through console in this format-> key 123456");
            pkey.action = event->passkey.params.action;
            if (scli_receive_key(&key)) {
                pkey.passkey = key;
            } else {
                pkey.passkey = 0;
                ESP_LOGE(tag, "Timeout! Passing 0 as the key");
            }
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d", rc);
        }
        return 0;

#endif
    }
    return 0;
}

static void
ble_ancs_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
ble_ancs_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Printing ADDR */
    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");
    /* Begin advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    ext_ble_ancs_advertise();
#else
    ble_ancs_advertise();
#endif
}

void ble_ancs_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void
app_main(void)
{
    int rc;

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }
    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = ble_ancs_on_reset;
    ble_hs_cfg.sync_cb = ble_ancs_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hs_cfg.sm_bonding = 1;
    /* Enable the appropriate bit masks to make sure the keys
     * that are needed are exchanged
     */
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_sc = 0;

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-ancs");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(ble_ancs_host_task);

}
