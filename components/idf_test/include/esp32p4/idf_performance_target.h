/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//TODO: IDF-8313 update after chips back and PLL setup
#define IDF_PERFORMANCE_MAX_SPI_CLK_FREQ                                        10*1000*1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        1000

/* Spinlock performance on esp32p4 is slower. May need to adjust these values once IDF-7898 is fixed */
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP                     380
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE             135
