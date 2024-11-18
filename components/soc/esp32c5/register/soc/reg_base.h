/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief Peripheral 0 Modules
 *
 */
#define DR_REG_UART0_BASE                         0x60000000
#define DR_REG_UART1_BASE                         0x60001000
#define DR_REG_SPIMEM0_BASE                       0x60002000
#define DR_REG_SPIMEM1_BASE                       0x60003000
#define DR_REG_I2C_BASE                           0x60004000
#define DR_REG_UHCI_BASE                          0x60005000
#define DR_REG_RMT_BASE                           0x60006000
#define DR_REG_LEDC_BASE                          0x60007000
#define DR_REG_TIMERG0_BASE                       0x60008000
#define DR_REG_TIMERG1_BASE                       0x60009000
#define DR_REG_SYSTIMER_BASE                      0x6000A000
#define DR_REG_TWAI0_BASE                         0x6000B000
#define DR_REG_I2S_BASE                           0x6000C000
#define DR_REG_TWAI1_BASE                         0x6000D000
#define DR_REG_APB_SARADC_BASE                    0x6000E000
#define DR_REG_USB_SERIAL_JTAG_BASE               0x6000F000
#define DR_REG_INTERRUPT_MATRIX_BASE              0x60010000
#define DR_REG_INTERRUPT_CORE0_BASE               DR_REG_INTERRUPT_MATRIX_BASE
#define DR_REG_PCNT_BASE                          0x60012000
#define DR_REG_SOC_ETM_BASE                       0x60013000
#define DR_REG_MCPWM_BASE                         0x60014000
#define DR_REG_PARL_IO_BASE                       0x60015000
#define DR_REG_PVT_MONITOR_BASE                   0x60019000
#define DR_REG_PSRAM_MEM_MONITOR_BASE             0x6001A000

/**
 * @brief Peripheral 1 Modules
 *
 */
#define DR_REG_AHB_DMA_BASE                       0x60080000
#define DR_REG_SPI2_BASE                          0x60081000
#define DR_REG_BITSCRAMBLER_BASE                  0x60082000
#define DR_REG_KEYMNG_BASE                        0x60087000
#define DR_REG_AES_BASE                           0x60088000
#define DR_REG_SHA_BASE                           0x60089000
#define DR_REG_RSA_BASE                           0x6008A000
#define DR_REG_ECC_MULT_BASE                      0x6008B000
#define DR_REG_DS_BASE                            0x6008C000
#define DR_REG_HMAC_BASE                          0x6008D000
#define DR_REG_ECDSA_BASE                         0x6008E000

/**
 * @brief HP Top Peripheral Modules
 *
 */
#define DR_REG_IO_MUX_BASE                        0x60090000
#define DR_REG_GPIO_BASE                          0x60091000
#define DR_REG_GPIO_EXT_BASE                      0x60091e00
#define DR_REG_MEM_MONITOR_BASE                   0x60092000
#define DR_REG_PAU_BASE                           0x60093000
#define DR_REG_HP_SYSTEM_BASE                     0x60095000
#define DR_REG_PCR_BASE                           0x60096000
#define DR_REG_TEE_BASE                           0x60098000
#define DR_REG_HP_APM_BASE                        0x60099000
#define DR_REG_LP_APM0_BASE                       0x60099800
#define DR_REG_MISC_BASE                          0x6009F000

/**
 * @brief Modem Module
 *
 */
#define DR_REG_MODEM0_BASE                        0x600A0000
#define DR_REG_MODEM1_BASE                        0x600AC000
#define DR_REG_MODEM_PWR0_BASE                    0x600AD000
#define DR_REG_MODEM_PWR1_BASE                    0x600AF000

#define PWDET_CONF_REG                            0x600A0810
#define DR_REG_I2C_ANA_MST_BASE                   0x600AF800
#define IEEE802154_REG_BASE                       0x600A3000

/**
 * @brief LP System (RTC) Modules
 *
 */
#define DR_REG_PMU_BASE                           0x600B0000
#define DR_REG_LP_CLKRST_BASE                     0x600B0400
#define DR_REG_LP_TIMER_BASE                      0x600B0C00
#define DR_REG_LP_AON_BASE                        0x600B1000
#define DR_REG_LP_UART_BASE                       0x600B1400
#define DR_REG_LP_I2C_BASE                        0x600B1800
#define DR_REG_LP_WDT_BASE                        0x600B1C00
#define DR_REG_LP_I2C_ANA_MST_BASE                0x600B2400
#define DR_REG_LPPERI_BASE                        0x600B2800
#define DR_REG_LP_ANA_BASE                        0x600B2C00
#define DR_REG_HUK_BASE                           0x600B3000
#define DR_REG_LP_TEE_BASE                        0x600B3400
#define DR_REG_LP_APM_BASE                        0x600B3800
#define DR_REG_LP_IO_MUX_BASE                     0x600B4000
#define DR_REG_LP_GPIO_BASE                       0x600B4400
#define DR_REG_EFUSE_BASE                         0x600B4800
#define DR_REG_OTP_DEBUG_BASE                     0x600B4D00

/**
 * @brief CPU Peripheral Modules
 *
 */
#define DR_REG_TRACE_BASE                         0x600C0000
#define DR_REG_ASSIST_DEBUG_BASE                  0x600C2000
#define DR_REG_INTPRI_BASE                        0x600C5000
#define DR_REG_CACHE_BASE                         0x600C8000  // CACHE_CONFIG/EXTMEM
#define DR_REG_CLINT_M_BASE                       0x20000000
