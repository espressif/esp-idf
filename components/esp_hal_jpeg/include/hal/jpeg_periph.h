/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#if SOC_JPEG_CODEC_SUPPORTED
#include "soc/regdma.h"
#include "soc/interrupts.h"
#include "soc/retention_periph_defs.h"
#include "soc/jpeg_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
    periph_retention_module_t module_id;
} jpeg_reg_ctx_link_t;

extern const jpeg_reg_ctx_link_t jpeg_regs_retention;

#ifdef __cplusplus
}
#endif

#endif // SOC_JPEG_CODEC_SUPPORTED
