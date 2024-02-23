/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
/**
 * This file contains SDIO Slave hardware specific requirements
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    //interrupts
    uint32_t new_packet_intr_mask;
} essl_sdio_def_t;

/// Definitions of ESP32 SDIO Slave hardware
extern essl_sdio_def_t ESSL_SDIO_DEF_ESP32;
/// Definitions of ESP32C6 SDIO Slave hardware
extern essl_sdio_def_t ESSL_SDIO_DEF_ESP32C6;

#ifdef __cplusplus
}
#endif
