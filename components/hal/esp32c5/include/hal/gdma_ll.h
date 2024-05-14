/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
#include "hal/gdma_beta3_ll.h"
#else
#include "hal/ahb_dma_ll.h"
#endif
