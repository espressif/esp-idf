/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef NVS_CONSTANTS_H
#define NVS_CONSTANTS_H

#include "spi_flash_mmap.h" // for SPI_FLASH_SEC_SIZE

// constants for the NVS to be used in the regular as well as bootloader implementations

#define NVS_CONST_PSB_INIT 0x1
#define NVS_CONST_PSB_FULL 0x2
#define NVS_CONST_PSB_FREEING 0x4
#define NVS_CONST_PSB_CORRUPT 0x8

#define NVS_CONST_ESB_WRITTEN 0x1
#define NVS_CONST_ESB_ERASED 0x2

#define NVS_CONST_PAGE_SIZE SPI_FLASH_SEC_SIZE

#define NVS_CONST_ENTRY_SIZE 32
#define NVS_CONST_ENTRY_COUNT 126
#define NVS_CONST_INVALID_ENTRY 0xffffffff

#define NVS_CONST_CHUNK_MAX_SIZE (NVS_CONST_ENTRY_SIZE * (NVS_CONST_ENTRY_COUNT - 1))
#define NVS_CONST_STR_LEN_MAX_SIZE (NVS_CONST_ENTRY_SIZE * (NVS_CONST_ENTRY_COUNT - 1))

#define NVS_CONST_NS_INDEX 0
#define NVS_CONST_NS_ANY 255

#define NVS_CONST_NVS_VERSION 0xfe // Decrement to upgrade

// Page States
// All bits set, default state after flash erase. Page has not been initialized yet.
#define NVS_CONST_PAGE_STATE_UNINITIALIZED 0xffffffff

// Page is initialized, and will accept writes.
#define NVS_CONST_PAGE_STATE_ACTIVE (NVS_CONST_PAGE_STATE_UNINITIALIZED & ~NVS_CONST_PSB_INIT)

// Page is marked as full and will not accept new writes.
#define NVS_CONST_PAGE_STATE_FULL (NVS_CONST_PAGE_STATE_ACTIVE & ~NVS_CONST_PSB_FULL)

// Data is being moved from this page to a new one.
#define NVS_CONST_PAGE_STATE_FREEING (NVS_CONST_PAGE_STATE_FULL & ~NVS_CONST_PSB_FREEING)

// Page was found to be in a corrupt and unrecoverable state.
// Instead of being erased immediately, it will be kept for diagnostics and data recovery.
// It will be erased once we run out out free pages.
#define NVS_CONST_PAGE_STATE_CORRUPT (NVS_CONST_PAGE_STATE_FREEING & ~PSB_CORRUPT)

// Page object wasn't loaded from flash memory
#define NVS_CONST_PAGE_STATE_INVALID 0

// Entry States
#define NVS_CONST_ENTRY_STATE_EMPTY   0x3 // 0b11, default state after flash erase
#define NVS_CONST_ENTRY_STATE_WRITTEN (NVS_CONST_ENTRY_STATE_EMPTY & ~NVS_CONST_ESB_WRITTEN) // entry was written
#define  NVS_CONST_ENTRY_STATE_ERASED (NVS_CONST_ENTRY_STATE_WRITTEN & ~NVS_CONST_ESB_ERASED) // entry was written and then erased
#define NVS_CONST_ENTRY_STATE_ILLEGAL 0x1 // only possible if flash is inconsistent
#define NVS_CONST_ENTRY_STATE_INVALID 0x4 // entry is in inconsistent state (write started but ESB_WRITTEN has not been set yet)

// Offsets within a NVS page
#define NVS_CONST_PAGE_HEADER_OFFSET 0
#define NVS_CONST_PAGE_ENTRY_TABLE_OFFSET (NVS_CONST_PAGE_HEADER_OFFSET + 32)
#define NVS_CONST_PAGE_ENTRY_DATA_OFFSET (NVS_CONST_PAGE_ENTRY_TABLE_OFFSET + 32)

#endif
