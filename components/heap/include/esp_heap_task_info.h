/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef CONFIG_HEAP_TASK_TRACKING

#ifdef __cplusplus
extern "C" {
#endif

// This macro controls how much space is provided for partitioning the per-task
// heap allocation info according to one or more sets of heap capabilities.
#define NUM_HEAP_TASK_CAPS 4

/** @brief Structure to collect per-task heap allocation totals partitioned by selected caps */
typedef struct {
    TaskHandle_t task;                ///< Task to which these totals belong
    size_t size[NUM_HEAP_TASK_CAPS];  ///< Total allocations partitioned by selected caps
    size_t count[NUM_HEAP_TASK_CAPS]; ///< Number of blocks partitioned by selected caps
} heap_task_totals_t;

/** @brief Structure providing details about a block allocated by a task */
typedef struct {
    TaskHandle_t task;                ///< Task that allocated the block
    void *address;                    ///< User address of allocated block
    uint32_t size;                    ///< Size of the allocated block
} heap_task_block_t;

/** @brief Structure to provide parameters to heap_caps_get_per_task_info
 *
 * The 'caps' and 'mask' arrays allow partitioning the per-task heap allocation
 * totals by selected sets of heap region capabilities so that totals for
 * multiple regions can be accumulated in one scan.  The capabilities flags for
 * each region ANDed with mask[i] are compared to caps[i] in order; the
 * allocations in that region are added to totals->size[i] and totals->count[i]
 * for the first i that matches.  To collect the totals without any
 * partitioning, set mask[0] and caps[0] both to zero.  The allocation totals
 * are returned in the 'totals' array of heap_task_totals_t structs.  To allow
 * easily comparing the totals array between consecutive calls, that array can
 * be left populated from one call to the next so the order of tasks is the
 * same even if some tasks have freed their blocks or have been deleted.  The
 * number of blocks prepopulated is given by num_totals, which is updated upon
 * return.  If there are more tasks with allocations than the capacity of the
 * totals array (given by max_totals), information for the excess tasks will be
 * not be collected.  The totals array pointer can be NULL if the totals are
 * not desired.
 *
 * The 'tasks' array holds a list of handles for tasks whose block details are
 * to be returned in the 'blocks' array of heap_task_block_t structs.  If the
 * tasks array pointer is NULL, block details for all tasks will be returned up
 * to the capacity of the buffer array, given by max_blocks.  The function
 * return value tells the number of blocks filled into the array.  The blocks
 * array pointer can be NULL if block details are not desired, or max_blocks
 * can be set to zero.
 */
typedef struct {
    int32_t caps[NUM_HEAP_TASK_CAPS]; ///< Array of caps for partitioning task totals
    int32_t mask[NUM_HEAP_TASK_CAPS]; ///< Array of masks under which caps must match
    TaskHandle_t *tasks;              ///< Array of tasks whose block info is returned
    size_t num_tasks;                 ///< Length of tasks array
    heap_task_totals_t *totals;       ///< Array of structs to collect task totals
    size_t *num_totals;               ///< Number of task structs currently in array
    size_t max_totals;                ///< Capacity of array of task totals structs
    heap_task_block_t *blocks;        ///< Array of task block details structs
    size_t max_blocks;                ///< Capacity of array of task block info structs
} heap_task_info_params_t;

/**
 * @brief Return per-task heap allocation totals and lists of blocks.
 *
 * For each task that has allocated memory from the heap, return totals for
 * allocations within regions matching one or more sets of capabilities.
 *
 * Optionally also return an array of structs providing details about each
 * block allocated by one or more requested tasks, or by all tasks.
 *
 * @param params Structure to hold all the parameters for the function
 * (@see heap_task_info_params_t).
 * @return Number of block detail structs returned (@see heap_task_block_t).
 */
extern size_t heap_caps_get_per_task_info(heap_task_info_params_t *params);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_HEAP_TASK_TRACKING
