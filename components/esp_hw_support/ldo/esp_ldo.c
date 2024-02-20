/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "stdatomic.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "hal/ldo_ll.h"
#include "esp_private/esp_ldo.h"

static const DRAM_ATTR char TAG[] = "ldo";

typedef struct ldo_unit_ctx_t {
    int unit_id;
    int voltage_mv;
    bool enabled;
    int ref_cnt;
    portMUX_TYPE spinlock;
} ldo_unit_ctx_t;

typedef struct ldo_ctx_t {
    _lock_t mutex;
    ldo_unit_ctx_t units[LDO_LL_UNIT_NUM];
} ldo_ctx_t;

static ldo_ctx_t s_ctx;    //LDO context
//LDO1 is always reserved for Flash usage
static atomic_bool s_ldo_unit_claimed[LDO_LL_UNIT_NUM] = {ATOMIC_VAR_INIT(true),
                                                          ATOMIC_VAR_INIT(false),
                                                          ATOMIC_VAR_INIT(false),
                                                          ATOMIC_VAR_INIT(false)};

static bool s_ldo_unit_claim(uint32_t unit);
static bool s_ldo_unit_free(uint32_t unit);
static bool s_ldo_unit_needs_claim(const esp_ldo_unit_init_cfg_t *init_config);


//This API should always success
esp_ldo_unit_handle_t esp_ldo_init_unit_early(const esp_ldo_unit_init_cfg_t *init_config)
{
    assert(init_config);
    assert(ldo_ll_is_valid_ldo_id(init_config->unit_id));

    int unit_id = LDO_ID2UNIT(init_config->unit_id);
    ldo_unit_ctx_t *unit = &s_ctx.units[unit_id];
    bool needs_claim = s_ldo_unit_needs_claim(init_config);
    bool success_claim = false;

    if (needs_claim) {
        success_claim = s_ldo_unit_claim(unit_id);
        assert(success_claim);
        unit->unit_id = unit_id;
        unit->voltage_mv = init_config->cfg.voltage_mv;
        unit->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        ldo_ll_set_output_voltage_mv(unit_id, init_config->cfg.voltage_mv);

        if (init_config->flags.enable_unit) {
            ldo_ll_enable(unit_id, true);
            unit->enabled = true;
        }
    } else {
        bool same_voltage = init_config->cfg.voltage_mv == unit->voltage_mv;
        assert(same_voltage);
    }

    unit->ref_cnt++;

    return unit;
}

esp_err_t esp_ldo_init_unit(const esp_ldo_unit_init_cfg_t *init_config, esp_ldo_unit_handle_t *ret_unit)
{
    ESP_RETURN_ON_FALSE(init_config && ret_unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(ldo_ll_is_valid_ldo_id(init_config->unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid ldo id");

    int unit_id = LDO_ID2UNIT(init_config->unit_id);
    ldo_unit_ctx_t *unit = &s_ctx.units[unit_id];
    bool needs_claim = s_ldo_unit_needs_claim(init_config);
    bool success_claim = false;

    if (needs_claim) {
        success_claim = s_ldo_unit_claim(unit_id);
        ESP_RETURN_ON_FALSE(success_claim, ESP_ERR_NOT_FOUND, TAG, "ldo%d is already in use", init_config->unit_id);
        unit->unit_id = unit_id;
        unit->voltage_mv = init_config->cfg.voltage_mv;
        unit->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        ldo_ll_set_output_voltage_mv(unit_id, init_config->cfg.voltage_mv);
        if (init_config->flags.enable_unit) {
            ldo_ll_enable(unit_id, true);
            unit->enabled = true;
        }
        ESP_LOGD(TAG, "new ldo %d is created", unit->unit_id + 1);
    } else {
        bool same_voltage = init_config->cfg.voltage_mv == unit->voltage_mv;
        ESP_RETURN_ON_FALSE(same_voltage, ESP_ERR_INVALID_ARG, TAG, "not same voltage, cannot share ldo%d", init_config->unit_id);
        ESP_LOGD(TAG, "new ldo %d is shared", unit->unit_id + 1);
    }

    portENTER_CRITICAL(&unit->spinlock);
    unit->ref_cnt++;
    portEXIT_CRITICAL(&unit->spinlock);

    *ret_unit = unit;
    return ESP_OK;
}

esp_err_t esp_ldo_deinit_unit(esp_ldo_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument: uninitialized handler");
    ESP_RETURN_ON_FALSE(unit->enabled == false, ESP_ERR_INVALID_STATE, TAG, "invalid state: ldo unit is still enabled");

    bool needs_free = false;
    portENTER_CRITICAL(&unit->spinlock);
    unit->ref_cnt -= 1;
    if (unit->ref_cnt == 0) {
        needs_free = true;
    }
    portEXIT_CRITICAL(&unit->spinlock);

    if (needs_free) {
        bool success_free = false;
        success_free = s_ldo_unit_free(unit->unit_id);
        ESP_RETURN_ON_FALSE(success_free, ESP_ERR_NOT_FOUND, TAG, "ldo%d isn't in use", unit->unit_id + 1);
        ESP_LOGD(TAG, "ldo unit%d is deleted", unit->unit_id + 1);
        _lock_acquire(&s_ctx.mutex);
        memset(&s_ctx.units[unit->unit_id], 0x0, sizeof(ldo_unit_ctx_t));
        _lock_release(&s_ctx.mutex);
    }

    return ESP_OK;
}

esp_err_t esp_ldo_enable_unit(esp_ldo_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(unit->enabled == false, ESP_ERR_INVALID_STATE, TAG, "invalid state: ldo unit is enabled already");

    ldo_ll_enable(unit->unit_id, true);
    unit->enabled = true;

    return ESP_OK;
}

esp_err_t esp_ldo_disable_unit(esp_ldo_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(unit->enabled == true, ESP_ERR_INVALID_STATE, TAG, "invalid state: ldo unit is disabled already");

    ldo_ll_enable(unit->unit_id, false);
    unit->enabled = false;

    return ESP_OK;
}

esp_err_t esp_ldo_usage_dump(FILE* stream)
{
    char line[100];
    fprintf(stream, "LDOs Info:\n");
    for (int i = 0; i < LDO_LL_UNIT_NUM; i++) {
        fprintf(stream, "%-15s %-14s %-14s\n", "LDO ID", "voltage_mv", "enabled");

        char *buf = line;
        size_t len = sizeof(line);
        memset(line, 0x0, len);
        snprintf(buf, len, "%-15d 0x%-12d %-11x\n",
                 s_ctx.units[i].unit_id + 1,
                 s_ctx.units[i].voltage_mv,
                 s_ctx.units[i].enabled);
        fputs(line, stream);
    }

    fprintf(stream, "You can use a disabled LDO unit, or share an enabled LDO unit\n");

    return ESP_OK;
}

static bool s_ldo_unit_claim(uint32_t unit)
{
    bool false_var = false;
    return atomic_compare_exchange_strong(&s_ldo_unit_claimed[unit], &false_var, true);
}

static bool s_ldo_unit_free(uint32_t unit)
{
    bool true_var = true;
    return atomic_compare_exchange_strong(&s_ldo_unit_claimed[unit], &true_var, false);
}

static bool s_ldo_unit_needs_claim(const esp_ldo_unit_init_cfg_t *init_config)
{
    bool needs_claim = false;
    int unit_id = LDO_ID2UNIT(init_config->unit_id);

    if (s_ctx.units[unit_id].ref_cnt == 0) {
        needs_claim = true;
    } else {
        if (!init_config->flags.shared_ldo) {
            needs_claim = true;
        }
    }

    return needs_claim;
}
