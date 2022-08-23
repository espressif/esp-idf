/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#endif

/* Configuration macros for multi-heap */

#ifdef CONFIG_HEAP_POISONING_LIGHT
#define MULTI_HEAP_POISONING
#endif

#ifdef CONFIG_HEAP_POISONING_COMPREHENSIVE
#define MULTI_HEAP_POISONING
#define MULTI_HEAP_POISONING_SLOW
#endif
