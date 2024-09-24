/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpio_sd_dev_t {
    volatile union {
        struct {
            uint32_t duty:        8;
            uint32_t prescale:    8;
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } channel[8];
    volatile union {
        struct {
            uint32_t reserved0: 31;
            uint32_t clk_en:     1;
        };
        uint32_t val;
    } cg;
    volatile union {
        struct {
            uint32_t reserved0: 31;
            uint32_t spi_swap:   1;
        };
        uint32_t val;
    } misc;
    volatile union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } version;
} gpio_sd_dev_t;

extern gpio_sd_dev_t SDM;

#ifdef __cplusplus
}
#endif
