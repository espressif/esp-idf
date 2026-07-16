/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_store.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "nimble/ble.h"
#include "esp_peripheral.h"
#include "ble_ams.h"

static uint16_t remote_command_handle;
static uint16_t remote_command_def_handle;
static uint16_t entity_update_handle;
static uint16_t entity_update_def_handle;
static uint16_t entity_attribute_handle;
static uint16_t entity_attribute_def_handle;
static uint16_t svc_e_handle;
static bool entity_update_notify_enabled = false;
static uint8_t pending_entity_id = 0;
static uint8_t pending_attribute_id = 0;
#define ENTITY_ATTR_REQ_QUEUE_LEN         4
#define ENTITY_UPDATE_SUB_CMD_MAX_LEN     8
#define ENTITY_UPDATE_SUB_QUEUE_LEN       3

typedef struct {
    uint16_t conn_handle;
    uint8_t entity_id;
    uint8_t attribute_id;
} entity_attr_req_t;

typedef struct {
    uint16_t conn_handle;
    uint8_t cmd_len;
    uint8_t cmd[ENTITY_UPDATE_SUB_CMD_MAX_LEN];
} entity_update_sub_req_t;

static bool entity_attr_req_in_flight;
static entity_attr_req_t entity_attr_req_queue[ENTITY_ATTR_REQ_QUEUE_LEN];
static uint8_t entity_attr_req_head;
static uint8_t entity_attr_req_tail;
static uint8_t entity_attr_req_count;

static bool entity_update_sub_in_flight;
static entity_update_sub_req_t entity_update_sub_queue[ENTITY_UPDATE_SUB_QUEUE_LEN];
static uint8_t entity_update_sub_head;
static uint8_t entity_update_sub_tail;
static uint8_t entity_update_sub_count;

// Apple Media Service UUID: 89D3502B-0F36-433A-8EF4-C502AD55F8DC
static ble_uuid128_t APPLE_MS_UUID = BLE_UUID128_INIT(
    0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xC5, 0xF4, 0x8E, 0x3A, 0x43, 0x36, 0x0F, 0x2B, 0x50, 0xD3, 0x89);
// Remote Command UUID: 9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2
static ble_uuid128_t remote_command_uuid = BLE_UUID128_INIT(
    0xC2, 0x51, 0xCA, 0xF7, 0x56, 0x0E, 0xDF, 0xB8, 0x8A, 0x4A, 0xB1, 0x57, 0xD8, 0x81, 0x3C, 0x9B);
// Entity Update UUID: 2F7CABCE-808D-411F-9A0C-BB92BA96C102
static ble_uuid128_t entity_update_uuid = BLE_UUID128_INIT(
    0x02, 0xC1, 0x96, 0xBA, 0x92, 0xBB, 0x0C, 0x9A, 0x1F, 0x41, 0x8D, 0x80, 0xCE, 0xAB, 0x7C, 0x2F);
// Entity Attribute UUID: C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7
static ble_uuid128_t entity_attribute_uuid = BLE_UUID128_INIT(
    0xD7, 0xD5, 0xBB, 0x70, 0xA8, 0xA3, 0xAB, 0xA6, 0xD8, 0x46, 0xAB, 0x23, 0x8C, 0xF3, 0xB2, 0xC6);

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t ext_adv_pattern_1[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0xab, 0xcd,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x18, 0x11,
    0x0d, BLE_HS_ADV_TYPE_COMP_NAME, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'a', 'm', 's', '-', 'e',
};
#endif

static const char *tag = "NimBLE_AMS";
static int ble_ams_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t own_addr_type;

/* Match Bluedroid ble_ams / ble_ancs: helps iOS Settings list the device. */
#define BLE_AMS_APPEARANCE          BLE_SVC_GAP_APPEARANCE_GEN_HID

#if !CONFIG_EXAMPLE_EXTENDED_ADV
#define BLE_AMS_ADV_SVC_UUID16      0x1812  /* HID Service */

static const ble_uuid16_t adv_uuids16[] = {
    BLE_UUID16_INIT(BLE_AMS_ADV_SVC_UUID16),
};
#endif

void ble_store_config_init(void);

static int ams_dsc_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                                 uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                                 void *arg);
static void ble_process_next_entity_attribute_request(void);
static void ble_process_next_entity_update_subscribe(void);

static void ams_discovery_state_reset(void)
{
    svc_e_handle = 0;
    remote_command_handle = 0;
    remote_command_def_handle = 0;
    entity_update_handle = 0;
    entity_update_def_handle = 0;
    entity_attribute_handle = 0;
    entity_attribute_def_handle = 0;
    entity_update_notify_enabled = false;
}

static void entity_attr_req_queue_reset(void)
{
    entity_attr_req_head = 0;
    entity_attr_req_tail = 0;
    entity_attr_req_count = 0;
    entity_attr_req_in_flight = false;
    pending_entity_id = 0;
    pending_attribute_id = 0;
}

static void entity_update_sub_queue_reset(void)
{
    entity_update_sub_head = 0;
    entity_update_sub_tail = 0;
    entity_update_sub_count = 0;
    entity_update_sub_in_flight = false;
}

static bool entity_attr_req_queue_push(uint16_t conn_handle, uint8_t entity_id, uint8_t attribute_id)
{
    if (entity_attr_req_count >= ENTITY_ATTR_REQ_QUEUE_LEN) {
        return false;
    }

    entity_attr_req_queue[entity_attr_req_tail].conn_handle = conn_handle;
    entity_attr_req_queue[entity_attr_req_tail].entity_id = entity_id;
    entity_attr_req_queue[entity_attr_req_tail].attribute_id = attribute_id;
    entity_attr_req_tail = (entity_attr_req_tail + 1) % ENTITY_ATTR_REQ_QUEUE_LEN;
    entity_attr_req_count++;
    return true;
}

static bool entity_attr_req_queue_pop(entity_attr_req_t *req)
{
    if (req == NULL || entity_attr_req_count == 0) {
        return false;
    }

    *req = entity_attr_req_queue[entity_attr_req_head];
    entity_attr_req_head = (entity_attr_req_head + 1) % ENTITY_ATTR_REQ_QUEUE_LEN;
    entity_attr_req_count--;
    return true;
}

static uint16_t ams_chr_dsc_end_handle(uint16_t chr_def_handle)
{
    uint16_t end_handle = svc_e_handle;

    if (remote_command_def_handle > chr_def_handle &&
        remote_command_def_handle - 1 < end_handle) {
        end_handle = remote_command_def_handle - 1;
    }
    if (entity_update_def_handle > chr_def_handle &&
        entity_update_def_handle - 1 < end_handle) {
        end_handle = entity_update_def_handle - 1;
    }
    if (entity_attribute_def_handle > chr_def_handle &&
        entity_attribute_def_handle - 1 < end_handle) {
        end_handle = entity_attribute_def_handle - 1;
    }

    return end_handle;
}

static int ams_discover_descriptors(uint16_t conn_handle, uint16_t chr_handle,
                                    uint16_t chr_def_handle, const char *chr_name)
{
    int rc;

    if (chr_handle == 0 || chr_def_handle == 0) {
        return 0;
    }

    rc = ble_gattc_disc_all_dscs(conn_handle,
                                 chr_handle,
                                 ams_chr_dsc_end_handle(chr_def_handle),
                                 ams_dsc_discovered_cb,
                                 (void *)(uintptr_t)chr_handle);
    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to discover %s descriptors: rc=%d", chr_name, rc);
    }

    return rc;
}

static bool entity_update_sub_queue_push(uint16_t conn_handle, const uint8_t *cmd, uint8_t cmd_len)
{
    if (entity_update_sub_count >= ENTITY_UPDATE_SUB_QUEUE_LEN ||
        cmd_len > ENTITY_UPDATE_SUB_CMD_MAX_LEN) {
        return false;
    }

    entity_update_sub_queue[entity_update_sub_tail].conn_handle = conn_handle;
    entity_update_sub_queue[entity_update_sub_tail].cmd_len = cmd_len;
    memcpy(entity_update_sub_queue[entity_update_sub_tail].cmd, cmd, cmd_len);
    entity_update_sub_tail = (entity_update_sub_tail + 1) % ENTITY_UPDATE_SUB_QUEUE_LEN;
    entity_update_sub_count++;
    return true;
}

static bool entity_update_sub_queue_pop(entity_update_sub_req_t *req)
{
    if (req == NULL || entity_update_sub_count == 0) {
        return false;
    }

    *req = entity_update_sub_queue[entity_update_sub_head];
    entity_update_sub_head = (entity_update_sub_head + 1) % ENTITY_UPDATE_SUB_QUEUE_LEN;
    entity_update_sub_count--;
    return true;
}

static int ams_entity_update_sub_write_cb(uint16_t conn_handle,
                                          const struct ble_gatt_error *error,
                                          struct ble_gatt_attr *attr,
                                          void *arg)
{
    if (error->status != 0) {
        MODLOG_DFLT(WARN, "Entity update subscribe write failed; status=%d\n", error->status);
    }

    entity_update_sub_in_flight = false;
    ble_process_next_entity_update_subscribe();
    return 0;
}

static void ble_process_next_entity_update_subscribe(void)
{
    entity_update_sub_req_t req;
    int rc;

    if (entity_update_sub_in_flight || !entity_update_sub_queue_pop(&req)) {
        return;
    }

    entity_update_sub_in_flight = true;
    MODLOG_DFLT(INFO, "Subscribe %s entity updates", EntityID_to_String(req.cmd[0]));

    rc = ble_gattc_write_flat(req.conn_handle,
                              entity_update_handle,
                              req.cmd,
                              req.cmd_len,
                              ams_entity_update_sub_write_cb,
                              NULL);
    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to write entity update request: rc=%d\n", rc);
        entity_update_sub_in_flight = false;
        ble_process_next_entity_update_subscribe();
    }
}

static void ble_subscribe_entity_updates(uint16_t conn_handle)
{
    const uint8_t track_cmd[] = {
        EntityIDTrack,
        TrackAttributeIDArtist,
        TrackAttributeIDAlbum,
        TrackAttributeIDTitle,
        TrackAttributeIDDuration,
    };
    const uint8_t player_cmd[] = {
        EntityIDPlayer,
        PlayerAttributeIDName,
        PlayerAttributeIDPlaybackInfo,
        PlayerAttributeIDVolume,
    };
    const uint8_t queue_cmd[] = {
        EntityIDQueue,
        QueueAttributeIDIndex,
        QueueAttributeIDCount,
        QueueAttributeIDShuffleMode,
        QueueAttributeIDRepeatMode,
    };

    entity_update_sub_queue_reset();

    if (!entity_update_sub_queue_push(conn_handle, track_cmd, sizeof(track_cmd)) ||
        !entity_update_sub_queue_push(conn_handle, player_cmd, sizeof(player_cmd)) ||
        !entity_update_sub_queue_push(conn_handle, queue_cmd, sizeof(queue_cmd))) {
        MODLOG_DFLT(WARN, "Failed to queue entity update subscribe requests");
        return;
    }

    ble_process_next_entity_update_subscribe();
}

void ble_send_remote_command(uint16_t conn_handle, uint8_t command_id)
{
    if (remote_command_handle == 0) {
        MODLOG_DFLT(WARN, "Remote command handle not discovered\n");
        return;
    }

    int rc = ble_gattc_write_flat(conn_handle,
                                  remote_command_handle,
                                  &command_id,
                                  1,
                                  NULL,
                                  NULL);
    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to write remote command: rc=%d\n", rc);
    }
}

static int ams_entity_attr_read_cb(uint16_t conn_handle,
                                  const struct ble_gatt_error *error,
                                  struct ble_gatt_attr *attr,
                                  void *arg)
{
    if (error->status == 0 && attr != NULL && attr->om != NULL) {
        ble_receive_entity_attribute_value(pending_entity_id,
                                           pending_attribute_id,
                                           attr->om->om_data,
                                           attr->om->om_len);
    } else if (error->status != 0) {
        MODLOG_DFLT(WARN, "Entity attribute read failed; status=%d\n", error->status);
    }

    entity_attr_req_in_flight = false;
    ble_process_next_entity_attribute_request();
    return 0;
}

static int ams_entity_attr_write_cb(uint16_t conn_handle,
                                    const struct ble_gatt_error *error,
                                    struct ble_gatt_attr *attr,
                                    void *arg)
{
    if (error->status != 0) {
        MODLOG_DFLT(WARN, "Entity attribute write failed; status=%d\n", error->status);
        entity_attr_req_in_flight = false;
        ble_process_next_entity_attribute_request();
        return 0;
    }

    int rc = ble_gattc_read(conn_handle, entity_attribute_handle, ams_entity_attr_read_cb, NULL);
    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to read entity attribute: rc=%d\n", rc);
        entity_attr_req_in_flight = false;
        ble_process_next_entity_attribute_request();
    }
    return 0;
}

static void ble_process_next_entity_attribute_request(void)
{
    entity_attr_req_t req;
    uint8_t cmd[2];
    int rc;

    if (entity_attribute_handle == 0) {
        MODLOG_DFLT(WARN, "Entity Attribute handle not discovered\n");
        return;
    }

    if (entity_attr_req_in_flight || !entity_attr_req_queue_pop(&req)) {
        return;
    }

    pending_entity_id = req.entity_id;
    pending_attribute_id = req.attribute_id;
    entity_attr_req_in_flight = true;
    cmd[0] = req.entity_id;
    cmd[1] = req.attribute_id;

    rc = ble_gattc_write_flat(req.conn_handle,
                              entity_attribute_handle,
                              cmd,
                              sizeof(cmd),
                              ams_entity_attr_write_cb,
                              NULL);
    if (rc != 0) {
        MODLOG_DFLT(WARN, "Failed to write entity attribute request: rc=%d\n", rc);
        entity_attr_req_in_flight = false;
        ble_process_next_entity_attribute_request();
    }
}

static void ble_request_entity_attribute(uint16_t conn_handle, uint8_t entity_id, uint8_t attribute_id)
{
    if (!entity_attr_req_queue_push(conn_handle, entity_id, attribute_id)) {
        MODLOG_DFLT(WARN,
                    "Entity Attribute request dropped (entity=%u attr=%u): queue full (max=%u)",
                    entity_id,
                    attribute_id,
                    ENTITY_ATTR_REQ_QUEUE_LEN);
        return;
    }

    ble_process_next_entity_attribute_request();
}

static int ams_on_notify_subscribe(uint16_t conn_handle,
                                   const struct ble_gatt_error *error,
                                   struct ble_gatt_attr *attr,
                                   void *arg)
{
    uint16_t chr_handle = (uint16_t)(uintptr_t)arg;

    MODLOG_DFLT(INFO,
                "Subscribe complete; status=%d conn_handle=%d chr_handle=0x%04x",
                error->status, conn_handle, chr_handle);

    if (error->status != 0) {
        if (chr_handle == remote_command_handle && entity_update_handle != 0) {
            MODLOG_DFLT(WARN, "Remote Command subscribe failed; continue Entity Update setup");
            return ams_discover_descriptors(conn_handle, entity_update_handle,
                                            entity_update_def_handle, "Entity Update");
        }
        return 0;
    }

    if (chr_handle == remote_command_handle) {
        MODLOG_DFLT(INFO, "Remote Command notifications enabled");
        ams_discover_descriptors(conn_handle, entity_update_handle,
                                 entity_update_def_handle, "Entity Update");
        return 0;
    }

    if (chr_handle == entity_update_handle && !entity_update_notify_enabled) {
        entity_update_notify_enabled = true;
        MODLOG_DFLT(INFO, "Entity Update notifications enabled");
        ble_subscribe_entity_updates(conn_handle);
    }

    return 0;
}

static int ams_dsc_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                                 uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                                 void *arg)
{
    int rc = 0;
    uint8_t cccd_val[2] = {0x01, 0x00};

    if (error->status == BLE_HS_EDONE) {
        return 0;
    }

    if (error->status != 0) {
        return error->status;
    }

    if (ble_uuid_cmp(&dsc->uuid.u, BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16)) == 0) {
        rc = ble_gattc_write_flat(conn_handle,
                                  dsc->handle,
                                  cccd_val,
                                  sizeof(cccd_val),
                                  ams_on_notify_subscribe,
                                  arg);
        if (rc != 0) {
            MODLOG_DFLT(WARN, "Failed to subscribe (rc=%d)", rc);
            if (chr_val_handle == remote_command_handle && entity_update_handle != 0) {
                return ams_discover_descriptors(conn_handle, entity_update_handle,
                                                entity_update_def_handle, "Entity Update");
            }
            return rc;
        }
    }

    return 0;
}

static int ams_chr_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                                 const struct ble_gatt_chr *chr, void *arg)
{
    if (error->status == BLE_HS_EDONE) {
        if (remote_command_handle != 0) {
            return ams_discover_descriptors(conn_handle, remote_command_handle,
                                            remote_command_def_handle, "Remote Command");
        }
        if (entity_update_handle != 0) {
            return ams_discover_descriptors(conn_handle, entity_update_handle,
                                            entity_update_def_handle, "Entity Update");
        }
        return 0;
    }

    if (error->status != 0) {
        return error->status;
    }

    if ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) &&
        ble_uuid_cmp(&chr->uuid.u, &remote_command_uuid.u) == 0) {
        remote_command_handle = chr->val_handle;
        remote_command_def_handle = chr->def_handle;
        MODLOG_DFLT(INFO, "Found Remote Command characteristic");
    } else if ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) &&
               ble_uuid_cmp(&chr->uuid.u, &entity_update_uuid.u) == 0) {
        entity_update_handle = chr->val_handle;
        entity_update_def_handle = chr->def_handle;
        MODLOG_DFLT(INFO, "Found Entity Update characteristic");
    } else if (ble_uuid_cmp(&chr->uuid.u, &entity_attribute_uuid.u) == 0) {
        entity_attribute_handle = chr->val_handle;
        entity_attribute_def_handle = chr->def_handle;
        MODLOG_DFLT(INFO, "Found Entity Attribute characteristic");
    }

    return 0;
}

static int ams_service_discovered_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                                     const struct ble_gatt_svc *svc, void *arg)
{
    if (error->status == BLE_HS_EDONE) {
        if (svc_e_handle == 0) {
            MODLOG_DFLT(WARN, "AMS service not found");
        }
        return 0;
    }

    if (error->status != 0) {
        MODLOG_DFLT(WARN, "AMS service discovery failed; status=%d", error->status);
        return error->status;
    }

    if (svc == NULL) {
        MODLOG_DFLT(WARN, "AMS service callback missing service data");
        return BLE_HS_EUNKNOWN;
    }

    svc_e_handle = svc->end_handle;
    return ble_gattc_disc_all_chrs(conn_handle, svc->start_handle, svc->end_handle,
                                   ams_chr_discovered_cb, NULL);
}

#if NIMBLE_BLE_CONNECT
static void ble_ams_print_conn_desc(struct ble_gap_conn_desc *desc)
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
static void ext_ble_ams_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;

    if (ble_gap_ext_adv_active(instance)) {
        return;
    }

    memset(&params, 0, sizeof(params));
    params.connectable = 1;
    params.own_addr_type = own_addr_type;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.tx_power = 127;
    params.sid = 1;
    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    rc = ble_gap_ext_adv_configure(instance, &params, NULL, ble_ams_gap_event, NULL);
    assert(rc == 0);

    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);

    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert(rc == 0);
}
#else
static void ble_ams_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;
    int rc;

    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    fields.appearance = BLE_AMS_APPEARANCE;
    fields.appearance_is_present = 1;

    fields.uuids16 = (ble_uuid16_t *)adv_uuids16;
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_ams_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
    }
}
#endif

static int ble_ams_gap_event(struct ble_gap_event *event, void *arg)
{
#if NIMBLE_BLE_CONNECT
    struct ble_gap_conn_desc desc;
    int rc;
#endif

    switch (event->type) {
#if NIMBLE_BLE_CONNECT
    case BLE_GAP_EVENT_CONNECT:
        MODLOG_DFLT(INFO, "connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            ble_ams_print_conn_desc(&desc);
        }
        MODLOG_DFLT(INFO, "\n");

        if (event->connect.status != 0) {
#if CONFIG_EXAMPLE_EXTENDED_ADV
            ext_ble_ams_advertise();
#else
            ble_ams_advertise();
#endif
            return 0;
        }

        rc = ble_gap_security_initiate(event->connect.conn_handle);
        if (rc != 0) {
            MODLOG_DFLT(INFO, "Security could not be initiated, rc = %d\n", rc);
            return ble_gap_terminate(event->connect.conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        ble_ams_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        ams_discovery_state_reset();
        entity_attr_req_queue_reset();
        entity_update_sub_queue_reset();

#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_ble_ams_advertise();
#else
        ble_ams_advertise();
#endif
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        MODLOG_DFLT(INFO, "connection updated; status=%d ", event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        ble_ams_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d", event->adv_complete.reason);
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_ble_ams_advertise();
#else
        ble_ams_advertise();
#endif
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        MODLOG_DFLT(INFO, "encryption change event; status=%d ", event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        ble_ams_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");

        if (event->enc_change.status != 0) {
            MODLOG_DFLT(WARN, "Encryption failed; skip AMS service discovery");
            return 0;
        }

        MODLOG_DFLT(INFO, "Connection secured\n");
        ams_discovery_state_reset();
        entity_attr_req_queue_reset();
        entity_update_sub_queue_reset();

        rc = ble_gattc_disc_svc_by_uuid(event->enc_change.conn_handle, &APPLE_MS_UUID.u,
                                        ams_service_discovered_cb, NULL);
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
        if (event->notify_rx.attr_handle == remote_command_handle) {
            ble_receive_remote_command_notification(event->notify_rx.om->om_data,
                                                    event->notify_rx.om->om_len);
        } else if (event->notify_rx.attr_handle == entity_update_handle) {
            if (event->notify_rx.om->om_len < 3) {
                MODLOG_DFLT(WARN, "Entity update notification too short (%d)",
                            event->notify_rx.om->om_len);
                return 0;
            }
            ble_receive_entity_update(event->notify_rx.om->om_data, event->notify_rx.om->om_len);
            if (event->notify_rx.om->om_data[2] & EntityUpdateFlagTruncated) {
                ble_request_entity_attribute(event->notify_rx.conn_handle,
                                             event->notify_rx.om->om_data[0],
                                             event->notify_rx.om->om_data[1]);
            }
        } else {
            MODLOG_DFLT(INFO, "unknown handle, receive notify value");
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
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);
        return BLE_GAP_REPEAT_PAIRING_RETRY;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        ESP_LOGI(tag, "PASSKEY_ACTION_EVENT started");
        struct ble_sm_io pkey = {0};
        int key = 0;

        if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
            pkey.action = event->passkey.params.action;
            pkey.passkey = 123456;
            ESP_LOGI(tag, "Enter passkey %" PRIu32 " on the peer side", pkey.passkey);
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
            ESP_LOGI(tag, "Passkey on device's display: %" PRIu32, event->passkey.params.numcmp);
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
    default:
        break;
    }

    return 0;
}

static void ble_ams_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void ble_ams_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

#if CONFIG_EXAMPLE_EXTENDED_ADV
    ext_ble_ams_advertise();
#else
    ble_ams_advertise();
#endif
}

static void ble_ams_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d", ret);
        return;
    }

    ble_hs_cfg.reset_cb = ble_ams_on_reset;
    ble_hs_cfg.sync_cb = ble_ams_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_sc = 0;

    int rc = ble_svc_gap_device_name_set("nimble-ams");
    assert(rc == 0);
    rc = ble_svc_gap_device_appearance_set(BLE_AMS_APPEARANCE);
    assert(rc == 0);

    ble_store_config_init();

    nimble_port_freertos_init(ble_ams_host_task);

    /* Initialize command line interface to accept input from user */
    rc = scli_init();
    if (rc != ESP_OK) {
        ESP_LOGE(tag, "scli_init() failed");
    }
}
