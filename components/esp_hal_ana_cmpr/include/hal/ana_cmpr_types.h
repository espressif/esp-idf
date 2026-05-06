/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Analog comparator interrupt type
 */
typedef enum {
    ANA_CMPR_CROSS_DISABLE,         /*!< Disable the cross event interrupt */
    ANA_CMPR_CROSS_POS,             /*!< Positive cross can trigger event interrupt */
    ANA_CMPR_CROSS_NEG,             /*!< Negative cross can trigger event interrupt */
    ANA_CMPR_CROSS_ANY,             /*!< Any cross can trigger event interrupt */
} ana_cmpr_cross_type_t;

/**
 * @brief Analog comparator internal reference voltage
 */
typedef enum {
    ANA_CMPR_REF_VOLT_0_PCT_VDD,    /*!< Internal reference voltage equals to 0% VDD */
    ANA_CMPR_REF_VOLT_10_PCT_VDD,   /*!< Internal reference voltage equals to 10% VDD */
    ANA_CMPR_REF_VOLT_20_PCT_VDD,   /*!< Internal reference voltage equals to 20% VDD */
    ANA_CMPR_REF_VOLT_30_PCT_VDD,   /*!< Internal reference voltage equals to 30% VDD */
    ANA_CMPR_REF_VOLT_40_PCT_VDD,   /*!< Internal reference voltage equals to 40% VDD */
    ANA_CMPR_REF_VOLT_50_PCT_VDD,   /*!< Internal reference voltage equals to 50% VDD */
    ANA_CMPR_REF_VOLT_60_PCT_VDD,   /*!< Internal reference voltage equals to 60% VDD */
    ANA_CMPR_REF_VOLT_70_PCT_VDD,   /*!< Internal reference voltage equals to 70% VDD */
} ana_cmpr_ref_voltage_t;

/**
 * @brief Analog comparator reference source
 */
typedef enum {
    ANA_CMPR_REF_SRC_INTERNAL,       /*!< Analog Comparator reference voltage comes from internal */
    ANA_CMPR_REF_SRC_EXTERNAL,       /*!< Analog Comparator reference voltage comes from external pin */
} ana_cmpr_ref_source_t;

/**
 * @brief Analog comparator internal reference hysteresis level
 *
 * @note The exact hysteresis voltage is hardware-dependent. The level-to-voltage
 *       mapping on ESP32-H4 is approximately:
 *       - LEVEL0: ~0V
 *       - LEVEL1: ~0.04V
 *       - LEVEL2: ~0.08V
 *       - LEVEL3: ~0.12V
 */
typedef enum {
    ANA_CMPR_REF_HYS_LEVEL0,         /*!< No hysteresis */
    ANA_CMPR_REF_HYS_LEVEL1,         /*!< Hysteresis level 1 */
    ANA_CMPR_REF_HYS_LEVEL2,         /*!< Hysteresis level 2 */
    ANA_CMPR_REF_HYS_LEVEL3,         /*!< Hysteresis level 3 */
} ana_cmpr_ref_hys_t;

/**
 * @brief Analog comparator scan mode
 */
typedef enum {
    ANA_CMPR_SCAN_MODE_FULL,         /*!< One trigger scans all enabled source channels */
    ANA_CMPR_SCAN_MODE_STEP,         /*!< One trigger scans one enabled source channel in sequence */
} ana_cmpr_scan_mode_t;

/**
 * @brief Analog Comparator ETM Events for each unit
 */
typedef enum {
    ANA_CMPR_EVENT_POS_CROSS,        /*!< Positive cross event when the source signal becomes higher than the reference signal */
    ANA_CMPR_EVENT_NEG_CROSS,        /*!< Negative cross event when the source signal becomes lower than the reference signal */
    ANA_CMPR_EVENT_MAX,              /*!< The max number of events, not a real event */
} ana_cmpr_event_type_t;

/**
 * @brief Analog Comparator ETM Tasks for each unit
 */
typedef enum {
    ANA_CMPR_TASK_START,  /*!< Start the comparison */
    ANA_CMPR_TASK_MAX,    /*!< The max number of tasks, not a real task */
} ana_cmpr_task_type_t;

#if SOC_ANA_CMPR_SUPPORTED
/**
 * @brief Analog comparator clock source
 */
typedef soc_periph_ana_cmpr_clk_src_t ana_cmpr_clk_src_t;
#else
/**
 * @brief Analog comparator clock source
 */
typedef int ana_cmpr_clk_src_t;
#endif

#ifdef __cplusplus
}
#endif
