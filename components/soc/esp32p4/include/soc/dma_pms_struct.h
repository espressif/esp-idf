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

/** Group: Version Control Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 539165460;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} pms_dma_date_reg_t;


/** Group: Clock Gating Registers */
/** Type of clk_en register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: Enable automatic clock gating.
         *  1: Keep the clock always on.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} pms_dma_clk_en_reg_t;


/** Group: Region Configuration Registers */
/** Type of regionn_low register
 *  Regionn start address configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** regionn_low : R/W; bitpos: [31:12]; default: 0;
         *  Configures the high 20 bits of the start address for regionn.
         */
        uint32_t regionn_low:20;
    };
    uint32_t val;
} pms_dma_regionn_low_reg_t;

/** Type of regionn_high register
 *  Regionn end address configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** regionn_high : R/W; bitpos: [31:12]; default: 1048575;
         *  Configures the high 20 bits of the end address for regionn.
         */
        uint32_t regionn_high:20;
    };
    uint32_t val;
} pms_dma_regionn_high_reg_t;


/** Group: DMA Masters Read and Write Permission Control Registers */
/** Type of gdma_chn_r_pms register
 *  GDMA chn read permission control register
 */
typedef union {
    struct {
        /** gdma_chn_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the permission for GDMA chn to read 32 address regions. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t gdma_chn_r_pms:32;
    };
    uint32_t val;
} pms_dma_gdma_chn_r_pms_reg_t;

/** Type of gdma_chn_w_pms register
 *  GDMA chn write permission control register
 */
typedef union {
    struct {
        /** gdma_chn_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the permission for GDMA chn to write 32 address regions. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t gdma_chn_w_pms:32;
    };
    uint32_t val;
} pms_dma_gdma_chn_w_pms_reg_t;

/** Type of ahb_pdma_adc_r_pms register
 *  GDMA-AHB ADC read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_adc_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by ADC. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_adc_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_adc_r_pms_reg_t;

/** Type of ahb_pdma_adc_w_pms register
 *  GDMA-AHB ADC write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_adc_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by ADC. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_adc_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_adc_w_pms_reg_t;

/** Type of ahb_pdma_i2s0_r_pms register
 *  GDMA-AHB I2S0 read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i2s0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by I2S0. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_i2s0_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i2s0_r_pms_reg_t;

/** Type of ahb_pdma_i2s0_w_pms register
 *  GDMA-AHB I2S0 write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i2s0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by I2S0. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_i2s0_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i2s0_w_pms_reg_t;

/** Type of ahb_pdma_i2s1_r_pms register
 *  GDMA-AHB I2S1 read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i2s1_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by I2S1. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_i2s1_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i2s1_r_pms_reg_t;

/** Type of ahb_pdma_i2s1_w_pms register
 *  GDMA-AHB I2S1 write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i2s1_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by I2S1. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_i2s1_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i2s1_w_pms_reg_t;

/** Type of ahb_pdma_i2s2_r_pms register
 *  GDMA-AHB I2S2 read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i2s2_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by I2S2. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_i2s2_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i2s2_r_pms_reg_t;

/** Type of ahb_pdma_i2s2_w_pms register
 *  GDMA-AHB I2S2 write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i2s2_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by I2S2. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_i2s2_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i2s2_w_pms_reg_t;

/** Type of ahb_pdma_i3c_mst_r_pms register
 *  GDMA-AHB I3C MST read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i3c_mst_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by I3C  master.
         *  Bit 0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_i3c_mst_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i3c_mst_r_pms_reg_t;

/** Type of ahb_pdma_i3c_mst_w_pms register
 *  GDMA-AHB I3C MST write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_i3c_mst_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by I3C  master.
         *  Bit 0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_i3c_mst_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_i3c_mst_w_pms_reg_t;

/** Type of ahb_pdma_uhci0_r_pms register
 *  GDMA-AHB UHCI read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_uhci0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by UHCI. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_uhci0_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_uhci0_r_pms_reg_t;

/** Type of ahb_pdma_uhci0_w_pms register
 *  GDMA-AHB UHCI write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_uhci0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by UHCI. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_uhci0_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_uhci0_w_pms_reg_t;

/** Type of ahb_pdma_rmt_r_pms register
 *  GDMA-AHB RMT read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_rmt_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by RMT. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_rmt_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_rmt_r_pms_reg_t;

/** Type of ahb_pdma_rmt_w_pms register
 *  GDMA-AHB RMT write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_rmt_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by RMT. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_rmt_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_rmt_w_pms_reg_t;

/** Type of axi_pdma_lcdcam_r_pms register
 *  GDMA-AXI LCD_CAM read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_lcdcam_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by LCD_CAM. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_lcdcam_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_lcdcam_r_pms_reg_t;

/** Type of axi_pdma_lcdcam_w_pms register
 *  GDMA-AXI LCD_CAM write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_lcdcam_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by LCD_CAM. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_lcdcam_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_lcdcam_w_pms_reg_t;

/** Type of axi_pdma_gpspi2_r_pms register
 *  GDMA-AXI GPSPI2 read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_gpspi2_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by GP-SPI2. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_gpspi2_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_gpspi2_r_pms_reg_t;

/** Type of axi_pdma_gpspi2_w_pms register
 *  GDMA-AXI GPSPI2 write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_gpspi2_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by GP-SPI2. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_gpspi2_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_gpspi2_w_pms_reg_t;

/** Type of axi_pdma_gpspi3_r_pms register
 *  GDMA-AXI GPSPI3 read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_gpspi3_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by GP-SPI3. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_gpspi3_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_gpspi3_r_pms_reg_t;

/** Type of axi_pdma_gpspi3_w_pms register
 *  AXI PDMA GPSPI3 write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_gpspi3_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by GP-SPI3. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_gpspi3_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_gpspi3_w_pms_reg_t;

/** Type of axi_pdma_parlio_r_pms register
 *  GDMA-AXI PARLIO read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_parlio_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by PARLIO
         *  (Parallel IO Controller). Bit 0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_parlio_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_parlio_r_pms_reg_t;

/** Type of axi_pdma_parlio_w_pms register
 *  GDMA-AXI PARLIO write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_parlio_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by PARLIO. Bit
         *  0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_parlio_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_parlio_w_pms_reg_t;

/** Type of axi_pdma_aes_r_pms register
 *  GDMA-AXI AES read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_aes_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by AES. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_aes_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_aes_r_pms_reg_t;

/** Type of axi_pdma_aes_w_pms register
 *  GDMA-AXI AES write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_aes_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by AES. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_aes_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_aes_w_pms_reg_t;

/** Type of axi_pdma_sha_r_pms register
 *  GDMA-AXI SHA read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_sha_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by SHA. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_sha_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_sha_r_pms_reg_t;

/** Type of axi_pdma_sha_w_pms register
 *  GDMA-AXI SHA write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_sha_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by SHA. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_sha_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_sha_w_pms_reg_t;

/** Type of dma2d_jpeg_pms_r register
 *  2D-DMA JPEG read permission control register
 */
typedef union {
    struct {
        /** dma2d_jpeg_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures 2D-DMA permission to read 32 address ranges requested by JPEG. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t dma2d_jpeg_r_pms:32;
    };
    uint32_t val;
} pms_dma_dma2d_jpeg_pms_r_reg_t;

/** Type of dma2d_jpeg_pms_w register
 *  2D-DMA JPEG write permission control register
 */
typedef union {
    struct {
        /** dma2d_jpeg_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures 2D-DMA permission to write 32 address ranges requested by JPEG. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t dma2d_jpeg_w_pms:32;
    };
    uint32_t val;
} pms_dma_dma2d_jpeg_pms_w_reg_t;

/** Type of usb_pms_r register
 *  High-speed USB 2.0 OTG read permission control register
 */
typedef union {
    struct {
        /** usb_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for high-speed USB 2.0 OTG to access 32 address ranges.
         *  Bit 0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t usb_r_pms:32;
    };
    uint32_t val;
} pms_dma_usb_pms_r_reg_t;

/** Type of usb_pms_w register
 *  High-speed USB 2.0 OTG write permission control register
 */
typedef union {
    struct {
        /** usb_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for high-speed USB 2.0 OTG to access 32 address ranges.
         *  Bit 0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t usb_w_pms:32;
    };
    uint32_t val;
} pms_dma_usb_pms_w_reg_t;

/** Type of gmac_pms_r register
 *  EMAC read permission control register
 */
typedef union {
    struct {
        /** gmac_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for EMAC to access 32 address ranges. Bit 0 corresponds
         *  to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t gmac_r_pms:32;
    };
    uint32_t val;
} pms_dma_gmac_pms_r_reg_t;

/** Type of gmac_pms_w register
 *  EMAC write permission control register
 */
typedef union {
    struct {
        /** gmac_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for EMAC to access 32 address ranges. Bit 0 corresponds
         *  to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t gmac_w_pms:32;
    };
    uint32_t val;
} pms_dma_gmac_pms_w_reg_t;

/** Type of sdmmc_pms_r register
 *  SDMMC read permission control register
 */
typedef union {
    struct {
        /** sdmmc_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for SDMMC to access 32 address ranges. Bit 0 corresponds
         *  to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t sdmmc_r_pms:32;
    };
    uint32_t val;
} pms_dma_sdmmc_pms_r_reg_t;

/** Type of sdmmc_pms_w register
 *  SDMMC write permission control register
 */
typedef union {
    struct {
        /** sdmmc_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for SDMMC to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t sdmmc_w_pms:32;
    };
    uint32_t val;
} pms_dma_sdmmc_pms_w_reg_t;

/** Type of usbotg11_pms_r register
 *  Full-speed USB 2.0 OTG full-speed read permission control register
 */
typedef union {
    struct {
        /** usbotg11_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for full-speed USB 2.0 OTG to access 32 address ranges.
         *  Bit 0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t usbotg11_r_pms:32;
    };
    uint32_t val;
} pms_dma_usbotg11_pms_r_reg_t;

/** Type of usbotg11_pms_w register
 *  Full-speed USB 2.0 OTG full-speed write permission control register
 */
typedef union {
    struct {
        /** usbotg11_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for full-speed USB 2.0 OTG to access 32 address ranges.
         *  Bit 0 corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t usbotg11_w_pms:32;
    };
    uint32_t val;
} pms_dma_usbotg11_pms_w_reg_t;

/** Type of trace0_pms_r register
 *  TRACE0 read permission control register
 */
typedef union {
    struct {
        /** trace0_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for TRACE0 to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t trace0_r_pms:32;
    };
    uint32_t val;
} pms_dma_trace0_pms_r_reg_t;

/** Type of trace0_pms_w register
 *  TRACE0 write permission control register
 */
typedef union {
    struct {
        /** trace0_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for TRACE0 to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t trace0_w_pms:32;
    };
    uint32_t val;
} pms_dma_trace0_pms_w_reg_t;

/** Type of trace1_pms_r register
 *  TRACE1 read permission control register
 */
typedef union {
    struct {
        /** trace1_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for TRACE1 to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t trace1_r_pms:32;
    };
    uint32_t val;
} pms_dma_trace1_pms_r_reg_t;

/** Type of trace1_pms_w register
 *  TRACE1 write permission control register
 */
typedef union {
    struct {
        /** trace1_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for TRACE1 to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t trace1_w_pms:32;
    };
    uint32_t val;
} pms_dma_trace1_pms_w_reg_t;

/** Type of l2mem_mon_pms_r register
 *  L2MEM Monitor read permission control register
 */
typedef union {
    struct {
        /** l2mem_mon_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for L2MEM MON. Each bit corresponds to a region. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t l2mem_mon_r_pms:32;
    };
    uint32_t val;
} pms_dma_l2mem_mon_pms_r_reg_t;

/** Type of l2mem_mon_pms_w register
 *  L2MEM Monitor write permission control register
 */
typedef union {
    struct {
        /** l2mem_mon_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for L2MEM monitor to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t l2mem_mon_w_pms:32;
    };
    uint32_t val;
} pms_dma_l2mem_mon_pms_w_reg_t;

/** Type of tcm_mon_pms_r register
 *  TCM Monitor read permission control register
 */
typedef union {
    struct {
        /** tcm_mon_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for TCM MON. Each bit corresponds to a region. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t tcm_mon_r_pms:32;
    };
    uint32_t val;
} pms_dma_tcm_mon_pms_r_reg_t;

/** Type of tcm_mon_pms_w register
 *  TCM Monitor write permission control register
 */
typedef union {
    struct {
        /** tcm_mon_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for TCM monitor to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t tcm_mon_w_pms:32;
    };
    uint32_t val;
} pms_dma_tcm_mon_pms_w_reg_t;

/** Type of regdma_pms_r register
 *  REGDMA read permission control register
 */
typedef union {
    struct {
        /** regdma_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for REGDMA. Each bit corresponds to a region. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t regdma_r_pms:32;
    };
    uint32_t val;
} pms_dma_regdma_pms_r_reg_t;

/** Type of regdma_pms_w register
 *  REGDMA write permission control register
 */
typedef union {
    struct {
        /** regdma_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for REGDMA. Each bit corresponds to a region. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t regdma_w_pms:32;
    };
    uint32_t val;
} pms_dma_regdma_pms_w_reg_t;

/** Type of h264_pms_r register
 *  H264 DMA read permission control register
 */
typedef union {
    struct {
        /** h264_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures read permission for H264 DMA to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t h264_r_pms:32;
    };
    uint32_t val;
} pms_dma_h264_pms_r_reg_t;

/** Type of h264_pms_w register
 *  H264 DMA write permission control register
 */
typedef union {
    struct {
        /** h264_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures write permission for H264 DMA to access 32 address ranges. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t h264_w_pms:32;
    };
    uint32_t val;
} pms_dma_h264_pms_w_reg_t;

/** Type of dma2d_ppa_pms_r register
 *  2D-DMA PPA read permission control register
 */
typedef union {
    struct {
        /** dma2d_ppa_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures 2D-DMA permission to read 32 address ranges requested by PPA
         *  (Pixel-Processing Accelerator). Bit 0 corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t dma2d_ppa_r_pms:32;
    };
    uint32_t val;
} pms_dma_dma2d_ppa_pms_r_reg_t;

/** Type of dma2d_ppa_pms_w register
 *  2D-DMA PPA write permission control register
 */
typedef union {
    struct {
        /** dma2d_ppa_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures 2D-DMA permission to write 32 address ranges requested by PPA. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t dma2d_ppa_w_pms:32;
    };
    uint32_t val;
} pms_dma_dma2d_ppa_pms_w_reg_t;

/** Type of dma2d_dummy_pms_r register
 *  2D-DMA dummy read permission control register
 */
typedef union {
    struct {
        /** dma2d_dummy_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures 2D-DMA permission to read 32 address ranges requested by Dummy. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t dma2d_dummy_r_pms:32;
    };
    uint32_t val;
} pms_dma_dma2d_dummy_pms_r_reg_t;

/** Type of dma2d_dummy_pms_w register
 *  2D-DMA dummy write permission control register
 */
typedef union {
    struct {
        /** dma2d_dummy_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures 2D-DMA permission to write 32 address ranges requested by Dummy. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t dma2d_dummy_w_pms:32;
    };
    uint32_t val;
} pms_dma_dma2d_dummy_pms_w_reg_t;

/** Type of ahb_pdma_dummy_r_pms register
 *  GDMA-AHB dummy read permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_dummy_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to read 32 address ranges requested by Dummy. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t ahb_pdma_dummy_r_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_dummy_r_pms_reg_t;

/** Type of ahb_pdma_dummy_w_pms register
 *  GDMA-AHB dummy write permission control register
 */
typedef union {
    struct {
        /** ahb_pdma_dummy_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AHB permission to write 32 address ranges requested by Dummy. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t ahb_pdma_dummy_w_pms:32;
    };
    uint32_t val;
} pms_dma_ahb_pdma_dummy_w_pms_reg_t;

/** Type of axi_pdma_dummy_r_pms register
 *  GDMA-AXI dummy read permission control register
 */
typedef union {
    struct {
        /** axi_pdma_dummy_r_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to read 32 address ranges requested by Dummy. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable read permission.
         *  1: Enable read permission.
         */
        uint32_t axi_pdma_dummy_r_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_dummy_r_pms_reg_t;

/** Type of axi_pdma_dummy_w_pms register
 *  GDMA-AXI dummy write permission control register
 */
typedef union {
    struct {
        /** axi_pdma_dummy_w_pms : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures GDMA-AXI permission to write 32 address ranges requested by Dummy. Bit 0
         *  corresponds to region0, and so on.
         *  0: Disable write permission.
         *  1: Enable write permission.
         */
        uint32_t axi_pdma_dummy_w_pms:32;
    };
    uint32_t val;
} pms_dma_axi_pdma_dummy_w_pms_reg_t;


typedef struct {
    volatile pms_dma_date_reg_t date;
    volatile pms_dma_clk_en_reg_t clk_en;
    volatile pms_dma_regionn_low_reg_t region0_low;
    volatile pms_dma_regionn_high_reg_t region0_high;
    volatile pms_dma_regionn_low_reg_t region1_low;
    volatile pms_dma_regionn_high_reg_t region1_high;
    volatile pms_dma_regionn_low_reg_t region2_low;
    volatile pms_dma_regionn_high_reg_t region2_high;
    volatile pms_dma_regionn_low_reg_t region3_low;
    volatile pms_dma_regionn_high_reg_t region3_high;
    volatile pms_dma_regionn_low_reg_t region4_low;
    volatile pms_dma_regionn_high_reg_t region4_high;
    volatile pms_dma_regionn_low_reg_t region5_low;
    volatile pms_dma_regionn_high_reg_t region5_high;
    volatile pms_dma_regionn_low_reg_t region6_low;
    volatile pms_dma_regionn_high_reg_t region6_high;
    volatile pms_dma_regionn_low_reg_t region7_low;
    volatile pms_dma_regionn_high_reg_t region7_high;
    volatile pms_dma_regionn_low_reg_t region8_low;
    volatile pms_dma_regionn_high_reg_t region8_high;
    volatile pms_dma_regionn_low_reg_t region9_low;
    volatile pms_dma_regionn_high_reg_t region9_high;
    volatile pms_dma_regionn_low_reg_t region10_low;
    volatile pms_dma_regionn_high_reg_t region10_high;
    volatile pms_dma_regionn_low_reg_t region11_low;
    volatile pms_dma_regionn_high_reg_t region11_high;
    volatile pms_dma_regionn_low_reg_t region12_low;
    volatile pms_dma_regionn_high_reg_t region12_high;
    volatile pms_dma_regionn_low_reg_t region13_low;
    volatile pms_dma_regionn_high_reg_t region13_high;
    volatile pms_dma_regionn_low_reg_t region14_low;
    volatile pms_dma_regionn_high_reg_t region14_high;
    volatile pms_dma_regionn_low_reg_t region15_low;
    volatile pms_dma_regionn_high_reg_t region15_high;
    volatile pms_dma_regionn_low_reg_t region16_low;
    volatile pms_dma_regionn_high_reg_t region16_high;
    volatile pms_dma_regionn_low_reg_t region17_low;
    volatile pms_dma_regionn_high_reg_t region17_high;
    volatile pms_dma_regionn_low_reg_t region18_low;
    volatile pms_dma_regionn_high_reg_t region18_high;
    volatile pms_dma_regionn_low_reg_t region19_low;
    volatile pms_dma_regionn_high_reg_t region19_high;
    volatile pms_dma_regionn_low_reg_t region20_low;
    volatile pms_dma_regionn_high_reg_t region20_high;
    volatile pms_dma_regionn_low_reg_t region21_low;
    volatile pms_dma_regionn_high_reg_t region21_high;
    volatile pms_dma_regionn_low_reg_t region22_low;
    volatile pms_dma_regionn_high_reg_t region22_high;
    volatile pms_dma_regionn_low_reg_t region23_low;
    volatile pms_dma_regionn_high_reg_t region23_high;
    volatile pms_dma_regionn_low_reg_t region24_low;
    volatile pms_dma_regionn_high_reg_t region24_high;
    volatile pms_dma_regionn_low_reg_t region25_low;
    volatile pms_dma_regionn_high_reg_t region25_high;
    volatile pms_dma_regionn_low_reg_t region26_low;
    volatile pms_dma_regionn_high_reg_t region26_high;
    volatile pms_dma_regionn_low_reg_t region27_low;
    volatile pms_dma_regionn_high_reg_t region27_high;
    volatile pms_dma_regionn_low_reg_t region28_low;
    volatile pms_dma_regionn_high_reg_t region28_high;
    volatile pms_dma_regionn_low_reg_t region29_low;
    volatile pms_dma_regionn_high_reg_t region29_high;
    volatile pms_dma_regionn_low_reg_t region30_low;
    volatile pms_dma_regionn_high_reg_t region30_high;
    volatile pms_dma_regionn_low_reg_t region31_low;
    volatile pms_dma_regionn_high_reg_t region31_high;
    volatile pms_dma_gdma_chn_r_pms_reg_t gdma_ch0_r_pms;
    volatile pms_dma_gdma_chn_w_pms_reg_t gdma_ch0_w_pms;
    volatile pms_dma_gdma_chn_r_pms_reg_t gdma_ch1_r_pms;
    volatile pms_dma_gdma_chn_w_pms_reg_t gdma_ch1_w_pms;
    volatile pms_dma_gdma_chn_r_pms_reg_t gdma_ch2_r_pms;
    volatile pms_dma_gdma_chn_w_pms_reg_t gdma_ch2_w_pms;
    volatile pms_dma_gdma_chn_r_pms_reg_t gdma_ch3_r_pms;
    volatile pms_dma_gdma_chn_w_pms_reg_t gdma_ch3_w_pms;
    volatile pms_dma_ahb_pdma_adc_r_pms_reg_t ahb_pdma_adc_r_pms;
    volatile pms_dma_ahb_pdma_adc_w_pms_reg_t ahb_pdma_adc_w_pms;
    volatile pms_dma_ahb_pdma_i2s0_r_pms_reg_t ahb_pdma_i2s0_r_pms;
    volatile pms_dma_ahb_pdma_i2s0_w_pms_reg_t ahb_pdma_i2s0_w_pms;
    volatile pms_dma_ahb_pdma_i2s1_r_pms_reg_t ahb_pdma_i2s1_r_pms;
    volatile pms_dma_ahb_pdma_i2s1_w_pms_reg_t ahb_pdma_i2s1_w_pms;
    volatile pms_dma_ahb_pdma_i2s2_r_pms_reg_t ahb_pdma_i2s2_r_pms;
    volatile pms_dma_ahb_pdma_i2s2_w_pms_reg_t ahb_pdma_i2s2_w_pms;
    volatile pms_dma_ahb_pdma_i3c_mst_r_pms_reg_t ahb_pdma_i3c_mst_r_pms;
    volatile pms_dma_ahb_pdma_i3c_mst_w_pms_reg_t ahb_pdma_i3c_mst_w_pms;
    volatile pms_dma_ahb_pdma_uhci0_r_pms_reg_t ahb_pdma_uhci0_r_pms;
    volatile pms_dma_ahb_pdma_uhci0_w_pms_reg_t ahb_pdma_uhci0_w_pms;
    volatile pms_dma_ahb_pdma_rmt_r_pms_reg_t ahb_pdma_rmt_r_pms;
    uint32_t reserved_15c[5];
    volatile pms_dma_ahb_pdma_rmt_w_pms_reg_t ahb_pdma_rmt_w_pms;
    volatile pms_dma_axi_pdma_lcdcam_r_pms_reg_t axi_pdma_lcdcam_r_pms;
    volatile pms_dma_axi_pdma_lcdcam_w_pms_reg_t axi_pdma_lcdcam_w_pms;
    volatile pms_dma_axi_pdma_gpspi2_r_pms_reg_t axi_pdma_gpspi2_r_pms;
    volatile pms_dma_axi_pdma_gpspi2_w_pms_reg_t axi_pdma_gpspi2_w_pms;
    volatile pms_dma_axi_pdma_gpspi3_r_pms_reg_t axi_pdma_gpspi3_r_pms;
    volatile pms_dma_axi_pdma_gpspi3_w_pms_reg_t axi_pdma_gpspi3_w_pms;
    volatile pms_dma_axi_pdma_parlio_r_pms_reg_t axi_pdma_parlio_r_pms;
    volatile pms_dma_axi_pdma_parlio_w_pms_reg_t axi_pdma_parlio_w_pms;
    volatile pms_dma_axi_pdma_aes_r_pms_reg_t axi_pdma_aes_r_pms;
    volatile pms_dma_axi_pdma_aes_w_pms_reg_t axi_pdma_aes_w_pms;
    volatile pms_dma_axi_pdma_sha_r_pms_reg_t axi_pdma_sha_r_pms;
    volatile pms_dma_axi_pdma_sha_w_pms_reg_t axi_pdma_sha_w_pms;
    volatile pms_dma_dma2d_jpeg_pms_r_reg_t dma2d_jpeg_pms_r;
    volatile pms_dma_dma2d_jpeg_pms_w_reg_t dma2d_jpeg_pms_w;
    volatile pms_dma_usb_pms_r_reg_t usb_pms_r;
    volatile pms_dma_usb_pms_w_reg_t usb_pms_w;
    volatile pms_dma_gmac_pms_r_reg_t gmac_pms_r;
    volatile pms_dma_gmac_pms_w_reg_t gmac_pms_w;
    volatile pms_dma_sdmmc_pms_r_reg_t sdmmc_pms_r;
    volatile pms_dma_sdmmc_pms_w_reg_t sdmmc_pms_w;
    volatile pms_dma_usbotg11_pms_r_reg_t usbotg11_pms_r;
    volatile pms_dma_usbotg11_pms_w_reg_t usbotg11_pms_w;
    volatile pms_dma_trace0_pms_r_reg_t trace0_pms_r;
    volatile pms_dma_trace0_pms_w_reg_t trace0_pms_w;
    volatile pms_dma_trace1_pms_r_reg_t trace1_pms_r;
    volatile pms_dma_trace1_pms_w_reg_t trace1_pms_w;
    volatile pms_dma_l2mem_mon_pms_r_reg_t l2mem_mon_pms_r;
    volatile pms_dma_l2mem_mon_pms_w_reg_t l2mem_mon_pms_w;
    volatile pms_dma_tcm_mon_pms_r_reg_t tcm_mon_pms_r;
    volatile pms_dma_tcm_mon_pms_w_reg_t tcm_mon_pms_w;
    volatile pms_dma_regdma_pms_r_reg_t regdma_pms_r;
    volatile pms_dma_regdma_pms_w_reg_t regdma_pms_w;
    uint32_t reserved_1f4[2];
    volatile pms_dma_h264_pms_r_reg_t h264_pms_r;
    volatile pms_dma_h264_pms_w_reg_t h264_pms_w;
    volatile pms_dma_dma2d_ppa_pms_r_reg_t dma2d_ppa_pms_r;
    volatile pms_dma_dma2d_ppa_pms_w_reg_t dma2d_ppa_pms_w;
    volatile pms_dma_dma2d_dummy_pms_r_reg_t dma2d_dummy_pms_r;
    volatile pms_dma_dma2d_dummy_pms_w_reg_t dma2d_dummy_pms_w;
    volatile pms_dma_ahb_pdma_dummy_r_pms_reg_t ahb_pdma_dummy_r_pms;
    volatile pms_dma_ahb_pdma_dummy_w_pms_reg_t ahb_pdma_dummy_w_pms;
    volatile pms_dma_axi_pdma_dummy_r_pms_reg_t axi_pdma_dummy_r_pms;
    volatile pms_dma_axi_pdma_dummy_w_pms_reg_t axi_pdma_dummy_w_pms;
} pms_dma_dev_t;

extern pms_dma_dev_t DMA_PMS;

#ifndef __cplusplus
_Static_assert(sizeof(pms_dma_dev_t) == 0x224, "Invalid size of pms_dma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
