/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/interrupts.h"

/*
 * For targets which don't have an ethernet MAC and the associated interrupt source,
 * we can reuse the Wifi interrupt source for ethernet, since QEMU doesn't emulate Wifi (yet).
 * We also map the EMAC registers to an unused address range.
 */
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#define ETS_ETH_MAC_INTR_SOURCE     ETS_WIFI_MAC_INTR_SOURCE
#define DR_REG_EMAC_BASE            0x600CD000
#endif
