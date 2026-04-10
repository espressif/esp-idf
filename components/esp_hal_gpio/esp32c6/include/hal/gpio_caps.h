/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define GPIO_CAPS_GET(_attr) _GPIO_ ## _attr

#define _GPIO_ETM_EVENT_CHANNELS_PER_GROUP    8
#define _GPIO_ETM_TASK_CHANNELS_PER_GROUP     8
