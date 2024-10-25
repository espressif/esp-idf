/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    //11b
    PHY_RATE_1M  = 0x0,
    PHY_RATE_2M  = 0x1,
    PHY_RATE_5M5 = 0x2,
    PHY_RATE_11M = 0x3,
    //11g
    PHY_RATE_6M  = 0xb,
    PHY_RATE_9M  = 0xf,
    PHY_RATE_12M = 0xa,
    PHY_RATE_18M = 0xe,
    PHY_RATE_24M = 0x9,
    PHY_RATE_36M = 0xd,
    PHY_RATE_48M = 0x8,
    PHY_RATE_54M = 0xc,
    //11n
    PHY_RATE_MCS0 = 0x10,
    PHY_RATE_MCS1 = 0x11,
    PHY_RATE_MCS2 = 0x12,
    PHY_RATE_MCS3 = 0x13,
    PHY_RATE_MCS4 = 0x14,
    PHY_RATE_MCS5 = 0x15,
    PHY_RATE_MCS6 = 0x16,
    PHY_RATE_MCS7 = 0x17,
    PHY_WIFI_RATE_MAX
} esp_phy_wifi_rate_t;

typedef enum {
    PHY_BLE_RATE_1M = 0,
    PHY_BLE_RATE_2M,
    PHY_BLE_RATE_125K,
    PHY_BLE_RATE_500k,
    PHY_BLE_RATE_MAX
} esp_phy_ble_rate_t;

typedef enum {
    PHY_BLE_TYPE_1010       = 0,
    PHY_BLE_TYPE_00001111   = 1,
    PHY_BLE_TYPE_prbs9      = 2,
    PHY_BLE_TYPE_00111100   = 4,
    PHY_BLE_TYPE_MAX
} esp_phy_ble_type_t;

/**
 * @brief Structure holding PHY RX result
 */
typedef struct {
    uint32_t phy_rx_correct_count; /*!< The number of desired packets received */
    int phy_rx_rssi;               /*!< Average RSSI of desired packets */
    uint32_t phy_rx_total_count;   /*!< The number of total packets received */
    uint32_t phy_rx_result_flag;   /*!< 0 means no RX info; 1 means the latest Wi-Fi RX info; 2 means the latest BLE RX info. */
} esp_phy_rx_result_t;

/**
 * @brief Wifi power domain power on
 */
void esp_wifi_power_domain_on(void);

/**
 * @brief Wifi power domain power off
 */
void esp_wifi_power_domain_off(void);

/**
 * @brief Environment variable configuration
 *
 * @param conf:
 *     Set to 1 to enter RF test mode.
 */
void esp_phy_rftest_config(uint8_t conf);

/**
 * @brief RF initialization configuration
 */
void esp_phy_rftest_init(void);

/**
 * @brief TX Continuous mode
 *
 * @param contin_en:
 *     Set to true for continuous packet sending, which can be used for certification testing;
 *     Set to false to cancel continuous mode, which is the default mode and can be used for WLAN tester.
*/
void esp_phy_tx_contin_en(bool contin_en);

/**
 * @brief HT40/HT20 mode selection
 *
 * @param en:
 *     Set to false to enter 11n HT20 mode;
 *     Set to true to enter 11n HT40 mode;
 **/
void esp_phy_cbw40m_en(bool en);

/**
 * @brief Wi-Fi TX command
 *
 * @param chan: channel setting, 1~14;
 * @param rate: rate setting;
 * @param backoff: Transmit power attenuation, unit is 0.25dB. For example, 4 means that the power is attenuated by 1dB;
 * @param length_byte: TX packet length configuration, indicating PSDU Length, unit is byte;
 * @param packet_delay: TX packet interval configuration, unit is us;
 * @param packet_num: The number of packets sent, 0 means sending packets continuously, other values represent the number of packets to send.
 */
void esp_phy_wifi_tx(uint32_t chan, esp_phy_wifi_rate_t rate, int8_t backoff, uint32_t length_byte, uint32_t packet_delay, uint32_t packet_num);

/**
 * @brief Test start/stop command, used to stop transmitting or receiving state.
 *
 * @param value:
 *     Value should be set to 3 before TX/RX.
 *     Set value to 0 to end TX/RX state.
 */
void esp_phy_test_start_stop(uint8_t value);

/**
 * @brief Wi-Fi RX command
 *
 * @param chan: channel setting, 1~14;
 * @param rate: rate setting;
 *
 */
void esp_phy_wifi_rx(uint32_t chan, esp_phy_wifi_rate_t rate);

/**
 * @brief Wi-Fi Carrier Wave(CW) TX command
 *
 * @param start: enable CW, 1 means transmit, 0 means stop transmitting;
 * @param chan: CW channel setting, 1~14;
 * @param backoff: CW power attenuation parameter, unit is 0.25dB. 4 indicates the power is attenuated by 1dB.
 *
 */
void esp_phy_wifi_tx_tone(uint32_t start, uint32_t chan, uint32_t backoff);

/**
 * @brief BLE TX command
 *
 * @param txpwr: Transmit power level. Tx power is about (level-8)*3 dBm, step is 3dB. Level 8 is around 0 dBm;
 * @param chan: channel setting, range is 0~39, corresponding frequency = 2402+chan*2;
 * @param len: Payload length setting, range is 0-255, unit is byte, 37 bytes is employed generally;
 * @param data_type: Data type setting;
 * @param syncw: Packet identification (need to be provided by the packet generator or instrument manufacturer), 0x71764129 is employed generally;
 * @param rate: rate setting;
 * @param tx_num_in: The number of packets sent, 0 means sending packets continuously, other values represent the number of packets to send.
 */
void esp_phy_ble_tx(uint32_t txpwr, uint32_t chan, uint32_t len, esp_phy_ble_type_t data_type, uint32_t syncw, esp_phy_ble_rate_t rate, uint32_t tx_num_in);

/**
 * @brief BLE RX command
 *
 * @param chan: channel selection, range is 0-39;
 *     Channels 0, 1, 2~10 correspond to 2404MHz, 2406MHz, 2408MHz~2424MHz respectively;
 *     Channels 11, 12, 13~36 correspond to 2428MHz, 2430MHz, 2432MHz~2478MHz respectively;
 *     Channel 37: 2402MHz, Channel 38: 2426MHz, Channel 39: 2480MHz;
 * @param syncw: Packet identification (need to be provided by the packet generator or instrument manufacturer), 0x71764129 is employed generally;
 * @param rate: rate setting;
 */
void esp_phy_ble_rx(uint32_t chan, uint32_t syncw, esp_phy_ble_rate_t rate);

/**
 * @brief BLE Carrier Wave(CW) TX command
 *
 * @param start: enable CW, 1 means transmit, 0 means stop transmitting;
 * @param chan: Single carrier transmission channel selection, range is 0~39, corresponding frequency freq = 2402+chan*2;
 * @param power: CW power attenuation parameter, unit is 0.25dB. 4 indicates the power is attenuated by 1dB.
 */
void esp_phy_bt_tx_tone(uint32_t start, uint32_t chan, uint32_t power);

/**
 * @brief Get some RX information
 *
 * @param rx_result: This struct for storing RX information;
 */
void esp_phy_get_rx_result(esp_phy_rx_result_t *rx_result);

#ifdef __cplusplus
}
#endif
