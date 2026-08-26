/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef HEART_RATE_H
#define HEART_RATE_H

/* Includes */
/* ESP APIs */
#include "esp_random.h"

/* Defines */
#define HEART_RATE_TASK_PERIOD (1000 / portTICK_PERIOD_MS)

/* Public function declarations */
uint8_t get_heart_rate(void);
void update_heart_rate(void);

#endif // HEART_RATE_H
