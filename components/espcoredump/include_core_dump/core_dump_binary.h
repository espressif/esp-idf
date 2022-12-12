/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_CORE_DUMP_BINARY_H_
#define ESP_CORE_DUMP_BINARY_H_

#include "esp_core_dump_types.h"

/**
 * @brief Initiate the binary core dump generation.
 *
 * @param info      Exception frame info generated when the panic occured.
 * @param write_cfg Structure containing the callbacks that will be called to
 *                  write the generated core dump file.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t.
 */
esp_err_t esp_core_dump_write_binary(core_dump_write_config_t *write_cfg);

#endif
