/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */


#define DR_REG_USB_OTGHS_BASE                     0x20300000
#define DR_REG_PAU_BASE                           0x20341000
#define DR_REG_SDMMC_BASE                         0x20342000
#define DR_REG_AHB_DMA_BASE                       0x20343000
#define DR_REG_JPEG_BASE                          0x20344000
#define DR_REG_PPA_BASE                           0x20345000
#define DR_REG_DMA2D_BASE                         0x20346000
#define DR_REG_AXI_DMA_BASE                       0x20348000
#define DR_REG_GMAC_BASE                          0x20350000
#define DR_REG_PVT_BASE                           0x20354000
#define DR_REG_RMT_BASE                           0x20355000
#define DR_REG_BITSCRAMBLER_BASE                  0x20356000
#define DR_REG_ASRC_BASE                          0x20357000
#define DR_REG_CNNT_SYS_REG_BASE                  0x20359000
#define DR_REG_MCPWM0_BASE                        0x20381000
#define DR_REG_MCPWM1_BASE                        0x20382000
#define DR_REG_MCPWM2_BASE                        0x20383000
#define DR_REG_MCPWM3_BASE                        0x20384000
#define DR_REG_I2C0_BASE                          0x20385000
#define DR_REG_I2C1_BASE                          0x20386000
#define DR_REG_I2S0_BASE                          0x20387000
#define DR_REG_I2S1_BASE                          0x20388000
#define DR_REG_PCNT0_BASE                         0x20389000
#define DR_REG_UART0_BASE                         0x2038A000
#define DR_REG_UART1_BASE                         0x2038B000
#define DR_REG_UART2_BASE                         0x2038C000
#define DR_REG_UART3_BASE                         0x2038D000
#define DR_REG_PARL_IO_BASE                       0x2038E000
#define DR_REG_GPSPI2_BASE                        0x2038F000
#define DR_REG_GPSPI3_BASE                        0x20390000
#define DR_REG_USB_SERIAL_JTAG_BASE               0x20391000
#define DR_REG_LEDC0_BASE                         0x20392000
#define DR_REG_SOC_ETM_BASE                       0x20393000
#define DR_REG_TWAIFD0_BASE                       0x20394000
#define DR_REG_TWAIFD1_BASE                       0x20395000
#define DR_REG_LCD_CAM_BASE                       0x20396000
#define DR_REG_UHCI0_BASE                         0x20398000
#define DR_REG_SYSTIMER_BASE                      0x20399000
#define DR_REG_ZERO_DET_BASE                      0x2039A000
#define DR_REG_CORDIC_BASE                        0x2039B000
#define DR_REG_LEDC1_BASE                         0x2039C000
#define DR_REG_PCNT1_BASE                         0x2039D000
#define DR_REG_FLASH_SPI0_BASE                    0x20500000
#define DR_REG_FLASH_SPI1_BASE                    0x20501000
#define DR_REG_PSRAM_MSPI0_BASE                   0x20502000
#define DR_REG_PSRAM_MSPI1_BASE                   0x20503000
#define DR_REG_TEE_BASE                           0x20504000
#define DR_REG_HP_APM_BASE                        0x20504400
#define DR_REG_HP_MEM_APM_BASE                    0x20504800
#define DR_REG_CPU_APM_BASE                       0x20504C00
#define DR_REG_HP_PERI0_PMS_BASE                  0x20505000
#define DR_REG_KEYMNG_BASE                        0x20506000
#define DR_REG_AES_BASE                           0x20508000
#define DR_REG_SHA_BASE                           0x20509000
#define DR_REG_RSA_BASE                           0x2050A000
#define DR_REG_ECC_BASE                           0x2050B000
#define DR_REG_DS_BASE                            0x2050C000
#define DR_REG_HMAC_BASE                          0x2050D000
#define DR_REG_ECDSA_BASE                         0x2050E000
#define DR_REG_RMA_BASE                           0x2050F000
#define DR_REG_ICM_SYS_BASE                       0x20510000
#define DR_REG_ICM_BASE                           0x20510400
#define DR_REG_AXI_PERF_MON_BASE                  0x20511000
#define DR_REG_TIMERG0_BASE                       0x20580000
#define DR_REG_TIMERG1_BASE                       0x20581000
#define DR_REG_IO_MUX_BASE                        0x20582000
#define DR_REG_GPIO_BASE                          0x20583000
#define DR_REG_GPIO_EXT_BASE                      0x20583E00
#define DR_REG_MSPI_IOMUX_BASE                    0x20584000
#define DR_REG_INTR0_BASE                         0x20585000
#define DR_REG_HP_SYS_BASE                        0x20586000
#define DR_REG_HP_SYS_CLKRST_BASE                 0x20587000
#define DR_REG_CNNT_PAD_CTRL_BASE                 0x20588000
#define DR_REG_HP_ALIVE_SYS_BASE                  0x20589000
#define DR_REG_HP_PERI1_PMS_BASE                  0x2058A000
#define DR_REG_LP_SYS_BASE                        0x20700000
#define DR_REG_LP_CLKRST_BASE                     0x20701000
#define DR_REG_LP_ANA_PERI_BASE                   0x20702000
#define DR_REG_HUK_BASE                           0x20703000
#define DR_REG_PMU_BASE                           0x20704000
#define DR_REG_TOUCH_AON_BASE                     0x20705000
#define DR_REG_LP_PERI_PMS_BASE                   0x20706000
#define DR_REG_LP_TEE_BASE                        0x20706800
#define DR_REG_LP_APM_BASE                        0x20706C00
#define DR_REG_LP_PERICLKRST_BASE                 0x20710000
#define DR_REG_LP_IO_MUX_BASE                     0x20712000
#define DR_REG_LP_GPIO_BASE                       0x20713000
#define DR_REG_LP_INTR_BASE                       0x20714000
#define DR_REG_EFUSE_BASE                         0x20715000
#define DR_REG_LP_TIMER_BASE                      0x20800000
#define DR_REG_LP_WDT_BASE                        0x20801000
#define DR_REG_RTCLOCKCALI0_BASE                  0x20802000
#define DR_REG_RTCLOCKCALI1_BASE                  0x20803000
#define DR_REG_LP_PWR_REG_BASE                    0x20804000
#define DR_REG_LP_ADI_BASE                        0x20805000
#define DR_REG_LP_UART_BASE                       0x20810000
#define DR_REG_LP_I2C_BASE                        0x20811000
#define DR_REG_LP_SPI_BASE                        0x20812000
#define DR_REG_LP_I2C_ANA_MST_BASE                0x20813000
#define DR_REG_LP_TRNG_BASE                       0x20814000
#define DR_REG_ADC_BASE                           0x20815000
#define DR_REG_TOUCH_SENS_BASE                    0x20816000
#define DR_REG_LP_MAILBOX_BASE                    0x20817000
#define DR_REG_TSENS_BASE                         0x20818000
#define DR_REG_LP_AHB_PDMA_BASE                   0x20819000
#define DR_REG_LP_DAC_BASE                        0x2081A000
#define DR_REG_CACHE_BASE                         0x2C000000
#define DR_REG_TRACE0_BASE                        0x2D000000
#define DR_REG_TRACE1_BASE                        0x2D001000
#define DR_REG_BUS_MONITOR_BASE                   0x2D002000
#define DR_REG_MEM_MONITOR0_BASE                  0x2D003000
#define DR_REG_MEM_MONITOR1_BASE                  0x2D004000



/**
 * This are module helper MACROs for quick module reference
 * including some module(renamed) address
 */
#define DR_REG_UART_BASE                        DR_REG_UART0_BASE
#define DR_REG_TIMERGROUP0_BASE                 DR_REG_TIMERG0_BASE
#define DR_REG_TIMERGROUP1_BASE                 DR_REG_TIMERG1_BASE
#define DR_REG_I2S_BASE                         DR_REG_I2S0_BASE
#define DR_REG_INTERRUPT_MATRIX_BASE            DR_REG_INTR0_BASE
#define DR_REG_MCPWM_BASE                       DR_REG_MCPWM0_BASE
#define DR_REG_PVT_MONITOR_BASE                 DR_REG_PVT_BASE
#define DR_REG_ECC_MULT_BASE                    (DR_REG_AES_BASE + 0x3000)
#define DR_REG_HP_CLKRST_BASE                   DR_REG_HP_SYS_CLKRST_BASE
#define DR_REG_DSPI_MEM_BASE                    DR_REG_PSRAM_MSPI0_BASE
#define DR_REG_INTERRUPT_CORE0_BASE             DR_REG_INTR0_BASE
#define DR_REG_INTERRUPT_CORE1_BASE             (DR_REG_INTR0_BASE + 0x800)
#define DR_REG_LPPERI_BASE                      DR_REG_LP_PERICLKRST_BASE
#define DR_REG_SDHOST_BASE                      DR_REG_SDMMC_BASE
#define DR_REG_TRACE_BASE                       DR_REG_TRACE0_BASE
#define DR_REG_MB_BASE                          DR_REG_LP_MAILBOX_BASE
#define DR_REG_AXI_IC_BASE                      (DR_REG_ICM_SYS_BASE + 0x400)
