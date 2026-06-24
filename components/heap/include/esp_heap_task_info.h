/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_HEAP_TASK_TRACKING || __DOXYGEN__

#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

/** @brief Structure providing details about memory usage of a given task on a heap. */
typedef struct {
    const char *name; ///< Pointer to the name of the heap defined in soc_memory_types[]
    uint32_t caps; ///< All caps supported by the heap (ORED)
    size_t size; ///< The available size of the heap
    size_t current_usage; ///< The current usage of a given task on the heap
    size_t peak_usage; ///< The peak usage since startup on a given task on the heap
    size_t alloc_count; ///< The current number of allocation by a given task on the heap
    heap_task_block_t *alloc_stat; ///< Pointer to an array of allocation stats for a given task on the heap
} heap_stat_t;

/** @brief Structure providing details about a task. */
typedef struct {
    char name[configMAX_TASK_NAME_LEN]; ///< Name of the task
    TaskHandle_t handle; ///< Pointer to the task handle.
    bool is_alive; ///< Information whether the task is alive (true) or deleted (false)
    size_t overall_peak_usage; ///< Information about the memory peak usage across all heaps of a given task
    size_t overall_current_usage; ///< Information about the memory current usage across all heaps of a given task
    size_t heap_count; ///< Number of different heaps the task has used since its creation
    heap_stat_t *heap_stat; ///< Pointer to an array containing statistics of the heaps used by the task
} task_stat_t;

/**
 * @brief User interface containing the statistics of a given task
 * and the associated memory usage of the task on each heap.
 */
typedef struct {
    task_stat_t stat; ///< Statistics of the task
    size_t heap_count; ///< size of user defined heap_stat array
    heap_stat_t *heap_stat_start; ///< Pointer to the start to the user defined heap_stat array
    size_t alloc_count; ///< size of user defined alloc_stat array
    heap_task_block_t *alloc_stat_start; ///< Pointer to the start to the user defined alloc_stat array
} heap_single_task_stat_t;

/**
 * @brief User interface containing the statistics of all tasks and the associated
 * memory usage of those tasks on each heap they use.
 */
typedef struct {
    size_t task_count; ///< user defined size of heap_single_task_stat_t array
    task_stat_t *stat_arr; ///< Pointer to the user defined array of heap_single_task_stat_t
    size_t heap_count; ///< size of user defined heap_stat array
    heap_stat_t *heap_stat_start; ///< Pointer to the start to the user defined heap_stat array
    size_t alloc_count; ///< size of user defined alloc_stat array
    heap_task_block_t *alloc_stat_start; ///< Pointer to the start to the user defined alloc_stat array
} heap_all_tasks_stat_t;

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

/**
 * @brief Return per-task heap memory usage and associated allocation information on each heap
 * for all tasks.
 *
 * For each task that has allocated memory from the heap, return information of memory usage and
 * allocation information of the task on each heap the task has used.
 *
 * @param tasks_stat Structure to hold the memory usage statistics of all tasks
 * (@see heap_all_tasks_stat_t).
 * @return ESP_OK if the information were gathered successfully.
 *         ESP_ERR_INVALID_ARG if the user defined field in heap_all_tasks_stat_t are not set properly
 */
esp_err_t heap_caps_get_all_task_stat(heap_all_tasks_stat_t *tasks_stat);

/**
 * @brief Return heap memory usage and associated allocation information on each heap for a given task.
 *
 * @param[in] task_handle handle of the task. If NULL, the function will get the current task
 *                        handle and return the statistics of this task.
 * @param[out] task_stat Structure to hold the memory usage statistics of the task defined by task_handle
 * @return ESP_OK if the information were gathered successfully.
 *         ESP_ERR_INVALID_ARG if the user defined field in heap_single_task_stat_t are not set properly
 */
esp_err_t heap_caps_get_single_task_stat(heap_single_task_stat_t *task_stat, TaskHandle_t task_handle);

/**
 * @brief Print heap memory usage and associated allocation information on each heap for all created tasks
 * since startup (running and deleted ones when CONFIG_HEAP_TRACK_DELETED_TASKS is enabled).
 *
 * @note This function is an alternative to heap_caps_get_all_task_stat if the goal is just to print information
 * and not manipulate them.
 *
 * @param stream The stream to dump to, if NULL then stdout is used
 */
void heap_caps_print_all_task_stat(FILE *stream);

/**
 * @brief Print summary information of all tasks
 *
 * @note The information printed by this function is an array formatted log of task_stat_t content for each running
 * task (and deleted ones if HEAP_TRACK_DELETED_TASKS is enabled)
 *
 * @param stream The stream to dump to, if NULL then stdout is used
 */
void heap_caps_print_all_task_stat_overview(FILE *stream);

/**
 * @brief Print heap memory usage and associated allocation information on each heap for a given task.
 *
 * @note This function is an alternative to heap_caps_get_single_task_stat if the goal is just to print information
 * and not manipulate them.
 *
 * @param stream The stream to dump to, if NULL then stdout is used
 * @param task_handle The task handle of the task to get memory usage and associated allocation information from.
 */
void heap_caps_print_single_task_stat(FILE *stream, TaskHandle_t task_handle);

/**
 * @brief Print summary information of a given task
 *
 * @note The information printed by this function is an array formatted log of task_stat_t content for the given
 * task. This function will not print the task summary information if the given task is deleted and
 * HEAP_TRACK_DELETED_TASKS is disabled.
 *
 * @param stream The stream to dump to, if NULL then stdout is used
 * @param task_handle The task handle of the task to get memory usage and associated allocation information from.
 */
void heap_caps_print_single_task_stat_overview(FILE *stream, TaskHandle_t task_handle);

/**
 * @brief Allocate the memory used to store the heap and alloc statistics and fill task_stat
 * with the pointer to those allocations and the number of heaps and allocs statistics available
 * for the given task.
 *
 * @note If NULL is passed as parameter for the task_handle, the information on the currently running
 * task will be returned. This function should be called prior to heap_caps_get_single_task_stat() if the user
 * wishes to use dynamic allocation to store statistics.
 *
 * @param task_handle The task from which to get the information. If NULL,
 * this function will return the number of heap used by the calling task.
 * @param task_stat Structure containing information filled by this function.
 * @return ESP_OK if the memory necessary to gather the statistics was allocated successfully.
 *         ESP_FAIL if not enough memory space is available to store all statistics.
 */
esp_err_t heap_caps_alloc_single_task_stat_arrays(heap_single_task_stat_t *task_stat, TaskHandle_t task_handle);

/**
 * @brief Free the memory allocated to store heap and alloc statistics by calling
 * heap_caps_alloc_single_task_stat_arrays.
 *
 * @param task_stat Structure from which to free the allocated memory used to store statistics
 */
void heap_caps_free_single_task_stat_arrays(heap_single_task_stat_t *task_stat);

/**
 * @brief Allocate the memory used to store the tasks, heaps and allocs statistics and fill tasks_stat
 * with the pointer to those allocations and the number of tasks, heaps and allocs statistics available.
 *
 * @note This function should be called prior to heap_caps_get_all_task_stat() if the user
 * wishes to use dynamic allocation to store statistics.
 *
 * @param tasks_stat Structure containing information filled by this function.
 * @return ESP_OK if the memory necessary to gather the statistics was allocated successfully.
 *         ESP_FAIL if not enough memory space is available to store all statistics.
 */
esp_err_t heap_caps_alloc_all_task_stat_arrays(heap_all_tasks_stat_t *tasks_stat);

/**
 * @brief Free the memory allocated to store task, heap and alloc statistics
 * by calling heap_caps_alloc_all_task_stat_arrays.
 *
 * @param tasks_stat Structure from which to free the allocated memory used to store statistics
 */
void heap_caps_free_all_task_stat_arrays(heap_all_tasks_stat_t *tasks_stat);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_HEAP_TASK_TRACKING || __DOXYGEN__
