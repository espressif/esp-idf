/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ARCH_XTENSA
#include "xtensa_context.h"
#elif CONFIG_IDF_TARGET_ARCH_RISCV
#include "riscv/rvruntime-frames.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ARCH_XTENSA
typedef XtExcFrame esp_cpu_frame_t;
#elif CONFIG_IDF_TARGET_ARCH_RISCV
typedef RvExcFrame esp_cpu_frame_t;
#endif

#ifdef __cplusplus
}
#endif
