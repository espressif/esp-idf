// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_bit_defs.h"
#endif

#include "sdkconfig.h"

#define PRO_CPU_NUM (0)

#define DR_REG_SYSTEM_BASE                      0x600c0000
#define DR_REG_SENSITIVE_BASE                   0x600c1000
#define DR_REG_INTERRUPT_BASE                   0x600c2000
#define DR_REG_EXTMEM_BASE                      0x600c4000
#define DR_REG_MMU_TABLE                        0x600c5000
#define DR_REG_AES_BASE                         0x6003a000
#define DR_REG_SHA_BASE                         0x6003b000
#define DR_REG_RSA_BASE                         0x6003c000
#define DR_REG_HMAC_BASE                        0x6003e000
#define DR_REG_DIGITAL_SIGNATURE_BASE           0x6003d000
#define DR_REG_GDMA_BASE                        0x6003f000
#define DR_REG_ASSIST_DEBUG_BASE                0x600ce000
#define DR_REG_DEDICATED_GPIO_BASE              0x600cf000
#define DR_REG_WORLD_CNTL_BASE                  0x600d0000
#define DR_REG_DPORT_END                        0x600d3FFC
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
#define DR_REG_UHCI0_BASE                       0x60014000
#define DR_REG_RMT_BASE                         0x60016000
#define DR_REG_LEDC_BASE                        0x60019000
#define DR_REG_EFUSE_BASE                       0x60008800
#define DR_REG_NRX_BASE                         0x6001CC00
#define DR_REG_BB_BASE                          0x6001D000
#define DR_REG_TIMERGROUP0_BASE                 0x6001F000
#define DR_REG_TIMERGROUP1_BASE                 0x60020000
#define DR_REG_SYSTIMER_BASE                    0x60023000
#define DR_REG_SPI2_BASE                        0x60024000
#define DR_REG_SYSCON_BASE                      0x60026000
#define DR_REG_APB_CTRL_BASE                    0x60026000    /* Old name for SYSCON, to be removed */
#define DR_REG_TWAI_BASE                        0x6002B000
#define DR_REG_I2S0_BASE                        0x6002D000
#define DR_REG_APB_SARADC_BASE                  0x60040000
#define DR_REG_USB_SERIAL_JTAG_BASE             0x60043000
#define DR_REG_AES_XTS_BASE                     0x600CC000

#define REG_UHCI_BASE(i)                        (DR_REG_UHCI0_BASE - (i) * 0x8000)
#define REG_UART_BASE(i)                        (DR_REG_UART_BASE + (i) * 0x10000)
#define REG_UART_AHB_BASE(i)                    (0x60000000 + (i) * 0x10000)
#define UART_FIFO_AHB_REG(i)                    (REG_UART_AHB_BASE(i) + 0x0)
#define REG_I2S_BASE(i)                         (DR_REG_I2S_BASE + (i) * 0x1E000)
#define REG_TIMG_BASE(i)                        (DR_REG_TIMERGROUP0_BASE + (i)*0x1000)
#define REG_SPI_MEM_BASE(i)                     (DR_REG_SPI0_BASE - (i) * 0x1000)
#define REG_I2C_BASE(i)                         (DR_REG_I2C_EXT_BASE + (i) * 0x14000 )

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr)

#ifndef __ASSEMBLER__
#define BIT(nr)                 (1UL << (nr))
#else
#define BIT(nr)                 (1 << (nr))
#endif

#ifndef __ASSEMBLER__

//write value to register
#define REG_WRITE(_r, _v) ({                                                                                           \
            (*(volatile uint32_t *)(_r)) = (_v);                                                                       \
        })

//read value from register
#define REG_READ(_r) ({                                                                                                \
            (*(volatile uint32_t *)(_r));                                                                              \
        })

//get bit or get bits from register
#define REG_GET_BIT(_r, _b)  ({                                                                                        \
            (*(volatile uint32_t*)(_r) & (_b));                                                                        \
        })

//set bit or set bits to register
#define REG_SET_BIT(_r, _b)  ({                                                                                        \
            (*(volatile uint32_t*)(_r) |= (_b));                                                                       \
        })

//clear bit or clear bits of register
#define REG_CLR_BIT(_r, _b)  ({                                                                                        \
            (*(volatile uint32_t*)(_r) &= ~(_b));                                                                      \
        })

//set bits of register controlled by mask
#define REG_SET_BITS(_r, _b, _m) ({                                                                                    \
            (*(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m)));                         \
        })

//get field from register, uses field _S & _V to determine mask
#define REG_GET_FIELD(_r, _f) ({                                                                                       \
            ((REG_READ(_r) >> (_f##_S)) & (_f##_V));                                                                   \
        })

//set field of a register from variable, uses field _S & _V to determine mask
#define REG_SET_FIELD(_r, _f, _v) ({                                                                                   \
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
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr)));                                                         \
        })

//write value to register
#define WRITE_PERI_REG(addr, val) ({                                                                                   \
            (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val);                                       \
        })

//clear bits of register controlled by mask
#define CLEAR_PERI_REG_MASK(reg, mask) ({                                                                              \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))));                                                     \
        })

//set bits of register controlled by mask
#define SET_PERI_REG_MASK(reg, mask) ({                                                                                \
            WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)));                                                        \
        })

//get bits of register controlled by mask
#define GET_PERI_REG_MASK(reg, mask) ({                                                                                \
            (READ_PERI_REG(reg) & (mask));                                                                             \
        })

//get bits of register controlled by highest bit and lowest bit
#define GET_PERI_REG_BITS(reg, hipos,lowpos) ({                                                                        \
            ((READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1));                                            \
        })

//set bits of register controlled by mask and shift
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) ({                                                                  \
            (WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift)) ));      \
        })

//get field of register
#define GET_PERI_REG_BITS2(reg, mask,shift) ({                                                                         \
            ((READ_PERI_REG(reg)>>(shift))&(mask));                                                                    \
        })

#endif /* !__ASSEMBLER__ */
//}}

//Periheral Clock {{
#define  APB_CLK_FREQ_ROM                            ( 40*1000000 )
#define  CPU_CLK_FREQ_ROM                            APB_CLK_FREQ_ROM
#define  UART_CLK_FREQ_ROM                           ( 40*1000000)
#define  EFUSE_CLK_FREQ_ROM                          ( 20*1000000)
#define  CPU_CLK_FREQ                                APB_CLK_FREQ
#if CONFIG_IDF_ENV_FPGA
#define  APB_CLK_FREQ                                ( 40*1000000 )
#else
#define  APB_CLK_FREQ                                ( 80*1000000 )
#endif
#define  REF_CLK_FREQ                                ( 1000000 )
#define  RTC_CLK_FREQ                                (20*1000000)
#define  XTAL_CLK_FREQ                               (40*1000000)
#define  UART_CLK_FREQ                               APB_CLK_FREQ
#define  WDT_CLK_FREQ                                APB_CLK_FREQ
#define  TIMER_CLK_FREQ                              (80000000>>4) //80MHz divided by 16
#define  SPI_CLK_DIV                                 4
#define  TICKS_PER_US_ROM                            40              // CPU is 80MHz
#define  GPIO_MATRIX_DELAY_NS                        0
//}}

/* Overall memory map */
#define SOC_DROM_LOW    0x3C000000
#define SOC_DROM_HIGH   0x3C800000
#define SOC_IROM_LOW    0x42000000
#define SOC_IROM_HIGH   0x42800000
#define SOC_IROM_MASK_LOW  0x40000000
#define SOC_IROM_MASK_HIGH 0x40060000
#define SOC_DROM_MASK_LOW 0x3FF00000
#define SOC_DROM_MASK_HIGH 0x3FF20000
#define SOC_IRAM_LOW    0x4037C000
#define SOC_IRAM_HIGH   0x403E0000
#define SOC_DRAM_LOW    0x3FC80000
#define SOC_DRAM_HIGH   0x3FCE0000
#define SOC_RTC_IRAM_LOW  0x50000000 // ESP32-C3 only has RTC slow memory
#define SOC_RTC_IRAM_HIGH 0x50002000
#define SOC_RTC_DRAM_LOW  0x50000000
#define SOC_RTC_DRAM_HIGH 0x50002000
#define SOC_RTC_DATA_LOW  0x50000000
#define SOC_RTC_DATA_HIGH 0x50002000

//First and last words of the D/IRAM region, for both the DRAM address as well as the IRAM alias.
#define SOC_DIRAM_IRAM_LOW    0x40380000
#define SOC_DIRAM_IRAM_HIGH   0x403E0000
#define SOC_DIRAM_DRAM_LOW    0x3FC80000
#define SOC_DIRAM_DRAM_HIGH   0x3FCE0000

// Region of memory accessible via DMA. See esp_ptr_dma_capable().
#define SOC_DMA_LOW  0x3FC88000
#define SOC_DMA_HIGH 0x3FD00000

// Region of RAM that is byte-accessible. See esp_ptr_byte_accessible().
#define SOC_BYTE_ACCESSIBLE_LOW     0x3FC88000
#define SOC_BYTE_ACCESSIBLE_HIGH    0x3FD00000

//Region of memory that is internal, as in on the same silicon die as the ESP32 CPUs
//(excluding RTC data region, that's checked separately.) See esp_ptr_internal().
#define SOC_MEM_INTERNAL_LOW        0x3FC80000
#define SOC_MEM_INTERNAL_HIGH       0x3FCE0000
#define SOC_MEM_INTERNAL_LOW1       0x40370000
#define SOC_MEM_INTERNAL_HIGH1      0x403E0000
#define SOC_MEM_INTERNAL_LOW2       0x600FE000
#define SOC_MEM_INTERNAL_HIGH2      0x60100000

#define SOC_MAX_CONTIGUOUS_RAM_SIZE (SOC_IRAM_HIGH - SOC_IRAM_LOW) ///< Largest span of contiguous memory (DRAM or IRAM) in the address space

// Region of address space that holds peripherals
#define SOC_PERIPHERAL_LOW 0x60000000
#define SOC_PERIPHERAL_HIGH 0x60100000

// Debug region, not used by software
#define SOC_DEBUG_LOW 0x20000000
#define SOC_DEBUG_HIGH 0x28000000

// Start (highest address) of ROM boot stack, only relevant during early boot
#define SOC_ROM_STACK_START         0x3fcebf10

//On RISC-V CPUs, the interrupt sources are all external interrupts, whose type, source and priority are configured by SW.
//There is no HW NMI conception. SW should controlled the masked levels through INT_THRESH_REG.

//CPU0 Interrupt number reserved in riscv/vector.S, not touch this.
#define ETS_T1_WDT_INUM                         24
#define ETS_CACHEERR_INUM                       25
#define ETS_MEMPROT_ERR_INUM                    26
//CPU0 Max valid interrupt number
#define ETS_MAX_INUM                            31

//CPU0 Interrupt number used in ROM, should be cancelled in SDK
#define ETS_SLC_INUM                            1
#define ETS_UART0_INUM                          5
#define ETS_UART1_INUM                          5
#define ETS_SPI2_INUM                           1
//CPU0 Interrupt number used in ROM code only when module init function called, should pay attention here.
#define ETS_GPIO_INUM       4

//Other interrupt number should be managed by the user

//Invalid interrupt for number interrupt matrix
#define ETS_INVALID_INUM                        0

//Interrupt medium level, used for INT WDT for example
#define SOC_INTERRUPT_LEVEL_MEDIUM              4
