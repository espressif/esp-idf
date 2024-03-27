/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE                  (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE                  (1) // ROM CRC library supports Big Endian
#define ESP_ROM_HAS_JPEG_DECODE             (1) // ROM has JPEG decode library
#define ESP_ROM_UART_CLK_IS_XTAL            (1) // UART clock source is selected to XTAL in ROM
#define ESP_ROM_USB_SERIAL_DEVICE_NUM       (3) // UART uses USB_SERIAL_JTAG port in ROM.
#define ESP_ROM_HAS_RETARGETABLE_LOCKING    (1) // ROM was built with retargetable locking
#define ESP_ROM_GET_CLK_FREQ                (1) // Get clk frequency with rom function `ets_get_cpu_frequency`
#define ESP_ROM_HAS_RVFPLIB                 (1) // ROM has the rvfplib
#define ESP_ROM_HAS_HAL_WDT                 (1) // ROM has the implementation of Watchdog HAL driver
#define ESP_ROM_HAS_HAL_SYSTIMER            (1) // ROM has the implementation of Systimer HAL driver
#define ESP_ROM_HAS_HEAP_TLSF               (1) // ROM has the implementation of the tlsf and multi-heap library
#define ESP_ROM_TLSF_CHECK_PATCH            (1) // ROM does not contain the patch of tlsf_check_pool()
#define ESP_ROM_MULTI_HEAP_WALK_PATCH       (1) // ROM does not contain the patch of multi_heap_walk()
#define ESP_ROM_HAS_LAYOUT_TABLE            (1) // ROM has the layout table
#define ESP_ROM_HAS_SPI_FLASH               (1) // ROM has the implementation of SPI Flash driver
#define ESP_ROM_HAS_REGI2C_BUG              (1) // ROM has the regi2c bug
#define ESP_ROM_HAS_NEWLIB                  (1) // ROM has newlib (at least parts of it) functions included
#define ESP_ROM_HAS_NEWLIB_NORMAL_FORMAT    (1) // ROM has the newlib normal/full version of formatting functions (as opposed to the nano versions)
#define ESP_ROM_REV0_HAS_NO_ECDSA_INTERFACE (1) // ECO 0 does not have ets_ecdsa_verify symbol, future revision will have it
#define ESP_ROM_WDT_INIT_PATCH              (1) // ROM version does not configure the clock
#define ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE    (1) // ROM needs to set cache MMU size according to instruction and rodata for flash mmap
#define ESP_ROM_RAM_APP_NEEDS_MMU_INIT      (1) // ROM doesn't init cache MMU when it's a RAM APP, needs MMU hal to init
#define ESP_ROM_HAS_SW_FLOAT                (1) // ROM has libgcc software floating point emulation functions
#define ESP_ROM_USB_OTG_NUM                 (-1) // No USB_OTG CDC in the ROM, set -1 for Kconfig usage.
#define ESP_ROM_HAS_VERSION                 (1) // ROM has version/eco information
#define ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB  (1) // ROM supports the HP core to jump to the RTC memory to execute stub code after waking up from deepsleep.
