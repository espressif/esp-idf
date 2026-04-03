/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_rom_caps.h"
#include "esp_rom_regi2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief REGI2C implementation layer
 *
 * If CONFIG_ESP_ROM_HAS_REGI2C_IMPL is not set,
 * the IDF provides its own implementation of the regi2c functions.
 * Otherwise, the ROM implementation is used directly.
 *
 * @note The APIs provided by this layer does not consider either clock gating to analog i2c master clock or atomicity of the operations.
 *       Upper layer (esp_private/regi2c_ctrl.h) provides APIs that considered both, and is recommended to use.
 *       If you are using the interfaces in this layer directly (when you want to group multiple regi2c operations together), please make sure to enable the clock gating and atomicity by yourself.
 */

/**
 * @brief Read internal register
 */
uint8_t _regi2c_impl_read(uint8_t block, uint8_t host_id, uint8_t reg_add);

/**
 * @brief Read internal register with mask
 */
uint8_t _regi2c_impl_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);

/**
 * @brief Write internal register
 */
void _regi2c_impl_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);

/**
 * @brief Write internal register with mask
 */
void _regi2c_impl_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

#if NON_OS_BUILD

/* If compiling for the non-FreeRTOS builds (e.g. bootloader), REGI2C read/write operations are always atomic */
#define regi2c_impl_read(...)              _regi2c_impl_read(__VA_ARGS__)
#define regi2c_impl_read_mask(...)         _regi2c_impl_read_mask(__VA_ARGS__)
#define regi2c_impl_write(...)             _regi2c_impl_write(__VA_ARGS__)
#define regi2c_impl_write_mask(...)        _regi2c_impl_write_mask(__VA_ARGS__)

#else // !NON_OS_BUILD

/* REGI2C read/write needs to be atomic, so any regi2c operation needs to be wrapped within a lock, this is implemented in esp_private/regi2c_ctrl.h */
#define regi2c_impl_read(...) ({ \
    (void)__DECLARE_REGI2C_ATOMIC_ENV; \
    int _temp; \
    do { \
        _temp = _regi2c_impl_read(__VA_ARGS__); \
    } while(0); \
    _temp; \
})

#define regi2c_impl_read_mask(...) ({ \
    (void)__DECLARE_REGI2C_ATOMIC_ENV; \
    int _temp; \
    do { \
        _temp = _regi2c_impl_read_mask(__VA_ARGS__); \
    } while(0); \
    _temp; \
})

#define regi2c_impl_write(...) do { \
        (void)__DECLARE_REGI2C_ATOMIC_ENV; \
        _regi2c_impl_write(__VA_ARGS__); \
    } while(0)

#define regi2c_impl_write_mask(...) do { \
        (void)__DECLARE_REGI2C_ATOMIC_ENV; \
        _regi2c_impl_write_mask(__VA_ARGS__); \
    } while(0)

#endif

#ifdef __cplusplus
}
#endif
