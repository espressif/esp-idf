/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Number of configurable DMA_PMS regions */
#define APM_DMA_PMS_REGION_NUM             (32)
/* Alignment required for start/end addresses for DMA_PMS regions */
#define APM_DMA_PMS_REGION_ADDR_ALIGN      (0x1000)
/* Register offset between DMA_PMS region bound address registers */
#define APM_DMA_PMS_REGION_ADDR_OFFSET     (0x08)
/* Register offset between DMA_PMS region attribute registers */
#define APM_DMA_PMS_REGION_ATTR_OFFSET     (0x08)

/* Number of configurable LP_PERI_PMS regions */
#if CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define APM_LP_PERI_PMS_REGION_NUM         (2)
#else
#define APM_LP_PERI_PMS_REGION_NUM         (8)
#endif
/* Alignment required for start/end addresses for LP_PERI_PMS regions */
#define APM_LP_PERI_PMS_REGION_ADDR_ALIGN  (4)

/* Number of configurable MSPI PMS regions (flash/PSRAM) */
#define APM_MSPI_PMS_REGION_NUM            (4)
/* Alignment required for start/end addresses for MSPI_PMS regions */
#define APM_MSPI_PMS_REGION_ADDR_ALIGN     (0x1000)

/* CPU_PERIPH slave mask */
#define APM_SLAVE_CPU_PERI_ALL_MASK        (0x0FCFU)
/* HP_PERIPH0 slave mask */
#define APM_SLAVE_HP_PERI0_ALL_MASK        (0x3FFFFFEFU)
/* HP_PERIPH1 slave mask */
#define APM_SLAVE_HP_PERI1_ALL_MASK        (0x1FDFEFFFFFULL)
/* LP_PERIPH slave mask */
#if CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define APM_SLAVE_LP_PERI_ALL_MASK         (0x00FFFFFFU)
#else
#define APM_SLAVE_LP_PERI_ALL_MASK         (0x01FFFFFFU)
#endif

#ifdef __cplusplus
}
#endif
