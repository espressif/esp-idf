/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Definitions of Espressif SDIO Slave hardware


#include "essl_sdio.h"

essl_sdio_def_t ESSL_SDIO_DEF_ESP32 = {
    .new_packet_intr_mask = BIT(23),
};

essl_sdio_def_t ESSL_SDIO_DEF_ESP32C6 = {
    .new_packet_intr_mask = BIT(23),
};
