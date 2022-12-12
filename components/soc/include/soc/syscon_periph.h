/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// ESP32-C6 ESP32-H2 TODO: IDF-5721
#if !CONFIG_IDF_TARGET_ESP32C6 && !CONFIG_IDF_TARGET_ESP32H2
#include "soc/syscon_reg.h"
#include "soc/syscon_struct.h"
#endif
