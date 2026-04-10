/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef H_GATTC_LATENCY_
#define H_GATTC_LATENCY_

#include "esp_gattc_api.h"
#include "esp_gap_ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Service and Characteristic UUIDs - must match peripheral */
#define REMOTE_SERVICE_UUID    0x1234
#define REMOTE_CHAR_UUID       0x5678

/* Profile ID */
#define PROFILE_APP_ID         0
#define INVALID_HANDLE         0

/* Remote device name */
#define REMOTE_DEVICE_NAME     "ESP_ACL_LATENCY"

/* Test configuration */
#define TEST_PACKET_COUNT      100
#define TEST_PACKET_INTERVAL_MS 20

/* Test packet structure */
typedef struct {
    uint16_t seq_num;
    uint8_t payload[240];
} __attribute__((packed)) test_packet_t;

/* Latency record */
typedef struct {
    uint16_t seq;
    int64_t send_time_us;
    int64_t recv_time_us;
    bool received;
} latency_record_t;

/* Latency test functions */
void latency_test_init(uint16_t conn_id, uint16_t char_handle);
void latency_test_start(void);
void latency_test_handle_notify(uint8_t *data, uint16_t len);
void latency_test_print_results(void);

#ifdef __cplusplus
}
#endif

#endif /* H_GATTC_LATENCY_ */
