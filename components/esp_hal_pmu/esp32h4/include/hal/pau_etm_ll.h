/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/soc_etm_struct.h"
#include "soc/soc_etm_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline bool pau_etm_ll_get_regdma_event_done_status(int index)
{
    return SOC_ETM.etm_evt_st4.val & (SOC_ETM_REGDMA_EVT_DONE0_ST << index);
}

static inline bool pau_etm_ll_get_regdma_event_err_status(int index)
{
    return SOC_ETM.etm_evt_st4.val & (SOC_ETM_REGDMA_EVT_ERR0_ST << index);
}

static inline bool pau_etm_ll_get_regdma_task_start_status(int index)
{
    return SOC_ETM.etm_task_st4.val & (SOC_ETM_REGDMA_TASK_START0_ST << index);
}

static inline void pau_etm_ll_clear_regdma_event_done_status(int index)
{
    SOC_ETM.etm_evt_st4_clr.val = SOC_ETM_REGDMA_EVT_DONE0_ST << index;
}

static inline void pau_etm_ll_clear_regdma_event_err_status(int index)
{
    SOC_ETM.etm_evt_st4_clr.val = SOC_ETM_REGDMA_EVT_ERR0_ST << index;
}

static inline void pau_etm_ll_clear_regdma_task_start_status(int index)
{
    SOC_ETM.etm_task_st4_clr.val = SOC_ETM_REGDMA_TASK_START0_ST << index;
}

#ifdef __cplusplus
}
#endif
