/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "soc/soc_caps.h"

/**
 * @brief PMU modes of HP system
 */
typedef enum {
    PMU_MODE_HP_ACTIVE = 0, /*!< PMU in HP_ACTIVE mode */
    PMU_MODE_HP_MODEM,      /*!< PMU in HP_MODEM mode */
    PMU_MODE_HP_SLEEP,      /*!< PMU in HP_SLEEP mode */
    PMU_MODE_HP_MAX,
} pmu_hp_mode_t;

/**
 * @brief PMU modes of LP system
 */
typedef enum {
    PMU_MODE_LP_ACTIVE = 0, /*!< PMU in LP_ACTIVE mode */
    PMU_MODE_LP_SLEEP,      /*!< PMU in LP_SLEEP mode */
    PMU_MODE_LP_MAX,
} pmu_lp_mode_t;

typedef enum {
    PMU_HP_PD_TOP = 0,      /*!< Power domain of digital top */
#if SOC_PM_SUPPORT_HP_AON_PD
    PMU_HP_PD_HP_AON = 1,   /*!< Power domain of always-on */
#endif
    PMU_HP_PD_CPU = 2,      /*!< Power domain of HP CPU */
    PMU_HP_PD_RESERVED = 3, /*!< Reserved power domain */
    PMU_HP_PD_WIFI = 4,     /*!< Power domain of WIFI */
} pmu_hp_power_domain_t;

#ifdef __cplusplus
}
#endif
