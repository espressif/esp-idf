/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of wifi_bb_cfg register
 *  register description
 */
typedef union {
    struct {
        /** wifi_bb_cfg : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t wifi_bb_cfg:32;
    };
    uint32_t val;
} syscon_wifi_bb_cfg_reg_t;

/** Type of wifi_bb_cfg_2 register
 *  register description
 */
typedef union {
    struct {
        /** wifi_bb_cfg_2 : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t wifi_bb_cfg_2:32;
    };
    uint32_t val;
} syscon_wifi_bb_cfg_2_reg_t;

/** Type of host_inf_sel register
 *  register description
 */
typedef union {
    struct {
        /** peri_io_swap : R/W; bitpos: [7:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_io_swap:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} syscon_host_inf_sel_reg_t;

/** Type of ext_mem_pms_lock register
 *  register description
 */
typedef union {
    struct {
        /** ext_mem_pms_lock : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t ext_mem_pms_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} syscon_ext_mem_pms_lock_reg_t;

/** Type of flash_ace0_attr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace0_attr : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t flash_ace0_attr:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_flash_ace0_attr_reg_t;

/** Type of flash_ace1_attr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace1_attr : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t flash_ace1_attr:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_flash_ace1_attr_reg_t;

/** Type of flash_ace2_attr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace2_attr : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t flash_ace2_attr:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_flash_ace2_attr_reg_t;

/** Type of flash_ace3_attr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace3_attr : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t flash_ace3_attr:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_flash_ace3_attr_reg_t;

/** Type of flash_ace0_addr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace0_addr_s : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t flash_ace0_addr_s:32;
    };
    uint32_t val;
} syscon_flash_ace0_addr_reg_t;

/** Type of flash_ace1_addr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace1_addr_s : R/W; bitpos: [31:0]; default: 4194304;
         *  Need add description
         */
        uint32_t flash_ace1_addr_s:32;
    };
    uint32_t val;
} syscon_flash_ace1_addr_reg_t;

/** Type of flash_ace2_addr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace2_addr_s : R/W; bitpos: [31:0]; default: 8388608;
         *  Need add description
         */
        uint32_t flash_ace2_addr_s:32;
    };
    uint32_t val;
} syscon_flash_ace2_addr_reg_t;

/** Type of flash_ace3_addr register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace3_addr_s : R/W; bitpos: [31:0]; default: 12582912;
         *  Need add description
         */
        uint32_t flash_ace3_addr_s:32;
    };
    uint32_t val;
} syscon_flash_ace3_addr_reg_t;

/** Type of flash_ace0_size register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace0_size : R/W; bitpos: [12:0]; default: 1024;
         *  Need add description
         */
        uint32_t flash_ace0_size:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} syscon_flash_ace0_size_reg_t;

/** Type of flash_ace1_size register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace1_size : R/W; bitpos: [12:0]; default: 1024;
         *  Need add description
         */
        uint32_t flash_ace1_size:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} syscon_flash_ace1_size_reg_t;

/** Type of flash_ace2_size register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace2_size : R/W; bitpos: [12:0]; default: 1024;
         *  Need add description
         */
        uint32_t flash_ace2_size:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} syscon_flash_ace2_size_reg_t;

/** Type of flash_ace3_size register
 *  register description
 */
typedef union {
    struct {
        /** flash_ace3_size : R/W; bitpos: [12:0]; default: 1024;
         *  Need add description
         */
        uint32_t flash_ace3_size:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} syscon_flash_ace3_size_reg_t;

/** Type of spi_mem_pms_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** spi_mem_reject_int : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t spi_mem_reject_int:1;
        /** spi_mem_reject_clr : WOD; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t spi_mem_reject_clr:1;
        /** spi_mem_reject_cde : RO; bitpos: [6:2]; default: 0;
         *  Need add description
         */
        uint32_t spi_mem_reject_cde:5;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} syscon_spi_mem_pms_ctrl_reg_t;

/** Type of spi_mem_reject_addr register
 *  register description
 */
typedef union {
    struct {
        /** spi_mem_reject_addr : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t spi_mem_reject_addr:32;
    };
    uint32_t val;
} syscon_spi_mem_reject_addr_reg_t;

/** Type of syscon_sdio_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** sdio_win_access_en : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t sdio_win_access_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} syscon_syscon_sdio_ctrl_reg_t;

/** Type of redcy_sig0 register
 *  register description
 */
typedef union {
    struct {
        /** redcy_sig0 : R/W; bitpos: [30:0]; default: 0;
         *  Need add description
         */
        uint32_t redcy_sig0:31;
        /** redcy_andor : RO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t redcy_andor:1;
    };
    uint32_t val;
} syscon_redcy_sig0_reg_t;

/** Type of redcy_sig1 register
 *  register description
 */
typedef union {
    struct {
        /** redcy_sig1 : R/W; bitpos: [30:0]; default: 0;
         *  Need add description
         */
        uint32_t redcy_sig1:31;
        /** redcy_nandor : RO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t redcy_nandor:1;
    };
    uint32_t val;
} syscon_redcy_sig1_reg_t;

/** Type of front_end_mem_pd register
 *  register description
 */
typedef union {
    struct {
        /** agc_mem_force_pu : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t agc_mem_force_pu:1;
        /** agc_mem_force_pd : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t agc_mem_force_pd:1;
        /** pbus_mem_force_pu : R/W; bitpos: [2]; default: 1;
         *  Need add description
         */
        uint32_t pbus_mem_force_pu:1;
        /** pbus_mem_force_pd : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t pbus_mem_force_pd:1;
        /** dc_mem_force_pu : R/W; bitpos: [4]; default: 1;
         *  Need add description
         */
        uint32_t dc_mem_force_pu:1;
        /** dc_mem_force_pd : R/W; bitpos: [5]; default: 0;
         *  Need add description
         */
        uint32_t dc_mem_force_pd:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} syscon_front_end_mem_pd_reg_t;

/** Type of retention_ctrl register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** nobypass_cpu_iso_rst : R/W; bitpos: [27]; default: 0;
         *  Need add description
         */
        uint32_t nobypass_cpu_iso_rst:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} syscon_retention_ctrl_reg_t;

/** Type of clkgate_force_on register
 *  register description
 */
typedef union {
    struct {
        /** rom_clkgate_force_on : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t rom_clkgate_force_on:2;
        /** sram_clkgate_force_on : R/W; bitpos: [5:2]; default: 15;
         *  Need add description
         */
        uint32_t sram_clkgate_force_on:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} syscon_clkgate_force_on_reg_t;

/** Type of mem_power_down register
 *  register description
 */
typedef union {
    struct {
        /** rom_power_down : R/W; bitpos: [1:0]; default: 0;
         *  Need add description
         */
        uint32_t rom_power_down:2;
        /** sram_power_down : R/W; bitpos: [5:2]; default: 0;
         *  Need add description
         */
        uint32_t sram_power_down:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} syscon_mem_power_down_reg_t;

/** Type of mem_power_up register
 *  register description
 */
typedef union {
    struct {
        /** rom_power_up : R/W; bitpos: [1:0]; default: 3;
         *  Need add description
         */
        uint32_t rom_power_up:2;
        /** sram_power_up : R/W; bitpos: [5:2]; default: 15;
         *  Need add description
         */
        uint32_t sram_power_up:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} syscon_mem_power_up_reg_t;

/** Type of rnd_data register
 *  register description
 */
typedef union {
    struct {
        /** rnd_data : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t rnd_data:32;
    };
    uint32_t val;
} syscon_rnd_data_reg_t;

/** Type of peri_backup_config register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_flow_err : RO; bitpos: [2:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_flow_err:3;
        /** peri_backup_addr_map_mode : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_addr_map_mode:1;
        /** peri_backup_burst_limit : R/W; bitpos: [8:4]; default: 8;
         *  Need add description
         */
        uint32_t peri_backup_burst_limit:5;
        /** peri_backup_tout_thres : R/W; bitpos: [18:9]; default: 50;
         *  Need add description
         */
        uint32_t peri_backup_tout_thres:10;
        /** peri_backup_size : R/W; bitpos: [28:19]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_size:10;
        /** peri_backup_start : WO; bitpos: [29]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_start:1;
        /** peri_backup_to_mem : R/W; bitpos: [30]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_to_mem:1;
        /** peri_backup_ena : R/W; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_ena:1;
    };
    uint32_t val;
} syscon_peri_backup_config_reg_t;

/** Type of peri_backup_apb_addr register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_apb_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_apb_start_addr:32;
    };
    uint32_t val;
} syscon_peri_backup_apb_addr_reg_t;

/** Type of peri_backup_mem_addr register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_mem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_mem_start_addr:32;
    };
    uint32_t val;
} syscon_peri_backup_mem_addr_reg_t;

/** Type of peri_backup_reg_map0 register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_reg_map0 : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_reg_map0:32;
    };
    uint32_t val;
} syscon_peri_backup_reg_map0_reg_t;

/** Type of peri_backup_reg_map1 register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_reg_map1 : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_reg_map1:32;
    };
    uint32_t val;
} syscon_peri_backup_reg_map1_reg_t;

/** Type of peri_backup_reg_map2 register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_reg_map2 : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_reg_map2:32;
    };
    uint32_t val;
} syscon_peri_backup_reg_map2_reg_t;

/** Type of peri_backup_reg_map3 register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_reg_map3 : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_reg_map3:32;
    };
    uint32_t val;
} syscon_peri_backup_reg_map3_reg_t;

/** Type of peri_backup_int_raw register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_done_int_raw : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_done_int_raw:1;
        /** peri_backup_err_int_raw : RO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_err_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_peri_backup_int_raw_reg_t;

/** Type of peri_backup_int_st register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_done_int_st : RO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_done_int_st:1;
        /** peri_backup_err_int_st : RO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_err_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_peri_backup_int_st_reg_t;

/** Type of peri_backup_int_ena register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_done_int_ena:1;
        /** peri_backup_err_int_ena : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_err_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_peri_backup_int_ena_reg_t;

/** Type of peri_backup_int_clr register
 *  register description
 */
typedef union {
    struct {
        /** peri_backup_done_int_clr : WO; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_done_int_clr:1;
        /** peri_backup_err_int_clr : WO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t peri_backup_err_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} syscon_peri_backup_int_clr_reg_t;

/** Type of syscon_regclk_conf register
 *  register description
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} syscon_syscon_regclk_conf_reg_t;

/** Type of syscon_date register
 *  register description
 */
typedef union {
    struct {
        /** syscon_date : R/W; bitpos: [31:0]; default: 34607184;
         *  Version control
         */
        uint32_t syscon_date:32;
    };
    uint32_t val;
} syscon_syscon_date_reg_t;


typedef struct {
    uint32_t reserved_000[3];
    volatile syscon_wifi_bb_cfg_reg_t wifi_bb_cfg;
    volatile syscon_wifi_bb_cfg_2_reg_t wifi_bb_cfg_2;
    uint32_t reserved_014[2];
    volatile syscon_host_inf_sel_reg_t host_inf_sel;
    volatile syscon_ext_mem_pms_lock_reg_t ext_mem_pms_lock;
    uint32_t reserved_024;
    volatile syscon_flash_ace0_attr_reg_t flash_ace0_attr;
    volatile syscon_flash_ace1_attr_reg_t flash_ace1_attr;
    volatile syscon_flash_ace2_attr_reg_t flash_ace2_attr;
    volatile syscon_flash_ace3_attr_reg_t flash_ace3_attr;
    volatile syscon_flash_ace0_addr_reg_t flash_ace0_addr;
    volatile syscon_flash_ace1_addr_reg_t flash_ace1_addr;
    volatile syscon_flash_ace2_addr_reg_t flash_ace2_addr;
    volatile syscon_flash_ace3_addr_reg_t flash_ace3_addr;
    volatile syscon_flash_ace0_size_reg_t flash_ace0_size;
    volatile syscon_flash_ace1_size_reg_t flash_ace1_size;
    volatile syscon_flash_ace2_size_reg_t flash_ace2_size;
    volatile syscon_flash_ace3_size_reg_t flash_ace3_size;
    uint32_t reserved_058[12];
    volatile syscon_spi_mem_pms_ctrl_reg_t spi_mem_pms_ctrl;
    volatile syscon_spi_mem_reject_addr_reg_t spi_mem_reject_addr;
    volatile syscon_syscon_sdio_ctrl_reg_t syscon_sdio_ctrl;
    volatile syscon_redcy_sig0_reg_t redcy_sig0;
    volatile syscon_redcy_sig1_reg_t redcy_sig1;
    volatile syscon_front_end_mem_pd_reg_t front_end_mem_pd;
    volatile syscon_retention_ctrl_reg_t retention_ctrl;
    volatile syscon_clkgate_force_on_reg_t clkgate_force_on;
    volatile syscon_mem_power_down_reg_t mem_power_down;
    volatile syscon_mem_power_up_reg_t mem_power_up;
    volatile syscon_rnd_data_reg_t rnd_data;
    volatile syscon_peri_backup_config_reg_t peri_backup_config;
    volatile syscon_peri_backup_apb_addr_reg_t peri_backup_apb_addr;
    volatile syscon_peri_backup_mem_addr_reg_t peri_backup_mem_addr;
    volatile syscon_peri_backup_reg_map0_reg_t peri_backup_reg_map0;
    volatile syscon_peri_backup_reg_map1_reg_t peri_backup_reg_map1;
    volatile syscon_peri_backup_reg_map2_reg_t peri_backup_reg_map2;
    volatile syscon_peri_backup_reg_map3_reg_t peri_backup_reg_map3;
    volatile syscon_peri_backup_int_raw_reg_t peri_backup_int_raw;
    volatile syscon_peri_backup_int_st_reg_t peri_backup_int_st;
    volatile syscon_peri_backup_int_ena_reg_t peri_backup_int_ena;
    volatile syscon_peri_backup_int_clr_reg_t peri_backup_int_clr;
    volatile syscon_syscon_regclk_conf_reg_t syscon_regclk_conf;
    uint32_t reserved_0e4[198];
    volatile syscon_syscon_date_reg_t syscon_date;
} syscon_dev_t;

extern syscon_dev_t SYSCON;

#ifndef __cplusplus
_Static_assert(sizeof(syscon_dev_t) == 0x400, "Invalid size of syscon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
