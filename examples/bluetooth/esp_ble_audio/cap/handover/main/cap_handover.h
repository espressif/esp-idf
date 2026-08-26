/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "sdkconfig.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_bap_api.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "CAP_HOV"

#define CONN_HANDLE_INIT        0xFFFF

#define SINK_STREAM_COUNT       MIN(CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT, \
                                    CONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT)

#define LOCAL_DEVICE_NAME       "CAP Handover"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

#define INIT_SCAN_INTERVAL      16      /* 10ms */
#define INIT_SCAN_WINDOW        16      /* 10ms */
#define CONN_INTERVAL           24      /* 30ms */
#define CONN_LATENCY            0
#define CONN_TIMEOUT            500     /* 5s */
#define CONN_MIN_CE_LEN         0xFFFF
#define CONN_MAX_CE_LEN         0xFFFF

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_INTERVAL_MS         200
#define PER_ADV_INTERVAL_MS     100

#define LOCAL_BROADCAST_ID      0x123456

#define HANDOVER_LC3_PRESET_DEFINE(_name, _loc) \
    ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_2_1_DEFINE( \
        _name, _loc, ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED)

int app_host_init(void);

int set_device_name(void);

int ext_scan_start(void);

int ext_scan_stop(void);

int conn_create(uint8_t addr_type, const uint8_t addr[6]);

int pairing_start(uint16_t conn_handle);

int exchange_mtu(uint16_t conn_handle);

void security_failed_recover(uint16_t conn_handle, uint8_t status);

int ext_adv_start_without_base(const uint8_t *ext_data, uint8_t ext_len);

int per_adv_data_start(const uint8_t *per_data, uint8_t per_len);

int adv_stop(void);

int local_public_addr_get(uint8_t addr[6]);

int pa_set_info_transfer(uint16_t conn_handle, const uint8_t peer_addr[6],
                         uint16_t service_data);

struct tx_stream {
    esp_ble_audio_cap_stream_t *stream;
    uint16_t seq_num;
    uint8_t *data;
    example_audio_tx_scheduler_t scheduler;
    bool is_broadcast;
};

struct peer_config {
    esp_ble_audio_cap_stream_t sink_streams[SINK_STREAM_COUNT];
    esp_ble_audio_bap_ep_t *sink_eps[SINK_STREAM_COUNT];
    size_t sink_ep_count;

    esp_ble_conn_t *conn;
    uint16_t conn_handle;
    uint8_t dst[6];

    bool disc_completed;
    bool mtu_exchanged;
};

extern struct peer_config peer;

esp_ble_audio_cap_unicast_group_t *unicast_group_get(void);

void unicast_group_set(esp_ble_audio_cap_unicast_group_t *group);

int unicast_group_delete(void);

esp_ble_audio_bap_lc3_preset_t *sink_preset_get(size_t idx);

int cap_handover_unicast_setup_and_start(void);

void cap_handover_unicast_gap_cb(esp_ble_audio_gap_app_event_t *event);

void cap_handover_unicast_gatt_cb(esp_ble_audio_gatt_app_event_t *event);

int cap_handover_unicast_start(void);

int cap_handover_unicast_init(void);

uint8_t *broadcast_ext_adv_data_get(uint8_t *data_len);

uint8_t *broadcast_base_data_get(esp_ble_audio_cap_broadcast_source_t *source,
                                 uint8_t *data_len);

esp_ble_audio_bap_lc3_preset_t *broadcast_preset_get(void);

esp_ble_audio_location_t broadcast_bis_location_get(size_t idx);

int cap_handover_proc_init(void);

int cap_handover_proc_discover(uint16_t conn_handle);

void cap_handover_proc_unicast_started(void);

void cap_handover_proc_broadcast_stopped(esp_ble_audio_cap_broadcast_source_t *source);

void cap_handover_diag_sample(const char *phase, bool has_sent);

void cap_handover_proc_reset(void);

void cap_handover_tx_stream_sent(esp_ble_audio_bap_stream_t *stream, void *user_data);

int cap_handover_tx_register_stream(esp_ble_audio_cap_stream_t *cap_stream, bool is_broadcast);

int cap_handover_tx_unregister_stream(esp_ble_audio_cap_stream_t *cap_stream);

void cap_handover_tx_init(void);
