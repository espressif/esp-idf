/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_assert.h"

#define STATIC_HAL_REG_CHECK(TAG, ENUM, VAL) ESP_STATIC_ASSERT((ENUM) == (VAL), #TAG": "#ENUM" definition no longer matches register value")
