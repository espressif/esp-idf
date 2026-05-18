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

#define TAG "CAP_INI"

#define CONN_HANDLE_INIT        0xFFFF

#if CONFIG_EXAMPLE_UNICAST
#define LOCAL_DEVICE_NAME       "CAP Initiator"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

/* ACL init parameters shared between bluedroid and nimble host wrappers.
 * Raw HCI units (scan: 0.625ms; conn interval: 1.25ms; timeout: 10ms). */
#define INIT_SCAN_INTERVAL      16      /* 10ms */
#define INIT_SCAN_WINDOW        16      /* 10ms */
#define CONN_INTERVAL           24      /* 30ms */
#define CONN_LATENCY            0
#define CONN_TIMEOUT            500     /* 5s */
#define CONN_MIN_CE_LEN         0xFFFF
#define CONN_MAX_CE_LEN         0xFFFF
#elif CONFIG_EXAMPLE_BROADCAST
#define LOCAL_DEVICE_NAME       "CAP Broadcast Source"

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_INTERVAL_MS         200
#define PER_ADV_INTERVAL_MS     100
#endif

int app_host_init(void);

int set_device_name(void);

#if CONFIG_EXAMPLE_UNICAST
int ext_scan_start(void);
int ext_scan_stop(void);

int conn_create(uint8_t addr_type, const uint8_t addr[6]);

int pairing_start(uint16_t conn_handle);

int exchange_mtu(uint16_t conn_handle);

void security_failed_recover(uint16_t conn_handle, uint8_t status);
#elif CONFIG_EXAMPLE_BROADCAST
int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len,
                  const uint8_t *per_data, uint8_t per_len);
#endif

struct tx_stream {
    esp_ble_audio_cap_stream_t *stream;
    uint16_t seq_num;
    uint8_t *data;
    example_audio_tx_scheduler_t scheduler;
    bool is_broadcast;
};

void cap_initiator_unicast_gap_cb(esp_ble_audio_gap_app_event_t *event);

void cap_initiator_unicast_gatt_cb(esp_ble_audio_gatt_app_event_t *event);

int cap_initiator_unicast_start(void);

int cap_initiator_unicast_init(void);

int cap_initiator_broadcast_start(void);

int cap_initiator_broadcast_init(void);

void cap_initiator_tx_stream_sent(esp_ble_audio_bap_stream_t *stream, void *user_data);

int cap_initiator_tx_register_stream(esp_ble_audio_cap_stream_t *cap_stream, bool is_broadcast);

int cap_initiator_tx_unregister_stream(esp_ble_audio_cap_stream_t *cap_stream);

void cap_initiator_tx_init(void);
