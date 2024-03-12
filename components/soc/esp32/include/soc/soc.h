/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "esp_assert.h"
#include "soc/soc_caps.h"
#endif

#include "esp_bit_defs.h"
#include "reg_base.h"

#define PRO_CPU_NUM (0)
#define APP_CPU_NUM (1)


#define SOC_MAX_CONTIGUOUS_RAM_SIZE 0x400000 ///< Largest span of contiguous memory (DRAM or IRAM) in the address space

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr)


#ifndef __ASSEMBLER__

#define IS_DPORT_REG(_r) (((_r) >= DR_REG_DPORT_BASE) && (_r) <= DR_REG_DPORT_END)

#if !defined( BOOTLOADER_BUILD ) && !defined( CONFIG_FREERTOS_UNICORE ) && SOC_DPORT_WORKAROUND
#define ASSERT_IF_DPORT_REG(_r, OP)  TRY_STATIC_ASSERT(!IS_DPORT_REG(_r), (Cannot use OP for DPORT registers use DPORT_##OP));
#else
#define ASSERT_IF_DPORT_REG(_r, OP)
#endif

//write value to register
#define REG_WRITE(_r, _v)  do {                                                                                        \
            ASSERT_IF_DPORT_REG((_r), REG_WRITE);                                                                      \
            (*(volatile uint32_t *)(_r)) = (_v);                                                                       \
        } while(0)

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
#define REG_SET_BIT(_r, _b)  do {                                                                                      \
            ASSERT_IF_DPORT_REG((_r), REG_SET_BIT);                                                                    \
            *(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r)) | (_b);                                            \
        } while(0)

//clear bit or clear bits of register
#define REG_CLR_BIT(_r, _b)  do {                                                                                      \
            ASSERT_IF_DPORT_REG((_r), REG_CLR_BIT);                                                                    \
            *(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r)) & (~(_b));                                         \
        } while(0)

//set bits of register controlled by mask
#define REG_SET_BITS(_r, _b, _m) do {                                                                                  \
            ASSERT_IF_DPORT_REG((_r), REG_SET_BITS);                                                                   \
            *(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m));                           \
        } while(0)

//get field from register, uses field _S & _V to determine mask
#define REG_GET_FIELD(_r, _f) ({                                                                                       \
            ASSERT_IF_DPORT_REG((_r), REG_GET_FIELD);                                                                  \
            ((REG_READ(_r) >> (_f##_S)) & (_f##_V));                                                                   \
        })

//set field of a register from variable, uses field _S & _V to determine mask
#define REG_SET_FIELD(_r, _f, _v) do {                                                                                 \
            ASSERT_IF_DPORT_REG((_r), REG_SET_FIELD);                                                                  \
            REG_WRITE((_r),((REG_READ(_r) & ~((_f##_V) << (_f##_S)))|(((_v) & (_f##_V))<<(_f##_S))));                  \
        } while(0)

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
#define WRITE_PERI_REG(addr, val) do {                                                                                 \
            ASSERT_IF_DPORT_REG((addr), WRITE_PERI_REG);                                                               \
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val);                                       \
        } while(0)

//clear bits of register controlled by mask
#define CLEAR_PERI_REG_MASK(reg, mask)  do {                                                                           \
            ASSERT_IF_DPORT_REG((reg), CLEAR_PERI_REG_MASK);                                                           \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))));                                                     \
        } while(0)

//set bits of register controlled by mask
#define SET_PERI_REG_MASK(reg, mask) do {                                                                              \
            ASSERT_IF_DPORT_REG((reg), SET_PERI_REG_MASK);                                                             \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)));                                                        \
        } while(0)

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
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) do {                                                                \
            ASSERT_IF_DPORT_REG((reg), SET_PERI_REG_BITS);                                                             \
            WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & (bit_map))<<(shift)) );      \
        } while(0)

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
#define  CPU_CLK_FREQ_MHZ_BTLD                       (80)               // The cpu clock frequency (in MHz) to set at 2nd stage bootloader system clock configuration
#define  CPU_CLK_FREQ                                APB_CLK_FREQ       //this may be incorrect, please refer to ESP_DEFAULT_CPU_FREQ_MHZ
#define  APB_CLK_FREQ                                ( 80*1000000 )       //unit: Hz
#define  REF_CLK_FREQ                                ( 1000000 )
#define  UART_CLK_FREQ                               APB_CLK_FREQ
#define  WDT_CLK_FREQ                                APB_CLK_FREQ
#define  TIMER_CLK_FREQ                              (80000000>>4) //80MHz divided by 16
#define  SPI_CLK_DIV                                 4
#define  TICKS_PER_US_ROM                            26              // CPU is 80MHz
#define  GPIO_MATRIX_DELAY_NS                        25
//}}

/* Overall memory map */
#define SOC_DROM_LOW            0x3F400000
#define SOC_DROM_HIGH           0x3F800000
#define SOC_DRAM_LOW            0x3FFAE000
#define SOC_DRAM_HIGH           0x40000000
#define SOC_IROM_LOW            0x400D0000
#define SOC_IROM_HIGH           0x40400000
#define SOC_IROM_MASK_LOW       0x40000000
#define SOC_IROM_MASK_HIGH      0x40070000
#define SOC_CACHE_PRO_LOW       0x40070000
#define SOC_CACHE_PRO_HIGH      0x40078000
#define SOC_CACHE_APP_LOW       0x40078000
#define SOC_CACHE_APP_HIGH      0x40080000
#define SOC_IRAM_LOW            0x40080000
#define SOC_IRAM_HIGH           0x400AA000
#define SOC_RTC_IRAM_LOW        0x400C0000
#define SOC_RTC_IRAM_HIGH       0x400C2000
#define SOC_RTC_DRAM_LOW        0x3FF80000
#define SOC_RTC_DRAM_HIGH       0x3FF82000
#define SOC_RTC_DATA_LOW        0x50000000
#define SOC_RTC_DATA_HIGH       0x50002000
#define SOC_EXTRAM_DATA_LOW     0x3F800000
#define SOC_EXTRAM_DATA_HIGH    0x3FC00000

#define SOC_EXTRAM_DATA_SIZE (SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW)

//First and last words of the D/IRAM region, for both the DRAM address as well as the IRAM alias.
#define SOC_DIRAM_IRAM_LOW    0x400A0000
#define SOC_DIRAM_IRAM_HIGH   0x400C0000
#define SOC_DIRAM_DRAM_LOW    0x3FFE0000
#define SOC_DIRAM_DRAM_HIGH   0x40000000
// Byte order of D/IRAM regions is reversed between accessing as DRAM or IRAM
#define SOC_DIRAM_INVERTED    1

// Region of memory accessible via DMA. See esp_ptr_dma_capable().
#define SOC_DMA_LOW  0x3FFAE000
#define SOC_DMA_HIGH 0x40000000

// Region of memory that is byte-accessible. See esp_ptr_byte_accessible().
#define SOC_BYTE_ACCESSIBLE_LOW     0x3FF90000
#define SOC_BYTE_ACCESSIBLE_HIGH    0x40000000

//Region of memory that is internal, as in on the same silicon die as the ESP32 CPUs
//(excluding RTC data region, that's checked separately.) See esp_ptr_internal().
#define SOC_MEM_INTERNAL_LOW        0x3FF90000
#define SOC_MEM_INTERNAL_HIGH       0x400C2000

// Start (highest address) of ROM boot stack, only relevant during early boot
#define SOC_ROM_STACK_START         0x3ffe3f20

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
#define ETS_FROM_CPU_INTR2_SOURCE               26/**< interrupt2 generated from a CPU, level*/ /* Used for IPC_ISR */
#define ETS_FROM_CPU_INTR3_SOURCE               27/**< interrupt3 generated from a CPU, level*/ /* Used for IPC_ISR */
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
#define ETS_LEDC_INTR_SOURCE                    43/**< interrupt of LED PWM, level*/
#define ETS_EFUSE_INTR_SOURCE                   44/**< interrupt of efuse, level, not likely to use*/
#define ETS_TWAI_INTR_SOURCE                    45/**< interrupt of twai, level*/
#define ETS_CAN_INTR_SOURCE                     ETS_TWAI_INTR_SOURCE
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
#define ETS_MAX_INTR_SOURCE                     69/**< total number of interrupt sources*/

//CPU0 Interrupt number reserved, not touch this.
#define ETS_WMAC_INUM                           0
#define ETS_BT_HOST_INUM                        1
#define ETS_WBB_INUM                            4

#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5

#define ETS_T1_WDT_CACHEERR_INUM                26
#define ETS_T1_WDT_INUM                         ETS_T1_WDT_CACHEERR_INUM
#define ETS_MEMACCESS_ERR_INUM                  ETS_T1_WDT_CACHEERR_INUM
/* backwards compatibility only, use ETS_MEMACCESS_ERR_INUM instead*/
#define ETS_CACHEERR_INUM                       ETS_MEMACCESS_ERR_INUM
#define ETS_IPC_ISR_INUM                        31

#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4

#define ETS_T1_WDT_INUM                         24
#define ETS_MEMACCESS_ERR_INUM                  25
/* backwards compatibility only, use ETS_MEMACCESS_ERR_INUM instead*/
#define ETS_CACHEERR_INUM                       ETS_MEMACCESS_ERR_INUM
#define ETS_IPC_ISR_INUM                        28

#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */

//CPU0 Interrupt number used in ROM, should be cancelled in SDK
#define ETS_SLC_INUM                            1
#define ETS_UART0_INUM                          5
#define ETS_UART1_INUM                          5
//Other interrupt number should be managed by the user

//Invalid interrupt for number interrupt matrix
#define ETS_INVALID_INUM                        6

// Interrupt number for the Interrupt watchdog
#define ETS_INT_WDT_INUM                         (ETS_T1_WDT_INUM)
