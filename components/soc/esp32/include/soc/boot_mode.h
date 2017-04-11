// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SOC_BOOT_MODE_H_
#define _SOC_BOOT_MODE_H_

#include "soc.h"

/*SPI Boot*/
#define IS_1XXXX(v)				(((v)&0x10)==0x10)

/*HSPI Boot*/
#define IS_010XX(v)				(((v)&0x1c)==0x08)

/*Download Boot, SDIO/UART0/UART1*/
#define IS_00XXX(v)				(((v)&0x18)==0x00)

/*Download Boot, SDIO/UART0/UART1,FEI_FEO V2*/
#define IS_00X00(v)				(((v)&0x1b)==0x00)

/*Download Boot, SDIO/UART0/UART1,FEI_REO V2*/
#define IS_00X01(v)				(((v)&0x1b)==0x01)

/*Download Boot, SDIO/UART0/UART1,REI_FEO V2*/
#define IS_00X10(v)				(((v)&0x1b)==0x02)

/*Download Boot, SDIO/UART0/UART1,REI_FEO V2*/
#define IS_00X11(v)				(((v)&0x1b)==0x03)

/*ATE/ANALOG Mode*/
#define IS_01110(v)				(((v)&0x1f)==0x0e)

/*Diagnostic Mode+UART0 download Mode*/
#define IS_01111(v)				(((v)&0x1f)==0x0f)

/*legacy SPI Boot*/
#define IS_01100(v)				(((v)&0x1f)==0x0c)

/*SDIO_Slave download Mode V1.1*/
#define IS_01101(v)				(((v)&0x1f)==0x0d)



#define BOOT_MODE_GET()                                 (GPIO_REG_READ(GPIO_STRAP))

/*do not include download mode*/
#define ETS_IS_UART_BOOT()				IS_01111(BOOT_MODE_GET())

/*all spi boot including spi/hspi/legacy*/
#define ETS_IS_FLASH_BOOT()				(IS_1XXXX(BOOT_MODE_GET()) || IS_010XX(BOOT_MODE_GET()) || IS_01100(BOOT_MODE_GET()))

/*all faster spi boot including spi/hspi*/
#define ETS_IS_FAST_FLASH_BOOT()				(IS_1XXXX(BOOT_MODE_GET()) || IS_010XX(BOOT_MODE_GET()))

/*all spi boot including spi/legacy*/
#define ETS_IS_SPI_FLASH_BOOT()				(IS_1XXXX(BOOT_MODE_GET()) || IS_01100(BOOT_MODE_GET()))

/*all spi boot including hspi/legacy*/
#define ETS_IS_HSPI_FLASH_BOOT()				IS_010XX(BOOT_MODE_GET())

/*all sdio V2 of failing edge input, failing edge output*/
#define ETS_IS_SDIO_FEI_FEO_V2_BOOT()				IS_00X00(BOOT_MODE_GET())

/*all sdio V2 of failing edge input, raising edge output*/
#define ETS_IS_SDIO_FEI_REO_V2_BOOT()				IS_00X01(BOOT_MODE_GET())

/*all sdio V2 of raising edge input, failing edge output*/
#define ETS_IS_SDIO_REI_FEO_V2_BOOT()				IS_00X10(BOOT_MODE_GET())

/*all sdio V2 of raising edge input, raising edge output*/
#define ETS_IS_SDIO_REI_REO_V2_BOOT()				IS_00X11(BOOT_MODE_GET())

/*all sdio V1 of raising edge input, failing edge output*/
#define ETS_IS_SDIO_REI_FEO_V1_BOOT()				IS_01101(BOOT_MODE_GET())

/*do not include download mode*/
#define ETS_IS_SDIO_BOOT()				IS_01101(BOOT_MODE_GET())

/*joint download boot*/
#define ETS_IS_SDIO_UART_BOOT()				IS_00XXX(BOOT_MODE_GET())

/*ATE mode*/
#define ETS_IS_ATE_BOOT()				IS_01110(BOOT_MODE_GET())

/*A bit to control flash boot print*/
#define ETS_IS_PRINT_BOOT()                             (BOOT_MODE_GET() & 0x2)

/*used by  ETS_IS_SDIO_UART_BOOT*/
#define SEL_NO_BOOT                                    0
#define SEL_SDIO_BOOT                                 BIT0
#define SEL_UART_BOOT                                BIT1

#endif /* _SOC_BOOT_MODE_H_ */
