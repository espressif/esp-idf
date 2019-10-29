// Copyright 2010-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "esp_assert.h"
#endif

#include <esp_bit_defs.h>

#define PRO_CPU_NUM (0)

#define SOC_MAX_CONTIGUOUS_RAM_SIZE (SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW) ///< Largest span of contiguous memory (DRAM or IRAM) in the address space

#define DR_REG_SYSTEM_BASE                      0x3f4c0000
#define DR_REG_SENSITIVE_BASE                   0x3f4c1000
#define DR_REG_INTERRUPT_BASE                   0x3f4c2000
#define DR_REG_DMA_COPY_BASE                    0x3f4c3000
#define DR_REG_EXTMEM_BASE                      0x3f4c4000
#define DR_REG_MMU_TABLE                        0x3f4c5000
#define DR_REG_ITAG_TABLE                       0x3f4c6000
#define DR_REG_DTAG_TABLE                       0x3f4c7000
#define DR_REG_AES_BASE                         0x3f4c8000
#define DR_REG_SHA_BASE                         0x3f4c9000
#define DR_REG_RSA_BASE                         0x3f4ca000
#define DR_REG_SECURE_BOOT_BASE                 0x3f4cb000
#define DR_REG_HMAC_BASE                        0x3f4cc000
#define DR_REG_DIGITAL_SINGNATURE_BASE          0x3f4cd000
#define DR_REG_ASSIST_DEBUG_BASE                0x3f4ce000
#define DR_REG_DEDICATED_GPIO_BASE              0x3f4cf000
#define DR_REG_INTRUSION_BASE                   0x3f4d0000
#define DR_REG_DPORT_END                        0x3f4d3FFC
#define DR_REG_UART_BASE                        0x3f400000
#define DR_REG_SPI1_BASE                        0x3f402000
#define DR_REG_SPI0_BASE                        0x3f403000
#define DR_REG_GPIO_BASE                        0x3f404000
#define DR_REG_GPIO_SD_BASE                     0x3f404f00
#define DR_REG_FE2_BASE                         0x3f405000
#define DR_REG_FE_BASE                          0x3f406000
#define DR_REG_FRC_TIMER_BASE                   0x3f407000
#define DR_REG_RTCCNTL_BASE                     0x3f408000
#define DR_REG_RTCIO_BASE                       0x3f408400
#define DR_REG_SENS_BASE                        0x3f408800
#define DR_REG_RTC_I2C_BASE                     0x3f408C00
#define DR_REG_IO_MUX_BASE                      0x3f409000
#define DR_REG_HINF_BASE                        0x3f40B000
#define DR_REG_I2S_BASE                         0x3f40F000
#define DR_REG_UART1_BASE                       0x3f410000
#define DR_REG_I2C_EXT_BASE                     0x3f413000
#define DR_REG_UHCI0_BASE                       0x3f414000
#define DR_REG_SLCHOST_BASE                     0x3f415000
#define DR_REG_RMT_BASE                         0x3f416000
#define DR_REG_PCNT_BASE                        0x3f417000
#define DR_REG_SLC_BASE                         0x3f418000
#define DR_REG_LEDC_BASE                        0x3f419000
#define DR_REG_EFUSE_BASE                       0x3f41A000
#define DR_REG_NRX_BASE                         0x3f41CC00
#define DR_REG_BB_BASE                          0x3f41D000
#define DR_REG_TIMERGROUP0_BASE                 0x3f41F000
#define DR_REG_TIMERGROUP1_BASE                 0x3f420000
#define DR_REG_RTC_SLOWMEM_BASE                 0x3f421000
#define DR_REG_SPI2_BASE                        0x3f424000
#define DR_REG_SPI3_BASE                        0x3f425000
#define DR_REG_SYSCON_BASE                      0x3f426000
#define DR_REG_APB_CTRL_BASE                    0x3f426000    /* Old name for SYSCON, to be removed */
#define DR_REG_I2C1_EXT_BASE                    0x3f427000
#define DR_REG_SPI4_BASE                        0x3f437000
#define DR_REG_USB_WRAP_BASE                    0x3f439000

#define REG_UHCI_BASE(i)         (DR_REG_UHCI0_BASE)
#define REG_UART_BASE( i )  (DR_REG_UART_BASE + (i) * 0x10000 )
#define REG_UART_AHB_BASE(i)  (0x60000000 + (i) * 0x10000 )
#define UART_FIFO_AHB_REG(i)  (REG_UART_AHB_BASE(i) + 0x0)
#define REG_I2S_BASE( i ) (DR_REG_I2S_BASE)
#define REG_TIMG_BASE(i)              (DR_REG_TIMERGROUP0_BASE + (i)*0x1000)
#define REG_SPI_MEM_BASE(i)     (DR_REG_SPI0_BASE - (i) * 0x1000)
#define REG_I2C_BASE(i)    (DR_REG_I2C_EXT_BASE + (i) * 0x14000 )

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr)

#ifndef __ASSEMBLER__

//write value to register
#define REG_WRITE(_r, _v) (*(volatile uint32_t *)(_r)) = (_v)

//read value from register
#define REG_READ(_r) (*(volatile uint32_t *)(_r))

//get bit or get bits from register
#define REG_GET_BIT(_r, _b)  (*(volatile uint32_t*)(_r) & (_b))

//set bit or set bits to register
#define REG_SET_BIT(_r, _b)  (*(volatile uint32_t*)(_r) |= (_b))

//clear bit or clear bits of register
#define REG_CLR_BIT(_r, _b)  (*(volatile uint32_t*)(_r) &= ~(_b))

//set bits of register controlled by mask
#define REG_SET_BITS(_r, _b, _m) (*(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m)))

//get field from register, uses field _S & _V to determine mask
#define REG_GET_FIELD(_r, _f) ((REG_READ(_r) >> (_f##_S)) & (_f##_V))

//set field of a register from variable, uses field _S & _V to determine mask
#define REG_SET_FIELD(_r, _f, _v) (REG_WRITE((_r),((REG_READ(_r) & ~((_f##_V) << (_f##_S)))|(((_v) & (_f##_V))<<(_f##_S)))))

//get field value from a variable, used when _f is not left shifted by _f##_S
#define VALUE_GET_FIELD(_r, _f) (((_r) >> (_f##_S)) & (_f))

//get field value from a variable, used when _f is left shifted by _f##_S
#define VALUE_GET_FIELD2(_r, _f) (((_r) & (_f))>> (_f##_S))

//set field value to a variable, used when _f is not left shifted by _f##_S
#define VALUE_SET_FIELD(_r, _f, _v) ((_r)=(((_r) & ~((_f) << (_f##_S)))|((_v)<<(_f##_S))))

//set field value to a variable, used when _f is left shifted by _f##_S
#define VALUE_SET_FIELD2(_r, _f, _v) ((_r)=(((_r) & ~(_f))|((_v)<<(_f##_S))))

//generate a value from a field value, used when _f is not left shifted by _f##_S
#define FIELD_TO_VALUE(_f, _v) (((_v)&(_f))<<_f##_S)

//generate a value from a field value, used when _f is left shifted by _f##_S
#define FIELD_TO_VALUE2(_f, _v) (((_v)<<_f##_S) & (_f))

//read value from register
#define READ_PERI_REG(addr) (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr)))

//write value to register
#define WRITE_PERI_REG(addr, val) (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val)

//clear bits of register controlled by mask
#define CLEAR_PERI_REG_MASK(reg, mask) WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))))

//set bits of register controlled by mask
#define SET_PERI_REG_MASK(reg, mask) WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)))

//get bits of register controlled by mask
#define GET_PERI_REG_MASK(reg, mask) (READ_PERI_REG(reg) & (mask))

//get bits of register controlled by highest bit and lowest bit
#define GET_PERI_REG_BITS(reg, hipos,lowpos) ((READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1))

//set bits of register controlled by mask and shift
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) (WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift)) ))

//get field of register
#define GET_PERI_REG_BITS2(reg, mask,shift) ((READ_PERI_REG(reg)>>(shift))&(mask))

#endif /* !__ASSEMBLER__ */
//}}

//Periheral Clock {{
#define  APB_CLK_FREQ_ROM                            ( 40*1000000 )
#define  CPU_CLK_FREQ_ROM                            APB_CLK_FREQ_ROM
#define  UART_CLK_FREQ_ROM                           APB_CLK_FREQ_ROM
#define  CPU_CLK_FREQ                                APB_CLK_FREQ
#define  APB_CLK_FREQ                                ( 80*1000000 )       //unit: Hz
#define  REF_CLK_FREQ                                ( 1000000 )
#define  UART_CLK_FREQ                               APB_CLK_FREQ
#define  WDT_CLK_FREQ                                APB_CLK_FREQ
#define  TIMER_CLK_FREQ                              (80000000>>4) //80MHz divided by 16
#define  SPI_CLK_DIV                                 4
#define  TICKS_PER_US_ROM                            40              // CPU is 80MHz
#define  GPIO_MATRIX_DELAY_NS                        25
//}}

/* Overall memory map */
#define SOC_DROM_LOW    0x3F000000
#define SOC_DROM_HIGH   0x3F400000
#define SOC_IROM_LOW    0x40080000
#define SOC_IROM_HIGH   0x40c00000
#define SOC_IRAM_LOW    0x40020000
#define SOC_IRAM_HIGH   0x40070000
#define SOC_DRAM_LOW    0x3FFB0000
#define SOC_DRAM_HIGH   0x40000000
#define SOC_RTC_IRAM_LOW  0x40070000
#define SOC_RTC_IRAM_HIGH 0x40072000
#define SOC_RTC_DRAM_LOW  0x3ff9e000
#define SOC_RTC_DRAM_HIGH 0x3ffa0000
#define SOC_RTC_DATA_LOW  0x50000000
#define SOC_RTC_DATA_HIGH 0x50002000
#define SOC_EXTRAM_DATA_LOW 0x3F500000
#define SOC_EXTRAM_DATA_HIGH 0x3FF90000
#define SOC_SLOW_EXTRAM_DATA_LOW 0x61800000
#define SOC_SLOW_EXTRAM_DATA_HIGH 0x61c00000

//First and last words of the D/IRAM region, for both the DRAM address as well as the IRAM alias.
#define SOC_DIRAM_IRAM_LOW    0x40020000
#define SOC_DIRAM_IRAM_HIGH   0x40070000
#define SOC_DIRAM_DRAM_LOW    0x3FFB0000
#define SOC_DIRAM_DRAM_HIGH   0x40000000

// Region of memory accessible via DMA. See esp_ptr_dma_capable().
#define SOC_DMA_LOW  0x3FFB0000
#define SOC_DMA_HIGH 0x40000000

// Region of memory that is byte-accessible. See esp_ptr_byte_accessible().
#define SOC_BYTE_ACCESSIBLE_LOW     0x3FF9E000
#define SOC_BYTE_ACCESSIBLE_HIGH    0x40000000

//Region of memory that is internal, as in on the same silicon die as the ESP32 CPUs
//(excluding RTC data region, that's checked separately.) See esp_ptr_internal().
#define SOC_MEM_INTERNAL_LOW        0x3FF9E000
#define SOC_MEM_INTERNAL_HIGH       0x40072000

//Interrupt hardware source table
//This table is decided by hardware, don't touch this.
#define ETS_WIFI_MAC_INTR_SOURCE                    0/**< interrupt of WiFi MAC, level*/
#define ETS_WIFI_MAC_NMI_SOURCE                     1/**< interrupt of WiFi MAC, NMI, use if MAC have bug to fix in NMI*/
#define ETS_WIFI_PWR_INTR_SOURCE                    2/**< */
#define ETS_WIFI_BB_INTR_SOURCE                     3/**< interrupt of WiFi BB, level, we can do some calibartion*/
#define ETS_BT_MAC_INTR_SOURCE                      4/**< will be cancelled*/
#define ETS_BT_BB_INTR_SOURCE                       5/**< interrupt of BT BB, level*/
#define ETS_BT_BB_NMI_SOURCE                        6/**< interrupt of BT BB, NMI, use if BB have bug to fix in NMI*/
#define ETS_RWBT_INTR_SOURCE                        7/**< interrupt of RWBT, level*/
#define ETS_RWBLE_INTR_SOURCE                       8/**< interrupt of RWBLE, level*/
#define ETS_RWBT_NMI_SOURCE                         9/**< interrupt of RWBT, NMI, use if RWBT have bug to fix in NMI*/
#define ETS_RWBLE_NMI_SOURCE                        10/**< interrupt of RWBLE, NMI, use if RWBT have bug to fix in NMI*/
#define ETS_SLC0_INTR_SOURCE                        11/**< interrupt of SLC0, level*/
#define ETS_SLC1_INTR_SOURCE                        12/**< interrupt of SLC1, level*/
#define ETS_UHCI0_INTR_SOURCE                       13/**< interrupt of UHCI0, level*/
#define ETS_UHCI1_INTR_SOURCE                       14/**< interrupt of UHCI1, level*/
#define ETS_TG0_T0_LEVEL_INTR_SOURCE                15/**< interrupt of TIMER_GROUP0, TIMER0, level, we would like use EDGE for timer if permission*/
#define ETS_TG0_T1_LEVEL_INTR_SOURCE                16/**< interrupt of TIMER_GROUP0, TIMER1, level, we would like use EDGE for timer if permission*/
#define ETS_TG0_WDT_LEVEL_INTR_SOURCE               17/**< interrupt of TIMER_GROUP0, WATCHDOG, level*/
#define ETS_TG0_LACT_LEVEL_INTR_SOURCE              18/**< interrupt of TIMER_GROUP0, LACT, level*/
#define ETS_TG1_T0_LEVEL_INTR_SOURCE                19/**< interrupt of TIMER_GROUP1, TIMER0, level, we would like use EDGE for timer if permission*/
#define ETS_TG1_T1_LEVEL_INTR_SOURCE                20/**< interrupt of TIMER_GROUP1, TIMER1, level, we would like use EDGE for timer if permission*/
#define ETS_TG1_WDT_LEVEL_INTR_SOURCE               21/**< interrupt of TIMER_GROUP1, WATCHDOG, level*/
#define ETS_TG1_LACT_LEVEL_INTR_SOURCE              22/**< interrupt of TIMER_GROUP1, LACT, level*/
#define ETS_GPIO_INTR_SOURCE                        23/**< interrupt of GPIO, level*/
#define ETS_GPIO_NMI_SOURCE                         24/**< interrupt of GPIO, NMI*/
#define ETS_GPIO_INTR_SOURCE2                       25/**< interrupt of GPIO, level*/
#define ETS_GPIO_NMI_SOURCE2                        26/**< interrupt of GPIO, NMI*/
#define ETS_DEDICATED_GPIO_INTR_SOURCE              27/**< interrupt of dedicated GPIO, level*/
#define ETS_FROM_CPU_INTR0_SOURCE                   28/**< interrupt0 generated from a CPU, level*/ /* Used for FreeRTOS */
#define ETS_FROM_CPU_INTR1_SOURCE                   29/**< interrupt1 generated from a CPU, level*/ /* Used for FreeRTOS */
#define ETS_FROM_CPU_INTR2_SOURCE                   30/**< interrupt2 generated from a CPU, level*/ /* Used for DPORT Access */
#define ETS_FROM_CPU_INTR3_SOURCE                   31/**< interrupt3 generated from a CPU, level*/ /* Used for DPORT Access */
#define ETS_SPI1_INTR_SOURCE                        32/**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
#define ETS_SPI2_INTR_SOURCE                        33/**< interrupt of SPI2, level*/
#define ETS_SPI3_INTR_SOURCE                        34/**< interrupt of SPI3, level*/
#define ETS_I2S0_INTR_SOURCE                        35/**< interrupt of I2S0, level*/
#define ETS_I2S1_INTR_SOURCE                        36/**< interrupt of I2S1, level*/
#define ETS_UART0_INTR_SOURCE                       37/**< interrupt of UART0, level*/
#define ETS_UART1_INTR_SOURCE                       38/**< interrupt of UART1, level*/
#define ETS_UART2_INTR_SOURCE                       39/**< interrupt of UART2, level*/
#define ETS_SDIO_HOST_INTR_SOURCE                   40/**< interrupt of SD/SDIO/MMC HOST, level*/
#define ETS_PWM0_INTR_SOURCE                        41/**< interrupt of PWM0, level, Reserved*/
#define ETS_PWM1_INTR_SOURCE                        42/**< interrupt of PWM1, level, Reserved*/
#define ETS_PWM2_INTR_SOURCE                        43/**< interrupt of PWM2, level*/
#define ETS_PWM3_INTR_SOURCE                        44/**< interruot of PWM3, level*/
#define ETS_LEDC_INTR_SOURCE                        45/**< interrupt of LED PWM, level*/
#define ETS_EFUSE_INTR_SOURCE                       46/**< interrupt of efuse, level, not likely to use*/
#define ETS_CAN_INTR_SOURCE                         47/**< interrupt of can, level*/
#define ETS_USB_INTR_SOURCE                         48/**< interrupt of USB, level*/
#define ETS_RTC_CORE_INTR_SOURCE                    49/**< interrupt of rtc core, level, include rtc watchdog*/
#define ETS_RMT_INTR_SOURCE                         50/**< interrupt of remote controller, level*/
#define ETS_PCNT_INTR_SOURCE                        51/**< interrupt of pluse count, level*/
#define ETS_I2C_EXT0_INTR_SOURCE                    52/**< interrupt of I2C controller1, level*/
#define ETS_I2C_EXT1_INTR_SOURCE                    53/**< interrupt of I2C controller0, level*/
#define ETS_RSA_INTR_SOURCE                         54/**< interrupt of RSA accelerator, level*/
#define ETS_SPI1_DMA_INTR_SOURCE                    55/**< interrupt of SPI1 DMA, SPI1 is for flash read/write, do not use this*/
#define ETS_SPI2_DMA_INTR_SOURCE                    56/**< interrupt of SPI2 DMA, level*/
#define ETS_SPI3_DMA_INTR_SOURCE                    57/**< interrupt of SPI3 DMA, level*/
#define ETS_WDT_INTR_SOURCE                         58/**< will be cancelled*/
#define ETS_TIMER1_INTR_SOURCE                      59/**< will be cancelled*/
#define ETS_TIMER2_INTR_SOURCE                      60/**< will be cancelled*/
#define ETS_TG0_T0_EDGE_INTR_SOURCE                 61/**< interrupt of TIMER_GROUP0, TIMER0, EDGE*/
#define ETS_TG0_T1_EDGE_INTR_SOURCE                 62/**< interrupt of TIMER_GROUP0, TIMER1, EDGE*/
#define ETS_TG0_WDT_EDGE_INTR_SOURCE                63/**< interrupt of TIMER_GROUP0, WATCH DOG, EDGE*/
#define ETS_TG0_LACT_EDGE_INTR_SOURCE               64/**< interrupt of TIMER_GROUP0, LACT, EDGE*/
#define ETS_TG1_T0_EDGE_INTR_SOURCE                 65/**< interrupt of TIMER_GROUP1, TIMER0, EDGE*/
#define ETS_TG1_T1_EDGE_INTR_SOURCE                 66/**< interrupt of TIMER_GROUP1, TIMER1, EDGE*/
#define ETS_TG1_WDT_EDGE_INTR_SOURCE                67/**< interrupt of TIMER_GROUP1, WATCHDOG, EDGE*/
#define ETS_TG1_LACT_EDGE_INTR_SOURCE               68/**< interrupt of TIMER_GROUP0, LACT, EDGE*/
#define ETS_CACHE_IA_INTR_SOURCE                    69/**< interrupt of Cache Invalied Access, LEVEL*/
#define ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE       70/**< interrupt of system timer 0, EDGE*/
#define ETS_SYSTIMER_TARGET1_EDGE_INTR_SOURCE       71/**< interrupt of system timer 1, EDGE*/
#define ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE       72/**< interrupt of system timer 2, EDGE*/
#define ETS_ASSIST_DEBUG_INTR_SOURCE                73/**< interrupt of Assist debug module, LEVEL*/
#define ETS_PMS_PRO_IRAM0_ILG_INTR_SOURCE           74/**< interrupt of illegal IRAM1 access, LEVEL*/
#define ETS_PMS_PRO_DRAM0_ILG_INTR_SOURCE           75/**< interrupt of illegal DRAM0 access, LEVEL*/
#define ETS_PMS_PRO_DPORT_ILG_INTR_SOURCE           76/**< interrupt of illegal DPORT access, LEVEL*/
#define ETS_PMS_PRO_AHB_ILG_INTR_SOURCE             77/**< interrupt of illegal AHB access, LEVEL*/
#define ETS_PMS_PRO_CACHE_ILG_INTR_SOURCE           78/**< interrupt of illegal CACHE access, LEVEL*/
#define ETS_PMS_DMA_APB_I_ILG_INTR_SOURCE           79/**< interrupt of illegal APB access, LEVEL*/
#define ETS_PMS_DMA_RX_I_ILG_INTR_SOURCE            80/**< interrupt of illegal DMA RX access, LEVEL*/
#define ETS_PMS_DMA_TX_I_ILG_INTR_SOURCE            81/**< interrupt of illegal DMA TX access, LEVEL*/
#define ETS_SPI0_REJECT_CACHE_INTR_SOURCE           82/**< interrupt of SPI0 Cache access rejected, LEVEL*/
#define ETS_SPI1_REJECT_CPU_INTR_SOURCE             83/**< interrupt of SPI1 access rejected, LEVEL*/
#define ETS_DMA_COPY_INTR_SOURCE                    84/**< interrupt of DMA copy, LEVEL*/
#define ETS_SPI4_DMA_INTR_SOURCE                    85/**< interrupt of SPI4 DMA, LEVEL*/
#define ETS_SPI4_INTR_SOURCE                        86/**< interrupt of SPI4, LEVEL*/
#define ETS_MAX_INTR_SOURCE                         87/**< number of interrupt sources */

//interrupt cpu using table, Please see the core-isa.h
/*************************************************************************************************************
 *      Intr num                Level           Type                    PRO CPU usage           APP CPU uasge
 *      0                       1               extern level            WMAC                    Reserved
 *      1                       1               extern level            BT/BLE Host HCI DMA     BT/BLE Host HCI DMA
 *      2                       1               extern level
 *      3                       1               extern level
 *      4                       1               extern level            WBB
 *      5                       1               extern level            BT/BLE Controller       BT/BLE Controller
 *      6                       1               timer                   FreeRTOS Tick(L1)       FreeRTOS Tick(L1)
 *      7                       1               software                BT/BLE VHCI             BT/BLE VHCI
 *      8                       1               extern level            BT/BLE BB(RX/TX)        BT/BLE BB(RX/TX)
 *      9                       1               extern level
 *      10                      1               extern edge
 *      11                      3               profiling
 *      12                      1               extern level
 *      13                      1               extern level
 *      14                      7               nmi                     Reserved                Reserved
 *      15                      3               timer                   FreeRTOS Tick(L3)       FreeRTOS Tick(L3)
 *      16                      5               timer
 *      17                      1               extern level
 *      18                      1               extern level
 *      19                      2               extern level
 *      20                      2               extern level
 *      21                      2               extern level
 *      22                      3               extern edge
 *      23                      3               extern level
 *      24                      4               extern level            TG1_WDT
 *      25                      4               extern level            CACHEERR
 *      26                      5               extern level
 *      27                      3               extern level            Reserved                Reserved
 *      28                      4               extern edge
 *      29                      3               software                Reserved                Reserved
 *      30                      4               extern edge             Reserved                Reserved
 *      31                      5               extern level
 *************************************************************************************************************
 */

//CPU0 Interrupt number reserved, not touch this.
#define ETS_WMAC_INUM                           0
#define ETS_BT_HOST_INUM                        1
#define ETS_WBB_INUM                            4
#define ETS_TG0_T1_INUM                         10 /**< use edge interrupt*/
#define ETS_FRC1_INUM                           22
#define ETS_T1_WDT_INUM                         24
#define ETS_CACHEERR_INUM                       25

//CPU0 Interrupt number used in ROM, should be cancelled in SDK
#define ETS_SLC_INUM                            1
#define ETS_UART0_INUM                          5
#define ETS_UART1_INUM                          5
//CPU0 Interrupt number used in ROM code only when module init function called, should pay attention here.
#define ETS_FRC_TIMER2_INUM 10 /* use edge*/
#define ETS_GPIO_INUM       4

//Other interrupt number should be managed by the user

//Invalid interrupt for number interrupt matrix
#define ETS_INVALID_INUM                        6
