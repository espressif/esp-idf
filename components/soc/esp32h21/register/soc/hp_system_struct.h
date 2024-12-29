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
/** Type of sys_external_device_encrypt_decrypt_control register
 *  EXTERNAL DEVICE ENCRYPTION/DECRYPTION configuration register
 */
typedef union {
    struct {
        /** sys_enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  Set this bit as 1 to enable mspi xts manual encrypt in spi boot mode.
         */
        uint32_t sys_enable_spi_manual_encrypt:1;
        /** sys_enable_download_db_encrypt : HRO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t sys_enable_download_db_encrypt:1;
        /** sys_enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Set this bit as 1 to enable mspi xts auto decrypt in download boot mode.
         */
        uint32_t sys_enable_download_g0cb_decrypt:1;
        /** sys_enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Set this bit as 1 to enable mspi xts manual encrypt in download boot mode.
         */
        uint32_t sys_enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_external_device_encrypt_decrypt_control_reg_t;

/** Type of sys_sram_usage_conf register
 *  HP memory usage configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** sys_sram_usage : R/W; bitpos: [14:10]; default: 0;
         *  0: cpu use hp-memory. 1: mac-dump accessing hp-memory.
         */
        uint32_t sys_sram_usage:5;
        uint32_t reserved_15:5;
        /** sys_mac_dump_alloc : R/W; bitpos: [24:20]; default: 0;
         *  reserved.
         */
        uint32_t sys_mac_dump_alloc:5;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_sram_usage_conf_reg_t;

/** Type of sys_sec_dpa_conf register
 *  HP anti-DPA security configuration register
 */
typedef union {
    struct {
        /** sys_sec_dpa_level : R/W; bitpos: [1:0]; default: 0;
         *  0: anti-DPA disable. 1~3: anti-DPA enable with different security level. The larger
         *  the number, the stronger the ability to resist DPA attacks and the higher the
         *  security level, but it will increase the computational overhead of the hardware
         *  crypto-accelerators. Only available if HP_SYS_SEC_DPA_CFG_SEL is 0.
         */
        uint32_t sys_sec_dpa_level:2;
        /** sys_sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  This field is used to select either HP_SYS_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL
         *  (from efuse) to control dpa_level. 0: EFUSE_SEC_DPA_LEVEL, 1: HP_SYS_SEC_DPA_LEVEL.
         */
        uint32_t sys_sec_dpa_cfg_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_sec_dpa_conf_reg_t;

/** Type of sys_rom_table_lock register
 *  Rom-Table lock register
 */
typedef union {
    struct {
        /** sys_rom_table_lock : R/W; bitpos: [0]; default: 0;
         *  XXXX
         */
        uint32_t sys_rom_table_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_rom_table_lock_reg_t;

/** Type of sys_rom_table register
 *  Rom-Table register
 */
typedef union {
    struct {
        /** sys_rom_table : R/W; bitpos: [31:0]; default: 0;
         *  XXXX
         */
        uint32_t sys_rom_table:32;
    };
    uint32_t val;
} hp_sys_rom_table_reg_t;

/** Type of sys_rng_loop register
 *  configure rng_ring
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** sys_sample_cnt : RO; bitpos: [31:24]; default: 0;
         *  This field get rng ring count
         */
        uint32_t sys_sample_cnt:8;
    };
    uint32_t val;
} hp_sys_rng_loop_reg_t;


/** Group: Timeout Register */
/** Type of sys_cpu_peri_timeout_conf register
 *  CPU_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** sys_cpu_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Set the timeout threshold for bus access, corresponding to the number of clock
         *  cycles of the clock domain.
         */
        uint32_t sys_cpu_peri_timeout_thres:16;
        /** sys_cpu_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Set this bit as 1 to clear timeout interrupt
         */
        uint32_t sys_cpu_peri_timeout_int_clear:1;
        /** sys_cpu_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Set this bit as 1 to enable timeout protection for accessing cpu peripheral
         *  registers
         */
        uint32_t sys_cpu_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_sys_cpu_peri_timeout_conf_reg_t;

/** Type of sys_cpu_peri_timeout_addr register
 *  CPU_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** sys_cpu_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Record the address information of abnormal access
         */
        uint32_t sys_cpu_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_sys_cpu_peri_timeout_addr_reg_t;

/** Type of sys_cpu_peri_timeout_uid register
 *  CPU_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** sys_cpu_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
         *  will be cleared after the interrupt is cleared.
         */
        uint32_t sys_cpu_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_sys_cpu_peri_timeout_uid_reg_t;

/** Type of sys_hp_peri_timeout_conf register
 *  HP_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** sys_hp_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Set the timeout threshold for bus access, corresponding to the number of clock
         *  cycles of the clock domain.
         */
        uint32_t sys_hp_peri_timeout_thres:16;
        /** sys_hp_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Set this bit as 1 to clear timeout interrupt
         */
        uint32_t sys_hp_peri_timeout_int_clear:1;
        /** sys_hp_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Set this bit as 1 to enable timeout protection for accessing hp peripheral registers
         */
        uint32_t sys_hp_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_sys_hp_peri_timeout_conf_reg_t;

/** Type of sys_hp_peri_timeout_addr register
 *  HP_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** sys_hp_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Record the address information of abnormal access
         */
        uint32_t sys_hp_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_sys_hp_peri_timeout_addr_reg_t;

/** Type of sys_hp_peri_timeout_uid register
 *  HP_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** sys_hp_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
         *  will be cleared after the interrupt is cleared.
         */
        uint32_t sys_hp_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_sys_hp_peri_timeout_uid_reg_t;


/** Group: Version Register */
/** Type of sys_date register
 *  Date register.
 */
typedef union {
    struct {
        /** sys_date : R/W; bitpos: [27:0]; default: 37782128;
         *  HP-SYSTEM date information/ HP-SYSTEM version information.
         */
        uint32_t sys_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_date_reg_t;


typedef struct {
    volatile hp_sys_external_device_encrypt_decrypt_control_reg_t sys_external_device_encrypt_decrypt_control;
    volatile hp_sys_sram_usage_conf_reg_t sys_sram_usage_conf;
    volatile hp_sys_sec_dpa_conf_reg_t sys_sec_dpa_conf;
    volatile hp_sys_cpu_peri_timeout_conf_reg_t sys_cpu_peri_timeout_conf;
    volatile hp_sys_cpu_peri_timeout_addr_reg_t sys_cpu_peri_timeout_addr;
    volatile hp_sys_cpu_peri_timeout_uid_reg_t sys_cpu_peri_timeout_uid;
    volatile hp_sys_hp_peri_timeout_conf_reg_t sys_hp_peri_timeout_conf;
    volatile hp_sys_hp_peri_timeout_addr_reg_t sys_hp_peri_timeout_addr;
    volatile hp_sys_hp_peri_timeout_uid_reg_t sys_hp_peri_timeout_uid;
    volatile hp_sys_rom_table_lock_reg_t sys_rom_table_lock;
    volatile hp_sys_rom_table_reg_t sys_rom_table;
    uint32_t reserved_02c;
    volatile hp_sys_rng_loop_reg_t sys_rng_loop;
    uint32_t reserved_034[242];
    volatile hp_sys_date_reg_t sys_date;
} hp_system_dev_t;

extern hp_system_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_system_dev_t) == 0x400, "Invalid size of hp_system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
