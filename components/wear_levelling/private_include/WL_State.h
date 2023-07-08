/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _WL_State_H_
#define _WL_State_H_
#include "esp_err.h"

/**
* @brief This structure is used to store current state of flash access
*
*/
#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

typedef struct ALIGNED_(32) WL_State_s {
public:
    uint32_t wl_dummy_sec_pos;             /*!< Current dummy sector position*/
    uint32_t wl_part_max_sec_pos;          /*!< Number of sectors in the partition (excluding config and state sectors) for dummy sector movement*/
    uint32_t wl_dummy_sec_move_count;      /*!< When dummy sector completes one cycle through the entire flash, wl_dummy_sector_move_count will be incremented by 1*/
    uint32_t wl_sec_erase_cycle_count;     /*!< After every 'sector erase cycle', this count will be incremented by 1*/
    uint32_t wl_max_sec_erase_cycle_count; /*!< Max wl_sector_erase_cycle_count after which block will be moved and dummy sector position record will be updated in state sector*/
    uint32_t wl_block_size;                /*!< WL partition block size*/
    uint32_t version;                      /*!< State id used to identify the version of current library implementation*/
    uint32_t wl_device_id;                 /*!< ID of current WL instance. Generated randomly when the state is first initialized*/
    uint32_t reserved[7];                  /*!< Reserved space for future use*/
    uint32_t crc32;                        /*!< CRC of structure*/
} wl_state_t;

#ifndef _MSC_VER // MSVS has different format for this define
static_assert(sizeof(wl_state_t) % 16 == 0, "Size of wl_state_t structure should be compatible with flash encryption");
#endif // _MSC_VER

#define WL_STATE_CRC_LEN_V1 offsetof(wl_state_t, wl_device_id)
#define WL_STATE_CRC_LEN_V2 offsetof(wl_state_t, crc32)

#endif // _WL_State_H_
