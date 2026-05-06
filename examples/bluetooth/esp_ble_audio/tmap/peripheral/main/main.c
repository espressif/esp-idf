/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#include "tmap_peripheral.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define ADV_HANDLE          0x00
#define ADV_SID             0
#define ADV_TX_POWER        127
#define ADV_ADDRESS         BLE_OWN_ADDR_PUBLIC
#define ADV_PRIMARY_PHY     BLE_HCI_LE_PHY_1M
#define ADV_SECONDARY_PHY   BLE_HCI_LE_PHY_2M
#define ADV_INTERVAL        BLE_GAP_ADV_ITVL_MS(200)

static uint8_t general_adv_data[] = {
    /* Flags */
    0x02, EXAMPLE_AD_TYPE_FLAGS, (EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR),
    /* Appearance */
    0x03, EXAMPLE_AD_TYPE_GAP_APPEARANCE, (ESP_BLE_AUDIO_APPEARANCE_WEARABLE_AUDIO_DEVICE_EARBUD & 0xFF),
    ((ESP_BLE_AUDIO_APPEARANCE_WEARABLE_AUDIO_DEVICE_EARBUD >> 8) & 0xFF),
    /* Incomplete List of 16-bit Service UUIDs */
    0x07, EXAMPLE_AD_TYPE_UUID16_SOME, (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_CAS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_CAS_VAL >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_TMAS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_TMAS_VAL >> 8) & 0xFF),
};

static uint8_t unicast_server_adv_data[] = {
    /* Target Announcement - 16-bit UUID */
    0x09, EXAMPLE_AD_TYPE_SERVICE_DATA16, (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF),
    ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED,
    (SINK_CONTEXT & 0xFF),
    ((SINK_CONTEXT >> 8) & 0xFF),
    (SOURCE_CONTEXT & 0xFF),
    ((SOURCE_CONTEXT >> 8) & 0xFF),
    0x00,   /* Metadata length */
};

static const uint8_t cap_adv_data[] = {
    /* CAS Service Data - 16-bit UUID */
    0x04, EXAMPLE_AD_TYPE_SERVICE_DATA16, (ESP_BLE_AUDIO_UUID_CAS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_CAS_VAL >> 8) & 0xFF),
    ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED,
};

static uint8_t tmap_adv_data[] = {
    /* TMAS Service Data - 16-bit UUID */
    0x05, EXAMPLE_AD_TYPE_SERVICE_DATA16, (ESP_BLE_AUDIO_UUID_TMAS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_TMAS_VAL >> 8) & 0xFF),
    ((ESP_BLE_AUDIO_TMAP_ROLE_UMR | \
      ESP_BLE_AUDIO_TMAP_ROLE_CT) & 0xFF),
    (((ESP_BLE_AUDIO_TMAP_ROLE_UMR | \
       ESP_BLE_AUDIO_TMAP_ROLE_CT) >> 8) & 0xFF),
};

static uint8_t dev_name_adv_data[] = {
    /* Complete Device Name */
    0x10, EXAMPLE_AD_TYPE_NAME_COMPLETE, 't', 'm', 'a', 'p', '_',
    'p', 'e', 'r', 'i', 'p', 'h', 'e', 'r', 'a', 'l',
};

#if CONFIG_BT_CSIP_SET_MEMBER
static uint8_t csis_rsi[ESP_BLE_AUDIO_CSIP_RSI_SIZE];

#define ADV_DATA_LEN    (sizeof(general_adv_data) + \
                         2 + sizeof(csis_rsi) + \
                         sizeof(tmap_adv_data) + \
                         sizeof(cap_adv_data) + \
                         sizeof(unicast_server_adv_data) + \
                         sizeof(dev_name_adv_data))
#else /* CONFIG_BT_CSIP_SET_MEMBER */
#define ADV_DATA_LEN    (sizeof(general_adv_data) + \
                         sizeof(tmap_adv_data) + \
                         sizeof(cap_adv_data) + \
                         sizeof(unicast_server_adv_data) + \
                         sizeof(dev_name_adv_data))
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

static uint8_t ext_adv_data[ADV_DATA_LEN];

static uint16_t default_conn_handle = CONN_HANDLE_INIT;

uint16_t default_conn_handle_get(void)
{
    return default_conn_handle;
}

static int set_ext_adv_data(void)
{
    uint16_t data_len = 0;

    memcpy(ext_adv_data, general_adv_data, sizeof(general_adv_data));
    data_len += sizeof(general_adv_data);

#if CONFIG_BT_CSIP_SET_MEMBER
    ext_adv_data[data_len] = 1 + sizeof(csis_rsi);
    ext_adv_data[data_len + 1] = EXAMPLE_AD_TYPE_CSIS_RSI;
    memcpy(ext_adv_data + data_len + 2, csis_rsi, sizeof(csis_rsi));
    data_len += (2 + sizeof(csis_rsi));
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

    memcpy(ext_adv_data + data_len, tmap_adv_data, sizeof(tmap_adv_data));
    data_len += sizeof(tmap_adv_data);

    memcpy(ext_adv_data + data_len, cap_adv_data, sizeof(cap_adv_data));
    data_len += sizeof(cap_adv_data);

    memcpy(ext_adv_data + data_len, unicast_server_adv_data, sizeof(unicast_server_adv_data));
    data_len += sizeof(unicast_server_adv_data);

    memcpy(ext_adv_data + data_len, dev_name_adv_data, sizeof(dev_name_adv_data));
    data_len += sizeof(dev_name_adv_data);

    if (data_len != sizeof(ext_adv_data)) {
        ESP_LOGE(TAG, "Mismatch ext adv data length %u, expected %u",
                 data_len, sizeof(ext_adv_data));
        return -1;
    }

    return 0;
}

static void ext_adv_start(void)
{
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
    int err;

    ext_params.connectable = 1;
    ext_params.scannable = 0;
    ext_params.legacy_pdu = 0;
    ext_params.own_addr_type = ADV_ADDRESS;
    ext_params.primary_phy = ADV_PRIMARY_PHY;
    ext_params.secondary_phy = ADV_SECONDARY_PHY;
    ext_params.tx_power = ADV_TX_POWER;
    ext_params.sid = ADV_SID;
    ext_params.itvl_min = ADV_INTERVAL;
    ext_params.itvl_max = ADV_INTERVAL;

    err = ble_gap_ext_adv_configure(ADV_HANDLE, &ext_params, NULL,
                                    example_audio_gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure ext adv params, err %d", err);
        return;
    }

    err = set_ext_adv_data();
    if (err) {
        return;
    }

    data = os_msys_get_pkthdr(sizeof(ext_adv_data), 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get ext adv mbuf");
        return;
    }

    err = os_mbuf_append(data, ext_adv_data, sizeof(ext_adv_data));
    if (err) {
        ESP_LOGE(TAG, "Failed to append ext adv data, err %d", err);
        os_mbuf_free_chain(data);
        return;
    }

    err = ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext adv data, err %d", err);
        return;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);
}

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             event->acl_connect.dst.val[5], event->acl_connect.dst.val[4],
             event->acl_connect.dst.val[3], event->acl_connect.dst.val[2],
             event->acl_connect.dst.val[1], event->acl_connect.dst.val[0]);

    default_conn_handle = event->acl_connect.conn_handle;
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    default_conn_handle = CONN_HANDLE_INIT;

    ext_adv_start();
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT:
        acl_connect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_DISCONNECT:
        acl_disconnect(event);
        break;
    default:
        break;
    }
}

static void gatt_mtu_change(esp_ble_audio_gatt_app_event_t *event)
{
    int err;

    ESP_LOGI(TAG, "MTU updated: handle %u mtu %u",
             event->gatt_mtu_change.conn_handle, event->gatt_mtu_change.mtu);

    if (event->gatt_mtu_change.mtu < ESP_BLE_AUDIO_ATT_MTU_MIN) {
        ESP_LOGW(TAG, "MTU %u below minimum %u",
                 event->gatt_mtu_change.mtu, ESP_BLE_AUDIO_ATT_MTU_MIN);
        return;
    }

    /* This function only needs to be invoked when:
     * - The device is a Peripheral (Link Layer role);
     * - The device works as a GATT client.
     */
    err = esp_ble_audio_gattc_disc_start(event->gatt_mtu_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start service discovery, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u",
             event->gatt_mtu_change.conn_handle);
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    ESP_LOGI(TAG, "Service discovery complete: handle %u status %u",
             event->gattc_disc_cmpl.conn_handle,
             event->gattc_disc_cmpl.status);

    if (event->gattc_disc_cmpl.status) {
        return;
    }

    tmap_discover_tmas();
}

static void iso_gatt_app_cb(esp_ble_audio_gatt_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GATT_EVENT_GATT_MTU_CHANGE:
        gatt_mtu_change(event);
        break;
    case ESP_BLE_AUDIO_GATT_EVENT_GATTC_DISC_CMPL:
        gattc_disc_cmpl(event);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    esp_ble_audio_start_info_t start_info = {0};
    esp_ble_audio_init_info_t init_info = {
        .gap_cb  = iso_gap_app_cb,
        .gatt_cb = iso_gatt_app_cb,
    };
    esp_err_t err;

    /* Initialize NVS — it is used to store PHY calibration data */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize BLE, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(&init_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = tmap_ct_umr_init();
    if (err) {
        return;
    }

#if CONFIG_BT_CSIP_SET_MEMBER
    err = csip_set_member_init();
    if (err) {
        return;
    }

    err = csip_generate_rsi(csis_rsi);
    if (err) {
        return;
    }

    start_info.csis_insts[0].svc_inst = csip_svc_inst_get();
    start_info.csis_insts[0].included_by_cas = true;
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

    err = vcp_vol_renderer_init();
    if (err) {
        return;
    }

    err = bap_unicast_sr_init();
    if (err) {
        return;
    }

    err = ccp_call_ctrl_init();
    if (err) {
        return;
    }

    err = mcp_ctlr_init();
    if (err) {
        return;
    }

    err = esp_ble_audio_common_start(&start_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    err = ble_svc_gap_device_name_set("TMAP Peripheral");
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    ext_adv_start();
}
