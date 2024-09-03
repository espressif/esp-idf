/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Type of blk0_rdata0 register */
typedef union {
    struct {
        /** rd_efuse_wr_dis : R; bitpos: [15:0]; default: 0;
         *  read for efuse_wr_disable
         */
        uint32_t rd_efuse_wr_dis:16;
        /** rd_efuse_rd_dis : R; bitpos: [19:16]; default: 0;
         *  read for efuse_rd_disable
         */
        uint32_t rd_efuse_rd_dis:4;
        /** rd_flash_crypt_cnt : R; bitpos: [26:20]; default: 0;
         *  read for flash_crypt_cnt
         */
        uint32_t rd_flash_crypt_cnt:7;
        /** rd_uart_download_dis : R; bitpos: [27]; default: 0;
         *  Disable UART download mode. Valid for ESP32 V3 and newer, only
         */
        uint32_t rd_uart_download_dis:1;
        /** reserved_0_28 : R; bitpos: [31:28]; default: 0;
         *  reserved
         */
        uint32_t reserved_0_28:4;
    };
    uint32_t val;
} efuse_blk0_rdata0_reg_t;

/** Type of blk0_rdata1 register */
typedef union {
    struct {
        /** rd_mac : R; bitpos: [31:0]; default: 0;
         *  MAC address
         */
        uint32_t rd_mac:32;
    };
    uint32_t val;
} efuse_blk0_rdata1_reg_t;

/** Type of blk0_rdata2 register */
typedef union {
    struct {
        /** rd_mac_1 : R; bitpos: [15:0]; default: 0;
         *  MAC address
         */
        uint32_t rd_mac_1:16;
        /** rd_mac_crc : R; bitpos: [23:16]; default: 0;
         *  CRC8 for MAC address
         */
        uint32_t rd_mac_crc:8;
        /** rd_reserve_0_88 : RW; bitpos: [31:24]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_88:8;
    };
    uint32_t val;
} efuse_blk0_rdata2_reg_t;

/** Type of blk0_rdata3 register */
typedef union {
    struct {
        /** rd_disable_app_cpu : R; bitpos: [0]; default: 0;
         *  Disables APP CPU
         */
        uint32_t rd_disable_app_cpu:1;
        /** rd_disable_bt : R; bitpos: [1]; default: 0;
         *  Disables Bluetooth
         */
        uint32_t rd_disable_bt:1;
        /** rd_chip_package_4bit : R; bitpos: [2]; default: 0;
         *  Chip package identifier #4bit
         */
        uint32_t rd_chip_package_4bit:1;
        /** rd_dis_cache : R; bitpos: [3]; default: 0;
         *  Disables cache
         */
        uint32_t rd_dis_cache:1;
        /** rd_spi_pad_config_hd : R; bitpos: [8:4]; default: 0;
         *  read for SPI_pad_config_hd
         */
        uint32_t rd_spi_pad_config_hd:5;
        /** rd_chip_package : RW; bitpos: [11:9]; default: 0;
         *  Chip package identifier
         */
        uint32_t rd_chip_package:3;
        /** rd_chip_cpu_freq_low : RW; bitpos: [12]; default: 0;
         *  If set alongside EFUSE_RD_CHIP_CPU_FREQ_RATED, the ESP32's max CPU frequency is
         *  rated for 160MHz. 240MHz otherwise
         */
        uint32_t rd_chip_cpu_freq_low:1;
        /** rd_chip_cpu_freq_rated : RW; bitpos: [13]; default: 0;
         *  If set, the ESP32's maximum CPU frequency has been rated
         */
        uint32_t rd_chip_cpu_freq_rated:1;
        /** rd_blk3_part_reserve : RW; bitpos: [14]; default: 0;
         *  If set, this bit indicates that BLOCK3[143:96] is reserved for internal use
         */
        uint32_t rd_blk3_part_reserve:1;
        /** rd_chip_ver_rev1 : RW; bitpos: [15]; default: 0;
         *  bit is set to 1 for rev1 silicon
         */
        uint32_t rd_chip_ver_rev1:1;
        /** rd_reserve_0_112 : RW; bitpos: [31:16]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_112:16;
    };
    uint32_t val;
} efuse_blk0_rdata3_reg_t;

/** Type of blk0_rdata4 register */
typedef union {
    struct {
        /** rd_clk8m_freq : R; bitpos: [7:0]; default: 0;
         *  8MHz clock freq override
         */
        uint32_t rd_clk8m_freq:8;
        /** rd_adc_vref : RW; bitpos: [12:8]; default: 0;
         *  True ADC reference voltage
         */
        uint32_t rd_adc_vref:5;
        /** rd_reserve_0_141 : RW; bitpos: [13]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_141:1;
        /** rd_xpd_sdio_reg : R; bitpos: [14]; default: 0;
         *  read for XPD_SDIO_REG
         */
        uint32_t rd_xpd_sdio_reg:1;
        /** rd_xpd_sdio_tieh : R; bitpos: [15]; default: 0;
         *  If XPD_SDIO_FORCE & XPD_SDIO_REG
         */
        uint32_t rd_xpd_sdio_tieh:1;
        /** rd_xpd_sdio_force : R; bitpos: [16]; default: 0;
         *  Ignore MTDI pin (GPIO12) for VDD_SDIO on reset
         */
        uint32_t rd_xpd_sdio_force:1;
        /** rd_reserve_0_145 : RW; bitpos: [31:17]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_145:15;
    };
    uint32_t val;
} efuse_blk0_rdata4_reg_t;

/** Type of blk0_rdata5 register */
typedef union {
    struct {
        /** rd_spi_pad_config_clk : R; bitpos: [4:0]; default: 0;
         *  read for SPI_pad_config_clk
         */
        uint32_t rd_spi_pad_config_clk:5;
        /** rd_spi_pad_config_q : R; bitpos: [9:5]; default: 0;
         *  read for SPI_pad_config_q
         */
        uint32_t rd_spi_pad_config_q:5;
        /** rd_spi_pad_config_d : R; bitpos: [14:10]; default: 0;
         *  read for SPI_pad_config_d
         */
        uint32_t rd_spi_pad_config_d:5;
        /** rd_spi_pad_config_cs0 : R; bitpos: [19:15]; default: 0;
         *  read for SPI_pad_config_cs0
         */
        uint32_t rd_spi_pad_config_cs0:5;
        /** rd_chip_ver_rev2 : R; bitpos: [20]; default: 0; */
        uint32_t rd_chip_ver_rev2:1;
        /** rd_reserve_0_181 : RW; bitpos: [21]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_181:1;
        /** rd_vol_level_hp_inv : R; bitpos: [23:22]; default: 0;
         *  This field stores the voltage level for CPU to run at 240 MHz, or for flash/PSRAM
         *  to run at 80 MHz.0x0: level 7; 0x1: level 6; 0x2: level 5; 0x3: level 4. (RO)
         */
        uint32_t rd_vol_level_hp_inv:2;
        /** rd_wafer_version_minor : R; bitpos: [25:24]; default: 0; */
        uint32_t rd_wafer_version_minor:2;
        /** rd_reserve_0_186 : RW; bitpos: [27:26]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_186:2;
        /** rd_flash_crypt_config : R; bitpos: [31:28]; default: 0;
         *  read for flash_crypt_config
         */
        uint32_t rd_flash_crypt_config:4;
    };
    uint32_t val;
} efuse_blk0_rdata5_reg_t;

/** Type of blk0_rdata6 register */
typedef union {
    struct {
        /** rd_coding_scheme : R; bitpos: [1:0]; default: 0;
         *  read for coding_scheme
         */
        uint32_t rd_coding_scheme:2;
        /** rd_console_debug_disable : R; bitpos: [2]; default: 0;
         *  read for console_debug_disable
         */
        uint32_t rd_console_debug_disable:1;
        /** rd_disable_sdio_host : R; bitpos: [3]; default: 0; */
        uint32_t rd_disable_sdio_host:1;
        /** rd_abs_done_0 : R; bitpos: [4]; default: 0;
         *  read for abstract_done_0
         */
        uint32_t rd_abs_done_0:1;
        /** rd_abs_done_1 : R; bitpos: [5]; default: 0;
         *  read for abstract_done_1
         */
        uint32_t rd_abs_done_1:1;
        /** rd_jtag_disable : R; bitpos: [6]; default: 0;
         *  Disable JTAG
         */
        uint32_t rd_jtag_disable:1;
        /** rd_disable_dl_encrypt : R; bitpos: [7]; default: 0;
         *  read for download_dis_encrypt
         */
        uint32_t rd_disable_dl_encrypt:1;
        /** rd_disable_dl_decrypt : R; bitpos: [8]; default: 0;
         *  read for download_dis_decrypt
         */
        uint32_t rd_disable_dl_decrypt:1;
        /** rd_disable_dl_cache : R; bitpos: [9]; default: 0;
         *  read for download_dis_cache
         */
        uint32_t rd_disable_dl_cache:1;
        /** rd_key_status : R; bitpos: [10]; default: 0;
         *  read for key_status
         */
        uint32_t rd_key_status:1;
        /** rd_reserve_0_203 : RW; bitpos: [31:11]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_203:21;
    };
    uint32_t val;
} efuse_blk0_rdata6_reg_t;

/** Type of blk0_wdata0 register */
typedef union {
    struct {
        /** wr_dis : RW; bitpos: [15:0]; default: 0;
         *  program for efuse_wr_disable
         */
        uint32_t wr_dis:16;
        /** rd_dis : RW; bitpos: [19:16]; default: 0;
         *  program for efuse_rd_disable
         */
        uint32_t rd_dis:4;
        /** flash_crypt_cnt : RW; bitpos: [26:20]; default: 0;
         *  program for flash_crypt_cnt
         */
        uint32_t flash_crypt_cnt:7;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} efuse_blk0_wdata0_reg_t;

/** Type of blk0_wdata1 register */
typedef union {
    struct {
        /** wifi_mac_crc_low : RW; bitpos: [31:0]; default: 0;
         *  program for low 32bit WIFI_MAC_Address
         */
        uint32_t wifi_mac_crc_low:32;
    };
    uint32_t val;
} efuse_blk0_wdata1_reg_t;

/** Type of blk0_wdata2 register */
typedef union {
    struct {
        /** wifi_mac_crc_high : RW; bitpos: [23:0]; default: 0;
         *  program for high 24bit WIFI_MAC_Address
         */
        uint32_t wifi_mac_crc_high:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_blk0_wdata2_reg_t;

/** Type of blk0_wdata3 register */
typedef union {
    struct {
        /** disable_app_cpu : R; bitpos: [0]; default: 0;
         *  Disables APP CPU
         */
        uint32_t disable_app_cpu:1;
        /** disable_bt : R; bitpos: [1]; default: 0;
         *  Disables Bluetooth
         */
        uint32_t disable_bt:1;
        /** chip_package_4bit : R; bitpos: [2]; default: 0;
         *  Chip package identifier #4bit
         */
        uint32_t chip_package_4bit:1;
        /** dis_cache : R; bitpos: [3]; default: 0;
         *  Disables cache
         */
        uint32_t dis_cache:1;
        /** spi_pad_config_hd : R; bitpos: [8:4]; default: 0;
         *  program for SPI_pad_config_hd
         */
        uint32_t spi_pad_config_hd:5;
        /** chip_package : RW; bitpos: [11:9]; default: 0;
         *  Chip package identifier
         */
        uint32_t chip_package:3;
        /** chip_cpu_freq_low : RW; bitpos: [12]; default: 0;
         *  If set alongside EFUSE_RD_CHIP_CPU_FREQ_RATED, the ESP32's max CPU frequency is
         *  rated for 160MHz. 240MHz otherwise
         */
        uint32_t chip_cpu_freq_low:1;
        /** chip_cpu_freq_rated : RW; bitpos: [13]; default: 0;
         *  If set, the ESP32's maximum CPU frequency has been rated
         */
        uint32_t chip_cpu_freq_rated:1;
        /** blk3_part_reserve : RW; bitpos: [14]; default: 0;
         *  If set, this bit indicates that BLOCK3[143:96] is reserved for internal use
         */
        uint32_t blk3_part_reserve:1;
        /** chip_ver_rev1 : RW; bitpos: [15]; default: 0;
         *  bit is set to 1 for rev1 silicon
         */
        uint32_t chip_ver_rev1:1;
        /** reserve_0_112 : RW; bitpos: [31:16]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t reserve_0_112:16;
    };
    uint32_t val;
} efuse_blk0_wdata3_reg_t;

/** Type of blk0_wdata4 register */
typedef union {
    struct {
        /** clk8m_freq : R; bitpos: [7:0]; default: 0;
         *  8MHz clock freq override
         */
        uint32_t clk8m_freq:8;
        /** adc_vref : RW; bitpos: [12:8]; default: 0;
         *  True ADC reference voltage
         */
        uint32_t adc_vref:5;
        /** reserve_0_141 : RW; bitpos: [13]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t reserve_0_141:1;
        /** xpd_sdio_reg : R; bitpos: [14]; default: 0;
         *  program for XPD_SDIO_REG
         */
        uint32_t xpd_sdio_reg:1;
        /** xpd_sdio_tieh : R; bitpos: [15]; default: 0;
         *  If XPD_SDIO_FORCE & XPD_SDIO_REG
         */
        uint32_t xpd_sdio_tieh:1;
        /** xpd_sdio_force : R; bitpos: [16]; default: 0;
         *  Ignore MTDI pin (GPIO12) for VDD_SDIO on reset
         */
        uint32_t xpd_sdio_force:1;
        /** reserve_0_145 : RW; bitpos: [31:17]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t reserve_0_145:15;
    };
    uint32_t val;
} efuse_blk0_wdata4_reg_t;

/** Type of blk0_wdata5 register */
typedef union {
    struct {
        /** spi_pad_config_clk : R; bitpos: [4:0]; default: 0;
         *  program for SPI_pad_config_clk
         */
        uint32_t spi_pad_config_clk:5;
        /** spi_pad_config_q : R; bitpos: [9:5]; default: 0;
         *  program for SPI_pad_config_q
         */
        uint32_t spi_pad_config_q:5;
        /** spi_pad_config_d : R; bitpos: [14:10]; default: 0;
         *  program for SPI_pad_config_d
         */
        uint32_t spi_pad_config_d:5;
        /** spi_pad_config_cs0 : R; bitpos: [19:15]; default: 0;
         *  program for SPI_pad_config_cs0
         */
        uint32_t spi_pad_config_cs0:5;
        /** chip_ver_rev2 : R; bitpos: [20]; default: 0; */
        uint32_t chip_ver_rev2:1;
        /** reserve_0_181 : RW; bitpos: [21]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t reserve_0_181:1;
        /** vol_level_hp_inv : R; bitpos: [23:22]; default: 0;
         *  This field stores the voltage level for CPU to run at 240 MHz, or for flash/PSRAM
         *  to run at 80 MHz.0x0: level 7; 0x1: level 6; 0x2: level 5; 0x3: level 4. (RO)
         */
        uint32_t vol_level_hp_inv:2;
        /** wafer_version_minor : R; bitpos: [25:24]; default: 0; */
        uint32_t wafer_version_minor:2;
        /** reserve_0_186 : RW; bitpos: [27:26]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t reserve_0_186:2;
        /** flash_crypt_config : R; bitpos: [31:28]; default: 0;
         *  program for flash_crypt_config
         */
        uint32_t flash_crypt_config:4;
    };
    uint32_t val;
} efuse_blk0_wdata5_reg_t;

/** Type of blk0_wdata6 register */
typedef union {
    struct {
        /** coding_scheme : RW; bitpos: [1:0]; default: 0;
         *  program for coding_scheme
         */
        uint32_t coding_scheme:2;
        /** console_debug_disable : RW; bitpos: [2]; default: 0;
         *  program for console_debug_disable
         */
        uint32_t console_debug_disable:1;
        /** disable_sdio_host : RW; bitpos: [3]; default: 0; */
        uint32_t disable_sdio_host:1;
        /** abs_done_0 : RW; bitpos: [4]; default: 0;
         *  program for abstract_done_0
         */
        uint32_t abs_done_0:1;
        /** abs_done_1 : RW; bitpos: [5]; default: 0;
         *  program for abstract_done_1
         */
        uint32_t abs_done_1:1;
        /** disable_jtag : RW; bitpos: [6]; default: 0;
         *  program for JTAG_disable
         */
        uint32_t disable_jtag:1;
        /** disable_dl_encrypt : RW; bitpos: [7]; default: 0;
         *  program for download_dis_encrypt
         */
        uint32_t disable_dl_encrypt:1;
        /** disable_dl_decrypt : RW; bitpos: [8]; default: 0;
         *  program for download_dis_decrypt
         */
        uint32_t disable_dl_decrypt:1;
        /** disable_dl_cache : RW; bitpos: [9]; default: 0;
         *  program for download_dis_cache
         */
        uint32_t disable_dl_cache:1;
        /** key_status : RW; bitpos: [10]; default: 0;
         *  program for key_status
         */
        uint32_t key_status:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} efuse_blk0_wdata6_reg_t;

/** Type of blk1_rdata0 register */
typedef union {
    struct {
        /** rd_block1 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1:32;
    };
    uint32_t val;
} efuse_blk1_rdata0_reg_t;

/** Type of blk1_rdata1 register */
typedef union {
    struct {
        /** rd_block1_1 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_1:32;
    };
    uint32_t val;
} efuse_blk1_rdata1_reg_t;

/** Type of blk1_rdata2 register */
typedef union {
    struct {
        /** rd_block1_2 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_2:32;
    };
    uint32_t val;
} efuse_blk1_rdata2_reg_t;

/** Type of blk1_rdata3 register */
typedef union {
    struct {
        /** rd_block1_3 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_3:32;
    };
    uint32_t val;
} efuse_blk1_rdata3_reg_t;

/** Type of blk1_rdata4 register */
typedef union {
    struct {
        /** rd_block1_4 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_4:32;
    };
    uint32_t val;
} efuse_blk1_rdata4_reg_t;

/** Type of blk1_rdata5 register */
typedef union {
    struct {
        /** rd_block1_5 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_5:32;
    };
    uint32_t val;
} efuse_blk1_rdata5_reg_t;

/** Type of blk1_rdata6 register */
typedef union {
    struct {
        /** rd_block1_6 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_6:32;
    };
    uint32_t val;
} efuse_blk1_rdata6_reg_t;

/** Type of blk1_rdata7 register */
typedef union {
    struct {
        /** rd_block1_7 : R; bitpos: [31:0]; default: 0;
         *  Flash encryption key
         */
        uint32_t rd_block1_7:32;
    };
    uint32_t val;
} efuse_blk1_rdata7_reg_t;

/** Type of blk2_rdata0 register */
typedef union {
    struct {
        /** rd_block2 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2:32;
    };
    uint32_t val;
} efuse_blk2_rdata0_reg_t;

/** Type of blk2_rdata1 register */
typedef union {
    struct {
        /** rd_block2_1 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_1:32;
    };
    uint32_t val;
} efuse_blk2_rdata1_reg_t;

/** Type of blk2_rdata2 register */
typedef union {
    struct {
        /** rd_block2_2 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_2:32;
    };
    uint32_t val;
} efuse_blk2_rdata2_reg_t;

/** Type of blk2_rdata3 register */
typedef union {
    struct {
        /** rd_block2_3 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_3:32;
    };
    uint32_t val;
} efuse_blk2_rdata3_reg_t;

/** Type of blk2_rdata4 register */
typedef union {
    struct {
        /** rd_block2_4 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_4:32;
    };
    uint32_t val;
} efuse_blk2_rdata4_reg_t;

/** Type of blk2_rdata5 register */
typedef union {
    struct {
        /** rd_block2_5 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_5:32;
    };
    uint32_t val;
} efuse_blk2_rdata5_reg_t;

/** Type of blk2_rdata6 register */
typedef union {
    struct {
        /** rd_block2_6 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_6:32;
    };
    uint32_t val;
} efuse_blk2_rdata6_reg_t;

/** Type of blk2_rdata7 register */
typedef union {
    struct {
        /** rd_block2_7 : R; bitpos: [31:0]; default: 0;
         *  Security boot key
         */
        uint32_t rd_block2_7:32;
    };
    uint32_t val;
} efuse_blk2_rdata7_reg_t;

/** Type of blk3_rdata0 register */
typedef union {
    struct {
        /** rd_custom_mac_crc : R; bitpos: [7:0]; default: 0;
         *  CRC8 for custom MAC address
         */
        uint32_t rd_custom_mac_crc:8;
        /** rd_custom_mac : R; bitpos: [31:8]; default: 0;
         *  Custom MAC address
         */
        uint32_t rd_custom_mac:24;
    };
    uint32_t val;
} efuse_blk3_rdata0_reg_t;

/** Type of blk3_rdata1 register */
typedef union {
    struct {
        /** rd_custom_mac_1 : R; bitpos: [23:0]; default: 0;
         *  Custom MAC address
         */
        uint32_t rd_custom_mac_1:24;
        /** reserved_3_56 : R; bitpos: [31:24]; default: 0;
         *  reserved
         */
        uint32_t reserved_3_56:8;
    };
    uint32_t val;
} efuse_blk3_rdata1_reg_t;

/** Type of blk3_rdata2 register */
typedef union {
    struct {
        /** rd_blk3_reserved_2 : R; bitpos: [31:0]; default: 0;
         *  read for BLOCK3
         */
        uint32_t rd_blk3_reserved_2:32;
    };
    uint32_t val;
} efuse_blk3_rdata2_reg_t;

/** Type of blk3_rdata3 register */
typedef union {
    struct {
        /** rd_adc1_tp_low : RW; bitpos: [6:0]; default: 0;
         *  ADC1 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t rd_adc1_tp_low:7;
        /** rd_adc1_tp_high : RW; bitpos: [15:7]; default: 0;
         *  ADC1 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t rd_adc1_tp_high:9;
        /** rd_adc2_tp_low : RW; bitpos: [22:16]; default: 0;
         *  ADC2 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t rd_adc2_tp_low:7;
        /** rd_adc2_tp_high : RW; bitpos: [31:23]; default: 0;
         *  ADC2 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t rd_adc2_tp_high:9;
    };
    uint32_t val;
} efuse_blk3_rdata3_reg_t;

/** Type of blk3_rdata4 register */
typedef union {
    struct {
        /** rd_secure_version : R; bitpos: [31:0]; default: 0;
         *  Secure version for anti-rollback
         */
        uint32_t rd_secure_version:32;
    };
    uint32_t val;
} efuse_blk3_rdata4_reg_t;

/** Type of blk3_rdata5 register */
typedef union {
    struct {
        /** reserved_3_160 : R; bitpos: [23:0]; default: 0;
         *  reserved
         */
        uint32_t reserved_3_160:24;
        /** rd_mac_version : R; bitpos: [31:24]; default: 0;
         *  Custom MAC version
         */
        uint32_t rd_mac_version:8;
    };
    uint32_t val;
} efuse_blk3_rdata5_reg_t;

/** Type of blk3_rdata6 register */
typedef union {
    struct {
        /** rd_blk3_reserved_6 : R; bitpos: [31:0]; default: 0;
         *  read for BLOCK3
         */
        uint32_t rd_blk3_reserved_6:32;
    };
    uint32_t val;
} efuse_blk3_rdata6_reg_t;

/** Type of blk3_rdata7 register */
typedef union {
    struct {
        /** rd_blk3_reserved_7 : R; bitpos: [31:0]; default: 0;
         *  read for BLOCK3
         */
        uint32_t rd_blk3_reserved_7:32;
    };
    uint32_t val;
} efuse_blk3_rdata7_reg_t;

/** Type of blk1_wdata0 register */
typedef union {
    struct {
        /** blk1_din0 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din0:32;
    };
    uint32_t val;
} efuse_blk1_wdata0_reg_t;

/** Type of blk1_wdata1 register */
typedef union {
    struct {
        /** blk1_din1 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din1:32;
    };
    uint32_t val;
} efuse_blk1_wdata1_reg_t;

/** Type of blk1_wdata2 register */
typedef union {
    struct {
        /** blk1_din2 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din2:32;
    };
    uint32_t val;
} efuse_blk1_wdata2_reg_t;

/** Type of blk1_wdata3 register */
typedef union {
    struct {
        /** blk1_din3 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din3:32;
    };
    uint32_t val;
} efuse_blk1_wdata3_reg_t;

/** Type of blk1_wdata4 register */
typedef union {
    struct {
        /** blk1_din4 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din4:32;
    };
    uint32_t val;
} efuse_blk1_wdata4_reg_t;

/** Type of blk1_wdata5 register */
typedef union {
    struct {
        /** blk1_din5 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din5:32;
    };
    uint32_t val;
} efuse_blk1_wdata5_reg_t;

/** Type of blk1_wdata6 register */
typedef union {
    struct {
        /** blk1_din6 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din6:32;
    };
    uint32_t val;
} efuse_blk1_wdata6_reg_t;

/** Type of blk1_wdata7 register */
typedef union {
    struct {
        /** blk1_din7 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK1
         */
        uint32_t blk1_din7:32;
    };
    uint32_t val;
} efuse_blk1_wdata7_reg_t;

/** Type of blk2_wdata0 register */
typedef union {
    struct {
        /** blk2_din0 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din0:32;
    };
    uint32_t val;
} efuse_blk2_wdata0_reg_t;

/** Type of blk2_wdata1 register */
typedef union {
    struct {
        /** blk2_din1 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din1:32;
    };
    uint32_t val;
} efuse_blk2_wdata1_reg_t;

/** Type of blk2_wdata2 register */
typedef union {
    struct {
        /** blk2_din2 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din2:32;
    };
    uint32_t val;
} efuse_blk2_wdata2_reg_t;

/** Type of blk2_wdata3 register */
typedef union {
    struct {
        /** blk2_din3 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din3:32;
    };
    uint32_t val;
} efuse_blk2_wdata3_reg_t;

/** Type of blk2_wdata4 register */
typedef union {
    struct {
        /** blk2_din4 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din4:32;
    };
    uint32_t val;
} efuse_blk2_wdata4_reg_t;

/** Type of blk2_wdata5 register */
typedef union {
    struct {
        /** blk2_din5 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din5:32;
    };
    uint32_t val;
} efuse_blk2_wdata5_reg_t;

/** Type of blk2_wdata6 register */
typedef union {
    struct {
        /** blk2_din6 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din6:32;
    };
    uint32_t val;
} efuse_blk2_wdata6_reg_t;

/** Type of blk2_wdata7 register */
typedef union {
    struct {
        /** blk2_din7 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK2
         */
        uint32_t blk2_din7:32;
    };
    uint32_t val;
} efuse_blk2_wdata7_reg_t;

/** Type of blk3_wdata0 register */
typedef union {
    struct {
        /** blk3_din0 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK3
         */
        uint32_t blk3_din0:32;
    };
    uint32_t val;
} efuse_blk3_wdata0_reg_t;

/** Type of blk3_wdata1 register */
typedef union {
    struct {
        /** blk3_din1 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK3
         */
        uint32_t blk3_din1:32;
    };
    uint32_t val;
} efuse_blk3_wdata1_reg_t;

/** Type of blk3_wdata2 register */
typedef union {
    struct {
        /** blk3_din2 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK3
         */
        uint32_t blk3_din2:32;
    };
    uint32_t val;
} efuse_blk3_wdata2_reg_t;

/** Type of blk3_wdata3 register */
typedef union {
    struct {
        /** adc1_tp_low : RW; bitpos: [6:0]; default: 0;
         *  ADC1 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t adc1_tp_low:7;
        /** adc1_tp_high : RW; bitpos: [15:7]; default: 0;
         *  ADC1 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t adc1_tp_high:9;
        /** adc2_tp_low : RW; bitpos: [22:16]; default: 0;
         *  ADC2 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t adc2_tp_low:7;
        /** adc2_tp_high : RW; bitpos: [31:23]; default: 0;
         *  ADC2 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
         */
        uint32_t adc2_tp_high:9;
    };
    uint32_t val;
} efuse_blk3_wdata3_reg_t;

/** Type of blk3_wdata4 register */
typedef union {
    struct {
        /** secure_version : R; bitpos: [31:0]; default: 0;
         *  Secure version for anti-rollback
         */
        uint32_t secure_version:32;
    };
    uint32_t val;
} efuse_blk3_wdata4_reg_t;

/** Type of blk3_wdata5 register */
typedef union {
    struct {
        /** blk3_din5 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK3
         */
        uint32_t blk3_din5:32;
    };
    uint32_t val;
} efuse_blk3_wdata5_reg_t;

/** Type of blk3_wdata6 register */
typedef union {
    struct {
        /** blk3_din6 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK3
         */
        uint32_t blk3_din6:32;
    };
    uint32_t val;
} efuse_blk3_wdata6_reg_t;

/** Type of blk3_wdata7 register */
typedef union {
    struct {
        /** blk3_din7 : RW; bitpos: [31:0]; default: 0;
         *  program for BLOCK3
         */
        uint32_t blk3_din7:32;
    };
    uint32_t val;
} efuse_blk3_wdata7_reg_t;

/** Type of clk register */
typedef union {
    struct {
        /** clk_sel0 : RW; bitpos: [7:0]; default: 82;
         *  efuse timing configure
         */
        uint32_t clk_sel0:8;
        /** clk_sel1 : RW; bitpos: [15:8]; default: 64;
         *  efuse timing configure
         */
        uint32_t clk_sel1:8;
        /** clk_en : RW; bitpos: [16]; default: 0; */
        uint32_t clk_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} efuse_clk_reg_t;

/** Type of conf register */
typedef union {
    struct {
        /** op_code : RW; bitpos: [15:0]; default: 0;
         *  efuse operation code
         */
        uint32_t op_code:16;
        /** force_no_wr_rd_dis : RW; bitpos: [16]; default: 1; */
        uint32_t force_no_wr_rd_dis:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} efuse_conf_reg_t;

/** Type of status register */
typedef union {
    struct {
        /** debug : R; bitpos: [31:0]; default: 0; */
        uint32_t debug:32;
    };
    uint32_t val;
} efuse_status_reg_t;

/** Type of cmd register */
typedef union {
    struct {
        /** read_cmd : RW; bitpos: [0]; default: 0;
         *  command for read
         */
        uint32_t read_cmd:1;
        /** pgm_cmd : RW; bitpos: [1]; default: 0;
         *  command for program
         */
        uint32_t pgm_cmd:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_cmd_reg_t;

/** Type of int_raw register */
typedef union {
    struct {
        /** read_done_int_raw : R; bitpos: [0]; default: 0;
         *  read done interrupt raw status
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : R; bitpos: [1]; default: 0;
         *  program done interrupt raw status
         */
        uint32_t pgm_done_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_raw_reg_t;

/** Type of int_st register */
typedef union {
    struct {
        /** read_done_int_st : R; bitpos: [0]; default: 0;
         *  read done interrupt status
         */
        uint32_t read_done_int_st:1;
        /** pgm_done_int_st : R; bitpos: [1]; default: 0;
         *  program done interrupt status
         */
        uint32_t pgm_done_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_st_reg_t;

/** Type of int_ena register */
typedef union {
    struct {
        /** read_done_int_ena : RW; bitpos: [0]; default: 0;
         *  read done interrupt enable
         */
        uint32_t read_done_int_ena:1;
        /** pgm_done_int_ena : RW; bitpos: [1]; default: 0;
         *  program done interrupt enable
         */
        uint32_t pgm_done_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_ena_reg_t;

/** Type of int_clr register */
typedef union {
    struct {
        /** read_done_int_clr : W; bitpos: [0]; default: 0;
         *  read done interrupt clear
         */
        uint32_t read_done_int_clr:1;
        /** pgm_done_int_clr : W; bitpos: [1]; default: 0;
         *  program done interrupt clear
         */
        uint32_t pgm_done_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_clr_reg_t;

/** Type of dac_conf register */
typedef union {
    struct {
        /** dac_clk_div : RW; bitpos: [7:0]; default: 40;
         *  efuse timing configure
         */
        uint32_t dac_clk_div:8;
        /** dac_clk_pad_sel : RW; bitpos: [8]; default: 0; */
        uint32_t dac_clk_pad_sel:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} efuse_dac_conf_reg_t;

/** Type of dec_status register */
typedef union {
    struct {
        /** dec_warnings : R; bitpos: [11:0]; default: 0;
         *  the decode result of 3/4 coding scheme has warning
         */
        uint32_t dec_warnings:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} efuse_dec_status_reg_t;

/** Type of date register */
typedef union {
    struct {
        /** date : RW; bitpos: [31:0]; default: 369370624; */
        uint32_t date:32;
    };
    uint32_t val;
} efuse_date_reg_t;


typedef struct {
    volatile efuse_blk0_rdata0_reg_t blk0_rdata0;
    volatile efuse_blk0_rdata1_reg_t blk0_rdata1;
    volatile efuse_blk0_rdata2_reg_t blk0_rdata2;
    volatile efuse_blk0_rdata3_reg_t blk0_rdata3;
    volatile efuse_blk0_rdata4_reg_t blk0_rdata4;
    volatile efuse_blk0_rdata5_reg_t blk0_rdata5;
    volatile efuse_blk0_rdata6_reg_t blk0_rdata6;
    volatile efuse_blk0_wdata0_reg_t blk0_wdata0;
    volatile efuse_blk0_wdata1_reg_t blk0_wdata1;
    volatile efuse_blk0_wdata2_reg_t blk0_wdata2;
    volatile efuse_blk0_wdata3_reg_t blk0_wdata3;
    volatile efuse_blk0_wdata4_reg_t blk0_wdata4;
    volatile efuse_blk0_wdata5_reg_t blk0_wdata5;
    volatile efuse_blk0_wdata6_reg_t blk0_wdata6;
    volatile efuse_blk1_rdata0_reg_t blk1_rdata0;
    volatile efuse_blk1_rdata1_reg_t blk1_rdata1;
    volatile efuse_blk1_rdata2_reg_t blk1_rdata2;
    volatile efuse_blk1_rdata3_reg_t blk1_rdata3;
    volatile efuse_blk1_rdata4_reg_t blk1_rdata4;
    volatile efuse_blk1_rdata5_reg_t blk1_rdata5;
    volatile efuse_blk1_rdata6_reg_t blk1_rdata6;
    volatile efuse_blk1_rdata7_reg_t blk1_rdata7;
    volatile efuse_blk2_rdata0_reg_t blk2_rdata0;
    volatile efuse_blk2_rdata1_reg_t blk2_rdata1;
    volatile efuse_blk2_rdata2_reg_t blk2_rdata2;
    volatile efuse_blk2_rdata3_reg_t blk2_rdata3;
    volatile efuse_blk2_rdata4_reg_t blk2_rdata4;
    volatile efuse_blk2_rdata5_reg_t blk2_rdata5;
    volatile efuse_blk2_rdata6_reg_t blk2_rdata6;
    volatile efuse_blk2_rdata7_reg_t blk2_rdata7;
    volatile efuse_blk3_rdata0_reg_t blk3_rdata0;
    volatile efuse_blk3_rdata1_reg_t blk3_rdata1;
    volatile efuse_blk3_rdata2_reg_t blk3_rdata2;
    volatile efuse_blk3_rdata3_reg_t blk3_rdata3;
    volatile efuse_blk3_rdata4_reg_t blk3_rdata4;
    volatile efuse_blk3_rdata5_reg_t blk3_rdata5;
    volatile efuse_blk3_rdata6_reg_t blk3_rdata6;
    volatile efuse_blk3_rdata7_reg_t blk3_rdata7;
    volatile efuse_blk1_wdata0_reg_t blk1_wdata0;
    volatile efuse_blk1_wdata1_reg_t blk1_wdata1;
    volatile efuse_blk1_wdata2_reg_t blk1_wdata2;
    volatile efuse_blk1_wdata3_reg_t blk1_wdata3;
    volatile efuse_blk1_wdata4_reg_t blk1_wdata4;
    volatile efuse_blk1_wdata5_reg_t blk1_wdata5;
    volatile efuse_blk1_wdata6_reg_t blk1_wdata6;
    volatile efuse_blk1_wdata7_reg_t blk1_wdata7;
    volatile efuse_blk2_wdata0_reg_t blk2_wdata0;
    volatile efuse_blk2_wdata1_reg_t blk2_wdata1;
    volatile efuse_blk2_wdata2_reg_t blk2_wdata2;
    volatile efuse_blk2_wdata3_reg_t blk2_wdata3;
    volatile efuse_blk2_wdata4_reg_t blk2_wdata4;
    volatile efuse_blk2_wdata5_reg_t blk2_wdata5;
    volatile efuse_blk2_wdata6_reg_t blk2_wdata6;
    volatile efuse_blk2_wdata7_reg_t blk2_wdata7;
    volatile efuse_blk3_wdata0_reg_t blk3_wdata0;
    volatile efuse_blk3_wdata1_reg_t blk3_wdata1;
    volatile efuse_blk3_wdata2_reg_t blk3_wdata2;
    volatile efuse_blk3_wdata3_reg_t blk3_wdata3;
    volatile efuse_blk3_wdata4_reg_t blk3_wdata4;
    volatile efuse_blk3_wdata5_reg_t blk3_wdata5;
    volatile efuse_blk3_wdata6_reg_t blk3_wdata6;
    volatile efuse_blk3_wdata7_reg_t blk3_wdata7;
    volatile efuse_clk_reg_t clk;
    volatile efuse_conf_reg_t conf;
    volatile efuse_status_reg_t status;
    volatile efuse_cmd_reg_t cmd;
    volatile efuse_int_raw_reg_t int_raw;
    volatile efuse_int_st_reg_t int_st;
    volatile efuse_int_ena_reg_t int_ena;
    volatile efuse_int_clr_reg_t int_clr;
    volatile efuse_dac_conf_reg_t dac_conf;
    volatile efuse_dec_status_reg_t dec_status;
    uint32_t reserved_120[55];
    volatile efuse_date_reg_t date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
