// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP32_SOC_H_
#define _ESP32_SOC_H_

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "esp_assert.h"
#endif

//Register Bits{{
#define BIT31   0x80000000
#define BIT30   0x40000000
#define BIT29   0x20000000
#define BIT28   0x10000000
#define BIT27   0x08000000
#define BIT26   0x04000000
#define BIT25   0x02000000
#define BIT24   0x01000000
#define BIT23   0x00800000
#define BIT22   0x00400000
#define BIT21   0x00200000
#define BIT20   0x00100000
#define BIT19   0x00080000
#define BIT18   0x00040000
#define BIT17   0x00020000
#define BIT16   0x00010000
#define BIT15   0x00008000
#define BIT14   0x00004000
#define BIT13   0x00002000
#define BIT12   0x00001000
#define BIT11   0x00000800
#define BIT10   0x00000400
#define BIT9     0x00000200
#define BIT8     0x00000100
#define BIT7     0x00000080
#define BIT6     0x00000040
#define BIT5     0x00000020
#define BIT4     0x00000010
#define BIT3     0x00000008
#define BIT2     0x00000004
#define BIT1     0x00000002
#define BIT0     0x00000001
//}}

#define PRO_CPU_NUM (0)
#define APP_CPU_NUM (1)

/* Overall memory map */
#define SOC_IROM_LOW    0x400D0000
#define SOC_IROM_HIGH   0x40400000
#define SOC_DROM_LOW    0x3F400000
#define SOC_DROM_HIGH   0x3F800000
#define SOC_RTC_IRAM_LOW  0x400C0000
#define SOC_RTC_IRAM_HIGH 0x400C2000
#define SOC_RTC_DATA_LOW  0x50000000
#define SOC_RTC_DATA_HIGH 0x50002000

#define DR_REG_DPORT_BASE                       0x3ff00000
#define DR_REG_DPORT_END                        0x3ff00FFC
#define DR_REG_RSA_BASE                         0x3ff02000
#define DR_REG_SHA_BASE                         0x3ff03000
#define DR_REG_UART_BASE                        0x3ff40000
#define DR_REG_SPI1_BASE                        0x3ff42000
#define DR_REG_SPI0_BASE                        0x3ff43000
#define DR_REG_GPIO_BASE                        0x3ff44000
#define DR_REG_GPIO_SD_BASE                     0x3ff44f00
#define DR_REG_FE2_BASE                         0x3ff45000
#define DR_REG_FE_BASE                          0x3ff46000
#define DR_REG_FRC_TIMER_BASE                   0x3ff47000
#define DR_REG_RTCCNTL_BASE                     0x3ff48000
#define DR_REG_RTCIO_BASE                       0x3ff48400
#define DR_REG_SENS_BASE                        0x3ff48800
#define DR_REG_IO_MUX_BASE                      0x3ff49000
#define DR_REG_RTCMEM0_BASE                     0x3ff61000
#define DR_REG_RTCMEM1_BASE                     0x3ff62000
#define DR_REG_RTCMEM2_BASE                     0x3ff63000
#define DR_REG_SYSCON_BASE                      0x3ff66000
#define DR_REG_HINF_BASE                        0x3ff4B000
#define DR_REG_UHCI1_BASE                       0x3ff4C000
#define DR_REG_I2S_BASE                         0x3ff4F000
#define DR_REG_UART1_BASE                       0x3ff50000
#define DR_REG_BT_BASE                          0x3ff51000
#define DR_REG_I2C_EXT_BASE                     0x3ff53000
#define DR_REG_UHCI0_BASE                       0x3ff54000
#define DR_REG_SLCHOST_BASE                     0x3ff55000
#define DR_REG_RMT_BASE                         0x3ff56000
#define DR_REG_PCNT_BASE                        0x3ff57000
#define DR_REG_SLC_BASE                         0x3ff58000
#define DR_REG_LEDC_BASE                        0x3ff59000
#define DR_REG_EFUSE_BASE                       0x3ff5A000
#define DR_REG_SPI_ENCRYPT_BASE                 0x3ff5B000
#define DR_REG_NRX_BASE                         0x3ff5CC00
#define DR_REG_BB_BASE                          0x3ff5D000
#define DR_REG_PWM_BASE                         0x3ff5E000
#define DR_REG_TIMERGROUP0_BASE                 0x3ff5F000
#define DR_REG_TIMERGROUP1_BASE                 0x3ff60000
#define DR_REG_SPI2_BASE                        0x3ff64000
#define DR_REG_SPI3_BASE                        0x3ff65000
#define DR_REG_APB_CTRL_BASE                    0x3ff66000
#define DR_REG_I2C1_EXT_BASE                    0x3ff67000
#define DR_REG_SDMMC_BASE                       0x3ff68000
#define DR_REG_EMAC_BASE                        0x3ff69000
#define DR_REG_PWM1_BASE                        0x3ff6C000
#define DR_REG_I2S1_BASE                        0x3ff6D000
#define DR_REG_UART2_BASE                       0x3ff6E000
#define DR_REG_PWM2_BASE                        0x3ff6F000
#define DR_REG_PWM3_BASE                        0x3ff70000
#define PERIPHS_SPI_ENCRYPT_BASEADDR		DR_REG_SPI_ENCRYPT_BASE

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr) 

#ifndef __ASSEMBLER__
#define BIT(nr)                 (1UL << (nr))
#else
#define BIT(nr)                 (1 << (nr))
#endif

#ifndef __ASSEMBLER__

#define IS_DPORT_REG(_r) (((_r) >= DR_REG_DPORT_BASE) && (_r) <= DR_REG_DPORT_END)

#if !defined( BOOTLOADER_BUILD ) && !defined( CONFIG_FREERTOS_UNICORE ) && defined( ESP_PLATFORM )
#define ASSERT_IF_DPORT_REG(_r, OP)  TRY_STATIC_ASSERT(!IS_DPORT_REG(_r), (Cannot use OP for DPORT registers use DPORT_##OP));
#else
#define ASSERT_IF_DPORT_REG(_r, OP)
#endif

//write value to register
#define REG_WRITE(_r, _v) ({                                                                                           \
            ASSERT_IF_DPORT_REG((_r), REG_WRITE);                                                                      \
            (*(volatile uint32_t *)(_r)) = (_v);                                                                       \
        })

//read value from register
#define REG_READ(_r) ({                                                                                                \
            ASSERT_IF_DPORT_REG((_r), REG_READ);                                                                       \
            (*(volatile uint32_t *)(_r));                                                                              \
        })

//get bit or get bits from register
#define REG_GET_BIT(_r, _b)  ({                                                                                        \
            ASSERT_IF_DPORT_REG((_r), REG_GET_BIT);                                                                    \
            (*(volatile uint32_t*)(_r) & (_b));                                                                        \
        })

//set bit or set bits to register
#define REG_SET_BIT(_r, _b)  ({                                                                                        \
            ASSERT_IF_DPORT_REG((_r), REG_SET_BIT);                                                                    \
            (*(volatile uint32_t*)(_r) |= (_b));                                                                       \
        })

//clear bit or clear bits of register
#define REG_CLR_BIT(_r, _b)  ({                                                                                        \
            ASSERT_IF_DPORT_REG((_r), REG_CLR_BIT);                                                                    \
            (*(volatile uint32_t*)(_r) &= ~(_b));                                                                      \
        })

//set bits of register controlled by mask
#define REG_SET_BITS(_r, _b, _m) ({                                                                                    \
            ASSERT_IF_DPORT_REG((_r), REG_SET_BITS);                                                                   \
            (*(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m)));                         \
        })

//get field from register, uses field _S & _V to determine mask
#define REG_GET_FIELD(_r, _f) ({                                                                                       \
            ASSERT_IF_DPORT_REG((_r), REG_GET_FIELD);                                                                  \
            ((REG_READ(_r) >> (_f##_S)) & (_f##_V));                                                                   \
        })

//set field of a register from variable, uses field _S & _V to determine mask
#define REG_SET_FIELD(_r, _f, _v) ({                                                                                   \
            ASSERT_IF_DPORT_REG((_r), REG_SET_FIELD);                                                                  \
            (REG_WRITE((_r),((REG_READ(_r) & ~((_f##_V) << (_f##_S)))|(((_v) & (_f##_V))<<(_f##_S)))));                \
        })

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
#define READ_PERI_REG(addr) ({                                                                                         \
            ASSERT_IF_DPORT_REG((addr), READ_PERI_REG);                                                                \
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr)));                                                         \
        })

//write value to register
#define WRITE_PERI_REG(addr, val) ({                                                                                   \
            ASSERT_IF_DPORT_REG((addr), WRITE_PERI_REG);                                                               \
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val);                                       \
        })

//clear bits of register controlled by mask
#define CLEAR_PERI_REG_MASK(reg, mask) ({                                                                              \
            ASSERT_IF_DPORT_REG((reg), CLEAR_PERI_REG_MASK);                                                           \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))));                                                     \
        })

//set bits of register controlled by mask
#define SET_PERI_REG_MASK(reg, mask) ({                                                                                \
            ASSERT_IF_DPORT_REG((reg), SET_PERI_REG_MASK);                                                             \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)));                                                        \
        })

//get bits of register controlled by mask
#define GET_PERI_REG_MASK(reg, mask) ({                                                                                \
            ASSERT_IF_DPORT_REG((reg), GET_PERI_REG_MASK);                                                             \
            (READ_PERI_REG(reg) & (mask));                                                                             \
        })

//get bits of register controlled by highest bit and lowest bit
#define GET_PERI_REG_BITS(reg, hipos,lowpos) ({                                                                        \
            ASSERT_IF_DPORT_REG((reg), GET_PERI_REG_BITS);                                                             \
            ((READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1));                                            \
        })

//set bits of register controlled by mask and shift
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) ({                                                                  \
            ASSERT_IF_DPORT_REG((reg), SET_PERI_REG_BITS);                                                             \
            (WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift)) ));      \
        })

//get field of register
#define GET_PERI_REG_BITS2(reg, mask,shift) ({                                                                         \
            ASSERT_IF_DPORT_REG((reg), GET_PERI_REG_BITS2);                                                            \
            ((READ_PERI_REG(reg)>>(shift))&(mask));                                                                    \
        })

#endif /* !__ASSEMBLER__ */
//}}

//Periheral Clock {{
#define  APB_CLK_FREQ_ROM                            ( 26*1000000 )
#define  CPU_CLK_FREQ_ROM                            APB_CLK_FREQ_ROM
#define  CPU_CLK_FREQ                                APB_CLK_FREQ
#define  APB_CLK_FREQ                                ( 80*1000000 )       //unit: Hz
#define  UART_CLK_FREQ                               APB_CLK_FREQ
#define  WDT_CLK_FREQ                                APB_CLK_FREQ
#define  TIMER_CLK_FREQ                              (80000000>>4) //80MHz divided by 16
#define  SPI_CLK_DIV                                 4
#define  TICKS_PER_US_ROM                            26              // CPU is 80MHz
//}}

//Interrupt hardware source table
//This table is decided by hardware, don't touch this.
#define ETS_WIFI_MAC_INTR_SOURCE                0/**< interrupt of WiFi MAC, level*/
#define ETS_WIFI_MAC_NMI_SOURCE                 1/**< interrupt of WiFi MAC, NMI, use if MAC have bug to fix in NMI*/
#define ETS_WIFI_BB_INTR_SOURCE                 2/**< interrupt of WiFi BB, level, we can do some calibartion*/
#define ETS_BT_MAC_INTR_SOURCE                  3/**< will be cancelled*/
#define ETS_BT_BB_INTR_SOURCE                   4/**< interrupt of BT BB, level*/
#define ETS_BT_BB_NMI_SOURCE                    5/**< interrupt of BT BB, NMI, use if BB have bug to fix in NMI*/
#define ETS_RWBT_INTR_SOURCE                    6/**< interrupt of RWBT, level*/
#define ETS_RWBLE_INTR_SOURCE                   7/**< interrupt of RWBLE, level*/
#define ETS_RWBT_NMI_SOURCE                     8/**< interrupt of RWBT, NMI, use if RWBT have bug to fix in NMI*/
#define ETS_RWBLE_NMI_SOURCE                    9/**< interrupt of RWBLE, NMI, use if RWBT have bug to fix in NMI*/
#define ETS_SLC0_INTR_SOURCE                    10/**< interrupt of SLC0, level*/
#define ETS_SLC1_INTR_SOURCE                    11/**< interrupt of SLC1, level*/
#define ETS_UHCI0_INTR_SOURCE                   12/**< interrupt of UHCI0, level*/
#define ETS_UHCI1_INTR_SOURCE                   13/**< interrupt of UHCI1, level*/
#define ETS_TG0_T0_LEVEL_INTR_SOURCE            14/**< interrupt of TIMER_GROUP0, TIMER0, level, we would like use EDGE for timer if permission*/
#define ETS_TG0_T1_LEVEL_INTR_SOURCE            15/**< interrupt of TIMER_GROUP0, TIMER1, level, we would like use EDGE for timer if permission*/
#define ETS_TG0_WDT_LEVEL_INTR_SOURCE           16/**< interrupt of TIMER_GROUP0, WATCHDOG, level*/
#define ETS_TG0_LACT_LEVEL_INTR_SOURCE          17/**< interrupt of TIMER_GROUP0, LACT, level*/
#define ETS_TG1_T0_LEVEL_INTR_SOURCE            18/**< interrupt of TIMER_GROUP1, TIMER0, level, we would like use EDGE for timer if permission*/
#define ETS_TG1_T1_LEVEL_INTR_SOURCE            19/**< interrupt of TIMER_GROUP1, TIMER1, level, we would like use EDGE for timer if permission*/
#define ETS_TG1_WDT_LEVEL_INTR_SOURCE           20/**< interrupt of TIMER_GROUP1, WATCHDOG, level*/
#define ETS_TG1_LACT_LEVEL_INTR_SOURCE          21/**< interrupt of TIMER_GROUP1, LACT, level*/
#define ETS_GPIO_INTR_SOURCE                    22/**< interrupt of GPIO, level*/
#define ETS_GPIO_NMI_SOURCE                     23/**< interrupt of GPIO, NMI*/
#define ETS_FROM_CPU_INTR0_SOURCE               24/**< interrupt0 generated from a CPU, level*/ /* Used for FreeRTOS */
#define ETS_FROM_CPU_INTR1_SOURCE               25/**< interrupt1 generated from a CPU, level*/ /* Used for FreeRTOS */
#define ETS_FROM_CPU_INTR2_SOURCE               26/**< interrupt2 generated from a CPU, level*/ /* Used for DPORT Access */
#define ETS_FROM_CPU_INTR3_SOURCE               27/**< interrupt3 generated from a CPU, level*/ /* Used for DPORT Access */
#define ETS_SPI0_INTR_SOURCE                    28/**< interrupt of SPI0, level, SPI0 is for Cache Access, do not use this*/
#define ETS_SPI1_INTR_SOURCE                    29/**< interrupt of SPI1, level, SPI1 is for flash read/write, do not use this*/
#define ETS_SPI2_INTR_SOURCE                    30/**< interrupt of SPI2, level*/
#define ETS_SPI3_INTR_SOURCE                    31/**< interrupt of SPI3, level*/
#define ETS_I2S0_INTR_SOURCE                    32/**< interrupt of I2S0, level*/
#define ETS_I2S1_INTR_SOURCE                    33/**< interrupt of I2S1, level*/
#define ETS_UART0_INTR_SOURCE                   34/**< interrupt of UART0, level*/
#define ETS_UART1_INTR_SOURCE                   35/**< interrupt of UART1, level*/
#define ETS_UART2_INTR_SOURCE                   36/**< interrupt of UART2, level*/
#define ETS_SDIO_HOST_INTR_SOURCE               37/**< interrupt of SD/SDIO/MMC HOST, level*/
#define ETS_ETH_MAC_INTR_SOURCE                 38/**< interrupt of ethernet mac, level*/
#define ETS_PWM0_INTR_SOURCE                    39/**< interrupt of PWM0, level, Reserved*/
#define ETS_PWM1_INTR_SOURCE                    40/**< interrupt of PWM1, level, Reserved*/
#define ETS_PWM2_INTR_SOURCE                    41/**< interrupt of PWM2, level*/
#define ETS_PWM3_INTR_SOURCE                    42/**< interruot of PWM3, level*/
#define ETS_LEDC_INTR_SOURCE                    43/**< interrupt of LED PWM, level*/
#define ETS_EFUSE_INTR_SOURCE                   44/**< interrupt of efuse, level, not likely to use*/
#define ETS_CAN_INTR_SOURCE                     45/**< interrupt of can, level*/
#define ETS_RTC_CORE_INTR_SOURCE                46/**< interrupt of rtc core, level, include rtc watchdog*/
#define ETS_RMT_INTR_SOURCE                     47/**< interrupt of remote controller, level*/
#define ETS_PCNT_INTR_SOURCE                    48/**< interrupt of pluse count, level*/
#define ETS_I2C_EXT0_INTR_SOURCE                49/**< interrupt of I2C controller1, level*/
#define ETS_I2C_EXT1_INTR_SOURCE                50/**< interrupt of I2C controller0, level*/
#define ETS_RSA_INTR_SOURCE                     51/**< interrupt of RSA accelerator, level*/
#define ETS_SPI1_DMA_INTR_SOURCE                52/**< interrupt of SPI1 DMA, SPI1 is for flash read/write, do not use this*/
#define ETS_SPI2_DMA_INTR_SOURCE                53/**< interrupt of SPI2 DMA, level*/
#define ETS_SPI3_DMA_INTR_SOURCE                54/**< interrupt of SPI3 DMA, level*/
#define ETS_WDT_INTR_SOURCE                     55/**< will be cancelled*/
#define ETS_TIMER1_INTR_SOURCE                  56/**< will be cancelled*/
#define ETS_TIMER2_INTR_SOURCE                  57/**< will be cancelled*/
#define ETS_TG0_T0_EDGE_INTR_SOURCE             58/**< interrupt of TIMER_GROUP0, TIMER0, EDGE*/
#define ETS_TG0_T1_EDGE_INTR_SOURCE             59/**< interrupt of TIMER_GROUP0, TIMER1, EDGE*/
#define ETS_TG0_WDT_EDGE_INTR_SOURCE            60/**< interrupt of TIMER_GROUP0, WATCH DOG, EDGE*/
#define ETS_TG0_LACT_EDGE_INTR_SOURCE           61/**< interrupt of TIMER_GROUP0, LACT, EDGE*/
#define ETS_TG1_T0_EDGE_INTR_SOURCE             62/**< interrupt of TIMER_GROUP1, TIMER0, EDGE*/
#define ETS_TG1_T1_EDGE_INTR_SOURCE             63/**< interrupt of TIMER_GROUP1, TIMER1, EDGE*/
#define ETS_TG1_WDT_EDGE_INTR_SOURCE            64/**< interrupt of TIMER_GROUP1, WATCHDOG, EDGE*/
#define ETS_TG1_LACT_EDGE_INTR_SOURCE           65/**< interrupt of TIMER_GROUP0, LACT, EDGE*/
#define ETS_MMU_IA_INTR_SOURCE                  66/**< interrupt of MMU Invalid Access, LEVEL*/
#define ETS_MPU_IA_INTR_SOURCE                  67/**< interrupt of MPU Invalid Access, LEVEL*/
#define ETS_CACHE_IA_INTR_SOURCE                68/**< interrupt of Cache Invalied Access, LEVEL*/

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
 *      10                      1               extern edge             Internal Timer
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
 *      22                      3               extern edge             FRC1 timer
 *      23                      3               extern level
 *      24                      4               extern level            TG1_WDT
 *      25                      4               extern level            CACHEERR
 *      26                      5               extern level            Reserved                Reserved
 *      27                      3               extern level            Reserved                Reserved
 *      28                      4               extern edge             
 *      29                      3               software                Reserved                Reserved
 *      30                      4               extern edge             Reserved                Reserved
 *      31                      5               extern level            DPORT ACCESS            DPORT ACCESS
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
#define ETS_DPORT_INUM                          31

//CPU0 Interrupt number used in ROM, should be cancelled in SDK
#define ETS_SLC_INUM                            1
#define ETS_UART0_INUM                          5
#define ETS_UART1_INUM                          5
//Other interrupt number should be managed by the user

//Invalid interrupt for number interrupt matrix
#define ETS_INVALID_INUM                        6

#endif /* _ESP32_SOC_H_ */
