/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Enable Quad I/O mode in bootloader (if configured)
 *
 * Queries attached SPI flash ID and sends correct SPI flash
 * commands to enable QIO or QOUT mode, then enables this mode.
 */
void bootloader_enable_qio_mode(void);

/**
 * @brief Read flash ID by sending 0x9F command
 * @return flash raw ID
 *     mfg_id = (ID >> 16) & 0xFF;
       flash_id = ID & 0xffff;
 */
uint32_t bootloader_read_flash_id(void);

#ifdef __cplusplus
}
#endif
