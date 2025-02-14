/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// TinyUSB Port selection
//
#if (CONFIG_IDF_TARGET_ESP32P4)
#define CONFIG_TINYUSB_RHPORT_HS
#endif // CONFIG_IDF_TARGET_ESP32P4

//
// TinyUSB Debug
//
#define CFG_TUSB_DEBUG              CONFIG_TINYUSB_DEBUG_LEVEL

//
// TinyUSB Peripheral Port selection (build-time)
//
#ifdef CONFIG_TINYUSB_RHPORT_HS
#   define CFG_TUSB_RHPORT1_MODE    OPT_MODE_DEVICE | OPT_MODE_HIGH_SPEED
#else
#   define CFG_TUSB_RHPORT0_MODE    OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED
#endif

//
// TinyUSB OS
//
#define CFG_TUSB_OS                 OPT_OS_FREERTOS

//
// TinyUSB specific memory attributes
//

//
// DCD DWC2 Mode
//
#define CFG_TUD_DWC2_SLAVE_ENABLE   0       // We don't use Slave/IRQ mode in this test

//
// DMA & Cache
//
// DMA Mode has a priority over Slave/IRQ mode and will be used if hardware supports it
#define CFG_TUD_DWC2_DMA_ENABLE     1       // Enable DMA

#if CONFIG_CACHE_L1_CACHE_LINE_SIZE
// To enable the dcd_dcache clean/invalidate/clean_invalidate calls
#   define CFG_TUD_MEM_DCACHE_ENABLE    1
#define CFG_TUD_MEM_DCACHE_LINE_SIZE    CONFIG_CACHE_L1_CACHE_LINE_SIZE
// NOTE: starting with esp-idf v5.3 there is specific attribute present: DRAM_DMA_ALIGNED_ATTR
#   define CFG_TUSB_MEM_SECTION         __attribute__((aligned(CONFIG_CACHE_L1_CACHE_LINE_SIZE))) DRAM_ATTR
#else
#   define CFG_TUD_MEM_CACHE_ENABLE     0
#   define CFG_TUSB_MEM_SECTION         TU_ATTR_ALIGNED(4) DRAM_ATTR
#endif // CONFIG_CACHE_L1_CACHE_LINE_SIZE

//
// TinyUSB DEVICE CONFIGURATION
//
#define CFG_TUD_ENABLED             1   // Enable Device stack

#ifdef __cplusplus
}
#endif
