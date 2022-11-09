/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "esp_fault.h"
#include "esp_attr.h"
#include "esp_memprot_err.h"
#include "hal/memprot_types.h"
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"

esp_err_t esp_mprot_set_split_addr(const esp_mprot_mem_t mem_type, const esp_mprot_split_addr_t line_type, const void *line_addr)
{
    return ESP_OK;
}

esp_err_t esp_mprot_get_split_addr(const esp_mprot_mem_t mem_type, const esp_mprot_split_addr_t line_type, void **line_addr)
{
    if (line_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *line_addr = NULL;

    return ESP_OK;
}

esp_err_t esp_mprot_get_default_main_split_addr(const esp_mprot_mem_t mem_type, void **def_split_addr)
{
    if (def_split_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *def_split_addr = NULL;

    return ESP_OK;
}

esp_err_t esp_mprot_set_split_addr_lock(const esp_mprot_mem_t mem_type)
{
    return ESP_OK;
}

esp_err_t esp_mprot_get_split_addr_lock(const esp_mprot_mem_t mem_type, bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *locked = false;

    return ESP_OK;
}

esp_err_t esp_mprot_set_pms_lock(const esp_mprot_mem_t mem_type)
{
    return ESP_OK;
}

esp_err_t esp_mprot_get_pms_lock(const esp_mprot_mem_t mem_type, bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *locked = false;

    return ESP_OK;
}

esp_err_t esp_mprot_set_pms_area(const esp_mprot_pms_area_t area_type, const uint32_t flags)
{
    return ESP_OK;
}

esp_err_t esp_mprot_get_pms_area(const esp_mprot_pms_area_t area_type, uint32_t *flags)
{
    if (flags == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *flags = MEMPROT_OP_NONE;

    return ESP_OK;
}

esp_err_t esp_mprot_set_monitor_lock(const esp_mprot_mem_t mem_type)
{
    return ESP_OK;
}

esp_err_t esp_mprot_get_monitor_lock(const esp_mprot_mem_t mem_type, bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *locked = false;

    return ESP_OK;
}

esp_err_t esp_mprot_set_monitor_en(const esp_mprot_mem_t mem_type, const bool enable)
{
    return ESP_OK;
}

esp_err_t esp_mprot_get_monitor_en(const esp_mprot_mem_t mem_type, bool *enabled)
{
    if (enabled == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *enabled = false;

    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_monitor_clear_intr(const esp_mprot_mem_t mem_type, const int core __attribute__((unused)))
{
    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_get_active_intr(esp_memp_intr_source_t *active_memp_intr)
{
    if (active_memp_intr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    active_memp_intr->mem_type = MEMPROT_TYPE_NONE;
    active_memp_intr->core = -1;

    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_is_conf_locked_any(bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *locked = false;

    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_is_intr_ena_any(bool *enabled)
{
    if (enabled == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *enabled = false;

    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_get_violate_addr(const esp_mprot_mem_t mem_type, void **fault_addr, const int core __attribute__((unused)))
{
    if (fault_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *fault_addr = NULL;

    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_get_violate_world(const esp_mprot_mem_t mem_type, esp_mprot_pms_world_t *world, const int core __attribute__((unused)))
{
    if (world == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *world = MEMPROT_PMS_WORLD_NONE;

    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_mprot_get_violate_operation(const esp_mprot_mem_t mem_type, uint32_t *oper, const int core __attribute__((unused)))
{
    if (oper == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *oper = MEMPROT_OP_NONE;

    return ESP_OK;
}

bool IRAM_ATTR esp_mprot_has_byte_enables(const esp_mprot_mem_t mem_type)
{
    return false;
}

esp_err_t IRAM_ATTR esp_mprot_get_violate_byte_enables(const esp_mprot_mem_t mem_type, uint32_t *byte_en, const int core __attribute__((unused)))
{
    if (byte_en == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *byte_en = 0;

    return ESP_OK;
}

esp_err_t esp_mprot_set_prot(const esp_memp_config_t *memp_config)
{
    return ESP_OK;
}
