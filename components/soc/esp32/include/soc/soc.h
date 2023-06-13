/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "esp_assert.h"
#include "soc/soc_caps.h"
#include "soc/interrupts.h"
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
#define  MODEM_REQUIRED_MIN_APB_CLK_FREQ             ( 80*1000000 )
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

#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
//interrupt cpu using table, Please see the core-isa.h
/*************************************************************************************************************
 *      Intr num                Level           Type                    PRO CPU usage           APP CPU uasge
 *      0                       1               extern level            WMAC                    Reserved
 *      1                       1               extern level            BT/BLE Host HCI DMA     BT/BLE Host HCI DMA
 *      2                       1               extern level
 *      3                       1               extern level
 *      4                       1               extern level            WBB
 *      5                       1               extern level
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
 *      16                      5               timer                   Reserved                Reserved
 *      17                      1               extern level
 *      18                      1               extern level
 *      19                      2               extern level
 *      20                      2               extern level
 *      21                      2               extern level
 *      22                      3               extern edge
 *      23                      3               extern level
 *      24                      4               extern level
 *      25                      4               extern level            BT/BLE Controller       BT/BLE Controller
 *      26                      5               extern level            TG1_WDT & CACHEERR
 *      27                      3               extern level            Reserved                Reserved
 *      28                      4               extern edge
 *      29                      3               software                BT/BLE hli              BT/BLE hli
 *      30                      4               extern edge             Reserved                Reserved
 *      31                      5               extern level            IPC_ISR                 IPC_ISR
 *************************************************************************************************************
 */

//CPU0 Interrupt number reserved, not touch this.
#define ETS_WMAC_INUM                           0
#define ETS_BT_HOST_INUM                        1
#define ETS_WBB_INUM                            4
#define ETS_TG0_T1_INUM                         10 /**< use edge interrupt*/
#define ETS_FRC1_INUM                           22
#define ETS_T1_WDT_CACHEERR_INUM                26
#define ETS_T1_WDT_INUM                         ETS_T1_WDT_CACHEERR_INUM
#define ETS_MEMACCESS_ERR_INUM                  ETS_T1_WDT_CACHEERR_INUM
/* backwards compatibility only, use ETS_MEMACCESS_ERR_INUM instead*/
#define ETS_CACHEERR_INUM                       ETS_MEMACCESS_ERR_INUM
#define ETS_IPC_ISR_INUM                        31

#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4

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
 *      28                      4               extern edge             IPC_ISR                 IPC_ISR
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
