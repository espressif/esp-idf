/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_eth_test_common.h"

#define ETHERTYPE_TX_STD        0x2222  // frame transmitted via emac_hal_transmit_frame
#define ETHERTYPE_TX_MULTI_2    0x2223  // frame transmitted via emac_hal_transmit_multiple_buf_frame (2 buffers)
#define ETHERTYPE_TX_MULTI_3    0x2224  // frame transmitted via emac_hal_transmit_multiple_buf_frame (3 buffers)

#define MINIMUM_TEST_FRAME_SIZE 64

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static const char *TAG = "esp32_eth_test_hal";

typedef struct
{
    SemaphoreHandle_t mutex;
    uint16_t expected_size;
    uint16_t expected_size_2;
    uint16_t expected_size_3;
} recv_hal_check_info_t;

static esp_err_t eth_recv_hal_check_cb(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv)
{
    emac_frame_t *pkt = (emac_frame_t *)buffer;
    recv_hal_check_info_t *recv_info = (recv_hal_check_info_t *)priv;
    uint16_t expected_size = recv_info->expected_size + recv_info->expected_size_2 + recv_info->expected_size_3;

    ESP_LOGI(TAG, "recv frame size: %" PRIu16, expected_size);
    TEST_ASSERT_EQUAL(expected_size, length);
    // frame transmitted via emac_hal_transmit_frame
    if (pkt->proto == ETHERTYPE_TX_STD) {
        for (int i = 0; i < recv_info->expected_size - ETH_HEADER_LEN; i++) {
            TEST_ASSERT_EQUAL(pkt->data[i], i & 0xFF);
        }
    // frame transmitted via emac_hal_transmit_multiple_buf_frame (2 buffers)
    } else if (pkt->proto == ETHERTYPE_TX_MULTI_2) {
        uint8_t *data_p = pkt->data;
        for (int i = 0; i < recv_info->expected_size - ETH_HEADER_LEN; i++) {
            TEST_ASSERT_EQUAL(*(data_p++), i & 0xFF);
        }
        int j = ETH_MAX_PAYLOAD_LEN;
        for (int i = 0; i < recv_info->expected_size_2; i++) {
            TEST_ASSERT_EQUAL(*(data_p++), j & 0xFF);
            j--;
        }
    // frame transmitted via emac_hal_transmit_multiple_buf_frame (3 buffers)
    } else if (pkt->proto == ETHERTYPE_TX_MULTI_3) {
        uint8_t *data_p = pkt->data;
        for (int i = 0; i < recv_info->expected_size - ETH_HEADER_LEN; i++) {
            TEST_ASSERT_EQUAL(*(data_p++), i & 0xFF);
        }
        int j = ETH_MAX_PAYLOAD_LEN;
        for (int i = 0; i < recv_info->expected_size_2; i++) {
            TEST_ASSERT_EQUAL(*(data_p++), j & 0xFF);
            j--;
        }
        for (int i = 0; i < recv_info->expected_size_3; i++) {
            TEST_ASSERT_EQUAL(*(data_p++), i & 0xFF);
        }
    } else {
        TEST_FAIL();
    }
    memset(buffer, 0, length);
    free(buffer);
    xSemaphoreGive(recv_info->mutex);
    return ESP_OK;
}

TEST_CASE("hal receive/transmit", "[emac_hal]")
{
    recv_hal_check_info_t recv_info;
    recv_info.mutex = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(recv_info.mutex);
    recv_info.expected_size = 0;
    recv_info.expected_size_2 = 0;
    recv_info.expected_size_3 = 0;

    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));

    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    extra_eth_config(eth_handle);

    // ---------------------------------------
    // Loopback greatly simplifies the test !!
    // ---------------------------------------
    bool loopback_en = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, eth_recv_hal_check_cb, &recv_info));

    // start the driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    // wait for connection start
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    // wait for connection establish
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    // create test frame
    emac_frame_t *test_pkt = calloc(1, ETH_MAX_PACKET_SIZE);
    test_pkt->proto = ETHERTYPE_TX_STD;
    memset(test_pkt->dest, 0xff, ETH_ADDR_LEN); // broadcast addr
    uint8_t local_mac_addr[ETH_ADDR_LEN] = { 0 };
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, local_mac_addr));
    memcpy(test_pkt->src, local_mac_addr, ETH_ADDR_LEN);
    // fill with data
    for (int i = 0; i < ETH_MAX_PAYLOAD_LEN; i++) {
        test_pkt->data[i] = i & 0xFF;
    }

    uint16_t transmit_size;

    ESP_LOGI(TAG, "Verify DMA descriptors are returned back to owner");
    // find if Rx or Tx buffer number is bigger and work with bigger number
    uint32_t config_eth_dma_max_buffer_num = MAX(CONFIG_ETH_DMA_RX_BUFFER_NUM, CONFIG_ETH_DMA_TX_BUFFER_NUM);
    // start with short frames since EMAC Rx FIFO may be different of size for different chips => it may help with following fail isolation
    for (int32_t i = 0; i < config_eth_dma_max_buffer_num*2; i++) {
        transmit_size = MINIMUM_TEST_FRAME_SIZE;
        ESP_LOGI(TAG, "transmit frame size: %" PRIu16 ", i = %" PRIi32, transmit_size, i);
        recv_info.expected_size = transmit_size;
        TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));
    }

    ESP_LOGI(TAG, "Verify that we are able to transmit/receive all frame sizes");
    // iteration over different sizes may help with fail isolation
    for (int i = 1; (MINIMUM_TEST_FRAME_SIZE *i) < ETH_MAX_PAYLOAD_LEN; i++) {
        transmit_size = MINIMUM_TEST_FRAME_SIZE * i;
        ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
        recv_info.expected_size = transmit_size;
        TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));
    }

    ESP_LOGI(TAG, "Verify that DMA driver correctly processes frame from EMAC descriptors at boundary conditions");
    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE;
    ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
    recv_info.expected_size = transmit_size;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE - 1;
    ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
    recv_info.expected_size = transmit_size;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE + 1;
    ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
    recv_info.expected_size = transmit_size;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = 2 * CONFIG_ETH_DMA_BUFFER_SIZE;
    ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
    recv_info.expected_size = transmit_size;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = 2 * CONFIG_ETH_DMA_BUFFER_SIZE - 1;
    ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
    recv_info.expected_size = transmit_size;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = 2 * CONFIG_ETH_DMA_BUFFER_SIZE + 1;
    ESP_LOGI(TAG, "transmit frame size: %" PRIu16, transmit_size);
    recv_info.expected_size = transmit_size;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    ESP_LOGI(TAG, "-- Verify transmission from multiple buffers --");
    uint16_t transmit_size_2;
    // allocated the second buffer
    uint8_t *pkt_data_2 = malloc(ETH_MAX_PAYLOAD_LEN);
    // fill with data (reverse order to differentiate the buffers)
    int j = ETH_MAX_PAYLOAD_LEN;
    for (int i = 0; i < ETH_MAX_PAYLOAD_LEN; i++) {
        pkt_data_2[i] = j & 0xFF;
        j--;
    }
    // change protocol number so the cb function is aware that frame was joint from two buffers
    test_pkt->proto = ETHERTYPE_TX_MULTI_2;

    ESP_LOGI(TAG, "Verify DMA descriptors are returned back to owner");
    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE / 2;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    for (int32_t i = 0; i < config_eth_dma_max_buffer_num*2; i++) {
        ESP_LOGI(TAG, "transmit joint frame size: %" PRIu16 ", i = %" PRIi32, transmit_size + transmit_size_2, i);
        TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 2, test_pkt, transmit_size, pkt_data_2, transmit_size_2));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));
    }

    ESP_LOGI(TAG, "Verify boundary conditions");
    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    ESP_LOGI(TAG, "transmit joint frame size: %" PRIu16, transmit_size + transmit_size_2);
    TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 2, test_pkt, transmit_size, pkt_data_2, transmit_size_2));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE - 1;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    ESP_LOGI(TAG, "transmit joint frame size: %" PRIu16, transmit_size + transmit_size_2);
    TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 2, test_pkt, transmit_size, pkt_data_2, transmit_size_2));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE + 1;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    ESP_LOGI(TAG, "transmit joint frame size: %" PRIu16, transmit_size + transmit_size_2);
    TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 2, test_pkt, transmit_size, pkt_data_2, transmit_size_2));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    uint16_t transmit_size_3 = 256;
    // allocated the third buffer
    uint8_t *pkt_data_3 = malloc(256);
    // fill with data
    for (int i = 0; i < 256; i++) {
        pkt_data_3[i] = i & 0xFF;
    }
    // change protocol number so the cb function is aware that frame was joint from three buffers
    test_pkt->proto = ETHERTYPE_TX_MULTI_3;
    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    transmit_size_3 = 256;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    recv_info.expected_size_3 = transmit_size_3;
    ESP_LOGI(TAG, "transmit joint frame size (3 buffs): %" PRIu16, transmit_size + transmit_size_2 + transmit_size_3);
    TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 3, test_pkt, transmit_size, pkt_data_2, transmit_size_2, pkt_data_3, transmit_size_3));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE - 1;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    transmit_size_3 = 256;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    recv_info.expected_size_3 = transmit_size_3;
    ESP_LOGI(TAG, "transmit joint frame size (3 buffs): %" PRIu16, transmit_size + transmit_size_2 + transmit_size_3);
    TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 3, test_pkt, transmit_size, pkt_data_2, transmit_size_2, pkt_data_3, transmit_size_3));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    transmit_size = CONFIG_ETH_DMA_BUFFER_SIZE + 1;
    transmit_size_2 = CONFIG_ETH_DMA_BUFFER_SIZE;
    transmit_size_3 = 256;
    recv_info.expected_size = transmit_size;
    recv_info.expected_size_2 = transmit_size_2;
    recv_info.expected_size_3 = transmit_size_3;
    ESP_LOGI(TAG, "transmit joint frame size (3 buffs): %" PRIu16, transmit_size + transmit_size_2 + transmit_size_3);
    TEST_ESP_OK(esp_eth_transmit_vargs(eth_handle, 3, test_pkt, transmit_size, pkt_data_2, transmit_size_2, pkt_data_3, transmit_size_3));
    TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(500)));

    free(test_pkt);
    free(pkt_data_2);
    free(pkt_data_3);

    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
    vSemaphoreDelete(recv_info.mutex);
}

#if CONFIG_IDF_TARGET_ESP32P4 // IDF-8993
#include "hal/emac_hal.h"
#include "hal/emac_ll.h"
#include "soc/emac_mac_struct.h"
static esp_err_t eth_recv_err_hal_check_cb(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv)
{
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)priv;
    free(buffer);
    xSemaphoreGive(mutex);
    return ESP_OK;
}

TEST_CASE("hal erroneous frames", "[emac_hal]")
{
    SemaphoreHandle_t mutex = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(mutex);

    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));

    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    extra_eth_config(eth_handle);

    // loopback greatly simplifies the test
    bool loopback_en = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, eth_recv_err_hal_check_cb, mutex));

    // start the driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    // wait for connection start
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    // wait for connection establish
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    // create test frame
    emac_frame_t *test_pkt = calloc(1, ETH_MAX_PACKET_SIZE);
    test_pkt->proto = ETHERTYPE_TX_STD;
    memset(test_pkt->dest, 0xff, ETH_ADDR_LEN); // broadcast addr
    uint8_t local_mac_addr[ETH_ADDR_LEN] = { 0 };
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, local_mac_addr));
    memcpy(test_pkt->src, local_mac_addr, ETH_ADDR_LEN);
    // fill with data
    for (int i = 0; i < ETH_MAX_PAYLOAD_LEN; i++) {
        test_pkt->data[i] = i & 0xFF;
    }

    emac_ll_checksum_offload_mode(&EMAC_MAC, ETH_CHECKSUM_SW);

    size_t transmit_size = 1072;
    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_pkt, transmit_size));
    TEST_ASSERT(xSemaphoreTake(mutex, pdMS_TO_TICKS(500)));

    free(test_pkt);

    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
    vSemaphoreDelete(mutex);
}
#endif
