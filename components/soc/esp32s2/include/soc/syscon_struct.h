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
#ifndef _SOC_SYSCON_STRUCT_H_
#define _SOC_SYSCON_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct syscon_dev_s {
    union {
        struct {
            uint32_t pre_div:     10;
            uint32_t clk_320m_en:  1;
            uint32_t clk_en:       1;
            uint32_t rst_tick:     1;
            uint32_t reserved13:   1;
            uint32_t soc_clk_sel:  2;
            uint32_t reserved16:  16;
        };
        uint32_t val;
    } sysclk_conf;
    union {
        struct {
            uint32_t xtal_tick:     8;
            uint32_t ck8m_tick:     8;
            uint32_t tick_enable:   1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } tick_conf;
    union {
        struct {
            uint32_t clk20_oen:       1;
            uint32_t clk22_oen:       1;
            uint32_t clk44_oen:       1;
            uint32_t clk_bb_oen:      1;
            uint32_t clk80_oen:       1;
            uint32_t clk160_oen:      1;
            uint32_t clk_320m_oen:    1;
            uint32_t clk_adc_inf_oen: 1;
            uint32_t clk_dac_cpu_oen: 1;
            uint32_t clk40x_bb_oen:   1;
            uint32_t clk_xtal_oen:    1;
            uint32_t reserved11:     21;
        };
        uint32_t val;
    } clk_out_en;
    union {
        struct {
            uint32_t peri_io_swap: 8;
            uint32_t reserved8:   24;
        };
        uint32_t val;
    } host_inf_sel;
    union {
        struct {
            uint32_t ext_mem_pms_lock: 1;
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } ext_mem_pms_lock;
    union {
        struct {
            uint32_t flash_ace0_attr: 3;
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } flash_ace0_attr;
    union {
        struct {
            uint32_t flash_ace1_attr: 3;
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } flash_ace1_attr;
    union {
        struct {
            uint32_t flash_ace2_attr: 3;
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } flash_ace2_attr;
    union {
        struct {
            uint32_t flash_ace3_attr: 3;
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } flash_ace3_attr;
    uint32_t flash_ace0_addr;                     /**/
    uint32_t flash_ace1_addr;                     /**/
    uint32_t flash_ace2_addr;                     /**/
    uint32_t flash_ace3_addr;                     /**/
    union {
        struct {
            uint32_t flash_ace0_size:16;
            uint32_t reserved16:     16;
        };
        uint32_t val;
    } flash_ace0_size;
    union {
        struct {
            uint32_t flash_ace1_size:16;
            uint32_t reserved16:     16;
        };
        uint32_t val;
    } flash_ace1_size;
    union {
        struct {
            uint32_t flash_ace2_size:16;
            uint32_t reserved16:     16;
        };
        uint32_t val;
    } flash_ace2_size;
    union {
        struct {
            uint32_t flash_ace3_size:16;
            uint32_t reserved16:     16;
        };
        uint32_t val;
    } flash_ace3_size;
    union {
        struct {
            uint32_t sram_ace0_attr: 3;
            uint32_t reserved3:     29;
        };
        uint32_t val;
    } sram_ace0_attr;
    union {
        struct {
            uint32_t sram_ace1_attr: 3;
            uint32_t reserved3:     29;
        };
        uint32_t val;
    } sram_ace1_attr;
    union {
        struct {
            uint32_t sram_ace2_attr: 3;
            uint32_t reserved3:     29;
        };
        uint32_t val;
    } sram_ace2_attr;
    union {
        struct {
            uint32_t sram_ace3_attr: 3;
            uint32_t reserved3:     29;
        };
        uint32_t val;
    } sram_ace3_attr;
    uint32_t sram_ace0_addr;                      /**/
    uint32_t sram_ace1_addr;                      /**/
    uint32_t sram_ace2_addr;                      /**/
    uint32_t sram_ace3_addr;                      /**/
    union {
        struct {
            uint32_t sram_ace0_size:16;
            uint32_t reserved16:    16;
        };
        uint32_t val;
    } sram_ace0_size;
    union {
        struct {
            uint32_t sram_ace1_size:16;
            uint32_t reserved16:    16;
        };
        uint32_t val;
    } sram_ace1_size;
    union {
        struct {
            uint32_t sram_ace2_size:16;
            uint32_t reserved16:    16;
        };
        uint32_t val;
    } sram_ace2_size;
    union {
        struct {
            uint32_t sram_ace3_size:16;
            uint32_t reserved16:    16;
        };
        uint32_t val;
    } sram_ace3_size;
    union {
        struct {
            uint32_t spi_mem_reject_int: 1;
            uint32_t spi_mem_reject_clr: 1;
            uint32_t spi_mem_reject_cde: 5;
            uint32_t reserved7:         25;
        };
        uint32_t val;
    } spi_mem_pms_ctrl;
    uint32_t spi_mem_reject_addr;                 /**/
    union {
        struct {
            uint32_t sdio_win_access_en: 1;
            uint32_t reserved1:         31;
        };
        uint32_t val;
    } sdio_ctrl;
    union {
        struct {
            uint32_t redcy_sig0: 31;
            uint32_t redcy_andor: 1;
        };
        uint32_t val;
    } redcy_sig0;
    union {
        struct {
            uint32_t redcy_sig1:  31;
            uint32_t redcy_nandor: 1;
        };
        uint32_t val;
    } redcy_sig1;
    uint32_t wifi_bb_cfg;                         /**/
    uint32_t wifi_bb_cfg_2;                       /**/
    uint32_t wifi_clk_en;                         /**/
    uint32_t wifi_rst_en;                         /**/
    union {
        struct {
            uint32_t agc_mem_force_pu:  1;
            uint32_t agc_mem_force_pd:  1;
            uint32_t pbus_mem_force_pu: 1;
            uint32_t pbus_mem_force_pd: 1;
            uint32_t dc_mem_force_pu:   1;
            uint32_t dc_mem_force_pd:   1;
            uint32_t reserved6:        26;
        };
        uint32_t val;
    } front_end_mem_pd;
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
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
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
    uint32_t date;                       /**/
} syscon_dev_t;
extern syscon_dev_t SYSCON;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_SYSCON_STRUCT_H_ */
