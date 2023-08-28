/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_err.h"

#include "mesh_v1.1/utils.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_cm_data_api.h"

extern int bt_mesh_comp_1_register(const void *comp);
extern int bt_mesh_models_metadata_register(const void *metadata, uint8_t metadata_page);

esp_err_t esp_ble_mesh_comp_1_register(const esp_ble_mesh_comp_1_t *comp)
{
    if (comp == NULL || comp->element_count == 0 ||
        comp->elements == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return bt_mesh_comp_1_register(comp);
}

esp_err_t esp_ble_mesh_models_metadata_register(const esp_ble_mesh_models_metadata_t *metadata,
                                                uint8_t metadata_page)
{
    if (metadata == NULL || metadata->element_count == 0 ||
        metadata->elements == NULL ||
        (metadata_page != 0 && metadata_page != 128)) {
        return ESP_ERR_INVALID_ARG;
    }

    return bt_mesh_models_metadata_register(metadata, metadata_page);
}
