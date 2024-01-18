/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "soc/regdma.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED
#define PAU_REGDMA_LINK_NUM     (REGDMA_LINK_ENTRY_NUM)

/**
 * @brief PAU REGDMA all link address buffer
 */
typedef regdma_entry_buf_t pau_regdma_link_addr_t;

#endif

#ifdef __cplusplus
}
#endif
