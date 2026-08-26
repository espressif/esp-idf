/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32S2
// ULP-RISC-V runs at 8.5MHz, which corresponds to 17/2 = 8.5 cycles per microsecond.
#define ULP_RISCV_CPU_FREQUENCY_HZ 8500000U
#define ULP_RISCV_CYCLES_PER_US_NUM 17U
#define ULP_RISCV_CYCLES_PER_US_DENOM 2U
#elif CONFIG_IDF_TARGET_ESP32S3
// ULP-RISC-V runs at 17.5MHz, which corresponds to 35/2 = 17.5 cycles per microsecond.
#define ULP_RISCV_CPU_FREQUENCY_HZ 17500000U
#define ULP_RISCV_CYCLES_PER_US_NUM 35U
#define ULP_RISCV_CYCLES_PER_US_DENOM 2U
#endif
