/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

/* This is a wrapper for revisions compatibility */

#pragma once

#include "soc/lpperi_rev0_0_struct.h"
#include "soc/lpperi_rev1_2_struct.h"

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compatible lpperi struct wrapper
 *
 */
typedef union {
    volatile lpperi_rev0_0_dev_t rev0_0;        /* Struct for LPPERI v0.0 < rev < v1.2 */
    volatile lpperi_rev1_2_dev_t rev1_2;        /* Struct for LPPERI rev >= v1.2*/
} lpperi_dev_t;

extern lpperi_dev_t LPPERI;

/** The LPPERI date version of chip revision 1.2*/
#define LPPERI_REV1_2_DATE   (0x2308030)

/**
 * @brief Set the register value compatibly
 * @param reg The register to set (can carry the field, like clk_en.lp_io_ck_en)
 * @param val The value to set
 */
#define LPPERI_REG_SET(reg, val)    (LPPERI.rev1_2.date.lpperi_date >= LPPERI_REV1_2_DATE ?  \
                                     (LPPERI.rev1_2.reg = (val)) : (LPPERI.rev0_0.reg = (val)))

/**
 * @brief Get the register value compatibly
 * @param reg The register to get (can carry the field, like clk_en.lp_io_ck_en)
 */
#define LPPERI_REG_GET(reg)         (LPPERI.rev1_2.date.lpperi_date >= LPPERI_REV1_2_DATE ?  \
                                     (LPPERI.rev1_2.reg) : (LPPERI.rev0_0.reg))

#ifdef __cplusplus
}
#endif
