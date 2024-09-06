/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define DR_REG_SYSTEM_BASE                      0x600c0000
#define DR_REG_SENSITIVE_BASE                   0x600c1000
#define DR_REG_INTERRUPT_BASE                   0x600c2000
#define DR_REG_EXTMEM_BASE                      0x600c4000 // CACHE_CONFIG
#define DR_REG_MMU_TABLE                        0x600c5000
#define DR_REG_SHA_BASE                         0x6003b000
#define DR_REG_ECC_MULT_BASE                    0x6003e000
#define DR_REG_GDMA_BASE                        0x6003f000
#define DR_REG_ASSIST_DEBUG_BASE                0x600ce000
#define DR_REG_DEDICATED_GPIO_BASE              0x600cf000
#define DR_REG_WORLD_CNTL_BASE                  0x600d0000
#define DR_REG_UART_BASE                        0x60000000
#define DR_REG_SPI1_BASE                        0x60002000
#define DR_REG_SPI0_BASE                        0x60003000
#define DR_REG_GPIO_BASE                        0x60004000
#define DR_REG_FE2_BASE                         0x60005000
#define DR_REG_FE_BASE                          0x60006000
#define DR_REG_RTCCNTL_BASE                     0x60008000
#define DR_REG_IO_MUX_BASE                      0x60009000
#define DR_REG_RTC_I2C_BASE                     0x6000e000
#define DR_REG_UART1_BASE                       0x60010000
#define DR_REG_I2C_EXT_BASE                     0x60013000
#define DR_REG_LEDC_BASE                        0x60019000
#define DR_REG_EFUSE_BASE                       0x60008800
#define DR_REG_NRX_BASE                         0x6001CC00
#define DR_REG_BB_BASE                          0x6001D000
#define DR_REG_TIMERGROUP0_BASE                 0x6001F000
#define DR_REG_SYSTIMER_BASE                    0x60023000
#define DR_REG_SPI2_BASE                        0x60024000
#define DR_REG_SYSCON_BASE                      0x60026000
#define DR_REG_APB_SARADC_BASE                  0x60040000
#define DR_REG_WDEVLE_BASE                      0x60045000
#define DR_REG_ETM_BIT_BASE                     0x6004B000
#define DR_REG_BLE_TIMER_BASE                   0x6004B800
#define DR_REG_BLE_SEC_BASE                     0x6004C000
#define DR_REG_COEX_BIT_BASE                    0x6004C400
#define DR_REG_MODEM_CLKRST_BASE                0x6004d800
#define DR_REG_I2C_MST_BASE                     0x6004E800
#define DR_REG_XTS_AES_BASE                     0x600CC000

/* For backward compatibility with the older register name */
#define DR_REG_AES_XTS_BASE                     DR_REG_XTS_AES_BASE
