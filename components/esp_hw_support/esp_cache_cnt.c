/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Chip-agnostic implementation of the cache access counter API, built on
 * top of the cache profile counter functions of hal/cache_ll.h and the
 * unit descriptor table in soc/cache_periph.h.
 */

#include <inttypes.h>
#include "esp_cache_cnt.h"
#include "soc/soc_caps.h"

float esp_cache_cnt_miss_ratio(const esp_cache_cnt_data_t *data)
{
    const uint32_t needed = ESP_CACHE_CNT_VALID_ACCESSES | ESP_CACHE_CNT_VALID_LINE_FILLS;
    if ((data->valid_mask & needed) != needed || data->accesses == 0) {
        return 0.0f;
    }
    return (float)data->line_fills / (float)data->accesses;
}

#if SOC_CACHE_CNT_SUPPORTED

#include "hal/cache_ll.h"

size_t esp_cache_cnt_num_units(void)
{
    return SOC_CACHE_CNT_UNITS_NUM;
}

esp_err_t esp_cache_cnt_get_unit_info(size_t unit, esp_cache_cnt_unit_info_t *out)
{
    if (unit >= SOC_CACHE_CNT_UNITS_NUM || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    const cache_profile_counter_unit_t *desc = &cache_periph_profile_counter_units[unit];
    out->name = desc->name;
    out->cache_level = desc->level;
    out->core_id = desc->core_id;
    out->traffic_type = desc->traffic;
    return ESP_OK;
}

esp_err_t esp_cache_cnt_start(void)
{
    cache_ll_clear_profile_counter();
    cache_ll_enable_profile_counter(true);
    return ESP_OK;
}

esp_err_t esp_cache_cnt_stop(void)
{
    cache_ll_enable_profile_counter(false);
    return ESP_OK;
}

esp_err_t esp_cache_cnt_clear(void)
{
    cache_ll_clear_profile_counter();
    return ESP_OK;
}

esp_err_t esp_cache_cnt_get(size_t unit, esp_cache_cnt_data_t *out)
{
    if (unit >= SOC_CACHE_CNT_UNITS_NUM || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *out = (esp_cache_cnt_data_t) { 0 };
    if (cache_ll_get_profile_counter(unit, CACHE_PROFILE_COUNTER_HIT, &out->accesses)) {
        out->valid_mask |= ESP_CACHE_CNT_VALID_ACCESSES;
    }
    if (cache_ll_get_profile_counter(unit, CACHE_PROFILE_COUNTER_MISS, &out->stall_events)) {
        out->valid_mask |= ESP_CACHE_CNT_VALID_STALL_EVENTS;
    }
    if (cache_ll_get_profile_counter(unit, CACHE_PROFILE_COUNTER_CONFLICT, &out->conflicts)) {
        out->valid_mask |= ESP_CACHE_CNT_VALID_CONFLICTS;
    }
    if (cache_ll_get_profile_counter(unit, CACHE_PROFILE_COUNTER_NXTLVL_RD, &out->line_fills)) {
        out->valid_mask |= ESP_CACHE_CNT_VALID_LINE_FILLS;
    }
    if (cache_ll_get_profile_counter(unit, CACHE_PROFILE_COUNTER_NXTLVL_WR, &out->writebacks)) {
        out->valid_mask |= ESP_CACHE_CNT_VALID_WRITEBACKS;
    }
    return ESP_OK;
}

static void print_counter(FILE *out, const esp_cache_cnt_data_t *data, uint32_t flag, uint32_t value)
{
    if (data->valid_mask & flag) {
        fprintf(out, " %12" PRIu32, value);
    } else {
        fprintf(out, " %12s", "-");
    }
}

esp_err_t esp_cache_cnt_dump(FILE *out)
{
    if (out == NULL) {
        out = stdout;
    }
    fprintf(out, "%-20s %12s %12s %12s %12s %10s\n",
            "unit", "accesses", "line fills", "writebacks", "conflicts", "miss rate");
    for (size_t unit = 0; unit < SOC_CACHE_CNT_UNITS_NUM; unit++) {
        esp_cache_cnt_unit_info_t info;
        esp_cache_cnt_data_t data;
        esp_cache_cnt_get_unit_info(unit, &info);
        esp_cache_cnt_get(unit, &data);
        fprintf(out, "%-20s", info.name);
        print_counter(out, &data, ESP_CACHE_CNT_VALID_ACCESSES, data.accesses);
        print_counter(out, &data, ESP_CACHE_CNT_VALID_LINE_FILLS, data.line_fills);
        print_counter(out, &data, ESP_CACHE_CNT_VALID_WRITEBACKS, data.writebacks);
        print_counter(out, &data, ESP_CACHE_CNT_VALID_CONFLICTS, data.conflicts);
        fprintf(out, " %9.2f%%\n", 100.0f * esp_cache_cnt_miss_ratio(&data));
    }
    return ESP_OK;
}

#else // !SOC_CACHE_CNT_SUPPORTED

size_t esp_cache_cnt_num_units(void)
{
    return 0;
}

esp_err_t esp_cache_cnt_get_unit_info(size_t unit, esp_cache_cnt_unit_info_t *out)
{
    (void) unit;
    (void) out;
    return ESP_ERR_INVALID_ARG;
}

esp_err_t esp_cache_cnt_start(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_cache_cnt_stop(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_cache_cnt_clear(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_cache_cnt_get(size_t unit, esp_cache_cnt_data_t *out)
{
    (void) unit;
    (void) out;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_cache_cnt_dump(FILE *out)
{
    (void) out;
    return ESP_ERR_NOT_SUPPORTED;
}

#endif // !SOC_CACHE_CNT_SUPPORTED
