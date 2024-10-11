/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
 *  EXTERNAL DEVICE ENCRYPTION/DECRYPTION configuration register
 */
typedef union {
    struct {
        /** enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to enable mspi xts manual encrypt in spi boot mode.
         */
        uint32_t enable_spi_manual_encrypt:1;
        /** enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t enable_download_db_encrypt:1;
        /** enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Set this bit as 1 to enable mspi xts auto decrypt in download boot mode.
         */
        uint32_t enable_download_g0cb_decrypt:1;
        /** enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Set this bit as 1 to enable mspi xts manual encrypt in download boot mode.
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
         *  0: anti-DPA disable. 1~3: anti-DPA enable with different security level. The larger
         *  the number, the stronger the ability to resist DPA attacks and the higher the
         *  security level, but it will increase the computational overhead of the hardware
         *  crypto-accelerators. Only available if HP_SYSTEM_SEC_DPA_CFG_SEL is 0.
         */
        uint32_t sec_dpa_level:2;
        /** sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  This field is used to select either HP_SYSTEM_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL
         *  (from efuse) to control dpa_level. 0: EFUSE_SEC_DPA_LEVEL, 1: HP_SYSTEM_SEC_DPA_LEVEL.
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

/** Type of retention_conf register
 *  Retention configuration register
 */
typedef union {
    struct {
        /** retention_disable : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to disable retention function. Not disable by default.
         */
        uint32_t retention_disable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_retention_conf_reg_t;

/** Type of rom_table_lock register
 *  Rom-Table lock register
 */
typedef union {
    struct {
        /** rom_table_lock : R/W; bitpos: [0]; default: 0;
         *  XXXX
         */
        uint32_t rom_table_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_rom_table_lock_reg_t;

/** Type of rom_table register
 *  Rom-Table register
 */
typedef union {
    struct {
        /** rom_table : R/W; bitpos: [31:0]; default: 0;
         *  XXXX
         */
        uint32_t rom_table:32;
    };
    uint32_t val;
} hp_system_rom_table_reg_t;

/** Type of core_debug_runstall_conf register
 *  Core Debug runstall configure register
 */
typedef union {
    struct {
        /** core_debug_runstall_enable : R/W; bitpos: [0]; default: 0;
         *  Set this field to 1 to enable debug runstall feature between HP-core and LP-core.
         */
        uint32_t core_debug_runstall_enable:1;
        uint32_t reserved_1:31;
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
         *  Set the timeout threshold for bus access, corresponding to the number of clock
         *  cycles of the clock domain.
         */
        uint32_t cpu_peri_timeout_thres:16;
        /** cpu_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Set this bit as 1 to clear timeout interrupt
         */
        uint32_t cpu_peri_timeout_int_clear:1;
        /** cpu_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Set this bit as 1 to enable timeout protection for accessing cpu peripheral
         *  registers
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
         *  Record the address information of abnormal access
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
         *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
         *  will be cleared after the interrupt is cleared.
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
         *  Set the timeout threshold for bus access, corresponding to the number of clock
         *  cycles of the clock domain.
         */
        uint32_t hp_peri_timeout_thres:16;
        /** hp_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Set this bit as 1 to clear timeout interrupt
         */
        uint32_t hp_peri_timeout_int_clear:1;
        /** hp_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Set this bit as 1 to enable timeout protection for accessing hp peripheral registers
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
         *  Record the address information of abnormal access
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
         *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
         *  will be cleared after the interrupt is cleared.
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


/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35676432;
         *  HP-SYSTEM date information/ HP-SYSTEM version information.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_system_date_reg_t;


typedef struct hp_system_dev_t {
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
    volatile hp_system_retention_conf_reg_t retention_conf;
    volatile hp_system_rom_table_lock_reg_t rom_table_lock;
    volatile hp_system_rom_table_reg_t rom_table;
    volatile hp_system_core_debug_runstall_conf_reg_t core_debug_runstall_conf;
    volatile hp_system_mem_test_conf_reg_t mem_test_conf;
    uint32_t reserved_048[230];
    volatile hp_system_rnd_eco_reg_t rnd_eco;
    volatile hp_system_rnd_eco_low_reg_t rnd_eco_low;
    volatile hp_system_rnd_eco_high_reg_t rnd_eco_high;
    uint32_t reserved_3ec[3];
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
