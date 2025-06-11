/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdint.h>
#include <stdbool.h>

/* --- cte type --- */
#define BLE_CTE_TYPE_AOA                        (0)
#define BLE_CTE_TYPE_AOD_1US                    (1)
#define BLE_CTE_TYPE_AOD_2US                    (2)

extern uint8_t antenna_use_gpio[];
extern const char direction_finding_logo[];

int ble_direction_finding_antenna_init(uint8_t* gpio_array,uint8_t gpio_array_len);
