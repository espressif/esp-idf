/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/soc_caps.h"
#if SOC_I2C_SUPPORTED  // TODO: [ESP32C5] IDF-8694
#include "soc/periph_defs.h"
#if SOC_PM_SUPPORT_TOP_PD
#include "soc/regdma.h"
#endif
#endif  // SOC_I2C_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2C_SUPPORTED
typedef struct {
    const uint8_t sda_out_sig;
    const uint8_t sda_in_sig;
    const uint8_t scl_out_sig;
    const uint8_t scl_in_sig;
    const uint8_t iomux_func;
    const uint8_t irq;
    const periph_module_t module;
} i2c_signal_conn_t;

extern const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM];

#if SOC_PM_SUPPORT_TOP_PD
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
} i2c_reg_ctx_link_t;

extern const i2c_reg_ctx_link_t i2c_regs_retention[SOC_HP_I2C_NUM];
#endif
#endif  // SOC_I2C_SUPPORTED

#ifdef __cplusplus
}
#endif
