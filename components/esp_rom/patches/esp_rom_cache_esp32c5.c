/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp32c5/rom/cache.h"

/**
 * @brief Dummy implementation of Cache_Set_IDROM_MMU_Size from ROM.
 *
 * NOTE: This ROM-provided API is intended to configure the Cache MMU size for
 * instruction (irom) and rodata (drom) sections in flash.
 *
 * On ESP32-C5, it also sets the start pages for flash irom and drom sections,
 * which involves accessing MMU registers directly. However, these MMU registers
 * are protected by the APM and direct access from the REE results in a fault.
 *
 * To prevent this:
 * - Mark this symbol with the `PROVIDE` attribute in the ROM linker script.
 * - Define this dummy function as a stub implementation.
 * - Wrap this function to be routed as a TEE service call
 *
 * @param irom_size Size of the instruction ROM region.
 * @param drom_size Size of the data ROM region.
 */
void Cache_Set_IDROM_MMU_Size(uint32_t irom_size, uint32_t drom_size)
{
    (void)irom_size;
    (void)drom_size;
}
