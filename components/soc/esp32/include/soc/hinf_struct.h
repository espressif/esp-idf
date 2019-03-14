// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_HINF_STRUCT_H_
#define _SOC_HINF_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t user_id_fn1:  16;
            uint32_t device_id_fn1:16;
        };
        uint32_t val;
    } cfg_data0;
    union {
        struct {
            uint32_t sdio_enable:      1;
            uint32_t sdio_ioready1:    1;
            uint32_t highspeed_enable: 1;
            uint32_t highspeed_mode:   1;
            uint32_t sdio_cd_enable:   1;
            uint32_t sdio_ioready2:    1;
            uint32_t sdio_int_mask:    1;
            uint32_t ioenable2:        1;
            uint32_t cd_disable:       1;
            uint32_t func1_eps:        1;
            uint32_t emp:              1;
            uint32_t ioenable1:        1;
            uint32_t sdio20_conf0:     4;
            uint32_t sdio_ver:        12;
            uint32_t func2_eps:        1;
            uint32_t sdio20_conf1:     3;
        };
        uint32_t val;
    } cfg_data1;
    uint32_t reserved_8;
    uint32_t reserved_c;
    uint32_t reserved_10;
    uint32_t reserved_14;
    uint32_t reserved_18;
    union {
        struct {
            uint32_t pin_state:     8;
            uint32_t chip_state:    8;
            uint32_t sdio_rst:      1;
            uint32_t sdio_ioready0: 1;
            uint32_t reserved18:   14;
        };
        uint32_t val;
    } cfg_data7;
    uint32_t cis_conf0;                     /**/
    uint32_t cis_conf1;                     /**/
    uint32_t cis_conf2;                     /**/
    uint32_t cis_conf3;                     /**/
    uint32_t cis_conf4;                     /**/
    uint32_t cis_conf5;                     /**/
    uint32_t cis_conf6;                     /**/
    uint32_t cis_conf7;                     /**/
    union {
        struct {
            uint32_t user_id_fn2:  16;
            uint32_t device_id_fn2:16;
        };
        uint32_t val;
    } cfg_data16;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t date;                          /**/
} hinf_dev_t;
extern hinf_dev_t HINF;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_HINF_STRUCT_H_ */
