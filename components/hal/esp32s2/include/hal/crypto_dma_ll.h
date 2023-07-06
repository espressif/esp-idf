/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "soc/hwcrypto_reg.h"
#include "soc/crypto_dma_reg.h"
#include "soc/dport_reg.h"

typedef enum {
    CRYPTO_DMA_AES= 0,
    CRYPTO_DMA_SHA,
} crypto_dma_mode_t;

/**
 * @brief Resets the DMA
 *
 */
static inline void crypto_dma_ll_reset(void)
{
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
}

/**
 * @brief Selects the crypto DMA mode
 *
 * @param mode Mode to use, AES or SHA
 */
static inline void crypto_dma_ll_set_mode(crypto_dma_mode_t mode)
{
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, mode);
}

/**
 * @brief Sets up the outlink for a transfer
 *
 * @param outlink_addr Address of the outlink buffer
 */
static inline void crypto_dma_ll_outlink_set(uint32_t outlink_addr)
{
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, outlink_addr & OUT_LINK_REG_OUTLINK_ADDR);
}

/**
 * @brief Sets up the inlink for a transfer
 *
 * @param inlink_addr Address of the inlink buffer
 */
static inline void crypto_dma_ll_inlink_set(uint32_t inlink_addr)
{
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, inlink_addr & IN_LINK_REG_INLINK_ADDR);
}

/**
 * @brief Starts the outlink
 *
 */
static inline void crypto_dma_ll_outlink_start(void)
{
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_START);
}

/**
 * @brief Starts the inlink
 *
 */
static inline void crypto_dma_ll_inlink_start(void)
{
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_START);
}

static inline bool crypto_dma_ll_inlink_is_eof(void)
{
    return ((REG_READ(CRYPTO_DMA_INT_RAW_REG) & INT_RAW_IN_SUC_EOF) == INT_RAW_IN_SUC_EOF);
}


#ifdef __cplusplus
}
#endif
