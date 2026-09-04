/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select crypto clock source
 *
 * @note Not supported. No-op.
 */
static inline __attribute__((always_inline)) void sec_ll_crypto_clk_src_sel(soc_module_clk_t src)
{
    (void)src;
}

#ifdef __cplusplus
}
#endif
