
// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#define MAX_PERFMON_EVENTS 118

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