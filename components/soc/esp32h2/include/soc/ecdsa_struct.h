/**
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "soc/ecdsa_rev_0_0_struct.h"
#include "soc/ecdsa_rev_1_2_struct.h"

/**
 * @brief Compatible ecdsa struct wrapper
 *
 */
typedef union {
    volatile ecdsa_dev_rev_0_0_t rev_0_0;
    volatile ecdsa_dev_rev_1_2_t rev_1_2;
} ecdsa_dev_t;

extern ecdsa_dev_t ECDSA;

/* Note: For ECDSA register on ESP32-H2, you need to use the ECDSA struct through
 * ECDSA_REG_GET and ECDSA_REG_SET to access the ECDSA peripheral register and its fields respectively.
 * For e.g., ECDSA_REG_SET(ECDSA.clk.clk_gate_force_on, enable) is used to set the register value.
 * The ECDSA struct should not be referenced directly.
 */

/** The ECDSA date version of chip revision 1.2*/
#define ECDSA_REV1_2_DATE   (0x2403120)

/**
 * @brief Set the register value compatibly
 * @param reg The register to set
 * @param val The value to set
 */
#define ECDSA_REG_SET(reg, val)    (ECDSA.rev_1_2.date.ecdsa_date >= ECDSA_REV1_2_DATE ?  \
                                     (ECDSA.rev_1_2.reg = (val)) : (ECDSA.rev_0_0.reg = (val)))

/**
 * @brief Get the register value compatibly
 * @param reg The register to get
 */
#define ECDSA_REG_GET(reg)         (ECDSA.rev_1_2.date.ecdsa_date >= ECDSA_REV1_2_DATE ?  \
                                     (ECDSA.rev_1_2.reg) : (ECDSA.rev_0_0.reg))

#ifdef __cplusplus
}
#endif
