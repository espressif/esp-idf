/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
* SPDX-License-Identifier: Unlicense OR CC0-1.0
*/

#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_cs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/hid/ble_svc_hid.h"
#include "services/ras/ble_svc_ras.h"
#include "ble_chan_initiator.h"

#define REAL_TIME_RANGING_DATA_BIT (1<<0)

#define BLE_UUID_RANGING_SERVICE_VAL (0x185B)
/** @brief UUID of the RAS Features Characteristic. **/
#define BLE_UUID_RAS_FEATURES_VAL (0x2C14)

/** @brief UUID of the Real-time Ranging Data Characteristic. **/
#define BLE_UUID_RAS_REALTIME_RD_VAL (0x2C15)

/** @brief UUID of the On-demand Ranging Data Characteristic. **/
#define BLE_UUID_RAS_ONDEMAND_RD_VAL (0x2C16)

/** @brief UUID of the RAS Control Point Characteristic. **/
#define BLE_UUID_RAS_CP_VAL (0x2C17)

/** @brief UUID of the Ranging Data Ready Characteristic. **/
#define BLE_UUID_RAS_RD_READY_VAL (0x2C18)

/** @brief UUID of the Ranging Data Overwritten Characteristic. **/
#define BLE_UUID_RAS_RD_OVERWRITTEN_VAL (0x2C19)

static const char *tag = "NimBLE_RAS_INITIATOR";
static int blecent_gap_event(struct ble_gap_event *event, void *arg);

void ble_store_config_init(void);

static uint8_t connh;
static uint16_t ble_svc_ras_feat_val_handle;
static uint16_t ble_svc_ras_rd_val_handle;
static uint16_t ble_svc_ras_rd_ov_val_handle;
static uint16_t ble_svc_ras_cp_val_handle;
static uint16_t ble_svc_ras_od_val_handle;

static int32_t most_recent_peer_ranging_counter = -1;
static int32_t most_recent_local_ranging_counter= -1 ;
static int32_t dropped_ranging_counter = -1;

static uint8_t subscribe_all = 0;
static struct cs_steps_data local_cs_steps_data[LOCAL_PROCEDURE_MEM];
static uint8_t nap;

static void
blecent_on_disc_complete(const struct peer *peer, int status, void *arg);
static int blecs_gap_event(struct ble_cs_event *event, void *arg);
int blecent_on_subscribe(uint16_t conn_handle,
                     const struct ble_gatt_error *error,
                     struct ble_gatt_attr *attr,
                     void *arg)
{
    MODLOG_DFLT(INFO, "Subscribe complete; status=%d conn_handle=%d "
                "attr_handle=%d\n",
                error->status, conn_handle, attr->handle);
    if (error->status == 0) {
        subscribe_all++;
        if (subscribe_all == 4){
            struct ble_cs_initiator_procedure_start_params param;
            memset(&param,0,sizeof param);
            param.conn_handle=conn_handle;
            param.cb=blecs_gap_event;
            param.cb_arg=NULL;
            ble_cs_initiator_procedure_start(&param);
        }
    }

    return 0;
}
static int ble_chan_ras_subsribe_by_uuid(ble_uuid_t *char_uuid,const struct peer *peer, uint16_t conn_handle)
 {
    int rc;
    uint8_t value[2];
    const struct peer_dsc *dsc;
    dsc = peer_dsc_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
                             char_uuid,
                             BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16));
    if (dsc == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks a CCCD for RAS Feature characteristic\n");
        return 1;
    }

    value[0] = 2;
    value[1] = 0;
    rc = ble_gattc_write_flat(conn_handle, dsc->dsc.handle,
             value, sizeof value, blecent_on_subscribe, NULL);

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to subscribe to characteristic ,rc=%d\n", rc);
        return 1;
    }
    return 0;
 }

void
ble_chan_ras_subscribe(const struct peer *peer, uint16_t conn_handle)
{
    int rc;
    const struct peer_svc* svc = peer_svc_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL));
    if (svc == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS \n");
        goto err;
    }

    rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_CP_VAL),peer,conn_handle);

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS CP or fail to subscribe \n");
        goto err;
    } else {
        MODLOG_DFLT(INFO, "Subscribed to the RAS Control Point characteristic\n");
    }

    rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_READY_VAL),peer,conn_handle);

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS Data Ready or fail to subscribe \n");
        goto err;
    } else {
        MODLOG_DFLT(INFO, "Subscribed to the RAS Data Ready characteristic\n");
    }

    rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_OVERWRITTEN_VAL),peer,conn_handle);

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS Data overwritten or fail to subscribe \n");
        goto err;
    }   else {
          MODLOG_DFLT(INFO, "Subscribed to the RAS Data Overwritten characteristic\n");
    }

    return ;

err:
    /* Terminate the connection. */
    ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

static int
blecent_on_custom_read(uint16_t conn_handle,
                       const struct ble_gatt_error *error,
                       struct ble_gatt_attr *attr,
                       void *arg)
{
    struct peer *peer = (struct peer *)arg;
    MODLOG_DFLT(INFO, "Read complete RAS FEATURES; status=%d conn_handle=%d", error->status, conn_handle);

    if (error->status == 0) {
        uint32_t var = 0;
        int rc = 0;
        uint16_t pktlen = (attr->om ? OS_MBUF_PKTLEN(attr->om) : 0);
        if (attr->om && pktlen > 0 && pktlen <= sizeof(var)) {
            os_mbuf_copydata(attr->om, 0, pktlen, &var);
         }
         else {
            MODLOG_DFLT(ERROR, "Error: Invalid data length=%u in the attribute\n", pktlen);
            return -1;
         }
        MODLOG_DFLT(INFO, " attr_handle = %u value = %" PRIu32, attr->handle, var);

        if (var & REAL_TIME_RANGING_DATA_BIT) {
            /* Subscribe real time ranging data char */
            rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_REALTIME_RD_VAL), peer, conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS on demand raging data or fail to subscribe \n");
                return -1;
            } else {
                MODLOG_DFLT(INFO, "Subscribed to the RAS Realtime Ranging Data characteristic\n");
            }
        } else {
            /* Subscribe On demand ranging data */
            rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_ONDEMAND_RD_VAL), peer, conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS on demand ranging data or fail to subscribe \n");
                return -1;
            } else {
                MODLOG_DFLT(INFO, "Subscribed to the RAS On Demand Ranging Data characteristic\n");
            }
        }
        ble_chan_ras_subscribe(peer, conn_handle);
    } else if (error->status == BLE_ATT_ERR_INSUFFICIENT_AUTHEN) {
        MODLOG_DFLT(INFO, "Error: Insufficient authentication to read the characteristic\n");
        return -1;
    }
    return 0;
}

/**
 * Called when service discovery of the specified peer has completed.
 */
static void
blecent_on_disc_complete(const struct peer *peer, int status, void *arg)
{
    int rc;
    if (status != 0) {
        /* Service discovery failed.  Terminate the connection. */
        MODLOG_DFLT(ERROR, "Error: Service discovery failed; status=%d "
                    "conn_handle=%d\n", status, peer->conn_handle);
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    /* Service discovery has completed successfully.  Now we have a complete
     * list of services, characteristics, and descriptors that the peer
     * supports.
     */
    MODLOG_DFLT(INFO, "Service discovery complete; status=%d "
                "conn_handle=%d\n", status, peer->conn_handle);

    const struct peer_chr * chr = peer_chr_find_uuid(peer,
                                                     BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
                                                     BLE_UUID16_DECLARE(BLE_UUID_RAS_CP_VAL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Control Point characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    ble_svc_ras_cp_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
                             BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_READY_VAL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Data Ready characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_rd_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
                             BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_OVERWRITTEN_VAL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Data Overwritten characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_rd_ov_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
                             BLE_UUID16_DECLARE(BLE_UUID_RAS_FEATURES_VAL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Feature characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_feat_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
                             BLE_UUID16_DECLARE(BLE_UUID_RAS_ONDEMAND_RD_VAL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS On Demand Ranging Data characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_od_val_handle = chr->chr.val_handle;

    rc = ble_gattc_read(peer->conn_handle, ble_svc_ras_feat_val_handle,
                        blecent_on_custom_read, (void *)peer);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "2 Error: Failed to read the custom subscribable characteristic; ""rc=%d\n", rc);
    }
}

/**
 * Initiates the GAP general discovery procedure.
 */
static void
blecent_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Tell the controller to filter duplicates; we don't want to process
     * repeated advertisements from the same device.
     */
    disc_params.filter_duplicates = 1;

    /**
     * Perform a passive scan.  I.e., don't send follow-up scan requests to
     * each advertiser.
     */
    disc_params.passive = 1;

    /* Use defaults for the rest of the parameters. */
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      blecent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery procedure; rc=%d\n",
                    rc);
    }
}

/**
 * Connects to the sender of the specified advertisement of it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the Alert Notification service.
 */
static void
blecent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;
    const uint8_t *adv_data = NULL;
    int adv_data_len = 0;
    int found = 0;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    struct ble_gap_ext_disc_desc *ext = (struct ble_gap_ext_disc_desc *)disc;
    addr = &ext->addr;
    adv_data = ext->data;
    adv_data_len = ext->length_data;
#else
    struct ble_gap_disc_desc *d = (struct ble_gap_disc_desc *)disc;
    addr = &d->addr;
    adv_data = d->data;
    adv_data_len = d->length_data;
#endif

    for (int i = 1; i < adv_data_len - 1; i++) {
        if (adv_data[i-1] == 0x03 && adv_data[i] == 0x5b && adv_data[i+1] == 0x18) {
            found = 1;
            MODLOG_DFLT(DEBUG, "Found 0x5b18 at index %d\n", i);
            break;
        }
    }
    if (!found) {
        MODLOG_DFLT(DEBUG, "0x5b18 not found in adv_data, skipping connect.\n");
        return;
    }
#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
     /* Scanning must be stopped before a connection can be initiated. */
      rc = ble_gap_disc_cancel();
      if (rc != 0) {
          MODLOG_DFLT(DEBUG, "Failed to cancel scan; rc=%d\n", rc);
          return;
      }
#endif

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }
    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL,
                        blecent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Connection failed.\n");
        return;
    }
}

static int blecs_gap_event(struct ble_cs_event *event, void *arg){

    switch(event->type){

        case BLE_CS_EVENT_CS_PROCEDURE_COMPLETE:
            MODLOG_DFLT(INFO, "LE CS Procedure Complete ,procedure counter");
            break;
        case BLE_CS_EVENT_SUBEVET_RESULT:
            MODLOG_DFLT(INFO, "LE CS Subevent Result ,procedure counter: %d\n", event->subev_result.procedure_counter);
            MODLOG_DFLT(INFO, "event->subev_result.procedure_done_status =%d\n",event->subev_result.procedure_done_status);
            if (event->subev_result.subevent_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED) {
                MODLOG_DFLT(INFO, "LE CS Subevent Result , status: Aborted\n");
                            dropped_ranging_counter=event->subev_result.procedure_counter;
                return 0;
            }

            if (dropped_ranging_counter == event->subev_result.procedure_counter) {
               MODLOG_DFLT(INFO, "Ranging procedure was aborted\n");
                return 0;
            }
            if (event->subev_result.num_steps_reported) {
                size_t copy_size = event->subev_result.num_steps_reported * sizeof(struct cs_steps_data);
                memcpy(&local_cs_steps_data, event->subev_result.steps, MIN(copy_size, sizeof(local_cs_steps_data)));
            }
            nap=event->subev_result.num_antenna_paths;
            most_recent_local_ranging_counter=event->subev_result.procedure_counter;
            if (event->subev_result.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE) {
                most_recent_local_ranging_counter=event->subev_result.procedure_counter;
            } else if (event->subev_result.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED) {
                MODLOG_DFLT(INFO, "LE CS Procedure , status: Aborted\n");
                memset(&local_cs_steps_data,0,sizeof local_cs_steps_data);
            }

            break;

        case BLE_CS_EVENT_SUBEVET_RESULT_CONTINUE:
            MODLOG_DFLT(INFO, "event->subev_result_continue.procedure_done_status =%d\n",event->subev_result_continue.procedure_done_status);
            break;
        default:
            return 0;

    }
    return 0;
}

static int exchange_func(uint16_t conn_handle, const struct ble_gatt_error *error, uint16_t mtu, void *arg)
{
    MODLOG_DFLT(INFO, "MTU exchange complete; status=%d conn_handle=%d mtu=%d\n",
                 error->status, conn_handle, mtu);
    return 0;
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that is
 * established.  blecent uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  blecent.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
blecent_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;

    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
        if (rc != 0) {
            return 0;
        }

        /* An advertisement report was received during GAP discovery. */
        print_adv_fields(&fields);

        /* Try to connect to the advertiser if it looks interesting. */
        blecent_connect_if_interesting(&event->disc);
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            /* Connection successfully established. */
            MODLOG_DFLT(INFO, "Connection established ");

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            MODLOG_DFLT(INFO, "\n");

            /* Remember peer. */
            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to add peer; rc=%d\n", rc);
                return 0;
            }
            connh = event->connect.conn_handle;
            subscribe_all = 0;
            rc = ble_gap_security_initiate(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(INFO, "Security could not be initiated, rc = %d\n", rc);
                return ble_gap_terminate(event->connect.conn_handle,
                                         BLE_ERR_REM_USER_CONN_TERM);
            }

        } else {
            /* Connection attempt failed; resume scanning. */
            MODLOG_DFLT(ERROR, "Error: Connection failed; status=%d\n",
                        event->connect.status);
            blecent_scan();
        }

        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        /* Forget about peer. */
        peer_delete(event->disconnect.conn.conn_handle);

        /* Resume scanning. */
        blecent_scan();
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        ble_gattc_exchange_mtu(event->enc_change.conn_handle, exchange_func, NULL);
        rc = peer_disc_all(event->enc_change.conn_handle,blecent_on_disc_complete, NULL);
        if(rc != 0) {
           MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
           return 0;
        }
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        /* Peer sent us a notification or indication. */
        MODLOG_DFLT(INFO, "received %s; conn_handle=%d attr_handle=%d "
                    "attr_len=%d\n",
                    event->notify_rx.indication ?
                    "indication" :
                    "notification",
                    event->notify_rx.conn_handle,
                    event->notify_rx.attr_handle,
                    OS_MBUF_PKTLEN(event->notify_rx.om));

        /* Attribute data is contained in event->notify_rx.om. Use
         * `os_mbuf_copydata` to copy the data received in notification mbuf */

        if(ble_svc_ras_cp_val_handle == event->notify_rx.attr_handle){
            MODLOG_DFLT(INFO, "RAS Control Point Indication\n");
            uint8_t value[RASCP_CMD_OPCODE_LEN + sizeof(uint16_t)] = {0};
            int len = OS_MBUF_PKTLEN(event->notify_rx.om);
            if (len < RASCP_CMD_OPCODE_LEN + sizeof(uint16_t)) {
                MODLOG_DFLT(ERROR, "Error: Invalid RAS Control Point Indication length %d\n", len);
                return 0;
            }
            os_mbuf_copydata(event->notify_rx.om, 0, RASCP_CMD_OPCODE_LEN + sizeof(uint16_t), &value);

            MODLOG_DFLT(INFO, "Received RAS Control Point Indication, ranging counter = %d\n", *(uint16_t *)&value[RASCP_CMD_OPCODE_LEN]);
            MODLOG_DFLT(INFO, "Received RAS Control Point Indication, opcode = %d\n", value[0]);

            if (value[0] == RASCP_RSP_OPCODE_COMPLETE_RD_RSP) {
                // Reuse the same
                value[0] =RASCP_OPCODE_ACK_RD ;
                uint16_t ranging_counter=most_recent_peer_ranging_counter;
                // copy ranging counter to the value
                memcpy(&value[RASCP_CMD_OPCODE_LEN], &ranging_counter, sizeof(uint16_t));
                rc = ble_gattc_write_no_rsp_flat(event->notify_rx.conn_handle, ble_svc_ras_cp_val_handle, &value, sizeof value);
            } else if( value[0] == RASCP_RSP_OPCODE_RSP_CODE) {
                MODLOG_DFLT(INFO, "Sucssefully completed the Ranging procedure\n");
                vTaskDelay(1000 / portTICK_PERIOD_MS);

            }
        }
        else if(ble_svc_ras_rd_val_handle == event->notify_rx.attr_handle){
            MODLOG_DFLT(INFO, "Received Ranging Data Ready Indication\n");
            uint16_t ranging_counter;
            os_mbuf_copydata(event->notify_rx.om, 0, sizeof(uint16_t), &ranging_counter);
            most_recent_peer_ranging_counter=ranging_counter;
            most_recent_local_ranging_counter= ranging_counter;

            if (most_recent_peer_ranging_counter!=most_recent_local_ranging_counter) {
               MODLOG_DFLT(INFO, "Ranging counter mismatch : %" PRId32 " , %" PRId32,
	                          most_recent_peer_ranging_counter,most_recent_local_ranging_counter);
                return 0;
            }

            uint8_t value[RASCP_CMD_OPCODE_LEN + sizeof(uint16_t)] = {0};
            value[0] = RASCP_OPCODE_GET_RD;

            memcpy(&value[RASCP_CMD_OPCODE_LEN], &ranging_counter, sizeof(uint16_t));
            rc = ble_gattc_write_no_rsp_flat(event->notify_rx.conn_handle, ble_svc_ras_cp_val_handle, &value, sizeof value);

        } else if (ble_svc_ras_rd_ov_val_handle == event->notify_rx.attr_handle) {
            /*
            handle the incoming notification
            */
        } else if (ble_svc_ras_od_val_handle == event->notify_rx.attr_handle) {
            MODLOG_DFLT(INFO, "Received On Demand Ranging Data\n");

            int notif_len = OS_MBUF_PKTLEN(event->notify_rx.om);

#define RAS_OD_RX_BUF_MAX 512
            if (notif_len >= sizeof(struct segment_header) && notif_len <= RAS_OD_RX_BUF_MAX) {
                uint8_t ras_buf[RAS_OD_RX_BUF_MAX];
                int rc_copy = os_mbuf_copydata(event->notify_rx.om, 0, notif_len, ras_buf);
                if (rc_copy == 0) {
                    uint8_t *data = ras_buf + sizeof(struct segment_header);
                    size_t data_len = notif_len - sizeof(struct segment_header);

                    MODLOG_DFLT(INFO,
                                "Received On Demand Ranging Data , len = %d\n",
                                notif_len);

                    for (size_t i = 0; i < data_len; i++) {
                        MODLOG_DFLT(INFO, "data[%d] = %d\n", (int)i, data[i]);
                    }
                }
            }
#undef RAS_OD_RX_BUF_MAX

            /*
            handle the incoming notification
            */
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

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_EXT_DISC:

        ext_print_adv_report(&event->ext_disc);
        blecent_connect_if_interesting(&event->ext_disc);
        return 0;
#endif
        return 0;
    default:
        return 0;
    }
}

static void
blecent_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
blecent_on_sync(void)
{
    /* Make sure we have set Host feature bit for Channel Sounding*/
    int rc;
    rc = ble_gap_set_host_feat(47,0x01);
    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Begin scanning for a peripheral to connect to. */
    blecent_scan();
}

void blecent_host_task(void *param)
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

    /* Configure the host. */
    ble_hs_cfg.reset_cb = blecent_on_reset;
    ble_hs_cfg.sync_cb = blecent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Initialize data structures to track connected peers. */
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble_ble_channel_sounding");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(blecent_host_task);

}
