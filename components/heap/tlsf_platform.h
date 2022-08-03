/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ESP_PLATFORM
#include "soc/soc.h"

#if !CONFIG_SPIRAM
#define TLSF_MAX_POOL_SIZE (SOC_DIRAM_DRAM_HIGH - SOC_DIRAM_DRAM_LOW)
#else
#define TLSF_MAX_POOL_SIZE SOC_EXTRAM_DATA_SIZE
#endif
#endif

#if (TLSF_MAX_POOL_SIZE <= (256 * 1024))
#define FL_INDEX_MAX_PLATFORM 18 //Each pool can have up 256KB
#elif (TLSF_MAX_POOL_SIZE <= (512 * 1024))
#define FL_INDEX_MAX_PLATFORM 19 //Each pool can have up 512KB
#elif (TLSF_MAX_POOL_SIZE <= (1 * 1024 * 1024))
#define FL_INDEX_MAX_PLATFORM 20 //Each pool can have up 1MB
#elif (TLSF_MAX_POOL_SIZE <= (2 * 1024 * 1024))
#define FL_INDEX_MAX_PLATFORM 21 //Each pool can have up 2MB
#elif (TLSF_MAX_POOL_SIZE <= (4 * 1024 * 1024))
#define FL_INDEX_MAX_PLATFORM 22 //Each pool can have up 4MB
#elif (TLSF_MAX_POOL_SIZE <= (8 * 1024 * 1024))
#define FL_INDEX_MAX_PLATFORM 23 //Each pool can have up 8MB
#elif (TLSF_MAX_POOL_SIZE <= (16 * 1024 * 1024))
#define FL_INDEX_MAX_PLATFORM 24 //Each pool can have up 16MB
#elif (TLSF_MAX_POOL_SIZE <= (32 * 1024 * 1024))
#define FL_INDEX_MAX_PLATFORM 25 //Each pool can have up 32MB
#else
#error "Higher TLSF pool sizes should be added for this new config"
#endif

/* Include from the TLSF submodule to force TLSF_INDEX_MAX_PLATFORM to be defined
 * when the TLSF repository is compiled in the IDF environment. */
#include "tlsf_common.h"

#ifdef __cplusplus
}
#endif
