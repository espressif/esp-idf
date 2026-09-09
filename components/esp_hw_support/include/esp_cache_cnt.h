/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "esp_err.h"
#include "hal/cache_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Cache access counter API.
 *
 * The set of counters differs between chips: the number of cache levels, the
 * number of request buses per cache, and which counters exist per bus all
 * vary. Instead of a fixed list of caches, the API exposes a chip-defined
 * list of counter "units". Each unit is one set of counters observing one
 * traffic stream (e.g. "instruction fetches from core 0 into the L1 cache").
 * Applications enumerate the units at runtime with esp_cache_cnt_num_units()
 * and esp_cache_cnt_get_unit_info(), so they keep working when a new chip
 * adds or removes units.
 */

/**
 * @brief Description of one counter unit.
 */
typedef struct {
    const char *name;                      /*!< Short human-readable name, e.g. "l1-icache-core0" */
    uint8_t cache_level;                   /*!< Cache level the counters belong to, counting from the CPU.
                                                On most chips there is a single level (the flash/PSRAM cache);
                                                on the ESP32-P4, level 1 is the L1 cache in front of internal
                                                memory and level 2 is the flash/PSRAM cache. */
    cache_profile_traffic_t traffic_type;  /*!< Kind of traffic observed */
    int8_t core_id;                        /*!< Core the traffic originates from, or -1 if unknown/mixed */
} esp_cache_cnt_unit_info_t;

/**
 * @name Flags for esp_cache_cnt_data_t::valid_mask
 *
 * Not every counter exists for every unit (e.g. instruction buses have no
 * write-back counter). A field of esp_cache_cnt_data_t is only meaningful if
 * the corresponding flag is set.
 * @{
 */
#define ESP_CACHE_CNT_VALID_ACCESSES      (1 << 0)
#define ESP_CACHE_CNT_VALID_STALL_EVENTS  (1 << 1)
#define ESP_CACHE_CNT_VALID_CONFLICTS     (1 << 2)
#define ESP_CACHE_CNT_VALID_LINE_FILLS    (1 << 3)
#define ESP_CACHE_CNT_VALID_WRITEBACKS    (1 << 4)
/** @} */

/**
 * @brief Counter values for one unit.
 *
 * Note that the hardware "hit" and "miss" counters do not directly hold the
 * number of hit and missed accesses:
 * - accesses: the hit counter increments once for every access that completes,
 *   whether or not it had to wait for a line fill first.
 * - stall_events: the miss counter increments repeatedly while an access is
 *   stalled on a miss, so it grows roughly with the total miss latency. This is
 *   only useful as a relative measure.
 * - line_fills: the next-level read counter increments once per line fetched
 *   from the next level, so it is the true miss count.
 *
 * The miss ratio of a cache is therefore line_fills / accesses.
 */
typedef struct {
    uint32_t valid_mask;    /*!< Bitwise OR of ESP_CACHE_CNT_VALID_* flags for the fields below */
    uint32_t accesses;      /*!< Completed accesses (hardware hit counter) */
    uint32_t stall_events;  /*!< Miss stall events; grows with total miss latency,
                                 NOT the number of missed accesses */
    uint32_t conflicts;     /*!< Conflicts between requesters on this cache */
    uint32_t line_fills;    /*!< Lines fetched from the next level (true miss count) */
    uint32_t writebacks;    /*!< Lines written back to the next level. Only present for data
                                 traffic on chips with a write-back cache (PSRAM support,
                                 see SOC_CACHE_WRITEBACK_SUPPORTED). */
} esp_cache_cnt_data_t;

/**
 * @brief Number of counter units on this chip.
 *
 * @return Number of units; 0 if the chip has no cache access counters.
 */
size_t esp_cache_cnt_num_units(void);

/**
 * @brief Get the description of a counter unit.
 *
 * @param unit      Unit index, 0 to esp_cache_cnt_num_units() - 1
 * @param[out] out  Unit description
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if unit is out of range or out is NULL
 */
esp_err_t esp_cache_cnt_get_unit_info(size_t unit, esp_cache_cnt_unit_info_t *out);

/**
 * @brief Clear and enable all cache access counters.
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_NOT_SUPPORTED if the target has no cache access counters
 */
esp_err_t esp_cache_cnt_start(void);

/**
 * @brief Disable all cache access counters. Counter values are retained.
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_NOT_SUPPORTED if the target has no cache access counters
 */
esp_err_t esp_cache_cnt_stop(void);

/**
 * @brief Reset all cache access counters to zero. Counting state is not changed.
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_NOT_SUPPORTED if the target has no cache access counters
 */
esp_err_t esp_cache_cnt_clear(void);

/**
 * @brief Read the current counter values for the given unit.
 *
 * @param unit      Unit index, 0 to esp_cache_cnt_num_units() - 1
 * @param[out] out  Counter values
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if unit is out of range or out is NULL
 *  - ESP_ERR_NOT_SUPPORTED if the target has no cache access counters
 */
esp_err_t esp_cache_cnt_get(size_t unit, esp_cache_cnt_data_t *out);

/**
 * @brief Miss ratio (0.0 to 1.0) computed from a set of counter values.
 *
 * @param data  Counter values obtained from esp_cache_cnt_get()
 *
 * @return Miss ratio; 0.0 if the unit does not provide the counters needed
 *         to compute it.
 */
float esp_cache_cnt_miss_ratio(const esp_cache_cnt_data_t *data);

/**
 * @brief Print a table with the current values of all cache access counters.
 *
 * @param out  Output stream; if NULL, print to stdout
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_NOT_SUPPORTED if the target has no cache access counters
 */
esp_err_t esp_cache_cnt_dump(FILE *out);

#ifdef __cplusplus
}
#endif
