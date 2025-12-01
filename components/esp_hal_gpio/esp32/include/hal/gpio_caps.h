/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define GPIO_CAPS_GET(_attr) _GPIO_ ## _attr

#define _GPIO_MATRIX_DELAY_NS       25      /* the gpio matrix signal routing const time */
