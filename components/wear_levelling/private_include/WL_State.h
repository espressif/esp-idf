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
    uint32_t pos;           /*!< current dummy block position*/
    uint32_t max_pos;       /*!< maximum amount of positions*/
    uint32_t move_count;    /*!< total amount of move counts. Used to calculate the address*/
    uint32_t access_count;  /*!< current access count*/
    uint32_t max_count;     /*!< max access count when block will be moved*/
    uint32_t block_size;    /*!< size of move block*/
    uint32_t version;       /*!< state id used to identify the version of current library implementation*/
    uint32_t device_id;     /*!< ID of current WL instance*/
    uint32_t reserved[7];   /*!< Reserved space for future use*/
    uint32_t crc;           /*!< CRC of structure*/
} wl_state_t;

#ifndef _MSC_VER // MSVS has different format for this define
static_assert(sizeof(wl_state_t) % 16 == 0, "Size of wl_state_t structure should be compatible with flash encryption");
#endif // _MSC_VER

#define WL_STATE_CRC_LEN_V1 offsetof(wl_state_t, device_id)
#define WL_STATE_CRC_LEN_V2 offsetof(wl_state_t, crc)

#endif // _WL_State_H_
