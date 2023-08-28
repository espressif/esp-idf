/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/interrupts.h"

#if CONFIG_IDF_TARGET_ESP32C3

/**
 * @brief Since ESP32-C3 target in QEMU doesn't support Wifi, re-use its interrupt source for ethernet
 */
#define ETS_ETH_MAC_INTR_SOURCE     ETS_WIFI_MAC_INTR_SOURCE


/**
 * @brief Use an empty I/O range for the ethernet registers
 */
#define DR_REG_EMAC_BASE            0x600CD000

#endif // CONFIG_IDF_TARGET_ESP32C3
