/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Register */
/** Type of system_external_device_encrypt_decrypt_control register
 *  External device encryption/decryption configuration register
 */
typedef union {
    struct {
        /** system_enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable MSPI XTS manual encryption in SPI boot mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t system_enable_spi_manual_encrypt:1;
        /** system_enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t system_enable_download_db_encrypt:1;
        /** system_enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable MSPI XTS auto decryption in download boot mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t system_enable_download_g0cb_decrypt:1;
        /** system_enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable MSPI XTS manual encryption in download boot
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t system_enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_external_device_encrypt_decrypt_control_reg_t;

/** Type of system_sram_usage_conf register
 *  HP memory usage configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** system_sram_usage : R/W; bitpos: [12:8]; default: 0;
         *  0: cpu use hp-memory. 1:mac-dump accessing hp-memory.
         */
        uint32_t system_sram_usage:5;
        uint32_t reserved_13:3;
        /** system_mac_dump_alloc : R/W; bitpos: [16]; default: 0;
         *  Set this bit as 1 to add an offset (64KB) when mac-dump accessing hp-memory.
         */
        uint32_t system_mac_dump_alloc:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_sram_usage_conf_reg_t;

/** Type of system_sec_dpa_conf register
 *  HP anti-DPA security configuration register
 */
typedef union {
    struct {
        /** system_sec_dpa_level : R/W; bitpos: [1:0]; default: 0;
         *  Configures whether or not to enable anti-DPA attack. Valid only when
         *  HP_SYSTEM_SEC_DPA_CFG_SEL is 0.
         *  0: Disable
         *  1-3: Enable. The larger the number, the higher the security level, which represents
         *  the ability to resist DPA attacks, with increased computational overhead of the
         *  hardware crypto-accelerators at the same time.
         */
        uint32_t system_sec_dpa_level:2;
        /** system_sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  Configures whether to select HP_SYSTEM_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL (from
         *  eFuse) to control DPA level.
         *  0: Select EFUSE_SEC_DPA_LEVEL
         *  1: Select HP_SYSTEM_SEC_DPA_LEVEL
         */
        uint32_t system_sec_dpa_cfg_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_sec_dpa_conf_reg_t;

/** Type of system_rom_table_lock register
 *  ROM-Table lock register
 */
typedef union {
    struct {
        /** system_rom_table_lock : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to lock the value contained in HP_SYSTEM_ROM_TABLE.
         *  0: Unlock
         *  1: Lock
         */
        uint32_t system_rom_table_lock:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_rom_table_lock_reg_t;

/** Type of system_rom_table register
 *  ROM-Table register
 */
typedef union {
    struct {
        /** system_rom_table : R/W; bitpos: [31:0]; default: 0;
         *  Software ROM-Table register, whose content can be modified only when
         *  HP_SYSTEM_ROM_TABLE_LOCK is 0.
         */
        uint32_t system_rom_table:32;
    };
    uint32_t val;
} hp_system_rom_table_reg_t;

/** Type of system_core_debug_runstall_conf register
 *  Core Debug RunStall configurion register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** system_core_runstalled : RO; bitpos: [1]; default: 0;
         *  Software can read this field to get the runstall status of hp-core. 1: stalled, 0:
         *  not stalled.
         */
        uint32_t system_core_runstalled:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_core_debug_runstall_conf_reg_t;

/** Type of system_sprom_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** system_sprom_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 112;
         *  reserved
         */
        uint32_t system_sprom_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sprom_ctrl_reg_t;

/** Type of system_spram_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** system_spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  reserved
         */
        uint32_t system_spram_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_spram_ctrl_reg_t;

/** Type of system_sprf_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** system_sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 8304;
         *  reserved
         */
        uint32_t system_sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sprf_ctrl_reg_t;

/** Type of system_sdprf_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** system_sdprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t system_sdprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sdprf_ctrl_reg_t;

/** Type of system_axi_mst_pri register
 *  AXI mst priority configuration register
 */
typedef union {
    struct {
        /** system_dma_priority : R/W; bitpos: [0]; default: 0;
         *  AHB-DMA arbitration priority for command channels between masters connected to
         *  ext_mem_DW_axi
         */
        uint32_t system_dma_priority:1;
        /** system_cache_priority : R/W; bitpos: [1]; default: 0;
         *  CACHE arbitration priority for command channels between masters connected to
         *  ext_mem_DW_axi
         */
        uint32_t system_cache_priority:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_axi_mst_pri_reg_t;

/** Type of system_sdio_ctrl register
 *  SDIO Control configuration register
 */
typedef union {
    struct {
        /** system_dis_sdio_prob : R/W; bitpos: [0]; default: 1;
         *  Set this bit as 1 to disable SDIO_PROB function. disable by default.
         */
        uint32_t system_dis_sdio_prob:1;
        /** system_sdio_win_access_en : R/W; bitpos: [1]; default: 1;
         *  Enable sdio slave to access other peripherals on the chip
         */
        uint32_t system_sdio_win_access_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_sdio_ctrl_reg_t;


/** Group: Timeout Register */
/** Type of system_cpu_peri_timeout_conf register
 *  CPU_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** system_cpu_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t system_cpu_peri_timeout_thres:16;
        /** system_cpu_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t system_cpu_peri_timeout_int_clear:1;
        /** system_cpu_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU peripheral
         *  registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t system_cpu_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_cpu_peri_timeout_conf_reg_t;

/** Type of system_cpu_peri_timeout_addr register
 *  CPU_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** system_cpu_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t system_cpu_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_system_cpu_peri_timeout_addr_reg_t;

/** Type of system_cpu_peri_timeout_uid register
 *  CPU_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** system_cpu_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Represents the master id[4:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t system_cpu_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_system_cpu_peri_timeout_uid_reg_t;

/** Type of system_hp_peri_timeout_conf register
 *  HP_PERI_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** system_hp_peri_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing HP peripheral
         *  register, corresponding to the number of clock cycles of the clock domain.
         */
        uint32_t system_hp_peri_timeout_thres:16;
        /** system_hp_peri_timeout_int_clear : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to clear timeout interrupt.
         *  0: No effect
         *  1: Clear timeout interrupt
         */
        uint32_t system_hp_peri_timeout_int_clear:1;
        /** system_hp_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing HP peripheral
         *  registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t system_hp_peri_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_hp_peri_timeout_conf_reg_t;

/** Type of system_hp_peri_timeout_addr register
 *  HP_PERI_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** system_hp_peri_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t system_hp_peri_timeout_addr:32;
    };
    uint32_t val;
} hp_system_hp_peri_timeout_addr_reg_t;

/** Type of system_hp_peri_timeout_uid register
 *  HP_PERI_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** system_hp_peri_timeout_uid : RO; bitpos: [6:0]; default: 0;
         *  Represents the master id[4:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t system_hp_peri_timeout_uid:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_system_hp_peri_timeout_uid_reg_t;


/** Group: Version Register */
/** Type of system_date register
 *  Date control and version control register
 */
typedef union {
    struct {
        /** system_date : R/W; bitpos: [27:0]; default: 37823088;
         *  Version control register.
         */
        uint32_t system_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_system_date_reg_t;


typedef struct {
    volatile hp_system_external_device_encrypt_decrypt_control_reg_t system_external_device_encrypt_decrypt_control;
    volatile hp_system_sram_usage_conf_reg_t system_sram_usage_conf;
    volatile hp_system_sec_dpa_conf_reg_t system_sec_dpa_conf;
    volatile hp_system_cpu_peri_timeout_conf_reg_t system_cpu_peri_timeout_conf;
    volatile hp_system_cpu_peri_timeout_addr_reg_t system_cpu_peri_timeout_addr;
    volatile hp_system_cpu_peri_timeout_uid_reg_t system_cpu_peri_timeout_uid;
    volatile hp_system_hp_peri_timeout_conf_reg_t system_hp_peri_timeout_conf;
    volatile hp_system_hp_peri_timeout_addr_reg_t system_hp_peri_timeout_addr;
    volatile hp_system_hp_peri_timeout_uid_reg_t system_hp_peri_timeout_uid;
    uint32_t reserved_024[3];
    volatile hp_system_rom_table_lock_reg_t system_rom_table_lock;
    volatile hp_system_rom_table_reg_t system_rom_table;
    volatile hp_system_core_debug_runstall_conf_reg_t system_core_debug_runstall_conf;
    volatile hp_system_sprom_ctrl_reg_t system_sprom_ctrl;
    volatile hp_system_spram_ctrl_reg_t system_spram_ctrl;
    volatile hp_system_sprf_ctrl_reg_t system_sprf_ctrl;
    volatile hp_system_sdprf_ctrl_reg_t system_sdprf_ctrl;
    uint32_t reserved_04c[5];
    volatile hp_system_axi_mst_pri_reg_t system_axi_mst_pri;
    volatile hp_system_sdio_ctrl_reg_t system_sdio_ctrl;
    uint32_t reserved_068[229];
    volatile hp_system_date_reg_t system_date;
} hp_system_dev_t;

extern hp_system_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_system_dev_t) == 0x400, "Invalid size of hp_system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
