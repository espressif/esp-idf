/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memprot LL error codes
 *
 */
typedef enum {
    MEMP_LL_OK = 0,
    MEMP_LL_FAIL = 1,
    MEMP_LL_ERR_SPLIT_ADDR_OUT_OF_RANGE = 2,
    MEMP_LL_ERR_SPLIT_ADDR_INVALID = 2,         /* temporary duplicate for S2 builds */
    MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED = 3,
    MEMP_LL_ERR_UNI_BLOCK_INVALID = 4,
    MEMP_LL_ERR_AREA_INVALID = 5,
    MEMP_LL_ERR_WORLD_INVALID = 6
} memprot_ll_err_t;

/**
 * @brief Memprot LL PMS World IDs
 *
 */
typedef enum {
    MEMP_LL_WORLD_NONE = 0x00,
    MEMP_LL_WORLD_0 = 0x01,
    MEMP_LL_WORLD_1 = 0x10
} memprot_ll_world_t;

/**
 * @brief Memprot LL PMS Area IDs
 *
 */
typedef enum {
    MEMP_LL_AREA_NONE = 0,
    MEMP_LL_AREA_LOW = 1,
    MEMP_LL_AREA_HIGH = 2
} memprot_ll_area_t;

#ifdef __cplusplus
}
#endif
