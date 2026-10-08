/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef __BLE_DTM_CONFIG_H__
#define __BLE_DTM_CONFIG_H__

#include "esp_err.h"

/**
 * @brief Register BLE DTM-related console commands.
 */
esp_err_t ble_dtm_config_register_commands(void);

#endif /* __BLE_DTM_CONFIG_H__ */
