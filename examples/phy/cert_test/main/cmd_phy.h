/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_phy_cert_test.h"

typedef struct {
    struct arg_int *enable;
    struct arg_end *end;
} phy_args_t;

#if SOC_WIFI_SUPPORTED
typedef struct {
    struct arg_int *channel;
    struct arg_int *rate;
    struct arg_int *attenuation;
    struct arg_int *length_byte;
    struct arg_int *packet_delay;
    struct arg_int *packet_num;
    struct arg_end *end;
} phy_wifi_tx_t;

typedef struct {
    struct arg_int *channel;
    struct arg_int *rate;
    struct arg_end *end;
} phy_wifi_rx_t;

typedef struct {
    struct arg_int *enable;
    struct arg_int *channel;
    struct arg_int *attenuation;
    struct arg_end *end;
} phy_wifiscwout_t;

typedef struct {
    uint32_t channel;
    esp_phy_wifi_rate_t rate;
    int8_t backoff;
    uint32_t length_byte;
    uint32_t packet_delay;
    uint32_t packet_num;
} phy_wifi_tx_s;

typedef struct {
    uint32_t channel;
    esp_phy_wifi_rate_t rate;
} phy_wifi_rx_s;
#endif

#if SOC_BT_SUPPORTED
typedef struct {
    struct arg_int *txpwr;
    struct arg_int *channel;
    struct arg_int *len;
    struct arg_int *data_type;
    struct arg_int *syncw;
    struct arg_int *rate;
    struct arg_int *tx_num_in;
    struct arg_end *end;
} phy_ble_tx_t;

typedef struct {
    struct arg_int *channel;
    struct arg_int *syncw;
    struct arg_int *rate;
    struct arg_end *end;
} phy_ble_rx_t;

typedef struct {
    struct arg_int *start;
    struct arg_int *channel;
    struct arg_int *attenuation;
    struct arg_end *end;
} phy_bt_tx_tone_t;

typedef struct {
    uint32_t txpwr;
    uint32_t channel;
    uint32_t len;
    esp_phy_ble_type_t data_type;
    uint32_t syncw;
    esp_phy_ble_rate_t rate;
    uint32_t tx_num_in;
} phy_ble_tx_s;

typedef struct {
    uint32_t channel;
    uint32_t syncw;
    esp_phy_ble_rate_t rate;
} phy_ble_rx_s;
#endif

void register_phy_cmd(void);

#ifdef __cplusplus
}
#endif
