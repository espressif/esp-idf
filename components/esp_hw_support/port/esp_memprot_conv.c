/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_memprot_err.h"
#include "hal/memprot_types.h"
#include "esp_memprot_types.h"

esp_err_t esp_mprot_ll_err_to_esp_err(const memprot_hal_err_t err)
{
    switch (err) {
    case MEMP_HAL_OK: return ESP_OK;
    case MEMP_HAL_ERR_SPLIT_ADDR_OUT_OF_RANGE: return ESP_ERR_MEMPROT_SPLIT_ADDR_OUT_OF_RANGE;
    case MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED: return ESP_ERR_MEMPROT_SPLIT_ADDR_UNALIGNED;
    case MEMP_HAL_ERR_UNI_BLOCK_INVALID: return ESP_ERR_MEMPROT_UNIMGMT_BLOCK_INVALID;
    case MEMP_HAL_ERR_WORLD_INVALID: return ESP_ERR_MEMPROT_WORLD_INVALID;
    case MEMP_HAL_ERR_AREA_INVALID: return ESP_ERR_MEMPROT_AREA_INVALID;
    default:
        return ESP_FAIL;
    }
}

esp_mprot_pms_world_t esp_mprot_ll_world_to_hl_world(const memprot_hal_world_t world)
{
    switch (world) {
    case MEMP_HAL_WORLD_NONE: return MEMPROT_PMS_WORLD_NONE;
    case MEMP_HAL_WORLD_0: return MEMPROT_PMS_WORLD_0;
    case MEMP_HAL_WORLD_1: return MEMPROT_PMS_WORLD_1;
    default:
        return MEMPROT_PMS_WORLD_INVALID;
    }
}

const char *esp_mprot_oper_type_to_str(const uint32_t oper_type)
{
    switch (oper_type) {
    case MEMPROT_OP_NONE: return "NONE";
    case MEMPROT_OP_READ: return "READ";
    case MEMPROT_OP_WRITE: return "WRITE";
    case MEMPROT_OP_EXEC: return "EXEC";
    default: return "INVALID";
    }
}

const char *esp_mprot_pms_world_to_str(const esp_mprot_pms_world_t world_type)
{
    switch (world_type) {
    case MEMPROT_PMS_WORLD_NONE:
        return "PMS_WORLD_NONE";
    case MEMPROT_PMS_WORLD_0:
        return "PMS_WORLD_0";
    case MEMPROT_PMS_WORLD_1:
        return "PMS_WORLD_1";
    case MEMPROT_PMS_WORLD_2:
        return "PMS_WORLD_2";
    case MEMPROT_PMS_WORLD_ALL:
        return "PMS_WORLD_ALL";
    default:
        return "PMS_WORLD_INVALID";
    }
}
