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

#ifndef _ESP32_SOC_H_
#define _ESP32_SOC_H_

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "esp_assert.h"
#include "esp_bit_defs.h"
#endif

#define PRO_CPU_NUM (0)

#define SOC_MAX_CONTIGUOUS_RAM_SIZE (SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW) ///< Largest span of contiguous memory (DRAM or IRAM) in the address space

#define DR_REG_SYSTEM_BASE                      0x3f4c0000
#define DR_REG_SENSITIVE_BASE                   0x3f4c1000
#define DR_REG_INTERRUPT_BASE                   0x3f4c2000
#define DR_REG_DMA_COPY_BASE                    0x3f4c3000
#define DR_REG_EXTMEM_BASE                      0x61800000
#define DR_REG_MMU_TABLE                        0x61801000
#define DR_REG_ITAG_TABLE                       0x61802000
#define DR_REG_DTAG_TABLE                       0x61803000
#define DR_REG_AES_BASE                         0x6003a000
#define DR_REG_SHA_BASE                         0x6003b000
#define DR_REG_RSA_BASE                         0x6003c000
#define DR_REG_HMAC_BASE                        0x6003e000
#define DR_REG_DIGITAL_SIGNATURE_BASE           0x6003d000
#define DR_REG_CRYPTO_DMA_BASE                  0x6003f000
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
#define DR_REG_MCP_BASE                         0x3f4c3000
#define DR_REG_EFUSE_BASE                       0x3f41A000
#define DR_REG_NRX_BASE                         0x3f41CC00
#define DR_REG_BB_BASE                          0x3f41D000
#define DR_REG_TIMERGROUP0_BASE                 0x3f41F000
#define DR_REG_TIMERGROUP1_BASE                 0x3f420000
#define DR_REG_RTC_SLOWMEM_BASE                 0x3f421000
#define DR_REG_SYSTIMER_BASE                    0x3f423000
#define DR_REG_SPI2_BASE                        0x3f424000
#define DR_REG_SPI3_BASE                        0x3f425000
#define DR_REG_SYSCON_BASE                      0x3f426000
#define DR_REG_APB_CTRL_BASE                    0x3f426000    /* Old name for SYSCON, to be removed */
#define DR_REG_I2C1_EXT_BASE                    0x3f427000
#define DR_REG_SPI4_BASE                        0x3f437000
#define DR_REG_USB_WRAP_BASE                    0x3f439000
#define DR_REG_APB_SARADC_BASE                  0x3f440000
#define DR_REG_USB_BASE                         0x60080000

#define REG_UHCI_BASE(i)         (DR_REG_UHCI0_BASE)
#define REG_UART_BASE( i )  (DR_REG_UART_BASE + (i) * 0x10000 )
#define REG_UART_AHB_BASE(i)  (0x60000000 + (i) * 0x10000 )
#define UART_FIFO_AHB_REG(i)  (REG_UART_AHB_BASE(i) + 0x0)
#define REG_I2S_BASE( i ) (DR_REG_I2S_BASE)
#define REG_TIMG_BASE(i)              (DR_REG_TIMERGROUP0_BASE + (i)*0x1000)
#define REG_SPI_MEM_BASE(i)     (DR_REG_SPI0_BASE - (i) * 0x1000)
#define REG_I2C_BASE(i)    (DR_REG_I2C_EXT_BASE + (i) * 0x14000 )

//Convenient way to replace the register ops when ulp riscv projects
//consume this file
#ifndef ULP_RISCV_REGISTER_OPS

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
#endif /* !ULP_RISCV_REGISTER_OPS */

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
#define  GPIO_MATRIX_DELAY_NS                        0
//}}

/* Overall memory map */
#define SOC_DROM_LOW    0x3F000000/*drom0 low address for icache*/
#define SOC_DROM_HIGH   0x3FF80000/*dram0 high address for dcache*/
#define SOC_IROM_LOW    0x40080000
#define SOC_IROM_HIGH   0x40800000
#define SOC_IROM_MASK_LOW  0x40000000
#define SOC_IROM_MASK_HIGH 0x4001A100
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
#define SOC_EXTRAM_DATA_HIGH 0x3FF80000

#define SOC_EXTRAM_DATA_SIZE (SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW)

//First and last words of the D/IRAM region, for both the DRAM address as well as the IRAM alias.
#define SOC_DIRAM_IRAM_LOW    0x40020000
#define SOC_DIRAM_IRAM_HIGH   0x40070000
#define SOC_DIRAM_DRAM_LOW    0x3FFB0000
#define SOC_DIRAM_DRAM_HIGH   0x40000000

// Region of memory accessible via DMA in internal memory. See esp_ptr_dma_capable().
#define SOC_DMA_LOW  0x3FFB0000
#define SOC_DMA_HIGH 0x40000000

// Region of memory accessible via DMA in external memory. See esp_ptr_dma_ext_capable().
#define SOC_DMA_EXT_LOW     0x3F500000
#define SOC_DMA_EXT_HIGH    0x3FF80000

// Region of memory that is byte-accessible. See esp_ptr_byte_accessible().
#define SOC_BYTE_ACCESSIBLE_LOW     0x3FF9E000
#define SOC_BYTE_ACCESSIBLE_HIGH    0x40000000

//Region of memory that is internal, as in on the same silicon die as the ESP32 CPUs
//(excluding RTC data region, that's checked separately.) See esp_ptr_internal().
#define SOC_MEM_INTERNAL_LOW        0x3FF9E000
#define SOC_MEM_INTERNAL_HIGH       0x40072000

// Start (highest address) of ROM boot stack, only relevant during early boot
#define SOC_ROM_STACK_START         0x3fffe70c

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
 *      28                      4               extern edge             DPORT ACCESS            DPORT ACCESS
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
#define ETS_DPORT_INUM                          28

//CPU0 Interrupt number used in ROM, should be cancelled in SDK
#define ETS_SLC_INUM                            1
#define ETS_UART0_INUM                          5
#define ETS_UART1_INUM                          5
#define ETS_SPI2_INUM                           1
//CPU0 Interrupt number used in ROM code only when module init function called, should pay attention here.
#define ETS_FRC_TIMER2_INUM 10 /* use edge*/
#define ETS_GPIO_INUM       4

//Other interrupt number should be managed by the user

//Invalid interrupt for number interrupt matrix
#define ETS_INVALID_INUM                        6

#endif /* _ESP32_SOC_H_ */
