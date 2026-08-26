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

/** Group: Version Control Registers */
/** Type of lp2hp_peri_pms_date register
 *  Version control register
 */
typedef union {
    struct {
        /** lp2hp_peri_pms_date : R/W; bitpos: [31:0]; default: 2294790;
         *  Version control register.
         */
        uint32_t lp2hp_peri_pms_date:32;
    };
    uint32_t val;
} pms_lp2hp_peri_pms_date_reg_t;


/** Group: Clock Gating Registers */
/** Type of lp2hp_peri_pms_clk_en register
 *  Clock gating register
 */
typedef union {
    struct {
        /** lp2hp_peri_pms_clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: Enable automatic clock gating.
         *  1: Keep the clock always on.
         */
        uint32_t lp2hp_peri_pms_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} pms_lp2hp_peri_pms_clk_en_reg_t;


/** Group: LP CPU Permission Control Registers */
/** Type of lp_mm_pms_reg0 register
 *  Permission control register0 for the LP CPU in machine mode
 */
typedef union {
    struct {
        /** lp_mm_psram_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access external RAM
         *  without going through cache.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_psram_allow:1;
        /** lp_mm_flash_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access external
         *  flash without going through cache.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_flash_allow:1;
        /** lp_mm_l2mem_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP L2M2M
         *  without going through cache.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_l2mem_allow:1;
        /** lp_mm_l2rom_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP ROM
         *  without going through cache.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_l2rom_allow:1;
        uint32_t reserved_4:2;
        /** lp_mm_trace0_allow : R/W; bitpos: [6]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access TRACE0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_trace0_allow:1;
        /** lp_mm_trace1_allow : R/W; bitpos: [7]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access TRACE1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_trace1_allow:1;
        /** lp_mm_cpu_bus_mon_allow : R/W; bitpos: [8]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access CPU bus
         *  monitor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_cpu_bus_mon_allow:1;
        /** lp_mm_l2mem_mon_allow : R/W; bitpos: [9]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access L2MEM
         *  monitor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_l2mem_mon_allow:1;
        /** lp_mm_spm_mon_allow : R/W; bitpos: [10]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access SPM monitor.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_spm_mon_allow:1;
        /** lp_mm_cache_allow : R/W; bitpos: [11]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access cache.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_cache_allow:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} pms_lp_mm_pms_reg0_reg_t;

/** Type of lp_mm_pms_reg1 register
 *  Permission control register1 for the LP CPU in machine mode
 */
typedef union {
    struct {
        /** lp_mm_hp_usbotg_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP
         *  high-speed USB 2.0 OTG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_usbotg_allow:1;
        /** lp_mm_hp_usbotg11_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP
         *  full-speed USB 2.0 OTG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_usbotg11_allow:1;
        /** lp_mm_hp_usbotg11_wrap_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP
         *  full-speed USB 2.0 OTG's wrap.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_usbotg11_wrap_allow:1;
        /** lp_mm_hp_gdma_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP DW-GDMA.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_gdma_allow:1;
        uint32_t reserved_4:1;
        /** lp_mm_hp_sdmmc_allow : R/W; bitpos: [5]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP SDMMC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_sdmmc_allow:1;
        /** lp_mm_hp_ahb_pdma_allow : R/W; bitpos: [6]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access GDMA-AHB.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_ahb_pdma_allow:1;
        /** lp_mm_hp_jpeg_allow : R/W; bitpos: [7]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP JPEG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_jpeg_allow:1;
        /** lp_mm_hp_ppa_allow : R/W; bitpos: [8]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP PPA.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_ppa_allow:1;
        /** lp_mm_hp_dma2d_allow : R/W; bitpos: [9]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP 2D-DMA.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_dma2d_allow:1;
        /** lp_mm_hp_key_manager_allow : R/W; bitpos: [10]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP key
         *  manager.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_key_manager_allow:1;
        /** lp_mm_hp_axi_pdma_allow : R/W; bitpos: [11]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP GDMA-AXI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_axi_pdma_allow:1;
        /** lp_mm_hp_flash_allow : R/W; bitpos: [12]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP flash
         *  MSPI controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_flash_allow:1;
        /** lp_mm_hp_psram_allow : R/W; bitpos: [13]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP PSRAM
         *  MSPI controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_psram_allow:1;
        /** lp_mm_hp_crypto_allow : R/W; bitpos: [14]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP CRYPTO.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_crypto_allow:1;
        /** lp_mm_hp_gmac_allow : R/W; bitpos: [15]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP EMAC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_gmac_allow:1;
        /** lp_mm_hp_usb_phy_allow : R/W; bitpos: [16]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP
         *  high-speed USB 2.0 OTG PHY.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_usb_phy_allow:1;
        /** lp_mm_hp_pvt_allow : R/W; bitpos: [17]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP PVT.
         *  0: Not allowed
         *  1: Allow
         */
        uint32_t lp_mm_hp_pvt_allow:1;
        /** lp_mm_hp_csi_host_allow : R/W; bitpos: [18]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP MIPI CSI
         *  host.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_csi_host_allow:1;
        /** lp_mm_hp_dsi_host_allow : R/W; bitpos: [19]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP MIPI DSI
         *  host.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_dsi_host_allow:1;
        /** lp_mm_hp_isp_allow : R/W; bitpos: [20]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP ISP.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_isp_allow:1;
        /** lp_mm_hp_h264_core_allow : R/W; bitpos: [21]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP H264
         *  Encoder.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_h264_core_allow:1;
        /** lp_mm_hp_rmt_allow : R/W; bitpos: [22]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP RMT.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_rmt_allow:1;
        /** lp_mm_hp_bitscrambler_allow : R/W; bitpos: [23]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP bit
         *  scrambler.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_bitscrambler_allow:1;
        /** lp_mm_hp_axi_icm_allow : R/W; bitpos: [24]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP AXI ICM.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_axi_icm_allow:1;
        /** lp_mm_hp_peri_pms_allow : R/W; bitpos: [25]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access
         *  HP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_peri_pms_allow:1;
        /** lp_mm_lp2hp_peri_pms_allow : R/W; bitpos: [26]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access
         *  LP2HP_PERI_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_lp2hp_peri_pms_allow:1;
        /** lp_mm_dma_pms_allow : R/W; bitpos: [27]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access
         *  HP_DMA_PMS_REG.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_dma_pms_allow:1;
        /** lp_mm_hp_h264_dma2d_allow : R/W; bitpos: [28]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access 2D-DMA.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_h264_dma2d_allow:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} pms_lp_mm_pms_reg1_reg_t;

/** Type of lp_mm_pms_reg2 register
 *  Permission control register2 for the LP CPU in machine mode
 */
typedef union {
    struct {
        /** lp_mm_hp_mcpwm0_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP MCPWM0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_mcpwm0_allow:1;
        /** lp_mm_hp_mcpwm1_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP MCPWM1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_mcpwm1_allow:1;
        /** lp_mm_hp_timer_group0_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP timer
         *  group0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_timer_group0_allow:1;
        /** lp_mm_hp_timer_group1_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP timer
         *  group1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_timer_group1_allow:1;
        /** lp_mm_hp_i2c0_allow : R/W; bitpos: [4]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I2C0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i2c0_allow:1;
        /** lp_mm_hp_i2c1_allow : R/W; bitpos: [5]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I2C1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i2c1_allow:1;
        /** lp_mm_hp_i2s0_allow : R/W; bitpos: [6]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I2S0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i2s0_allow:1;
        /** lp_mm_hp_i2s1_allow : R/W; bitpos: [7]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I2S1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i2s1_allow:1;
        /** lp_mm_hp_i2s2_allow : R/W; bitpos: [8]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I2S2.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i2s2_allow:1;
        /** lp_mm_hp_pcnt_allow : R/W; bitpos: [9]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP PCNT.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_pcnt_allow:1;
        /** lp_mm_hp_uart0_allow : R/W; bitpos: [10]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP UART0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_uart0_allow:1;
        /** lp_mm_hp_uart1_allow : R/W; bitpos: [11]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP UART1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_uart1_allow:1;
        /** lp_mm_hp_uart2_allow : R/W; bitpos: [12]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP UART2.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_uart2_allow:1;
        /** lp_mm_hp_uart3_allow : R/W; bitpos: [13]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP UART3.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_uart3_allow:1;
        /** lp_mm_hp_uart4_allow : R/W; bitpos: [14]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP UART4.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_uart4_allow:1;
        /** lp_mm_hp_parlio_allow : R/W; bitpos: [15]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP PARLIO.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_parlio_allow:1;
        /** lp_mm_hp_gpspi2_allow : R/W; bitpos: [16]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP GP-SPI2.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_gpspi2_allow:1;
        /** lp_mm_hp_gpspi3_allow : R/W; bitpos: [17]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP GP-SPI3.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_gpspi3_allow:1;
        /** lp_mm_hp_usbdevice_allow : R/W; bitpos: [18]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP
         *  USB/Serial JTAG Controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_usbdevice_allow:1;
        /** lp_mm_hp_ledc_allow : R/W; bitpos: [19]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP LEDC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_ledc_allow:1;
        uint32_t reserved_20:1;
        /** lp_mm_hp_etm_allow : R/W; bitpos: [21]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP ETM.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_etm_allow:1;
        /** lp_mm_hp_intrmtx_allow : R/W; bitpos: [22]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP interrupt
         *  matrix.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_intrmtx_allow:1;
        /** lp_mm_hp_twai0_allow : R/W; bitpos: [23]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP TWAI0.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_twai0_allow:1;
        /** lp_mm_hp_twai1_allow : R/W; bitpos: [24]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP TWAI1.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_twai1_allow:1;
        /** lp_mm_hp_twai2_allow : R/W; bitpos: [25]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP TWAI2.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_twai2_allow:1;
        /** lp_mm_hp_i3c_mst_allow : R/W; bitpos: [26]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I3C
         *  master controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i3c_mst_allow:1;
        /** lp_mm_hp_i3c_slv_allow : R/W; bitpos: [27]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP I3C slave
         *  controller.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_i3c_slv_allow:1;
        /** lp_mm_hp_lcdcam_allow : R/W; bitpos: [28]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP LCD_CAM.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_lcdcam_allow:1;
        uint32_t reserved_29:1;
        /** lp_mm_hp_adc_allow : R/W; bitpos: [30]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP ADC.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_adc_allow:1;
        /** lp_mm_hp_uhci_allow : R/W; bitpos: [31]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP UHCI.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_uhci_allow:1;
    };
    uint32_t val;
} pms_lp_mm_pms_reg2_reg_t;

/** Type of lp_mm_pms_reg3 register
 *  Permission control register3 for the LP CPU in machine mode
 */
typedef union {
    struct {
        /** lp_mm_hp_gpio_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP GPIO
         *  Matrix.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_gpio_allow:1;
        /** lp_mm_hp_iomux_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP IO MUX.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_iomux_allow:1;
        /** lp_mm_hp_systimer_allow : R/W; bitpos: [2]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP system
         *  timer.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_systimer_allow:1;
        /** lp_mm_hp_sys_reg_allow : R/W; bitpos: [3]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access HP system
         *  register.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_sys_reg_allow:1;
        /** lp_mm_hp_clkrst_allow : R/W; bitpos: [4]; default: 1;
         *  Configures whether the LP CPU in machine mode has permission to access
         *  HP_SYS_CLKRST.
         *  0: Not allowed
         *  1: Allowed
         */
        uint32_t lp_mm_hp_clkrst_allow:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} pms_lp_mm_pms_reg3_reg_t;


typedef struct {
    volatile pms_lp2hp_peri_pms_date_reg_t lp2hp_peri_pms_date;
    volatile pms_lp2hp_peri_pms_clk_en_reg_t lp2hp_peri_pms_clk_en;
    volatile pms_lp_mm_pms_reg0_reg_t lp_mm_pms_reg0;
    uint32_t reserved_00c[9];
    volatile pms_lp_mm_pms_reg1_reg_t lp_mm_pms_reg1;
    uint32_t reserved_034[28];
    volatile pms_lp_mm_pms_reg2_reg_t lp_mm_pms_reg2;
    uint32_t reserved_0a8[29];
    volatile pms_lp_mm_pms_reg3_reg_t lp_mm_pms_reg3;
} lp2hp_peri_pms_dev_t;

extern lp2hp_peri_pms_dev_t LP2HP_PERI_PMS;

#ifndef __cplusplus
_Static_assert(sizeof(lp2hp_peri_pms_dev_t) == 0x120, "Invalid size of lp2hp_peri_pms_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
