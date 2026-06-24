/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
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
         *  Configures whether or not to enable MSPI XTS manual encryption in SPI boot mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t enable_spi_manual_encrypt:1;
        /** enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t enable_download_db_encrypt:1;
        /** enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable MSPI XTS auto decryption in download boot mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t enable_download_g0cb_decrypt:1;
        /** enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable MSPI XTS manual encryption in download boot
         *  mode.
         *  0: Disable
         *  1: Enable
         */
        uint32_t enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_external_device_encrypt_decrypt_control_reg_t;

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
         *  Configures whether or not to lock the value contained in HP_SYSTEM_ROM_TABLE.
         *  0: Unlock
         *  1: Lock
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
         *  LP CPU.
         *  0: Disable
         *  1: Enable
         */
        uint32_t core_debug_runstall_enable:1;
        /** core0_runstalled : RO; bitpos: [1]; default: 0;
         *  Software can read this field to get the runstall status of hp-core0. 1: stalled, 0:
         *  not stalled.
         */
        uint32_t core0_runstalled:1;
        /** core1_runstalled : RO; bitpos: [2]; default: 0;
         *  Software can read this field to get the runstall status of hp-core1. 1: stalled, 0:
         *  not stalled.
         */
        uint32_t core1_runstalled:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_core_debug_runstall_conf_reg_t;

/** Type of sprom_ctrl register
 *  reserved
 */
typedef union {
    struct {
        /** sprom_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 80;
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
        /** spram_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 10320;
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
        /** sprf_mem_aux_ctrl : R/W; bitpos: [31:0]; default: 10320;
         *  reserved
         */
        uint32_t sprf_mem_aux_ctrl:32;
    };
    uint32_t val;
} hp_system_sprf_ctrl_reg_t;

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

/** Type of appcpu_boot_addr register
 *  reserved
 */
typedef union {
    struct {
        /** appcpu_boot_addr : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t appcpu_boot_addr:32;
    };
    uint32_t val;
} hp_system_appcpu_boot_addr_reg_t;

/** Type of axi_mst_pri register
 *  AXI mst priority configuration register
 */
typedef union {
    struct {
        /** dma_priority : R/W; bitpos: [0]; default: 0;
         *  AHB-DMA arbitration priority for command channels between masters connected to
         *  ext_mem_DW_axi
         */
        uint32_t dma_priority:1;
        /** cache_priority : R/W; bitpos: [1]; default: 0;
         *  CACHE arbitration priority for command channels between masters connected to
         *  ext_mem_DW_axi
         */
        uint32_t cache_priority:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_axi_mst_pri_reg_t;

/** Type of cpu_wakeup_event register
 *  NA
 */
typedef union {
    struct {
        /** cpu0_wakeup_event : R/W; bitpos: [0]; default: 0;
         *  Configures the cpu0 to exit WFI mode
         */
        uint32_t cpu0_wakeup_event:1;
        /** cpu1_wakeup_event : R/W; bitpos: [1]; default: 0;
         *  Configures the cpu1 to exit WFI mode
         */
        uint32_t cpu1_wakeup_event:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_cpu_wakeup_event_reg_t;

/** Type of rst_en register
 *  PCR clock gating configure register
 */
typedef union {
    struct {
        /** hpsysreg_rst_en : R/W; bitpos: [0]; default: 0;
         *  Set 0 to reset hp_system_reg module
         */
        uint32_t hpsysreg_rst_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_rst_en_reg_t;


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
         *  registers.
         *  0: Disable
         *  1: Enable
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
         *  Configures whether or not to clear timeout interrupt.
         *  0: No effect
         *  1: Clear timeout interrupt
         */
        uint32_t hp_peri_timeout_int_clear:1;
        /** hp_peri_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing HP peripheral
         *  registers.
         *  0: Disable
         *  1: Enable
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


/** Group: PMS Register */
/** Type of cpu_peri_pms_conf register
 *  CPU Peripherals PMS configuration register
 */
typedef union {
    struct {
        /** cpu_peri_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear cpu peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t cpu_peri_pms_exception_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_cpu_peri_pms_conf_reg_t;

/** Type of cpu_peri_pms_exception_info register
 *  CPU Peripherals PMS exception info record register
 */
typedef union {
    struct {
        /** cpu_peri_pms_exception_det : RO; bitpos: [0]; default: 0;
         *  Represents whether the cpu peripheral pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t cpu_peri_pms_exception_det:1;
        /** cpu_peri_pms_exception_id : RO; bitpos: [5:1]; default: 0;
         *  Represents the master id when cpu peripheral pms has been triggered.
         */
        uint32_t cpu_peri_pms_exception_id:5;
        /** cpu_peri_pms_exception_mode : RO; bitpos: [7:6]; default: 0;
         *  Represents the security mode when cpu peripheral pms has been triggered.
         */
        uint32_t cpu_peri_pms_exception_mode:2;
        /** cpu_peri_pms_exception_addr : RO; bitpos: [31:8]; default: 0;
         *  Represents the access address (bit23~bit0) when cpu peripheral pms has been
         *  triggered.
         */
        uint32_t cpu_peri_pms_exception_addr:24;
    };
    uint32_t val;
} hp_system_cpu_peri_pms_exception_info_reg_t;

/** Type of hp_peri_pms_conf register
 *  HP Peripherals PMS configuration register
 */
typedef union {
    struct {
        /** hp_peri_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear hp peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t hp_peri_pms_exception_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_hp_peri_pms_conf_reg_t;

/** Type of hp_peri_pms_exception_info register
 *  HP Peripherals PMS exception info record register
 */
typedef union {
    struct {
        /** hp_peri_pms_exception_det : RO; bitpos: [0]; default: 0;
         *  Represents whether the hp peripheral pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t hp_peri_pms_exception_det:1;
        /** hp_peri_pms_exception_id : RO; bitpos: [5:1]; default: 0;
         *  Represents the master id when hp peripheral pms has been triggered.
         */
        uint32_t hp_peri_pms_exception_id:5;
        /** hp_peri_pms_exception_mode : RO; bitpos: [7:6]; default: 0;
         *  Represents the security mode when hp peripheral pms has been triggered.
         */
        uint32_t hp_peri_pms_exception_mode:2;
        /** hp_peri_pms_exception_addr : RO; bitpos: [31:8]; default: 0;
         *  Represents the access address (bit23~bit0) when hp peripheral pms has been
         *  triggered.
         */
        uint32_t hp_peri_pms_exception_addr:24;
    };
    uint32_t val;
} hp_system_hp_peri_pms_exception_info_reg_t;

/** Type of modem_peri_pms_conf register
 *  MODEM Peripherals PMS configuration register
 */
typedef union {
    struct {
        /** modem_peri_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear modem peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t modem_peri_pms_exception_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_modem_peri_pms_conf_reg_t;

/** Type of modem_peri_pms_exception_info register
 *  MODEM Peripherals PMS exception info record register
 */
typedef union {
    struct {
        /** modem_peri_pms_exception_det : RO; bitpos: [0]; default: 0;
         *  Represents whether the modem peripheral pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t modem_peri_pms_exception_det:1;
        /** modem_peri_pms_exception_id : RO; bitpos: [5:1]; default: 0;
         *  Represents the master id when modem peripheral pms has been triggered.
         */
        uint32_t modem_peri_pms_exception_id:5;
        /** modem_peri_pms_exception_mode : RO; bitpos: [7:6]; default: 0;
         *  Represents the security mode when modem peripheral pms has been triggered.
         */
        uint32_t modem_peri_pms_exception_mode:2;
        /** modem_peri_pms_exception_addr : RO; bitpos: [31:8]; default: 0;
         *  Represents the access address (bit23~bit0) when modem peripheral pms has been
         *  triggered.
         */
        uint32_t modem_peri_pms_exception_addr:24;
    };
    uint32_t val;
} hp_system_modem_peri_pms_exception_info_reg_t;

/** Type of peri_pms_int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** hp_peri_pms_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable hp peri pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t hp_peri_pms_int_en:1;
        /** cpu_peri_pms_int_en : R/W; bitpos: [1]; default: 0;
         *  Configures to enable cpu peri pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t cpu_peri_pms_int_en:1;
        /** modem_peri_pms_int_en : R/W; bitpos: [2]; default: 0;
         *  Configures to enable modem peri pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t modem_peri_pms_int_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_peri_pms_int_en_reg_t;


/** Group: ID Register */
/** Type of id register
 *  ID register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** rom_id : RO; bitpos: [27:12]; default: 0;
         *  Represents the ROM ID of chip
         */
        uint32_t rom_id:16;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_system_id_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Date control and version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38813728;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_system_date_reg_t;


typedef struct {
    volatile hp_system_external_device_encrypt_decrypt_control_reg_t external_device_encrypt_decrypt_control;
    uint32_t reserved_004[2];
    volatile hp_system_cpu_peri_timeout_conf_reg_t cpu_peri_timeout_conf;
    volatile hp_system_cpu_peri_timeout_addr_reg_t cpu_peri_timeout_addr;
    volatile hp_system_cpu_peri_timeout_uid_reg_t cpu_peri_timeout_uid;
    volatile hp_system_hp_peri_timeout_conf_reg_t hp_peri_timeout_conf;
    volatile hp_system_hp_peri_timeout_addr_reg_t hp_peri_timeout_addr;
    volatile hp_system_hp_peri_timeout_uid_reg_t hp_peri_timeout_uid;
    uint32_t reserved_024[3];
    volatile hp_system_sdio_ctrl_reg_t sdio_ctrl;
    uint32_t reserved_034;
    volatile hp_system_rom_table_lock_reg_t rom_table_lock;
    volatile hp_system_rom_table_reg_t rom_table;
    volatile hp_system_core_debug_runstall_conf_reg_t core_debug_runstall_conf;
    uint32_t reserved_044[11];
    volatile hp_system_sprom_ctrl_reg_t sprom_ctrl;
    volatile hp_system_spram_ctrl_reg_t spram_ctrl;
    volatile hp_system_sprf_ctrl_reg_t sprf_ctrl;
    uint32_t reserved_07c;
    volatile hp_system_bitscrambler_peri_sel_reg_t bitscrambler_peri_sel;
    volatile hp_system_appcpu_boot_addr_reg_t appcpu_boot_addr;
    volatile hp_system_axi_mst_pri_reg_t axi_mst_pri;
    uint32_t reserved_08c;
    volatile hp_system_cpu_peri_pms_conf_reg_t cpu_peri_pms_conf;
    volatile hp_system_cpu_peri_pms_exception_info_reg_t cpu_peri_pms_exception_info;
    volatile hp_system_hp_peri_pms_conf_reg_t hp_peri_pms_conf;
    volatile hp_system_hp_peri_pms_exception_info_reg_t hp_peri_pms_exception_info;
    volatile hp_system_modem_peri_pms_conf_reg_t modem_peri_pms_conf;
    volatile hp_system_modem_peri_pms_exception_info_reg_t modem_peri_pms_exception_info;
    volatile hp_system_peri_pms_int_en_reg_t peri_pms_int_en;
    uint32_t reserved_0ac[5];
    volatile hp_system_cpu_wakeup_event_reg_t cpu_wakeup_event;
    uint32_t reserved_0c4[198];
    volatile hp_system_id_reg_t id;
    uint32_t reserved_3e0[4];
    volatile hp_system_rst_en_reg_t rst_en;
    uint32_t reserved_3f4[2];
    volatile hp_system_date_reg_t date;
} hp_system_dev_t;

extern hp_system_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_system_dev_t) == 0x400, "Invalid size of hp_system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
