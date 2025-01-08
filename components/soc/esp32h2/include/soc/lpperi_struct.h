/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

/* For compatibility of ECO0 and ECO5 */

#pragma once

#include "soc/lpperi_rev0_0_struct.h"
#include "soc/lpperi_rev1_2_struct.h"

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compatible lpperi struct for ECO0-ECO4 and ECO5
 *
 */
typedef struct {
    volatile lpperi_clk_en_reg_t *clk_en;
    volatile lpperi_reset_en_reg_t *reset_en;
    volatile lpperi_rng_cfg_reg_t *rng_cfg;
    volatile lpperi_rng_data_reg_t *rng_data;
    volatile lpperi_cpu_reg_t *cpu;
    volatile lpperi_bus_timeout_reg_t *bus_timeout;
    volatile lpperi_bus_timeout_addr_reg_t *bus_timeout_addr;
    volatile lpperi_bus_timeout_uid_reg_t *bus_timeout_uid;
    volatile lpperi_mem_ctrl_reg_t *mem_ctrl;
    volatile lpperi_interrupt_source_reg_t *interrupt_source;
    volatile lpperi_debug_sel0_reg_t *debug_sel0;
    volatile lpperi_debug_sel1_reg_t *debug_sel1;
    volatile lpperi_date_reg_t *date;
} lpperi_dev_t;

extern lpperi_dev_t LPPERI;  // Compatible instance that achieved in 'lpperi_struct.c'

/**
 * @brief Initialize the LP_PERI register address according to the register version
 */
void lpperi_compatible_reg_addr_init(void);

#ifdef __cplusplus
}
#endif
