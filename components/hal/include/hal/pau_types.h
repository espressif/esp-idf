/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#define PAU_REGDMA_LINK_NUM     (SOC_PM_PAU_LINK_NUM)

/**
 * @brief PAU REGDMA all link address buffer
 */
typedef void * pau_regdma_link_addr_t[PAU_REGDMA_LINK_NUM];
