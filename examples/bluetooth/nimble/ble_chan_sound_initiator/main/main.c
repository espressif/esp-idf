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
#include "host/ble_cs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/hid/ble_svc_hid.h"
#include "services/ras/ble_svc_ras.h"
#include "blecent.h"

//#include "services/ras/src/rreq/ble_svc_ras_rreq.c"

#if CONFIG_EXAMPLE_USE_CI_ADDRESS
#ifdef CONFIG_IDF_TARGET_ESP32
#define TEST_CI_ADDRESS_CHIP_OFFSET (0)
#elif CONFIG_IDF_TARGET_ESP32C2
#define TEST_CI_ADDRESS_CHIP_OFFSET (1)
#elif CONFIG_IDF_TARGET_ESP32C3
#define TEST_CI_ADDRESS_CHIP_OFFSET (2)
#elif CONFIG_IDF_TARGET_ESP32C6
#define TEST_CI_ADDRESS_CHIP_OFFSET (3)
#elif CONFIG_IDF_TARGET_ESP32C5
#define TEST_CI_ADDRESS_CHIP_OFFSET (4)
#elif CONFIG_IDF_TARGET_ESP32H2
#define TEST_CI_ADDRESS_CHIP_OFFSET (5)
#elif CONFIG_IDF_TARGET_ESP32P4
#define TEST_CI_ADDRESS_CHIP_OFFSET (6)
#elif CONFIG_IDF_TARGET_ESP32S3
#define TEST_CI_ADDRESS_CHIP_OFFSET (7)
#elif CONFIG_IDF_TARGET_ESP32C61
#define TEST_CI_ADDRESS_CHIP_OFFSET (8)
#endif
#endif

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


/*** The UUID of the service containing the subscribable characteristic ***/
static const ble_uuid_t * remote_svc_uuid =
    BLE_UUID128_DECLARE(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                        0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

/*** The UUID of the subscribable chatacteristic ***/
static const ble_uuid_t * remote_chr_uuid =
    BLE_UUID128_DECLARE(0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
                        0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33);

static const char *tag = "NimBLE_RAS_INITIATOR";
static int blecent_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t peer_addr[6];


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
static void
blecent_on_disc_complete(const struct peer *peer, int status, void *arg);
int blecent_on_subscribe(uint16_t conn_handle,
                     const struct ble_gatt_error *error,
                     struct ble_gatt_attr *attr,
                     void *arg)
{
    struct peer *peer;
    MODLOG_DFLT(INFO, "Subscribe complete; status=%d conn_handle=%d "
                "attr_handle=%d\n",
                error->status, conn_handle, attr->handle);


    return 0;
}
static int
blecent_on_custom_read(uint16_t conn_handle,
                       const struct ble_gatt_error *error,
                       struct ble_gatt_attr *attr,
                       void *arg)
{
    MODLOG_DFLT(INFO,
                "Read complete for the subscribable characteristic; "
                "status=%d conn_handle=%d", error->status, conn_handle);
    int rc=0;
    extern void bt_hci_log_hci_data_show(void);
    bt_hci_log_hci_data_show();
    if (error->status == 0) {
        MODLOG_DFLT(INFO, " attr_handle=%d value=", attr->handle);
        print_mbuf(attr->om);
        extern void bt_hci_log_hci_adv_show(void);

        //extern void bt_hci_log_hci_data_show(void);
        bt_hci_log_hci_data_show();
        //bt_hci_log_hci_adv_show();

        return 0;
    }
    else if(error->status==261){
        // MODLOG_DFLT(INFO, "Error: Insufficient authentication to read the characteristic\n");
        //     printf("Error: Insufficient authentication to read the characteristic\n");
        //     rc = ble_gap_security_initiate(connh);

        //     if (rc != 0) {
        //         MODLOG_DFLT(INFO, "Security could not be initiated, rc = %d\n", rc);
        //         return ble_gap_terminate(connh,
        //                                  BLE_ERR_REM_USER_CONN_TERM);
        //     }

        //     rc = peer_disc_all(connh,
        //         blecent_on_disc_complete, NULL);
        //         if(rc != 0) {
        //             MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
        //             return 0;
        //         }
        //     vTaskDelay(1000);
        return 0;
    }

    MODLOG_DFLT(INFO, "\n");

    return 0;
}
static int ble_chan_ras_subsribe_by_uuid(ble_uuid_t *char_uuid,const struct peer *peer, uint8_t conn_handle)
 {
    int rc;
    int value[2];
    const struct peer_dsc *dsc;
    dsc = peer_dsc_find_uuid(peer,
        BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),
        char_uuid,
        BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16));
    if (dsc == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer lacks a CCCD for RAS Feature characteristic\n");
        return 1;
    }

    value[0] = 1;
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
ble_chan_ras_subscribe(const struct peer *peer, uint8_t conn_handle)
{

    int rc;
    /* Read the supported-new-alert-category characteristic. */
   const struct peer_svc* svc = peer_svc_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL));
    if (svc == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS \n");
        goto err;
    }

    // subscribe the  on demad ranging data char
    rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_ONDEMAND_RD_VAL),peer,conn_handle);

    if(rc!=0){
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS on demand raging data or fail to subscribe \n");
        goto err;
    }

    // control point char
    rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_CP_VAL),peer,conn_handle);

    if(rc!=0){
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS CP or fail to subscribe \n");
        goto err;
    }else{
        MODLOG_DFLT(INFO, "Subscribed to the RAS Control Point characteristic\n");
    }


    rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_READY_VAL),peer,conn_handle);

    if(rc!=0){
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS Data Ready or fail to subscribe \n");
        goto err;
    }


   rc = ble_chan_ras_subsribe_by_uuid(BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_OVERWRITTEN_VAL),peer,conn_handle);

   if(rc!=0){
       MODLOG_DFLT(ERROR, "Error: Peer doesn't support the RAS Data overwritten or fail to subscribe \n");
       goto err;
   }

    rc = ble_gattc_read(conn_handle, ble_svc_ras_cp_val_handle,
                        blecent_on_custom_read, NULL);
    rc = ble_gattc_read(conn_handle, ble_svc_ras_feat_val_handle,
                        blecent_on_custom_read, NULL);
    rc = ble_gattc_read(conn_handle, ble_svc_ras_rd_val_handle,
                            blecent_on_custom_read, NULL);
    rc = ble_gattc_read(conn_handle, ble_svc_ras_rd_ov_val_handle,
                            blecent_on_custom_read, NULL);

    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to read the custom subscribable characteristic; ""rc=%d\n", rc);
        goto err;
    }

return ;

err:
    /* Terminate the connection. */
    ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}
/**
 * Application callback.  Called when the attempt to subscribe to notifications
 * for the ANS Unread Alert Status characteristic has completed.
 */



/**
 * Called when service discovery of the specified peer has completed.
 */
static void
blecent_on_disc_complete(const struct peer *peer, int status, void *arg)
{

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

    struct peer_chr * chr = peer_chr_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),BLE_UUID16_DECLARE(BLE_UUID_RAS_CP_VAL));
    if(chr == NULL){
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Control Point characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    ble_svc_ras_cp_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_READY_VAL));
    if(chr == NULL){
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Data Ready characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_rd_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),BLE_UUID16_DECLARE(BLE_UUID_RAS_RD_OVERWRITTEN_VAL));
    if(chr == NULL){
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Data Overwritten characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_rd_ov_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),BLE_UUID16_DECLARE(BLE_UUID_RAS_FEATURES_VAL));
    if(chr == NULL){
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS Feature characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_feat_val_handle = chr->chr.val_handle;

    chr = peer_chr_find_uuid(peer,BLE_UUID16_DECLARE(BLE_UUID_RANGING_SERVICE_VAL),BLE_UUID16_DECLARE(BLE_UUID_RAS_ONDEMAND_RD_VAL));
    if(chr == NULL){
        MODLOG_DFLT(ERROR, "Error: Peer lacks the RAS On Demand Ranging Data characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }
    ble_svc_ras_od_val_handle = chr->chr.val_handle;


    ble_chan_ras_subscribe(peer,peer->conn_handle);
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
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the Alert Notification service.
 */
#if CONFIG_EXAMPLE_EXTENDED_ADV
static int
ext_blecent_should_connect(const struct ble_gap_ext_disc_desc *disc)
{
    int offset = 0;
    int ad_struct_len = 0;
#if CONFIG_EXAMPLE_USE_CI_ADDRESS
    uint32_t *addr_offset;
#endif // CONFIG_EXAMPLE_USE_CI_ADDRESS

    if (disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
        return 0;
    }
    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen    ("ADDR_ANY")) != 0)) {
#if !CONFIG_EXAMPLE_USE_CI_ADDRESS
        ESP_LOGI(tag, "Peer address from menuconfig: %s", CONFIG_EXAMPLE_PEER_ADDR);
        /* Convert string to address */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);
#else
        addr_offset = (uint32_t *)&peer_addr[1];
        *addr_offset = atoi(CONFIG_EXAMPLE_PEER_ADDR);
        peer_addr[5] = 0xC3;
        peer_addr[0] = TEST_CI_ADDRESS_CHIP_OFFSET;
#endif // !CONFIG_EXAMPLE_USE_CI_ADDRESS
        if (memcmp(peer_addr, disc->addr.val, sizeof(disc->addr.val)) != 0) {
            return 0;
        }
    }

    /* The device has to advertise support for the Alert Notification
    * service (0x1811).
    */
    do {
        ad_struct_len = disc->data[offset];

        if (!ad_struct_len) {
            break;
        }

	/* Search if ANS UUID is advertised */
        if (disc->data[offset] == 0x03 && disc->data[offset + 1] == 0x03) {
            if ( disc->data[offset + 2] == 0x18 && disc->data[offset + 3] == 0x11 ) {
                return 1;
            }
        }

        offset += ad_struct_len + 1;

     } while ( offset < disc->length_data );

    return 0;
}
#else
static int
blecent_should_connect(const struct ble_gap_disc_desc *disc)
{
    struct ble_hs_adv_fields fields;
    int rc;
    int i;


    /* The device has to be advertising connectability. */
    if (disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {

        return 0;
    }

    rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
    if (rc != 0) {
        return 0;
    }

    return 0;
}
#endif

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

    /* Don't do anything if we don't care about this advertiser. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    if (!ext_blecent_should_connect((struct ble_gap_ext_disc_desc *)disc)) {
        return;
    }
#else
    if (!blecent_should_connect((struct ble_gap_disc_desc *)disc)) {
        return;
    }
#endif

#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
    /* Scanning must be stopped before a connection can be initiated. */
    rc = ble_gap_disc_cancel();
    if (rc != 0) {
        MODLOG_DFLT(DEBUG, "Failed to cancel scan; rc=%d\n", rc);
        return;
    }
#endif

    /* Figure out address to use for connect (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Try to connect the the advertiser.  Allow 30 seconds (30000 ms) for
     * timeout.
     */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    addr = &((struct ble_gap_ext_disc_desc *)disc)->addr;
#else
    addr = &((struct ble_gap_disc_desc *)disc)->addr;
#endif

    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL,
                         blecent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to connect to device; addr_type=%d "
                    "addr=%s; rc=%d\n",
                    addr->type, addr_str(addr->val), rc);
        return;
    }
}

#define 	BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE   0x0
#define 	BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_PARTIAL   0x1
#define 	BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED   0xF

#define LOCAL_PROCEDURE_MEM  1024  // Replace with an appropriate constant value


static struct cs_steps_data local_cs_steps_data[LOCAL_PROCEDURE_MEM];
static struct cs_steps_data peer_cs_steps_data[BLE_RAS_PROCEDURE_MEM];
static uint8_t nap;

static int blecs_gap_event(struct ble_cs_event *event, void *arg){
    int rc=0;
    rc = peer_disc_all(connh,
       blecent_on_disc_complete, NULL);
       if(rc != 0) {
           MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
          return 0;
       }

    switch(event->type){
        case BLE_CS_EVENT_CS_PROCEDURE_COMPLETE:
            printf("LE CS Procedure Complete , procedure counter");
            break;
        case BLE_CS_EVENT_SUBEVET_RESULT:
            MODLOG_DFLT(INFO, "LE CS Subevent Result , procedure counter: %d\n", event->subev_result.procedure_counter);

            if(event->subev_result.subevent_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED){
                MODLOG_DFLT(INFO, "LE CS Subevent Result , status: Aborted\n");
                    dropped_ranging_counter=event->subev_result.procedure_counter;
                return 0;
            }

            if(dropped_ranging_counter==event->subev_result.procedure_counter){
                MODLOG_DFLT(INFO, "Ranging procedure was aborted\n");
                return 0;
            }
            if(event->subev_result.num_steps_reported){
                memcpy(&local_cs_steps_data, event->subev_result.steps, sizeof local_cs_steps_data);

            }
            nap=event->subev_result.num_antenna_paths;

            if(event->subev_result.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE){
                most_recent_local_ranging_counter=event->subev_result.procedure_counter;
            }
            else if(event->subev_result.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED){
                MODLOG_DFLT(INFO, "LE CS Procedure , status: Aborted\n");
                memset(&local_cs_steps_data,0,sizeof local_cs_steps_data);
            }

            break;

        case BLE_CS_EVENT_SUBEVET_RESULT_CONTINUE:
                printf("LE CS Subevent Result Continue , procedure counter:");
            break;
        default:
            return 0;

    }
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
#if MYNEWT_VAL(BLE_HCI_VS)
#if MYNEWT_VAL(BLE_POWER_CONTROL)
    struct ble_gap_set_auto_pcl_params params;
#endif
#endif
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

            rc = peer_disc_all(event->connect.conn_handle,
                blecent_on_disc_complete, NULL);
                if(rc != 0) {
                    MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
                    return 0;
                }
            connh = event->connect.conn_handle;
              //  vTaskDelay(1000);

            /** Initiate security - It will perform
             * Pairing (Exchange keys)
             * Bonding (Store keys)
             * Encryption (Enable encryption)
             * Will invoke event BLE_GAP_EVENT_ENC_CHANGE
             **/
            // struct ble_cs_initiator_procedure_start_params param;
            // memset(&param,0,sizeof param);
            // param.conn_handle=event->enc_change.conn_handle;
            // param.cb=blecs_gap_event;
            // param.cb_arg=NULL;

            // ble_cs_initiator_procedure_start(&param);

            // vTaskDelay(1000);


            // extern void bt_hci_log_hci_data_show(void);
            // bt_hci_log_hci_data_show();
            //bt_hci_log_hci_adv_show();


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

#if MYNEWT_VAL(BLE_EATT_CHAN_NUM) > 0
        /* Reset EATT config */
        bearers = 0;
        for (int i = 0; i < MYNEWT_VAL(BLE_EATT_CHAN_NUM); i++) {
            cids[i] = 0;
        }
#endif

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
         /* Perform service discovery */
        //   rc = peer_disc_all(event->connect.conn_handle,
        //    blecent_on_disc_complete, NULL);
        //    if(rc != 0) {
        //        MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
        //       return 0;
        //    }

        printf("conn-handle = %x\n",event->enc_change.conn_handle);
        // struct ble_cs_initiator_procedure_start_params param;
        // memset(&param,0,sizeof param);
        // param.conn_handle=event->enc_change.conn_handle;
        // param.cb=blecs_gap_event;
        // param.cb_arg=NULL;

        // ble_cs_initiator_procedure_start(&param);
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

        }
        else if(ble_svc_ras_rd_val_handle == event->notify_rx.attr_handle){
            uint16_t ranging_counter;
            os_mbuf_copydata(event->notify_rx.om, 0, sizeof(uint16_t), &ranging_counter);
            most_recent_peer_ranging_counter=ranging_counter;


            if(most_recent_peer_ranging_counter!=most_recent_local_ranging_counter){
                MODLOG_DFLT(INFO, "Ranging counter mismatch : %d , %d \n",most_recent_peer_ranging_counter,most_recent_local_ranging_counter);
                return 0;
            }



        }
        else if(ble_svc_ras_rd_ov_val_handle == event->notify_rx.attr_handle){

        }
        else if(ble_svc_ras_od_val_handle == event->notify_rx.attr_handle){

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
        /* An advertisement report was received during GAP discovery. */
        ext_print_adv_report(&event->disc);

        blecent_connect_if_interesting(&event->disc);
        return 0;
#endif

#if MYNEWT_VAL(BLE_POWER_CONTROL)
    case BLE_GAP_EVENT_TRANSMIT_POWER:
	MODLOG_DFLT(INFO, "Transmit power event : status=%d conn_handle=%d reason=%d "
                          "phy=%d power_level=%d power_level_flag=%d delta=%d",
		    event->transmit_power.status,
		    event->transmit_power.conn_handle,
		    event->transmit_power.reason,
		    event->transmit_power.phy,
		    event->transmit_power.transmit_power_level,
		    event->transmit_power.transmit_power_level_flag,
		    event->transmit_power.delta);
	return 0;

    case BLE_GAP_EVENT_PATHLOSS_THRESHOLD:
	MODLOG_DFLT(INFO, "Pathloss threshold event : conn_handle=%d current path loss=%d "
                          "zone_entered =%d",
		    event->pathloss_threshold.conn_handle,
		    event->pathloss_threshold.current_path_loss,
		    event->pathloss_threshold.zone_entered);
	return 0;
#endif

#if MYNEWT_VAL(BLE_EATT_CHAN_NUM) > 0
    case BLE_GAP_EVENT_EATT:
    int i;
    MODLOG_DFLT(INFO, "EATT %s : conn_handle=%d cid=%d",
            event->eatt.status ? "disconnected" : "connected",
            event->eatt.conn_handle,
            event->eatt.cid);
    if (event->eatt.status) {
        /* Remove CID from the list of saved CIDs */
        for (i = 0; i < bearers; i++) {
            if (cids[i] == event->eatt.cid) {
                break;
            }
        }
        while (i < (bearers - 1)) {
            cids[i] = cids[i + 1];
            i += 1;
        }
        cids[i] = 0;

        /* Now Abort */
        return 0;
    }
    cids[bearers] = event->eatt.cid;
    bearers += 1;
    if (bearers != MYNEWT_VAL(BLE_EATT_CHAN_NUM)) {
        /* Wait until all EATT bearers are connected before proceeding */
        return 0;
    }
    /* Set the default bearer to use for further procedures */
    rc = ble_att_set_default_bearer_using_cid(event->eatt.conn_handle, cids[0]);
    if (rc != 0) {
        MODLOG_DFLT(INFO, "Cannot set default EATT bearer, rc = %d\n", rc);
        return rc;
    }

    /* Perform service discovery */
    rc = peer_disc_all(event->eatt.conn_handle,
                blecent_on_disc_complete, NULL);
    if(rc != 0) {
        MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
        return 0;
    }
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
    printf("on sync \n");
    int rc;
    rc = ble_gap_set_host_feat(47,0x01);
    printf("rc = %d\n",rc);
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

#if CONFIG_EXAMPLE_INIT_DEINIT_LOOP
/* This function showcases stack init and deinit procedure. */
static void stack_init_deinit(void)
{
    int rc;
    while(1) {

        vTaskDelay(1000);

        ESP_LOGI(tag, "Deinit host");

        rc = nimble_port_stop();
        if (rc == 0) {
            nimble_port_deinit();
        } else {
            ESP_LOGI(tag, "Nimble port stop failed, rc = %d", rc);
            break;
        }

        vTaskDelay(1000);

        ESP_LOGI(tag, "Init host");

        rc = nimble_port_init();
        if (rc != ESP_OK) {
            ESP_LOGI(tag, "Failed to init nimble %d ", rc);
            break;
        }

        nimble_port_freertos_init(blecent_host_task);

        ESP_LOGI(tag, "Waiting for 1 second");
    }
}
#endif

void
app_main(void)
{
    int rc;
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
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
    rc = ble_svc_gap_device_name_set("nimble-blecent");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(blecent_host_task);

#if CONFIG_EXAMPLE_INIT_DEINIT_LOOP
    stack_init_deinit();
#endif

#if MYNEWT_VAL(BLE_EATT_CHAN_NUM) > 0
    bearers = 0;
    for (int i = 0; i < MYNEWT_VAL(BLE_EATT_CHAN_NUM); i++) {
        cids[i] = 0;
    }
#endif

}
