// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_GPIO_STRUCT_H_
#define _SOC_GPIO_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    uint32_t bt_select;                             /**/
    uint32_t out;                                   /**/
    uint32_t out_w1ts;                              /**/
    uint32_t out_w1tc;                              /**/
    union {
        struct {
            uint32_t data:      22;
            uint32_t reserved22:10;
        };
        uint32_t val;
    } out1;
    union {
        struct {
            uint32_t data:      22;
            uint32_t reserved22:10;
        };
        uint32_t val;
    } out1_w1ts;
    union {
        struct {
            uint32_t data:      22;
            uint32_t reserved22:10;
        };
        uint32_t val;
    } out1_w1tc;
    union {
        struct {
            uint32_t sel:        8;
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } sdio_select;
    uint32_t enable;                                /**/
    uint32_t enable_w1ts;                           /**/
    uint32_t enable_w1tc;                           /**/
    union {
        struct {
            uint32_t data:       22;
            uint32_t reserved22: 10;
        };
        uint32_t val;
    } enable1;
    union {
        struct {
            uint32_t data:       22;
            uint32_t reserved22: 10;
        };
        uint32_t val;
    } enable1_w1ts;
    union {
        struct {
            uint32_t data:       22;
            uint32_t reserved22: 10;
        };
        uint32_t val;
    } enable1_w1tc;
    union {
        struct {
            uint32_t strapping: 16;
            uint32_t reserved16:16;
        };
        uint32_t val;
    } strap;
    uint32_t in;                                    /**/
    union {
        struct {
            uint32_t data:        22;
            uint32_t reserved22:  10;
        };
        uint32_t val;
    } in1;
    uint32_t status;                                /**/
    uint32_t status_w1ts;                           /**/
    uint32_t status_w1tc;                           /**/
    union {
        struct {
            uint32_t intr_st:         22;
            uint32_t reserved22:      10;
        };
        uint32_t val;
    } status1;
    union {
        struct {
            uint32_t intr_st:         22;
            uint32_t reserved22:      10;
        };
        uint32_t val;
    } status1_w1ts;
    union {
        struct {
            uint32_t intr_st:         22;
            uint32_t reserved22:      10;
        };
        uint32_t val;
    } status1_w1tc;
    uint32_t pcpu_int;                              /**/
    uint32_t pcpu_nmi_int;                          /**/
    uint32_t cpusdio_int;                           /**/
    union {
        struct {
            uint32_t intr:      22;
            uint32_t reserved22:10;
        };
        uint32_t val;
    } pcpu_int1;
    union {
        struct {
            uint32_t intr:          22;
            uint32_t reserved22:    10;
        };
        uint32_t val;
    } pcpu_nmi_int1;
    union {
        struct {
            uint32_t intr:      22;
            uint32_t reserved22:10;
        };
        uint32_t val;
    } cpusdio_int1;
    union {
        struct {
            uint32_t sync2_bypass:       2;
            uint32_t pad_driver:         1;
            uint32_t sync1_bypass:       2;
            uint32_t reserved5:          2;
            uint32_t int_type:           3;
            uint32_t wakeup_enable:      1;
            uint32_t config:             2;
            uint32_t int_ena:            5;
            uint32_t reserved18:        14;
        };
        uint32_t val;
    } pin[54];
    uint32_t status_next;                             /**/
    union {
        struct {
            uint32_t intr_st_next:         22;
            uint32_t reserved22:           10;
        };
        uint32_t val;
    } status_next1;
    union {
        struct {
            uint32_t func_sel:         6;
            uint32_t sig_in_inv:       1;
            uint32_t sig_in_sel:       1;
            uint32_t reserved8:       24;
        };
        uint32_t val;
    } func_in_sel_cfg[256];
    union {
        struct {
            uint32_t func_sel:          9;
            uint32_t inv_sel:           1;
            uint32_t oen_sel:           1;
            uint32_t oen_inv_sel:       1;
            uint32_t reserved12:       20;
        };
        uint32_t val;
    } func_out_sel_cfg[54];
    union {
        struct {
            uint32_t clk_en:     1;
            uint32_t reserved1: 31;
        };
        uint32_t val;
    } clock_gate;
    uint32_t reserved_630;
    uint32_t reserved_634;
    uint32_t reserved_638;
    uint32_t reserved_63c;
    uint32_t reserved_640;
    uint32_t reserved_644;
    uint32_t reserved_648;
    uint32_t reserved_64c;
    uint32_t reserved_650;
    uint32_t reserved_654;
    uint32_t reserved_658;
    uint32_t reserved_65c;
    uint32_t reserved_660;
    uint32_t reserved_664;
    uint32_t reserved_668;
    uint32_t reserved_66c;
    uint32_t reserved_670;
    uint32_t reserved_674;
    uint32_t reserved_678;
    uint32_t reserved_67c;
    uint32_t reserved_680;
    uint32_t reserved_684;
    uint32_t reserved_688;
    uint32_t reserved_68c;
    uint32_t reserved_690;
    uint32_t reserved_694;
    uint32_t reserved_698;
    uint32_t reserved_69c;
    uint32_t reserved_6a0;
    uint32_t reserved_6a4;
    uint32_t reserved_6a8;
    uint32_t reserved_6ac;
    uint32_t reserved_6b0;
    uint32_t reserved_6b4;
    uint32_t reserved_6b8;
    uint32_t reserved_6bc;
    uint32_t reserved_6c0;
    uint32_t reserved_6c4;
    uint32_t reserved_6c8;
    uint32_t reserved_6cc;
    uint32_t reserved_6d0;
    uint32_t reserved_6d4;
    uint32_t reserved_6d8;
    uint32_t reserved_6dc;
    uint32_t reserved_6e0;
    uint32_t reserved_6e4;
    uint32_t reserved_6e8;
    uint32_t reserved_6ec;
    uint32_t reserved_6f0;
    uint32_t reserved_6f4;
    uint32_t reserved_6f8;
    union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } date;
} gpio_dev_t;
extern gpio_dev_t GPIO;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_GPIO_STRUCT_H_ */
