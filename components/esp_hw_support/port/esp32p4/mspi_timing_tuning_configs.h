/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#define MSPI_TIMING_CONFIG_NUM_MAX          32  //This should be larger than the max available timing config num
#define MSPI_TIMING_TEST_DATA_LEN           128
#define MSPI_TIMING_PSRAM_TEST_DATA_ADDR    0x80
#define MSPI_TIMING_DELAYLINE_TEST_NUMS     100

#define MSPI_TIMING_CORE_CLOCK_DIV          1
#if CONFIG_SPIRAM_SPEED_250M
#define MSPI_TIMING_PSRAM_NEEDS_TUNING      1
#define MSPI_TIMING_MPLL_FREQ_MHZ           500
#elif CONFIG_SPIRAM_SPEED_200M
#define MSPI_TIMING_PSRAM_NEEDS_TUNING      1
#define MSPI_TIMING_MPLL_FREQ_MHZ           400
#else
#define MSPI_TIMING_MPLL_FREQ_MHZ           400
#endif

#define MSPI_TIMING_PSRAM_DTR_MODE          CONFIG_SPIRAM_MODE_HEX
