/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define DR_REG_TRACE0_BASE                        0x60000000
#define DR_REG_TRACE1_BASE                        0x60001000
#define DR_REG_BUS_MONITOR_BASE                   0x60002000
#define DR_REG_INTPRI_BASE                        0x60005000
#define DR_REG_CACHE_BASE                         0x60008000
#define DR_REG_GPSPI2_BASE                        0x60010000
#define DR_REG_GPSPI3_BASE                        0x60011000
#define DR_REG_UART0_BASE                         0x60012000
#define DR_REG_UART1_BASE                         0x60013000
#define DR_REG_UHCI0_BASE                         0x60014000
#define DR_REG_I2C0_BASE                          0x60015000
#define DR_REG_I2C1_BASE                          0x60016000
#define DR_REG_I2S0_BASE                          0x60017000
#define DR_REG_PARL_IO_BASE                       0x60018000
#define DR_REG_MCPWM0_BASE                        0x60019000
#define DR_REG_MCPWM1_BASE                        0x6001A000
#define DR_REG_LEDC_BASE                          0x6001B000
#define DR_REG_TWAIFD_BASE                        0x6001C000
#define DR_REG_USB_SERIAL_JTAG_BASE               0x6001D000
#define DR_REG_RMT_BASE                           0x6001E000
#define DR_REG_AHB_DMA_BASE                       0x6001F000
#define DR_REG_PAU_BASE                           0x60020000
#define DR_REG_SOC_ETM_BASE                       0x60021000
#define DR_REG_APB_SARADC_BASE                    0x60022000
#define DR_REG_SYSTIMER_BASE                      0x60023000
#define DR_REG_MEM_MONITOR_BASE                   0x60025000
#define DR_REG_PVT_BASE                           0x60026000
#define DR_REG_PCNT_BASE                          0x60027000
#define DR_REG_SAMPLE_RATE_CONVERTER_BASE         0x60028000
#define DR_REG_ZERO_DET_BASE                      0x60029000
#define DR_REG_USB_OTG_FS_CORE0_BASE              0x60040000
#define DR_REG_USB_OTG_FS_CORE1_BASE              0x6007F000
#define DR_REG_USB_OTG_FS_PHY_BASE                0x60080000
#define DR_REG_TIMERG0_BASE                       0x60090000
#define DR_REG_TIMERG1_BASE                       0x60091000
#define DR_REG_IO_MUX_BASE                        0x60092000
#define DR_REG_GPIO_BASE                          0x60093000
#define DR_REG_GPIO_EXT_BASE                      0x60093E00
#define DR_REG_PCR_BASE                           0x60094000
#define DR_REG_SPIMEM0_BASE                       0x60098000
#define DR_REG_SPIMEM1_BASE                       0x60099000
#define DR_REG_INTMTX0_BASE                       0x6009A000
#define DR_REG_INTMTX1_BASE                       0x6009B000
#define DR_REG_HP_SYSTEM_BASE                     0x6009C000
#define DR_REG_HP_APM_BASE                        0x6009D000
#define DR_REG_CPU_APM_BASE                       0x6009E000
#define DR_REG_TEE_BASE                           0x6009F000
#define DR_REG_KEYMNG_BASE                        0x600A5000
#define DR_REG_AES_BASE                           0x600A6000
#define DR_REG_SHA_BASE                           0x600A7000
#define DR_REG_ECC_BASE                           0x600A8000
#define DR_REG_HMAC_BASE                          0x600A9000
#define DR_REG_ECDSA_BASE                         0x600AA000
#define DR_REG_HUK_BASE                           0x600B1000
#define DR_REG_LP_TEE_BASE                        0x600B1400
#define DR_REG_EFUSE_BASE                         0x600B1800
#define DR_REG_OTP_DEBUG_BASE                     0x600B1C00
#define DR_REG_TRNG_BASE                          0x600B2000
#define DR_REG_PMU_BASE                           0x600B2400
#define DR_REG_LP_AON_BASE                        0x600B2800
#define DR_REG_LP_ANA_PERI_BASE                   0x600B2C00
#define DR_REG_LP_CLKRST_BASE                     0x600B3000
#define DR_REG_LPPERI_BASE                        0x600B3400
#define DR_REG_LP_IO_MUX_BASE                     0x600B3800
#define DR_REG_LP_GPIO_BASE                       0x600B3C00
#define DR_REG_LP_TIMER_BASE                      0x600B5000
#define DR_REG_LP_WDT_BASE                        0x600B5400
#define DR_REG_TOUCH_SENS_BASE                    0x600B5800
#define DR_REG_TOUCH_AON_BASE                     0x600B5C00
#define DR_REG_I2C_ANA_MST_BASE                   0x600CF800 // (DR_REG_MODEM_PWR_BASE + 0x2800)

#define DR_REG_CLINT_M_BASE                       0x20000000    // TODO: [ESP32H4] IDF-12303 inherit from verify code, need check
