/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_BOOT_MODE_H_
#define _SOC_BOOT_MODE_H_

#include "soc.h"

/*SPI Boot*/
#define IS_1XXX(v)                              (((v)&0x08)==0x08)

/*Download Boot, USB/SPI(or SDIO_V2)/UART0/UART1*/
#define IS_01XX(v)                              (((v)&0x0c)==0x04)

/*Download Boot, USB/SPI(or SDIO_V2)/UART0/UART1*/
#define IS_0100(v)                              (((v)&0x0f)==0x04)

/*Download Boot, USB/SPI(or SDIO_V2)/UART0/UART1*/
#define IS_0101(v)                              (((v)&0x0f)==0x05)

/*Download Boot, USB/SPI(or SDIO_V2)/UART0/UART1*/
#define IS_0110(v)                              (((v)&0x0f)==0x06)

/*Download Boot, USB/SPI(or SDIO_V2)/UART0/UART1*/
#define IS_0111(v)                              (((v)&0x0f)==0x07)

/*Diagnostic Mode1+USB download Mode*/
#define IS_0000(v)                              (((v)&0x0f)==0x00)

/*SPI(or SDIO_V1) download Mode*/
#define IS_0001(v)                              (((v)&0x0f)==0x01)

/*Diagnostic Mode0+UART0 download Mode*/
#define IS_0010(v)                              (((v)&0x0f)==0x02)

/*ATE/ANALOG Mode*/
#define IS_0011(v)                              (((v)&0x0f)==0x03)

/*print control*/
#define IS_X1XX(v)                              (((v)&0x04)==0x04)

#define BOOT_MODE_GET()                         (GPIO_REG_READ(GPIO_STRAP_REG))

#define ETS_PRINT_CONTROL_HIGH_LEVEL()          IS_X1XX(BOOT_MODE_GET())

/*do not include download mode*/
#define ETS_IS_UART_BOOT()                      IS_0010(BOOT_MODE_GET())

#define ETS_IS_USB_BOOT()                       IS_0000(BOOT_MODE_GET())

/*all spi boot including spi/legacy*/
#define ETS_IS_FLASH_BOOT()                     IS_1XXX(BOOT_MODE_GET())

/*all faster spi boot including spi*/
#define ETS_IS_FAST_FLASH_BOOT()                IS_1XXX(BOOT_MODE_GET())

#if SUPPORT_SDIO_DOWNLOAD

/*all sdio V2 of failing edge input, failing edge output*/
#define ETS_IS_SDIO_FEI_FEO_V2_BOOT()           IS_0100(BOOT_MODE_GET())

/*all sdio V2 of failing edge input, raising edge output*/
#define ETS_IS_SDIO_FEI_REO_V2_BOOT()           IS_0101(BOOT_MODE_GET())

/*all sdio V2 of raising edge input, failing edge output*/
#define ETS_IS_SDIO_REI_FEO_V2_BOOT()           IS_0110(BOOT_MODE_GET())

/*all sdio V2 of raising edge input, raising edge output*/
#define ETS_IS_SDIO_REI_REO_V2_BOOT()           IS_0111(BOOT_MODE_GET())

/*all sdio V1 of raising edge input, failing edge output*/
#define ETS_IS_SDIO_REI_FEO_V1_BOOT()           IS_0001(BOOT_MODE_GET())

/*do not include joint download mode*/
#define ETS_IS_SDIO_BOOT()                      IS_0001(BOOT_MODE_GET())
#else

/*do not include joint download mode*/
#define ETS_IS_SPI_DOWNLOAD_BOOT()              IS_0001(BOOT_MODE_GET())

#endif

/*joint download boot*/
#define ETS_IS_JOINT_DOWNLOAD_BOOT()            IS_01XX(BOOT_MODE_GET())

/*ATE mode*/
#define ETS_IS_ATE_BOOT()                       IS_0011(BOOT_MODE_GET())

/*used by  ETS_IS_SDIO_UART_BOOT*/
#define SEL_NO_BOOT                             0
#define SEL_SDIO_BOOT                           BIT0
#define SEL_UART_BOOT                           BIT1
#define SEL_SPI_SLAVE_BOOT                      BIT2
#define SEL_USB_BOOT                            BIT3

#endif /* _SOC_BOOT_MODE_H_ */
