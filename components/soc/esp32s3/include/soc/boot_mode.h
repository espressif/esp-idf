// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "soc.h"

/*SPI Boot*/
#define IS_1XXX(v)                              (((v)&0x08)==0x08)

/*Download Boot, USB/SPI(or SDIO_V2)/UART0/UART1*/
#define IS_00XX(v)                              (((v)&0x0c)==0x00)

/*Download Boot, SDIO/UART0/UART1,FEI_FEO V2*/
#define IS_0000(v)                              (((v)&0x0f)==0x00)

/*Download Boot, SDIO/UART0/UART1,FEI_REO V2*/
#define IS_0001(v)                              (((v)&0x0f)==0x01)

/*Download Boot, SDIO/UART0/UART1,REI_FEO V2*/
#define IS_0010(v)                              (((v)&0x0f)==0x02)

/*Download Boot, SDIO/UART0/UART1,REI_REO V2*/
#define IS_0011(v)                              (((v)&0x0f)==0x03)

/*legacy SPI Boot*/
#define IS_0100(v)                              (((v)&0x0f)==0x04)

/*ATE/ANALOG Mode*/
#define IS_0101(v)                              (((v)&0x0f)==0x05)

/*SPI(or SDIO_V1) download Mode*/
#define IS_0110(v)                              (((v)&0x0f)==0x06)

/*Diagnostic Mode+UART0 download Mode*/
#define IS_0111(v)                              (((v)&0x0f)==0x07)



#define BOOT_MODE_GET()                         (GPIO_REG_READ(GPIO_STRAP_REG))

/*do not include download mode*/
#define ETS_IS_UART_BOOT()                      IS_0111(BOOT_MODE_GET())

/*all spi boot including spi/legacy*/
#define ETS_IS_FLASH_BOOT()                     (IS_1XXX(BOOT_MODE_GET()) || IS_0100(BOOT_MODE_GET()))

/*all faster spi boot including spi*/
#define ETS_IS_FAST_FLASH_BOOT()                IS_1XXX(BOOT_MODE_GET())

#if SUPPORT_SDIO_DOWNLOAD

/*all sdio V2 of failing edge input, failing edge output*/
#define ETS_IS_SDIO_FEI_FEO_V2_BOOT()           IS_0000(BOOT_MODE_GET())

/*all sdio V2 of failing edge input, raising edge output*/
#define ETS_IS_SDIO_FEI_REO_V2_BOOT()           IS_0001(BOOT_MODE_GET())

/*all sdio V2 of raising edge input, failing edge output*/
#define ETS_IS_SDIO_REI_FEO_V2_BOOT()           IS_0010(BOOT_MODE_GET())

/*all sdio V2 of raising edge input, raising edge output*/
#define ETS_IS_SDIO_REI_REO_V2_BOOT()           IS_0011(BOOT_MODE_GET())

/*all sdio V1 of raising edge input, failing edge output*/
#define ETS_IS_SDIO_REI_FEO_V1_BOOT()           IS_0110(BOOT_MODE_GET())

/*do not include joint download mode*/
#define ETS_IS_SDIO_BOOT()                      IS_0110(BOOT_MODE_GET())
#else

/*do not include joint download mode*/
#define ETS_IS_SPI_DOWNLOAD_BOOT()              IS_0110(BOOT_MODE_GET())

#endif

/*joint download boot*/
#define ETS_IS_JOINT_DOWNLOAD_BOOT()                  IS_00XX(BOOT_MODE_GET())

/*ATE mode*/
#define ETS_IS_ATE_BOOT()                       IS_0101(BOOT_MODE_GET())

/*used by  ETS_IS_SDIO_UART_BOOT*/
#define SEL_NO_BOOT                             0
#define SEL_SDIO_BOOT                           BIT0
#define SEL_UART_BOOT                           BIT1
#define SEL_SPI_SLAVE_BOOT                      BIT2
