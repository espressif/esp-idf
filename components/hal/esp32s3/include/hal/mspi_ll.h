/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/syscon_struct.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  flash access control config struct
 */
typedef union {
    uint32_t val;
    struct {
        uint32_t sec_x: 1;  // Secure world cache execute access
        uint32_t sec_r: 1;  // Secure world cache read access
        uint32_t sec_w: 1;  // Secure world cache write access
        uint32_t nsec_x: 1; // Non-secure world cache execute access
        uint32_t nsec_r: 1; // Non-secure world cache execute read
        uint32_t nsec_w: 1; // Non-secure world cache execute write
        uint32_t spi1_r: 1; // SPI1 read access
        uint32_t spi1_w: 1; // SPI1 write access
    };
} mspi_ll_flash_ace_ctrl_t;

/**
 * @brief Set PMS flash protection start address
 *
 * @param region  PMS region id (0~3)
 * @param address Starting address
 *
 */
__attribute__((always_inline))
static inline void mspi_ll_set_flash_protection_addr(uint32_t region, uint32_t address)
{
    switch(region){
        case 0:
            SYSCON.flash_ace0_addr = address;
            break;
        case 1:
            SYSCON.flash_ace1_addr = address;
            break;
        case 2:
            SYSCON.flash_ace2_addr = address;
            break;
        case 3:
            SYSCON.flash_ace3_addr = address;
            break;
        default:
            HAL_ASSERT(false);
    }
}

/**
 * @brief Set PMS flash protection size
 *
 * @param region  PMS region id (0~3)
 * @param size    Size, in number of 64kB pages
 *
 */
__attribute__((always_inline))
static inline void mspi_ll_set_flash_protection_size(uint32_t region, uint32_t size)
{
    switch(region){
        case 0:
            SYSCON.flash_ace0_size.flash_ace0_size = size;
            break;
        case 1:
            SYSCON.flash_ace1_size.flash_ace1_size = size;
            break;
        case 2:
            SYSCON.flash_ace2_size.flash_ace2_size = size;
            break;
        case 3:
            SYSCON.flash_ace3_size.flash_ace3_size = size;
            break;
        default:
            HAL_ASSERT(false);
    }
}

/**
 * @brief Set PMS flash protection access ctrl bits
 *
 * @param region  PMS region id (0~3)
 * @param address ctrl bits, RWX per mode
 *
 */
__attribute__((always_inline))
static inline void mspi_ll_set_flash_protection_access(uint32_t region, mspi_ll_flash_ace_ctrl_t ctrl)
{
    switch(region){
        case 0:
            SYSCON.flash_ace0_attr.flash_ace0_attr = ctrl.val;
            break;
        case 1:
            SYSCON.flash_ace1_attr.flash_ace1_attr = ctrl.val;
            break;
        case 2:
            SYSCON.flash_ace2_attr.flash_ace2_attr = ctrl.val;
            break;
        case 3:
            SYSCON.flash_ace3_attr.flash_ace3_attr = ctrl.val;
            break;
        default:
            HAL_ASSERT(false);
    }
}

#ifdef __cplusplus
}
#endif
