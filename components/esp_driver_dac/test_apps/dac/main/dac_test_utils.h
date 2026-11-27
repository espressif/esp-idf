/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

/* Match the sample storage type to the default voltage-output resolution. */
#if SOC_DAC_RESOLUTION > 8
typedef uint16_t dac_test_sample_t;
#else
typedef uint8_t dac_test_sample_t;
#endif
