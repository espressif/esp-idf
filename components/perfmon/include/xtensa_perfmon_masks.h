
/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _xtensa_perfmon_masks_H_
#define _xtensa_perfmon_masks_H_
#include <inttypes.h>
#include "xtensa/xt_perf_consts.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Description for select parameter
 *
 * Structure defines description for different select values for performance counters
 */
typedef struct xtensa_perfmon_select {
    int select; /*!<  Selected counter */
    const char *description; /*!<  Description for selected counter */
} xtensa_perfmon_select_t;

/**
 * @brief Description for mask parameter
 *
 * Structure defines description for different select and mask values for performance counters
 */
typedef struct xtensa_perfmon_masks {
    int select; /*!<  Selected counter */
    int mask; /*!<  Selected mask for counter */
    const char *description; /*!<  Description for selected mask */
} xtensa_perfmon_masks_t;

// Maximum amount of performance counter events
#define MAX_PERFMON_EVENTS 119

/**
 * @brief Select value description table
 *
 * Table contains description for different 'select' values for performance counter
 */
extern const xtensa_perfmon_select_t xtensa_perfmon_select_table[];

/**
 * @brief Mask value description table
 *
 * Table contains description for different 'select' and 'mask' values for performance counter
 */
extern const xtensa_perfmon_masks_t xtensa_perfmon_masks_table[];

/**
 * @brief All available counters
 *
 * Table contains all available counters
 */
extern const uint32_t xtensa_perfmon_select_mask_all[MAX_PERFMON_EVENTS * 2];

#ifdef __cplusplus
}
#endif
#endif // _xtensa_perfmon_masks_H_
