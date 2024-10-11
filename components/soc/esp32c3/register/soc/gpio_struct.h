/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_GPIO_STRUCT_H_
#define _SOC_GPIO_STRUCT_H_
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct gpio_dev_s {
    uint32_t bt_select;                               /**/
    union {
        struct {
            uint32_t data:         26;
            uint32_t reserved26:    6;
        };
        uint32_t val;
    } out;
    union {
        struct {
            uint32_t out_w1ts:  26;
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } out_w1ts;
    union {
        struct {
            uint32_t out_w1tc:  26;
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } out_w1tc;
    uint32_t reserved_10;
    uint32_t reserved_14;
    uint32_t reserved_18;
    union {
        struct {
            uint32_t sel:        8;
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } sdio_select;
    union {
        struct {
            uint32_t data:       26;
            uint32_t reserved26:  6;
        };
        uint32_t val;
    } enable;
    union {
        struct {
            uint32_t enable_w1ts:26;
            uint32_t reserved26:  6;
        };
        uint32_t val;
    } enable_w1ts;
    union {
        struct {
            uint32_t enable_w1tc:26;
            uint32_t reserved26:  6;
        };
        uint32_t val;
    } enable_w1tc;
    uint32_t reserved_2c;
    uint32_t reserved_30;
    uint32_t reserved_34;
    union {
        struct {
            uint32_t strapping: 16;
            uint32_t reserved16:16;
        };
        uint32_t val;
    } strap;
    union {
        struct {
            uint32_t data:        26;
            uint32_t reserved26:   6;
        };
        uint32_t val;
    } in;
    uint32_t reserved_40;
    union {
        struct {
            uint32_t intr_st:         26;
            uint32_t reserved26:       6;
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t status_w1ts:26;
            uint32_t reserved26:  6;
        };
        uint32_t val;
    } status_w1ts;
    union {
        struct {
            uint32_t status_w1tc:26;
            uint32_t reserved26:  6;
        };
        uint32_t val;
    } status_w1tc;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    union {
        struct {
            uint32_t intr:      26;
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } pcpu_int;
    union {
        struct {
            uint32_t intr:          26;
            uint32_t reserved26:     6;
        };
        uint32_t val;
    } pcpu_nmi_int;
    union {
        struct {
            uint32_t intr:      26;
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } cpusdio_int;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
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
    } pin[26];
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
    uint32_t reserved_100;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    union {
        struct {
            uint32_t intr_st_next:         26;
            uint32_t reserved26:            6;
        };
        uint32_t val;
    } status_next;
    uint32_t reserved_150;
    union {
        struct {
            uint32_t func_sel:         5;
            uint32_t sig_in_inv:       1;
            uint32_t sig_in_sel:       1;
            uint32_t reserved7:       25;
        };
        uint32_t val;
    } func_in_sel_cfg[128];
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    uint32_t reserved_3fc;
    uint32_t reserved_400;
    uint32_t reserved_404;
    uint32_t reserved_408;
    uint32_t reserved_40c;
    uint32_t reserved_410;
    uint32_t reserved_414;
    uint32_t reserved_418;
    uint32_t reserved_41c;
    uint32_t reserved_420;
    uint32_t reserved_424;
    uint32_t reserved_428;
    uint32_t reserved_42c;
    uint32_t reserved_430;
    uint32_t reserved_434;
    uint32_t reserved_438;
    uint32_t reserved_43c;
    uint32_t reserved_440;
    uint32_t reserved_444;
    uint32_t reserved_448;
    uint32_t reserved_44c;
    uint32_t reserved_450;
    uint32_t reserved_454;
    uint32_t reserved_458;
    uint32_t reserved_45c;
    uint32_t reserved_460;
    uint32_t reserved_464;
    uint32_t reserved_468;
    uint32_t reserved_46c;
    uint32_t reserved_470;
    uint32_t reserved_474;
    uint32_t reserved_478;
    uint32_t reserved_47c;
    uint32_t reserved_480;
    uint32_t reserved_484;
    uint32_t reserved_488;
    uint32_t reserved_48c;
    uint32_t reserved_490;
    uint32_t reserved_494;
    uint32_t reserved_498;
    uint32_t reserved_49c;
    uint32_t reserved_4a0;
    uint32_t reserved_4a4;
    uint32_t reserved_4a8;
    uint32_t reserved_4ac;
    uint32_t reserved_4b0;
    uint32_t reserved_4b4;
    uint32_t reserved_4b8;
    uint32_t reserved_4bc;
    uint32_t reserved_4c0;
    uint32_t reserved_4c4;
    uint32_t reserved_4c8;
    uint32_t reserved_4cc;
    uint32_t reserved_4d0;
    uint32_t reserved_4d4;
    uint32_t reserved_4d8;
    uint32_t reserved_4dc;
    uint32_t reserved_4e0;
    uint32_t reserved_4e4;
    uint32_t reserved_4e8;
    uint32_t reserved_4ec;
    uint32_t reserved_4f0;
    uint32_t reserved_4f4;
    uint32_t reserved_4f8;
    uint32_t reserved_4fc;
    uint32_t reserved_500;
    uint32_t reserved_504;
    uint32_t reserved_508;
    uint32_t reserved_50c;
    uint32_t reserved_510;
    uint32_t reserved_514;
    uint32_t reserved_518;
    uint32_t reserved_51c;
    uint32_t reserved_520;
    uint32_t reserved_524;
    uint32_t reserved_528;
    uint32_t reserved_52c;
    uint32_t reserved_530;
    uint32_t reserved_534;
    uint32_t reserved_538;
    uint32_t reserved_53c;
    uint32_t reserved_540;
    uint32_t reserved_544;
    uint32_t reserved_548;
    uint32_t reserved_54c;
    uint32_t reserved_550;
    union {
        struct {
            uint32_t func_sel:          8;
            uint32_t inv_sel:           1;
            uint32_t oen_sel:           1;
            uint32_t oen_inv_sel:       1;
            uint32_t reserved11:       21;
        };
        uint32_t val;
    } func_out_sel_cfg[26];
    uint32_t reserved_5bc;
    uint32_t reserved_5c0;
    uint32_t reserved_5c4;
    uint32_t reserved_5c8;
    uint32_t reserved_5cc;
    uint32_t reserved_5d0;
    uint32_t reserved_5d4;
    uint32_t reserved_5d8;
    uint32_t reserved_5dc;
    uint32_t reserved_5e0;
    uint32_t reserved_5e4;
    uint32_t reserved_5e8;
    uint32_t reserved_5ec;
    uint32_t reserved_5f0;
    uint32_t reserved_5f4;
    uint32_t reserved_5f8;
    uint32_t reserved_5fc;
    uint32_t reserved_600;
    uint32_t reserved_604;
    uint32_t reserved_608;
    uint32_t reserved_60c;
    uint32_t reserved_610;
    uint32_t reserved_614;
    uint32_t reserved_618;
    uint32_t reserved_61c;
    uint32_t reserved_620;
    uint32_t reserved_624;
    uint32_t reserved_628;
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
