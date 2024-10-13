/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Register */
/** Type of external_device_encrypt_decrypt_control register
 *  External device encryption/decryption configuration register
 */
typedef union {
    struct {
        /** enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable MSPI XTS manual encryption in SPI boot mode.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t enable_spi_manual_encrypt:1;
        /** enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t enable_download_db_encrypt:1;
        /** enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable MSPI XTS auto decryption in download boot
         *  mode.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t enable_download_g0cb_decrypt:1;
        /** enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable MSPI XTS manual encryption in download boot
         *  mode. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_external_device_encrypt_decrypt_control_reg_t;

/** Type of sram_usage_conf register
 *  HP memory usage configuration register
 */
typedef union {
    struct {
        /** cache_usage : HRO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t cache_usage:1;
        uint32_t reserved_1:7;
        /** sram_usage : R/W; bitpos: [11:8]; default: 0;
         *  0: cpu use hp-memory. 1:mac-dump accessing hp-memory.
         */
        uint32_t sram_usage:4;
        uint32_t reserved_12:4;
        /** mac_dump_alloc : R/W; bitpos: [16]; default: 0;
         *  Set this bit as 1 to add an offset (64KB) when mac-dump accessing hp-memory.
         */
        uint32_t mac_dump_alloc:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_sram_usage_conf_reg_t;

/** Type of sec_dpa_conf register
 *  HP anti-DPA security configuration register
 */
typedef union {
    struct {
        /** sec_dpa_level : R/W; bitpos: [1:0]; default: 0;
         *  Configures whether or not to enable anti-DPA attack. Valid only when
         *  HP_SYSTEM_SEC_DPA_CFG_SEL is 0. \\
         *  0: Disable\\
         *  1-3: Enable. The larger the number, the higher the security level, which represents
         *  the ability to resist DPA attacks, with increased computational overhead of the
         *  hardware crypto-accelerators at the same time. \\
         */
        uint32_t sec_dpa_level:2;
        /** sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  Configures whether to select HP_SYSTEM_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL (from
         *  eFuse) to control DPA level. \\
         *  0: Select EFUSE_SEC_DPA_LEVEL\\
         *  1: Select HP_SYSTEM_SEC_DPA_LEVEL\\
         */
        uint32_t sec_dpa_cfg_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_sec_dpa_conf_reg_t;

/** Type of sdio_ctrl register
 *  SDIO Control configuration register
 */
typedef union {
    struct {
        /** dis_sdio_prob : R/W; bitpos: [0]; default: 1;
         *  Set this bit as 1 to disable SDIO_PROB function. disable by default.
         */
        uint32_t dis_sdio_prob:1;
        /** sdio_win_access_en : R/W; bitpos: [1]; default: 1;
         *  Enable sdio slave to access other peripherals on the chip
         */
        uint32_t sdio_win_access_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_sdio_ctrl_reg_t;

/** Type of rom_table_lock register
 *  ROM-Table lock register
 */
typedef union {
    struct {
        /** rom_table_lock : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to lock the value contained in HP_SYSTEM_ROM_TABLE. \\
         *  0: Unlock \\
         *  1: Lock \\
         */
        uint32_t rom_table_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_rom_table_lock_reg_t;

/** Type of rom_table register
 *  ROM-Table register
 */
typedef union {
    struct {
        /** rom_table : R/W; bitpos: [31:0]; default: 0;
         *  Software ROM-Table register, whose content can be modified only when
         *  HP_SYSTEM_ROM_TABLE_LOCK is 0.
         */
        uint32_t rom_table:32;
    };
    uint32_t val;
} hp_system_rom_table_reg_t;

/** Type of core_debug_runstall_conf register
 *  Core Debug RunStall configurion register
 */
typedef union {
    struct {
        /** core_debug_runstall_enable : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable debug RunStall functionality between HP CPU and
         *  LP CPU.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t core_debug_runstall_enable:1;
        /** core_runstalled : RO; bitpos: [1]; default: 0;
         *  Software can read this field to get the runstall status of hp-core. 1: stalled, 0:
         *  not stalled.
         */
        uint32_t core_runstalled:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_core_debug_runstall_conf_reg_t;

/** Type of mem_test_conf register
 *  MEM_TEST configuration register
 */
typedef union {
    struct {
        /** hp_mem_wpulse : R/W; bitpos: [2:0]; default: 0;
         *  This field controls hp system memory WPULSE parameter.
         */
        uint32_t hp_mem_wpulse:3;
        /** hp_mem_wa : R/W; bitpos: [5:3]; default: 4;
         *  This field controls hp system memory WA parameter.
         */
        uint32_t hp_mem_wa:3;
        /** hp_mem_ra : R/W; bitpos: [7:6]; default: 0;
         *  This field controls hp system memory RA parameter.
         */
        uint32_t hp_mem_ra:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_mem_test_conf_reg_t;

/** Type of sprom_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** sprom_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 112;
         *  reserved
         */
        uint32_t sprom_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sprom_ctrl_reg_t;

/** Type of spram_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  reserved
         */
        uint32_t spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_spram_ctrl_reg_t;

/** Type of sprf_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  reserved
         */
        uint32_t sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sprf_ctrl_reg_t;

/** Type of sdprf_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** sdprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t sdprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sdprf_ctrl_reg_t;

/** Type of bitscrambler_peri_sel register
 *  reserved
 */
typedef union {
    struct {
        /** bitscrambler_rx_sel : R/W; bitpos: [3:0]; default: 0;
         *  select peri that will be connected to bitscrambler,dir : receive data from bs
         */
        uint32_t bitscrambler_rx_sel:4;
        /** bitscrambler_tx_sel : R/W; bitpos: [7:4]; default: 0;
         *  select peri that will be connected to bitscrambler,dir : transfer data to peri
         */
        uint32_t bitscrambler_tx_sel:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_bitscrambler_peri_sel_reg_t;

/** Type of clock_gate register
 *  HP-SYSTEM clock gating configure register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to force on clock gating.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_clock_gate_reg_t;


/** Group: Timeout Register */
/** Type of cpu_peri_timeout_conf register
 *  CPU_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** cpu_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t cpu_peri_timeout_thres:16;
        /** cpu_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t cpu_peri_timeout_int_clear:1;
        /** cpu_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU peripheral
         *  registers.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t cpu_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_cpu_peri_timeout_conf_reg_t;

/** Type of cpu_peri_timeout_addr register
 *  CPU_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** cpu_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t cpu_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_system_cpu_peri_timeout_addr_reg_t;

/** Type of cpu_peri_timeout_uid register
 *  CPU_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** cpu_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Represents the master id[4:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t cpu_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_system_cpu_peri_timeout_uid_reg_t;

/** Type of hp_peri_timeout_conf register
 *  HP_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** hp_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing HP peripheral
         *  register, corresponding to the number of clock cycles of the clock domain.
         */
        uint32_t hp_peri_timeout_thres:16;
        /** hp_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear timeout interrupt.\\
         *  0: No effect\\
         *  1: Clear timeout interrupt\\
         */
        uint32_t hp_peri_timeout_int_clear:1;
        /** hp_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing HP peripheral
         *  registers.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t hp_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_hp_peri_timeout_conf_reg_t;

/** Type of hp_peri_timeout_addr register
 *  HP_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** hp_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t hp_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_system_hp_peri_timeout_addr_reg_t;

/** Type of hp_peri_timeout_uid register
 *  HP_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** hp_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Represents the master id[4:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t hp_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_system_hp_peri_timeout_uid_reg_t;

/** Type of modem_peri_timeout_conf register
 *  MODEM_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** modem_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Set the timeout threshold for bus access, corresponding to the number of clock
         *  cycles of the clock domain.
         */
        uint32_t modem_peri_timeout_thres:16;
        /** modem_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Set this bit as 1 to clear timeout interrupt
         */
        uint32_t modem_peri_timeout_int_clear:1;
        /** modem_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Set this bit as 1 to enable timeout protection for accessing modem registers
         */
        uint32_t modem_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_modem_peri_timeout_conf_reg_t;

/** Type of modem_peri_timeout_addr register
 *  MODEM_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** modem_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Record the address information of abnormal access
         */
        uint32_t modem_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_system_modem_peri_timeout_addr_reg_t;

/** Type of modem_peri_timeout_uid register
 *  MODEM_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** modem_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
         *  will be cleared after the interrupt is cleared.
         */
        uint32_t modem_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_system_modem_peri_timeout_uid_reg_t;


/** Group: Redcy ECO Registers */
/** Type of rnd_eco register
 *  redcy eco register.
 */
typedef union {
    struct {
        /** redcy_ena : W/R; bitpos: [0]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_ena:1;
        /** redcy_result : RO; bitpos: [1]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_rnd_eco_reg_t;

/** Type of rnd_eco_low register
 *  redcy eco low register.
 */
typedef union {
    struct {
        /** redcy_low : W/R; bitpos: [31:0]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_low:32;
    };
    uint32_t val;
} hp_system_rnd_eco_low_reg_t;

/** Type of rnd_eco_high register
 *  redcy eco high register.
 */
typedef union {
    struct {
        /** redcy_high : W/R; bitpos: [31:0]; default: 4294967295;
         *  Only reserved for ECO.
         */
        uint32_t redcy_high:32;
    };
    uint32_t val;
} hp_system_rnd_eco_high_reg_t;


/** Group: Debug Register */
/** Type of debug register
 *  HP-SYSTEM debug register
 */
typedef union {
    struct {
        /** fpga_debug : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t fpga_debug:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_debug_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date control and version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36774016;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_system_date_reg_t;


typedef struct {
    volatile hp_system_external_device_encrypt_decrypt_control_reg_t external_device_encrypt_decrypt_control;
    volatile hp_system_sram_usage_conf_reg_t sram_usage_conf;
    volatile hp_system_sec_dpa_conf_reg_t sec_dpa_conf;
    volatile hp_system_cpu_peri_timeout_conf_reg_t cpu_peri_timeout_conf;
    volatile hp_system_cpu_peri_timeout_addr_reg_t cpu_peri_timeout_addr;
    volatile hp_system_cpu_peri_timeout_uid_reg_t cpu_peri_timeout_uid;
    volatile hp_system_hp_peri_timeout_conf_reg_t hp_peri_timeout_conf;
    volatile hp_system_hp_peri_timeout_addr_reg_t hp_peri_timeout_addr;
    volatile hp_system_hp_peri_timeout_uid_reg_t hp_peri_timeout_uid;
    volatile hp_system_modem_peri_timeout_conf_reg_t modem_peri_timeout_conf;
    volatile hp_system_modem_peri_timeout_addr_reg_t modem_peri_timeout_addr;
    volatile hp_system_modem_peri_timeout_uid_reg_t modem_peri_timeout_uid;
    volatile hp_system_sdio_ctrl_reg_t sdio_ctrl;
    uint32_t reserved_034;
    volatile hp_system_rom_table_lock_reg_t rom_table_lock;
    volatile hp_system_rom_table_reg_t rom_table;
    volatile hp_system_core_debug_runstall_conf_reg_t core_debug_runstall_conf;
    volatile hp_system_mem_test_conf_reg_t mem_test_conf;
    uint32_t reserved_048[10];
    volatile hp_system_sprom_ctrl_reg_t sprom_ctrl;
    volatile hp_system_spram_ctrl_reg_t spram_ctrl;
    volatile hp_system_sprf_ctrl_reg_t sprf_ctrl;
    volatile hp_system_sdprf_ctrl_reg_t sdprf_ctrl;
    volatile hp_system_bitscrambler_peri_sel_reg_t bitscrambler_peri_sel;
    uint32_t reserved_084[215];
    volatile hp_system_rnd_eco_reg_t rnd_eco;
    volatile hp_system_rnd_eco_low_reg_t rnd_eco_low;
    volatile hp_system_rnd_eco_high_reg_t rnd_eco_high;
    uint32_t reserved_3ec[2];
    volatile hp_system_debug_reg_t debug;
    volatile hp_system_clock_gate_reg_t clock_gate;
    volatile hp_system_date_reg_t date;
} hp_system_dev_t;

extern hp_system_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_system_dev_t) == 0x400, "Invalid size of hp_system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
