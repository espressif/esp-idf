/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define DEDIC_GPIO_CAPS_GET(_attr) _DEDIC_GPIO_ ## _attr

#define _DEDIC_GPIO_OUT_CHANS_PER_CPU           8 /*!< 8 outward channels on each CPU core */
#define _DEDIC_GPIO_IN_CHANS_PER_CPU            8 /*!< 8 inward channels on each CPU core */
/*!< The dedicated GPIO (a.k.a. fast GPIO) is featured by some customized CPU instructions, which is always enabled */
#define _DEDIC_GPIO_CPU_PERIPH_ALWAYS_ENABLE    1
