/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Basic address */
#define DR_REG_HPCPUTCP_BASE          0x3FF00000
#define DR_REG_HPPERIPH0_BASE         0x50000000
#define DR_REG_HPPERIPH1_BASE         0x500C0000
#define DR_REG_LPAON_BASE             0x50110000
#define DR_REG_LPPERIPH_BASE          0x50120000

/* This is raw module base from digital team
 * some of them may not be used in rom
 * just keep them for a reference
 */
/*
 * @module: CPU-PERIPHERAL
 *
 * @base: 0x3FF00000
 *
 * @size: 128KB
 */
#define DR_REG_TRACE0_BASE            (DR_REG_HPCPUTCP_BASE + 0x4000)
#define DR_REG_TRACE1_BASE            (DR_REG_HPCPUTCP_BASE + 0x5000)
#define DR_REG_CPU_BUS_MON_BASE       (DR_REG_HPCPUTCP_BASE + 0x6000)
#define DR_REG_L2MEM_MON_BASE         (DR_REG_HPCPUTCP_BASE + 0xE000)
#define DR_REG_TCM_MON_BASE           (DR_REG_HPCPUTCP_BASE + 0xF000)
#define DR_REG_CACHE_BASE             (DR_REG_HPCPUTCP_BASE + 0x10000)

/*
 * @module: PERIPHERAL0
 *
 * @base: 0x50000000
 *
 * @size: 768KB
 */
#define DR_REG_USB2_BASE              (DR_REG_HPPERIPH0_BASE + 0x0)
#define DR_REG_USB11_BASE             (DR_REG_HPPERIPH0_BASE + 0x40000)
#define DR_REG_USB_WRAP_BASE          (DR_REG_HPPERIPH0_BASE + 0x80000)
#define DR_REG_GDMA_BASE              (DR_REG_HPPERIPH0_BASE + 0x81000)
#define DR_REG_REGDMA_BASE            (DR_REG_HPPERIPH0_BASE + 0x82000)
#define DR_REG_SDMMC_BASE             (DR_REG_HPPERIPH0_BASE + 0x83000)
#define DR_REG_H264_CORE_BASE         (DR_REG_HPPERIPH0_BASE + 0x84000)
#define DR_REG_AHB_DMA_BASE           (DR_REG_HPPERIPH0_BASE + 0x85000)
#define DR_REG_JPEG_BASE              (DR_REG_HPPERIPH0_BASE + 0x86000)
#define DR_REG_PPA_BASE               (DR_REG_HPPERIPH0_BASE + 0x87000)
#define DR_REG_DMA2D_BASE             (DR_REG_HPPERIPH0_BASE + 0x88000)
#define DR_REG_KEYMNG_BASE            (DR_REG_HPPERIPH0_BASE + 0x89000)
#define DR_REG_AXI_DMA_BASE           (DR_REG_HPPERIPH0_BASE + 0x8A000)
#define DR_REG_FLASH_SPI0_BASE        (DR_REG_HPPERIPH0_BASE + 0x8C000)
#define DR_REG_FLASH_SPI1_BASE        (DR_REG_HPPERIPH0_BASE + 0x8D000)
#define DR_REG_PSRAM_MSPI0_BASE       (DR_REG_HPPERIPH0_BASE + 0x8E000)
#define DR_REG_PSRAM_MSPI1_BASE       (DR_REG_HPPERIPH0_BASE + 0x8F000)
#define DR_REG_CRYPTO_BASE            (DR_REG_HPPERIPH0_BASE + 0x90000)
#define DR_REG_EMAC_BASE              (DR_REG_HPPERIPH0_BASE + 0x98000)
#define DR_REG_USBPHY_BASE            (DR_REG_HPPERIPH0_BASE + 0x9C000)
#define DR_REG_DDRPHY_BASE            (DR_REG_HPPERIPH0_BASE + 0x9D000)
#define DR_REG_PVT_BASE               (DR_REG_HPPERIPH0_BASE + 0x9E000)
#define DR_REG_CSI_HOST_BASE          (DR_REG_HPPERIPH0_BASE + 0x9F000)
#define DR_REG_CSI_BRG_BASE           (DR_REG_HPPERIPH0_BASE + 0x9F800)
#define DR_REG_DSI_HOST_BASE          (DR_REG_HPPERIPH0_BASE + 0xA0000)
#define DR_REG_DSI_BRG_BASE           (DR_REG_HPPERIPH0_BASE + 0xA0800)
#define DR_REG_ISP_BASE               (DR_REG_HPPERIPH0_BASE + 0xA1000)
#define DR_REG_RMT_BASE               (DR_REG_HPPERIPH0_BASE + 0xA2000)
#define DR_REG_BITSCRAM_BASE          (DR_REG_HPPERIPH0_BASE + 0xA3000)
#define DR_REG_AXI_ICM_BASE           (DR_REG_HPPERIPH0_BASE + 0xA4000)
#define DR_REG_AXI_ICM_QOS_BASE       (DR_REG_AXI_ICM_BASE   + 0x400)
#define DR_REG_HP_PERI_PMS_BASE       (DR_REG_HPPERIPH0_BASE + 0xA5000)
#define DR_REG_LP2HP_PERI_PMS_BASE    (DR_REG_HPPERIPH0_BASE + 0xA5800)
#define DR_REG_DMA_PMS_BASE           (DR_REG_HPPERIPH0_BASE + 0xA6000)
#define DR_REG_H264_DMA_2D_BASE       (DR_REG_HPPERIPH0_BASE + 0xA7000)
/*
 * @module: PERIPHERAL1
 *
 * @base: 0x500C0000
 *
 * @size: 256KB
 */
#define DR_REG_MCPWM0_BASE            (DR_REG_HPPERIPH1_BASE + 0x0)
#define DR_REG_MCPWM1_BASE            (DR_REG_HPPERIPH1_BASE + 0x1000)
#define DR_REG_TIMG0_BASE             (DR_REG_HPPERIPH1_BASE + 0x2000)
#define DR_REG_TIMG1_BASE             (DR_REG_HPPERIPH1_BASE + 0x3000)
#define DR_REG_I2C0_BASE              (DR_REG_HPPERIPH1_BASE + 0x4000)
#define DR_REG_I2C1_BASE              (DR_REG_HPPERIPH1_BASE + 0x5000)
#define DR_REG_I2S0_BASE              (DR_REG_HPPERIPH1_BASE + 0x6000)
#define DR_REG_I2S1_BASE              (DR_REG_HPPERIPH1_BASE + 0x7000)
#define DR_REG_I2S2_BASE              (DR_REG_HPPERIPH1_BASE + 0x8000)
#define DR_REG_PCNT_BASE              (DR_REG_HPPERIPH1_BASE + 0x9000)
#define DR_REG_UART0_BASE             (DR_REG_HPPERIPH1_BASE + 0xA000)
#define DR_REG_UART1_BASE             (DR_REG_HPPERIPH1_BASE + 0xB000)
#define DR_REG_UART2_BASE             (DR_REG_HPPERIPH1_BASE + 0xC000)
#define DR_REG_UART3_BASE             (DR_REG_HPPERIPH1_BASE + 0xD000)
#define DR_REG_UART4_BASE             (DR_REG_HPPERIPH1_BASE + 0xE000)
#define DR_REG_PARIO_BASE             (DR_REG_HPPERIPH1_BASE + 0xF000)
#define DR_REG_SPI2_BASE              (DR_REG_HPPERIPH1_BASE + 0x10000)
#define DR_REG_SPI3_BASE              (DR_REG_HPPERIPH1_BASE + 0x11000)
#define DR_REG_USB2JTAG_BASE          (DR_REG_HPPERIPH1_BASE + 0x12000)
#define DR_REG_LEDC_BASE              (DR_REG_HPPERIPH1_BASE + 0x13000)
#define DR_REG_ETM_BASE               (DR_REG_HPPERIPH1_BASE + 0x15000)
#define DR_REG_INTR_BASE              (DR_REG_HPPERIPH1_BASE + 0x16000)
#define DR_REG_TWAI0_BASE             (DR_REG_HPPERIPH1_BASE + 0x17000)
#define DR_REG_TWAI1_BASE             (DR_REG_HPPERIPH1_BASE + 0x18000)
#define DR_REG_TWAI2_BASE             (DR_REG_HPPERIPH1_BASE + 0x19000)
#define DR_REG_I3C_MST_BASE           (DR_REG_HPPERIPH1_BASE + 0x1A000)
#define DR_REG_I3C_SLV_BASE           (DR_REG_HPPERIPH1_BASE + 0x1B000)
#define DR_REG_LCDCAM_BASE            (DR_REG_HPPERIPH1_BASE + 0x1C000)
#define DR_REG_ADC_BASE               (DR_REG_HPPERIPH1_BASE + 0x1E000)
#define DR_REG_UHCI_BASE              (DR_REG_HPPERIPH1_BASE + 0x1F000)
#define DR_REG_GPIO_BASE              (DR_REG_HPPERIPH1_BASE + 0x20000)
#define DR_REG_GPIO_EXT_BASE          (DR_REG_HPPERIPH1_BASE + 0x20F00)
#define DR_REG_IO_MUX_BASE            (DR_REG_HPPERIPH1_BASE + 0x21000)
#define DR_REG_IOMUX_MSPI_PIN_BASE    (DR_REG_HPPERIPH1_BASE + 0x21200)
#define DR_REG_SYSTIMER_BASE          (DR_REG_HPPERIPH1_BASE + 0x22000)
#define DR_REG_MEM_MON_BASE           (DR_REG_HPPERIPH1_BASE + 0x23000)
#define DR_REG_AUDIO_ADDC_BASE        (DR_REG_HPPERIPH1_BASE + 0x24000)
#define DR_REG_HP_SYS_BASE            (DR_REG_HPPERIPH1_BASE + 0x25000)
#define DR_REG_HP_SYS_CLKRST_BASE     (DR_REG_HPPERIPH1_BASE + 0x26000)

/*
 * @module: LP AON
 *
 * @base: 0x50110000
 *
 * @size: 64KB
 */
#define DR_REG_LP_SYS_BASE            (DR_REG_LPAON_BASE + 0x0)
#define DR_REG_LP_CLKRST_BASE         (DR_REG_LPAON_BASE + 0x1000)
#define DR_REG_LP_TIMER_BASE          (DR_REG_LPAON_BASE + 0x2000)
#define DR_REG_LP_ANALOG_PERI_BASE    (DR_REG_LPAON_BASE + 0x3000)
#define DR_REG_LP_HUK_BASE            (DR_REG_LPAON_BASE + 0x4000)
#define DR_REG_HUK_BASE               (DR_REG_LP_HUK_BASE)
#define DR_REG_PMU_BASE               (DR_REG_LPAON_BASE + 0x5000)
#define DR_REG_LP_WDT_BASE            (DR_REG_LPAON_BASE + 0x6000)
#define DR_REG_LP_MB_BASE             (DR_REG_LPAON_BASE + 0x8000)
#define DR_REG_RTC_BASE               (DR_REG_LPAON_BASE + 0x9000)

/*
 * @module: LP PERI
 *
 * @base: 0x50120000
 *
 * @size: 64KB
 */
#define DR_REG_LP_PERI_CLKRST_BASE    (DR_REG_LPPERIPH_BASE + 0x0)
#define DR_REG_LP_PERI_BASE           (DR_REG_LPPERIPH_BASE + 0x0)
#define DR_REG_LP_UART_BASE           (DR_REG_LPPERIPH_BASE + 0x1000)
#define DR_REG_LP_I2C_BASE            (DR_REG_LPPERIPH_BASE + 0x2000)
#define DR_REG_LP_SPI_BASE            (DR_REG_LPPERIPH_BASE + 0x3000)
#define DR_REG_I2C_ANA_MST_BASE       (DR_REG_LPPERIPH_BASE + 0x4000)
#define DR_REG_LP_I2S_BASE            (DR_REG_LPPERIPH_BASE + 0x5000)
#define DR_REG_LP_ADC_BASE            (DR_REG_LPPERIPH_BASE + 0x7000)
#define DR_REG_LP_TOUCH_BASE          (DR_REG_LPPERIPH_BASE + 0x8000)
#define DR_REG_LP_GPIO_BASE           (DR_REG_LPPERIPH_BASE + 0xA000)
#define DR_REG_LP_IOMUX_BASE          (DR_REG_LPPERIPH_BASE + 0xB000)
#define DR_REG_LP_INTR_BASE           (DR_REG_LPPERIPH_BASE + 0xC000)
#define DR_REG_EFUSE_BASE             (DR_REG_LPPERIPH_BASE + 0xD000)
#define DR_REG_LP_PERI_PMS_BASE       (DR_REG_LPPERIPH_BASE + 0xE000)
#define DR_REG_HP2LP_PERI_PMS_BASE    (DR_REG_LPPERIPH_BASE + 0xE800)
#define DR_REG_LP_TSENSOR_BASE        (DR_REG_LPPERIPH_BASE + 0xF000)

/**
 * @brief: Special memory address
 */
#define LP_I2S_RAM_BASE               0x50125c00
#define MIPI_CSI_BRG_MEM_BASE         0x50104000
#define MIPI_DSI_BRG_MEM_BASE         0x50105000


/**
 * This are module helper MACROs for quick module reference
 * including some module(renamed) address
 */
#define DR_REG_UART_BASE                        DR_REG_UART0_BASE
#define DR_REG_UHCI0_BASE                       DR_REG_UHCI_BASE
#define DR_REG_TIMERGROUP0_BASE                 DR_REG_TIMG0_BASE
#define DR_REG_TIMERGROUP1_BASE                 DR_REG_TIMG1_BASE
#define DR_REG_I2S_BASE                         DR_REG_I2S0_BASE
#define DR_REG_USB_SERIAL_JTAG_BASE             DR_REG_USB2JTAG_BASE
#define DR_REG_INTERRUPT_MATRIX_BASE            DR_REG_INTR_BASE
#define DR_REG_SOC_ETM_BASE                     DR_REG_ETM_BASE
#define DR_REG_MCPWM_BASE                       DR_REG_MCPWM0_BASE
#define DR_REG_PARL_IO_BASE                     DR_REG_PARIO_BASE
#define DR_REG_PVT_MONITOR_BASE                 DR_REG_PVT_BASE
#define DR_REG_AES_BASE                         (DR_REG_CRYPTO_BASE + 0x0)
#define DR_REG_SHA_BASE                         (DR_REG_CRYPTO_BASE + 0x1000)
#define DR_REG_RSA_BASE                         (DR_REG_CRYPTO_BASE + 0x2000)
#define DR_REG_ECC_MULT_BASE                    (DR_REG_CRYPTO_BASE + 0x3000)
#define DR_REG_DS_BASE                          (DR_REG_CRYPTO_BASE + 0x4000)
#define DR_REG_DIGITAL_SIGNATURE_BASE           DR_REG_DS_BASE
#define DR_REG_HMAC_BASE                        (DR_REG_CRYPTO_BASE + 0x5000)
#define DR_REG_ECDSA_BASE                       (DR_REG_CRYPTO_BASE + 0x6000)
#define DR_REG_MEM_MONITOR_BASE                 DR_REG_L2MEM_MON_BASE
#define DR_REG_HP_CLKRST_BASE                   DR_REG_HP_SYS_CLKRST_BASE
#define DR_REG_DSPI_MEM_BASE                    DR_REG_PSRAM_MSPI0_BASE
#define DR_REG_INTERRUPT_CORE0_BASE             DR_REG_INTR_BASE
#define DR_REG_INTERRUPT_CORE1_BASE             (DR_REG_INTR_BASE + 0x800)
#define DR_REG_LPPERI_BASE                      DR_REG_LP_PERI_CLKRST_BASE
#define DR_REG_CPU_BUS_MONITOR_BASE             DR_REG_CPU_BUS_MON_BASE

#define DR_REG_PAU_BASE                         DR_REG_REGDMA_BASE

//TODO: IDF-7688
// #define DR_REG_TRACE_BASE                       0x600C0000
#define DR_REG_ASSIST_DEBUG_BASE                0x3FF06000
