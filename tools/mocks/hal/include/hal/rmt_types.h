/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * NOTE: this is not the original header file from the hal component. It is a stripped-down copy to support mocking.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef int rmt_clock_source_t;

typedef union {
    struct {
        unsigned int duration0 : 15; /*!< Duration of level0 */
        unsigned int level0 : 1;     /*!< Level of the first part */
        unsigned int duration1 : 15; /*!< Duration of level1 */
        unsigned int level1 : 1;     /*!< Level of the second part */
    };
    unsigned int val; /*!< Equivalent unsigned value for the RMT symbol */
} rmt_symbol_word_t;

#ifdef __cplusplus
}
#endif
