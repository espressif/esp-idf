/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "private/esp_coexist_debug.h"

#if CONFIG_ESP_COEX_GPIO_DEBUG
#ifdef COEX_GPIO_DEBUG_DIAG_GENERAL
void diagram_bind_io_to_evt(void) {
    /* Bind IO to coexist evt */
#if CONFIG_IDF_TARGET_ESP32
    coex_bind_io_to_evt(0, 8);
    coex_bind_io_to_evt(0, 11);
    coex_bind_io_to_evt(1, 16);
#else
    coex_bind_io_to_evt(0, 0);
    coex_bind_io_to_evt(1, 1);
#endif
    /* Bind IO to Wi-Fi evt */
    wifi_bind_io_to_evt(2, 9);
    wifi_bind_io_to_evt(3, 10);
    wifi_bind_io_to_evt(4, 11);

    wifi_bind_io_to_evt(5, 0);
    wifi_bind_io_to_evt(6, 1);
}
#elif defined(COEX_GPIO_DEBUG_DIAG_WIFI)
void diagram_bind_io_to_evt(void) {
    /* Bind IO to coexist evt */
#if CONFIG_IDF_TARGET_ESP32
    coex_bind_io_to_evt(0, 8);
    coex_bind_io_to_evt(0, 11);
    coex_bind_io_to_evt(1, 16);
#else
    coex_bind_io_to_evt(0, 0);
    coex_bind_io_to_evt(1, 1);
#endif
    /* Bind IO to Wi-Fi evt */
    wifi_bind_io_to_evt(2, 9);
    wifi_bind_io_to_evt(3, 10);
    wifi_bind_io_to_evt(4, 11);

    wifi_bind_io_to_evt(5, 0);
    wifi_bind_io_to_evt(6, 1);

    wifi_bind_io_to_evt(7, 3);
    wifi_bind_io_to_evt(8, 4);
    wifi_bind_io_to_evt(9, 5);
    wifi_bind_io_to_evt(10, 6);
    wifi_bind_io_to_evt(11, 7);
}
#else
void diagram_bind_io_to_evt(void) {
}
#endif

#endif
