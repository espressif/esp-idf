/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define DEDIC_GPIO_CAPS_GET(_attr) _DEDIC_GPIO_ ## _attr

#define _DEDIC_GPIO_OUT_CHANS_PER_CPU    8 /*!< 8 outward channels on each CPU core */
#define _DEDIC_GPIO_IN_CHANS_PER_CPU     8 /*!< 8 inward channels on each CPU core */
