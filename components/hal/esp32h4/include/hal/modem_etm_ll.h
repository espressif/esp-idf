/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "hal/assert.h"
#include "soc/soc_etm_struct.h"
#include "soc/soc_etm_reg.h"
#include "soc/soc_etm_source.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODEM_LL_ETM_EVENT_TABLE(event_type) \
    (uint32_t[4]){ \
        [0] = MODEM_EVT_G0, \
        [1] = MODEM_EVT_G1, \
        [2] = MODEM_EVT_G2, \
        [3] = MODEM_EVT_G3, \
    }[event_type]

#define MODEM_LL_ETM_TASK_TABLE(task_type) \
    (uint32_t[4]){ \
        [0] = MODEM_TASK_G0, \
        [1] = MODEM_TASK_G1, \
        [2] = MODEM_TASK_G2, \
        [3] = MODEM_TASK_G3, \
    }[task_type]

static inline bool modem_etm_ll_get_group_event_status(int group_id)
{
    switch (group_id) {
        case 0:
            return SOC_ETM.etm_evt_st6.etm_modem_evt_g0_st;
        case 1:
            return SOC_ETM.etm_evt_st6.etm_modem_evt_g1_st;
        case 2:
            return SOC_ETM.etm_evt_st6.etm_modem_evt_g2_st;
        case 3:
            return SOC_ETM.etm_evt_st6.etm_modem_evt_g3_st;
        default:
            HAL_ASSERT(false);
    }
    return false;
}

static inline bool modem_etm_ll_get_group_task_status(int group_id)
{
    switch (group_id) {
        case 0:
            return SOC_ETM.etm_task_st5.etm_modem_task_g0_st;
        case 1:
            return SOC_ETM.etm_task_st5.etm_modem_task_g1_st;
        case 2:
            return SOC_ETM.etm_task_st5.etm_modem_task_g2_st;
        case 3:
            return SOC_ETM.etm_task_st5.etm_modem_task_g3_st;
        default:
            HAL_ASSERT(false);
    }
    return false;
}

static inline void modem_etm_ll_clear_group_event_status(int group_id)
{
    SOC_ETM.etm_evt_st6_clr.val = SOC_ETM_MODEM_EVT_G0_ST_CLR << group_id;
}

static inline void modem_etm_ll_clear_group_task_status(int group_id)
{
    SOC_ETM.etm_task_st5_clr.val = SOC_ETM_MODEM_TASK_G0_ST_CLR << group_id;
}

#ifdef __cplusplus
}
#endif
