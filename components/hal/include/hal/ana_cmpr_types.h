/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Analog comparator interrupt type
 *
 */
typedef enum {
    ANA_CMPR_CROSS_DISABLE,         /*!< Disable the cross event interrupt */
    ANA_CMPR_CROSS_POS,             /*!< Positive cross can trigger event interrupt */
    ANA_CMPR_CROSS_NEG,             /*!< Negative cross can trigger event interrupt */
    ANA_CMPR_CROSS_ANY,             /*!< Any cross can trigger event interrupt */
} ana_cmpr_cross_type_t;

/**
 * @brief Analog comparator internal reference voltage
 *
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

#ifdef __cplusplus
}
#endif
