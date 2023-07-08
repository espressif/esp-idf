/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H2 PAU(Power Assist Unit) register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/pau_reg.h"
#include "soc/pau_struct.h"
#include "hal/pau_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline __attribute__((always_inline)) uint32_t pau_ll_get_regdma_backup_flow_error(pau_dev_t *dev)
{
    return dev->regdma_conf.flow_err;
}

static inline __attribute__((always_inline)) void pau_ll_select_regdma_entry_link(pau_dev_t *dev, int link)
{
    dev->regdma_conf.link_sel = link;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_entry_link_backup_direction(pau_dev_t *dev, bool to_mem)
{
    dev->regdma_conf.to_mem = to_mem ? 1 : 0;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_entry_link_backup_start_enable(pau_dev_t *dev)
{
    dev->regdma_conf.start = 1;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_entry_link_backup_start_disable(pau_dev_t *dev)
{
    dev->regdma_conf.start = 0;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_link0_addr(pau_dev_t *dev, void *link_addr)
{
    dev->regdma_link_0_addr.val = (uint32_t)link_addr;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_link1_addr(pau_dev_t *dev, void *link_addr)
{
    dev->regdma_link_1_addr.val = (uint32_t)link_addr;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_link2_addr(pau_dev_t *dev, void *link_addr)
{
    dev->regdma_link_2_addr.val = (uint32_t)link_addr;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_link3_addr(pau_dev_t *dev, void *link_addr)
{
    dev->regdma_link_3_addr.val = (uint32_t)link_addr;
}

static inline __attribute__((always_inline)) uint32_t pau_ll_get_regdma_current_link_addr(pau_dev_t *dev)
{
    return dev->regdma_current_link_addr.val;
}

static inline __attribute__((always_inline)) uint32_t pau_ll_get_regdma_backup_addr(pau_dev_t *dev)
{
    return dev->regdma_backup_addr.val;
}

static inline __attribute__((always_inline)) uint32_t pau_ll_get_regdma_memory_addr(pau_dev_t *dev)
{
    return dev->regdma_mem_addr.val;
}

static inline __attribute__((always_inline)) uint32_t pau_ll_get_regdma_intr_raw_signal(pau_dev_t *dev)
{
    return dev->int_raw.val;
}

static inline __attribute__((always_inline)) uint32_t pau_ll_get_regdma_intr_status(pau_dev_t *dev)
{
    return dev->int_st.val;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_backup_done_intr_enable(pau_dev_t *dev)
{
    dev->int_ena.done_int_ena = 1;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_backup_done_intr_disable(pau_dev_t *dev)
{
    dev->int_ena.done_int_ena = 0;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_backup_error_intr_enable(pau_dev_t *dev)
{
    dev->int_ena.error_int_ena = 1;
}

static inline __attribute__((always_inline)) void pau_ll_set_regdma_backup_error_intr_disable(pau_dev_t *dev)
{
    dev->int_ena.error_int_ena = 0;
}

static inline __attribute__((always_inline)) void pau_ll_clear_regdma_backup_done_intr_state(pau_dev_t *dev)
{
    dev->int_clr.done_int_clr = 1;
}

static inline __attribute__((always_inline)) void pau_ll_clear_regdma_backup_error_intr_state(pau_dev_t *dev)
{
    dev->int_clr.error_int_clr = 1;
}

#ifdef __cplusplus
}
#endif
