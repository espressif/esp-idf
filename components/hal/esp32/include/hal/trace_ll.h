/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/dport_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void trace_ll_mem_enable(int cpu, bool enable)
{
    int reg[] = {DPORT_PRO_TRACEMEM_ENA_REG, DPORT_APP_TRACEMEM_ENA_REG};
    DPORT_WRITE_PERI_REG(reg[cpu], enable);
}

static inline void trace_ll_set_mode(int mode)
{
    DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, mode);
}

#ifdef __cplusplus
}
#endif
