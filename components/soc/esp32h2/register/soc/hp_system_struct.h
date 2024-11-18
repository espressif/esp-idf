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
        /** enable_download_db_encrypt : HRO; bitpos: [1]; default: 0;
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
} hp_sys_external_device_encrypt_decrypt_control_reg_t;

/** Type of sram_usage_conf register
 *  HP memory usage configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** sram_usage : R/W; bitpos: [14:10]; default: 0;
         *  0: cpu use hp-memory. 1: mac-dump accessing hp-memory.
         */
        uint32_t sram_usage:5;
        uint32_t reserved_15:5;
        /** mac_dump_alloc : R/W; bitpos: [24:20]; default: 0;
         *  reserved.
         */
        uint32_t mac_dump_alloc:5;
        uint32_t reserved_25:6;
        /** cache_usage : HRO; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t cache_usage:1;
    };
    uint32_t val;
} hp_sys_sram_usage_conf_reg_t;

/** Type of sec_dpa_conf register
 *  HP anti-DPA security configuration register
 */
typedef union {
    struct {
        /** sec_dpa_level : R/W; bitpos: [1:0]; default: 0;
         *  0: anti-DPA disable. 1~3: anti-DPA enable with different security level. The larger
         *  the number, the stronger the ability to resist DPA attacks and the higher the
         *  security level, but it will increase the computational overhead of the hardware
         *  crypto-accelerators. Only available if HP_SYS_SEC_DPA_CFG_SEL is 0.
         */
        uint32_t sec_dpa_level:2;
        /** sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  This field is used to select either HP_SYS_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL
         *  (from efuse) to control dpa_level. 0: EFUSE_SEC_DPA_LEVEL, 1: HP_SYS_SEC_DPA_LEVEL.
         */
        uint32_t sec_dpa_cfg_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_sec_dpa_conf_reg_t;

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
} hp_sys_rom_table_lock_reg_t;

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
} hp_sys_rom_table_reg_t;

/** Type of mem_test_conf register
 *  MEM_TEST configuration register
 */
typedef union {
    struct {
        /** hp_mem_wpulse : R/W; bitpos: [2:0]; default: 0;
         *  This field controls hp system memory WPULSE parameter. 0b000 for 1.1V/1.0V/0.9V
         *  operating Voltage.
         */
        uint32_t hp_mem_wpulse:3;
        /** hp_mem_wa : R/W; bitpos: [5:3]; default: 5;
         *  This field controls hp system memory WA parameter. 0b100 for 1.1V operating
         *  Voltage, 0b101 for 1.0V, 0b110 for 0.9V.
         */
        uint32_t hp_mem_wa:3;
        /** hp_mem_ra : R/W; bitpos: [7:6]; default: 0;
         *  This field controls hp system memory RA parameter. 0b00 for 1.1V/1.0V operating
         *  Voltage, 0b01 for 0.9V.
         */
        uint32_t hp_mem_ra:2;
        /** hp_mem_rm : R/W; bitpos: [11:8]; default: 2;
         *  This field controls hp system memory RM parameter. 0b0011 for 1.1V operating
         *  Voltage, 0b0010 for 1.0V, 0b0000 for 0.9V.
         */
        uint32_t hp_mem_rm:4;
        /** rom_rm : R/W; bitpos: [15:12]; default: 2;
         *  This field controls rom RM parameter. 0b0011 for 1.1V operating Voltage, 0b0010 for
         *  1.0V, 0b0010(default) or 0b0001(slow) for 0.9V.
         */
        uint32_t rom_rm:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} hp_sys_mem_test_conf_reg_t;

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
} hp_sys_clock_gate_reg_t;


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
} hp_sys_cpu_peri_timeout_conf_reg_t;

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
} hp_sys_cpu_peri_timeout_addr_reg_t;

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
} hp_sys_cpu_peri_timeout_uid_reg_t;

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
} hp_sys_hp_peri_timeout_conf_reg_t;

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
} hp_sys_hp_peri_timeout_addr_reg_t;

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
} hp_sys_hp_peri_timeout_uid_reg_t;


/** Group: Redcy ECO Registers */
/** Type of rnd_eco register
 *  redcy eco register.
 */
typedef union {
    struct {
        /** redcy_ena : R/W; bitpos: [0]; default: 0;
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
} hp_sys_rnd_eco_reg_t;

/** Type of rnd_eco_low register
 *  redcy eco low register.
 */
typedef union {
    struct {
        /** redcy_low : R/W; bitpos: [31:0]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_low:32;
    };
    uint32_t val;
} hp_sys_rnd_eco_low_reg_t;

/** Type of rnd_eco_high register
 *  redcy eco high register.
 */
typedef union {
    struct {
        /** redcy_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  Only reserved for ECO.
         */
        uint32_t redcy_high:32;
    };
    uint32_t val;
} hp_sys_rnd_eco_high_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35689073;
         *  HP-SYSTEM date information/ HP-SYSTEM version information.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_date_reg_t;


typedef struct {
    volatile hp_sys_external_device_encrypt_decrypt_control_reg_t external_device_encrypt_decrypt_control;
    volatile hp_sys_sram_usage_conf_reg_t sram_usage_conf;
    volatile hp_sys_sec_dpa_conf_reg_t sec_dpa_conf;
    volatile hp_sys_cpu_peri_timeout_conf_reg_t cpu_peri_timeout_conf;
    volatile hp_sys_cpu_peri_timeout_addr_reg_t cpu_peri_timeout_addr;
    volatile hp_sys_cpu_peri_timeout_uid_reg_t cpu_peri_timeout_uid;
    volatile hp_sys_hp_peri_timeout_conf_reg_t hp_peri_timeout_conf;
    volatile hp_sys_hp_peri_timeout_addr_reg_t hp_peri_timeout_addr;
    volatile hp_sys_hp_peri_timeout_uid_reg_t hp_peri_timeout_uid;
    volatile hp_sys_rom_table_lock_reg_t rom_table_lock;
    volatile hp_sys_rom_table_reg_t rom_table;
    volatile hp_sys_mem_test_conf_reg_t mem_test_conf;
    uint32_t reserved_030[236];
    volatile hp_sys_rnd_eco_reg_t rnd_eco;
    volatile hp_sys_rnd_eco_low_reg_t rnd_eco_low;
    volatile hp_sys_rnd_eco_high_reg_t rnd_eco_high;
    uint32_t reserved_3ec[3];
    volatile hp_sys_clock_gate_reg_t clock_gate;
    volatile hp_sys_date_reg_t date;
} hp_sys_dev_t;

extern hp_sys_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_sys_dev_t) == 0x400, "Invalid size of hp_sys_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
