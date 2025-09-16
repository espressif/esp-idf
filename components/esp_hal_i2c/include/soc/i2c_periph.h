/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/soc_caps.h"
#if SOC_I2C_SUPPORTED
#include "soc/regdma.h"
#include "soc/interrupts.h"
#if SOC_I2C_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *module_name;
    const uint8_t sda_out_sig;
    const uint8_t sda_in_sig;
    const uint8_t scl_out_sig;
    const uint8_t scl_in_sig;
    const uint8_t iomux_func;
    const uint8_t irq;
} i2c_signal_conn_t;

extern const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM];

#if SOC_I2C_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
    periph_retention_module_t module_id;
} i2c_reg_ctx_link_t;

extern const i2c_reg_ctx_link_t i2c_regs_retention[SOC_HP_I2C_NUM];
#endif

#ifdef __cplusplus
}
#endif
#endif  // SOC_I2C_SUPPORTED
