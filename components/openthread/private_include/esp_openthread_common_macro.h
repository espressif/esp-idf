/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define OT_PLAT_LOG_TAG "OPENTHREAD"

#ifndef MS_PER_S
#define MS_PER_S 1000
#endif

#ifndef US_PER_MS
#define US_PER_MS 1000
#endif

#ifndef US_PER_S
#define US_PER_S (MS_PER_S * US_PER_MS)
#endif

#define ESP_OPENTHREAD_UART_BUFFER_SIZE CONFIG_OPENTHREAD_UART_BUFFER_SIZE
