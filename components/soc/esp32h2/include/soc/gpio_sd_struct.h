// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_GPIO_SD_STRUCT_H_
#define _SOC_GPIO_SD_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct gpio_sd_dev_s {
    union {
        struct {
            uint32_t duty:         8;
            uint32_t prescale:     8;
            uint32_t reserved16:  16;
        };
        uint32_t val;
    } channel[4];
    uint32_t reserved_10;
    uint32_t reserved_14;
    uint32_t reserved_18;
    uint32_t reserved_1c;
    union {
        struct {
            uint32_t reserved0: 31;
            uint32_t clk_en:     1;
        };
        uint32_t val;
    } cg;
    union {
        struct {
            uint32_t reserved0:      30;
            uint32_t function_clk_en: 1;
            uint32_t spi_swap:        1;
        };
        uint32_t val;
    } misc;
    union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } version;
} gpio_sd_dev_t;
extern gpio_sd_dev_t SIGMADELTA;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_GPIO_SD_STRUCT_H_ */
