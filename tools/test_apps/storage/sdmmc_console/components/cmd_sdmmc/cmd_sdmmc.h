/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SDMMC_HOST_SUPPORTED
/**
 * @brief Register console commands related to SDMMC host driver
 */
void register_sdmmc_host(void);
#endif

/**
 * @brief Register console commands related to sdmmc protocol layer
 */
void register_sdmmc_common(void);

#ifdef __cplusplus
}
#endif
