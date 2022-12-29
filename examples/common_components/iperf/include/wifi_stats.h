/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS || CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS

int wifi_cmd_get_tx_statistics(int argc, char **argv);
int wifi_cmd_clr_tx_statistics(int argc, char **argv);

int wifi_cmd_get_rx_statistics(int argc, char **argv);
int wifi_cmd_clr_rx_statistics(int argc, char **argv);

#endif

#ifdef __cplusplus
}
#endif
