/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_gattc_api.h"
#include "gattc_latency.h"

static const char *TAG = "LATENCY_TEST";

/* Global variables */
static latency_record_t records[TEST_PACKET_COUNT];
static uint16_t test_conn_id = 0;
static uint16_t char_handle = 0;
static test_packet_t send_packets[TEST_PACKET_COUNT];
static bool test_running = false;
static bool test_initialized = false;
static esp_gatt_if_t test_gattc_if = 0;

/**
 * Fill random data
 */
static void
fill_random_data(uint8_t *data, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        data[i] = esp_random() & 0xFF;
    }
}

/**
 * Initialize latency test
 */
void
latency_test_init(uint16_t conn_id, uint16_t handle)
{
    test_conn_id = conn_id;
    char_handle = handle;
    test_running = false;
    test_initialized = true;

    memset(records, 0, sizeof(records));
    memset(send_packets, 0, sizeof(send_packets));

    ESP_LOGI(TAG, "Latency test initialized, conn_id=%d, handle=%d", conn_id, handle);
}

/**
 * Set GATT interface
 */
void
latency_test_set_gattc_if(esp_gatt_if_t gattc_if)
{
    test_gattc_if = gattc_if;
}

/**
 * Send single test packet
 */
static int
send_test_packet(uint16_t seq)
{
    if (char_handle == 0 || test_gattc_if == 0) {
        ESP_LOGE(TAG, "Test not initialized");
        return -1;
    }

    /* Prepare packet */
    test_packet_t *pkt = &send_packets[seq];
    pkt->seq_num = seq;
    fill_random_data(pkt->payload, sizeof(pkt->payload));

    /* Record send time */
    records[seq].seq = seq;
    records[seq].send_time_us = esp_timer_get_time();
    records[seq].received = false;

    /* Send write command (write without response) */
    esp_err_t ret = esp_ble_gattc_write_char(test_gattc_if,
                             test_conn_id,
                             char_handle,
                             sizeof(test_packet_t),
                             (uint8_t *)pkt,
                             ESP_GATT_WRITE_TYPE_NO_RSP,
                             ESP_GATT_AUTH_REQ_NONE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Write failed: %s", esp_err_to_name(ret));
        return -1;
    }

    return 0;
}

/**
 * Test task
 */
static void
latency_test_task(void *arg)
{
    ESP_LOGI(TAG, "====================================");
    ESP_LOGI(TAG, "Starting latency test...");
    ESP_LOGI(TAG, "Packets: %d, Interval: %d ms", TEST_PACKET_COUNT, TEST_PACKET_INTERVAL_MS);
    ESP_LOGI(TAG, "====================================");

    test_running = true;

    /* Send all test packets */
    for (int i = 0; i < TEST_PACKET_COUNT; i++) {
        int rc = send_test_packet(i);
        if (rc != 0) {
            ESP_LOGW(TAG, "Send failed for seq=%d", i);
        }

        /* Wait interval */
        vTaskDelay(pdMS_TO_TICKS(TEST_PACKET_INTERVAL_MS));
    }

    ESP_LOGI(TAG, "All packets sent, waiting for responses...");

    /* Wait for all responses */
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* Print results */
    latency_test_print_results();

    test_running = false;
    vTaskDelete(NULL);
}

/**
 * Start latency test
 */
void
latency_test_start(void)
{
    if (test_running) {
        ESP_LOGW(TAG, "Test already running");
        return;
    }

    if (!test_initialized || char_handle == 0) {
        ESP_LOGE(TAG, "Test not initialized");
        return;
    }

    xTaskCreate(latency_test_task, "latency_test", 4096, NULL, 5, NULL);
}

/**
 * Handle notification
 */
void
latency_test_handle_notify(uint8_t *data, uint16_t len)
{
    if (!test_running) {
        return;
    }

    /* Record receive time */
    int64_t recv_time = esp_timer_get_time();

    /* Parse packet */
    if (len < sizeof(test_packet_t)) {
        ESP_LOGW(TAG, "Invalid packet size: %d", len);
        return;
    }

    test_packet_t *recv_pkt = (test_packet_t *)data;
    uint16_t seq = recv_pkt->seq_num;

    /* Validate sequence number */
    if (seq >= TEST_PACKET_COUNT) {
        ESP_LOGW(TAG, "Invalid seq number: %d", seq);
        return;
    }

    /* Verify data consistency */
    if (memcmp(recv_pkt, &send_packets[seq], sizeof(test_packet_t)) != 0) {
        ESP_LOGW(TAG, "Data mismatch for seq=%d", seq);
        return;
    }

    /* Record receive time */
    records[seq].recv_time_us = recv_time;
    records[seq].received = true;

    /* Calculate latency */
    int64_t rtt_us = recv_time - records[seq].send_time_us;
    int64_t latency_us = rtt_us / 2;

    /* Print real-time result */
    printf("SEQ=%3d, RTT=%6lld us, Latency=%6lld us (%.2f ms)\n",
           seq, rtt_us, latency_us, (float)latency_us / 1000.0);
}

/**
 * Print statistics
 */
void
latency_test_print_results(void)
{
    int64_t total_latency = 0;
    int64_t min_latency = INT64_MAX;
    int64_t max_latency = 0;
    int valid_count = 0;
    int lost_count = 0;

    /* Collect statistics */
    for (int i = 0; i < TEST_PACKET_COUNT; i++) {
        if (records[i].received) {
            int64_t rtt = records[i].recv_time_us - records[i].send_time_us;
            int64_t latency = rtt / 2;

            total_latency += latency;
            valid_count++;

            if (latency < min_latency) {
                min_latency = latency;
            }
            if (latency > max_latency) {
                max_latency = latency;
            }
        } else {
            lost_count++;
        }
    }

    /* Print results */
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "         Test Results Summary");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Total packets:     %d", TEST_PACKET_COUNT);
    ESP_LOGI(TAG, "Received:          %d", valid_count);
    ESP_LOGI(TAG, "Lost:              %d (%.1f%%)", lost_count,
             (float)lost_count * 100.0 / TEST_PACKET_COUNT);

    if (valid_count > 0) {
        int64_t avg_latency = total_latency / valid_count;

        ESP_LOGI(TAG, "----------------------------------------");
        ESP_LOGI(TAG, "Average latency:   %lld us (%.2f ms)", avg_latency,
                 (float)avg_latency / 1000.0);
        ESP_LOGI(TAG, "Min latency:       %lld us (%.2f ms)", min_latency,
                 (float)min_latency / 1000.0);
        ESP_LOGI(TAG, "Max latency:       %lld us (%.2f ms)", max_latency,
                 (float)max_latency / 1000.0);
    }

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "");
}
