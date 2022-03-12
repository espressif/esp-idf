/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once


#ifdef __cplusplus
extern "C" {
#endif

/**
 * MMU Page size
 */
typedef enum {
    MMU_PAGE_16KB,
    MMU_PAGE_32KB,
    MMU_PAGE_64KB,
} mmu_page_size_t;

/**
 * External physical memory
 */
typedef enum {
    MMU_TARGET_FLASH0,
    MMU_TARGET_PSRAM0,
} mmu_target_t;

#ifdef __cplusplus
}
#endif
