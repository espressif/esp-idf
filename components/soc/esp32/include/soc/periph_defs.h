/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PERIPH_UART1_MODULE,
    PERIPH_UART2_MODULE,
    PERIPH_I2S0_MODULE,
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_SPI_MODULE,
    PERIPH_HSPI_MODULE,
    PERIPH_VSPI_MODULE,
    PERIPH_RNG_MODULE,
    PERIPH_WIFI_MODULE,
    PERIPH_BT_MODULE,
    PERIPH_WIFI_BT_COMMON_MODULE,
    PERIPH_BT_BASEBAND_MODULE,
    PERIPH_PHY_CALIBRATION_MODULE,
    PERIPH_MODULE_MAX
} shared_periph_module_t;

#ifdef __cplusplus
}
#endif
