/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define DR_REG_UART0_BASE                       0x60000000
#define DR_REG_UART1_BASE                       0x60001000
#define DR_REG_MSPI0_BASE                       0x60002000
#define DR_REG_MSPI1_BASE                       0x60003000
#define DR_REG_I2C_BASE                         0x60004000
#define DR_REG_UART2_BASE                       0x60006000
#define DR_REG_LEDC_BASE                        0x60007000
#define DR_REG_TIMG0_BASE                       0x60008000
#define DR_REG_TIMG1_BASE                       0x60009000
#define DR_REG_SYSTIMER_BASE                    0x6000A000
#define DR_REG_I2S_BASE                         0x6000C000
#define DR_REG_SARADC_BASE                      0x6000E000
#define DR_REG_USB_SERIAL_JTAG_BASE             0x6000F000
#define DR_REG_INTERRUPT_MATRIX_BASE            0x60010000
#define DR_REG_SOC_ETM_BASE                     0x60013000
#define DR_REG_PVT_MONITOR_BASE                 0x60019000
#define DR_REG_PSRAM_MEM_MONITOR_BASE           0x6001A000
#define DR_REG_AHB_GDMA_BASE                    0x60080000
#define DR_REG_GPSPI_BASE                       0x60081000
#define DR_REG_SHA_BASE                         0x60089000
#define DR_REG_ECC_BASE                         0x6008B000
#define DR_REG_ECDSA_BASE                       0x6008E000
#define DR_REG_IO_MUX_BASE                      0x60090000
#define DR_REG_GPIO_BASE                        0x60091000
#define DR_REG_TCM_MEM_MONITOR_BASE             0x60092000
#define DR_REG_PAU_BASE                         0x60093000
#define DR_REG_HP_SYSTEM_BASE                   0x60095000
#define DR_REG_PCR_BASE                         0x60096000
#define DR_REG_TEE_REG_BASE                     0x60098000
#define DR_REG_HP_APM_REG_BASE                  0x60099000
#define DR_REG_MISC_BASE                        0x6009F000
#define DR_REG_MODEM0_BASE                      0x600A0000
#define DR_REG_MODEM1_BASE                      0x600AC000
#define DR_REG_MODEM_PWR0_BASE                  0x600AD000
#define DR_REG_MODEM_PWR1_BASE                  0x600AF000
#define DR_REG_I2C_ANA_MST_BASE                 0x600AF800  //TODO: [ESP32C61] IDF-9276, from verify
#define DR_REG_PMU_BASE                         0x600B0000
#define DR_REG_LP_CLKRST_BASE                   0x600B0400
#define DR_REG_LP_TIMER_BASE                    0x600B0C00
#define DR_REG_LP_AON_BASE                      0x600B1000
#define DR_REG_LP_WDT_BASE                      0x600B1C00
#define DR_REG_LPPERI_BASE                      0x600B2800
#define DR_REG_LP_ANA_BASE                      0x600B2C00
#define DR_REG_LP_TEE_BASE                      0x600B3400
#define DR_REG_LP_APM_BASE                      0x600B3800
#define DR_REG_LP_IO_MUX_BASE                   0x600B4000
#define DR_REG_LP_GPIO_BASE                     0x600B4400
#define DR_REG_EFUSE0_BASE                      0x600B4800
#define DR_REG_EFUSE1_BASE                      0x600B4C00
#define DR_REG_TRACE_BASE                       0x600C0000
#define DR_REG_ASSIST_DEBUG_BASE                0x600C2000
#define DR_REG_INTPRI_BASE                      0x600C5000
#define DR_REG_CACHE_BASE                       0x600C8000
