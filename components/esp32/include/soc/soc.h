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

#ifndef _ESP32_SOC_H_
#define _ESP32_SOC_H_

#include <stdint.h>

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

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr) 

#define BIT(nr)                 (1UL << (nr))

#define REG_WRITE(_r, _v)    (*(volatile uint32_t *)(_r)) = (_v)
#define REG_READ(_r) (*(volatile uint32_t *)(_r))

#define REG_GET_BIT(_r, _b)  (*(volatile uint32_t*)(_r) & (_b))
#define REG_SET_BIT(_r, _b)  (*(volatile uint32_t*)(_r) |= (_b))
#define REG_CLR_BIT(_r, _b)  (*(volatile uint32_t*)(_r) &= ~(_b))
#define REG_SET_BITS(_r, _b, _m) (*(volatile uint32_t*)(_r) = (*(volatile uint32_t*)(_r) & ~(_m)) | ((_b) & (_m)))
#define VALUE_GET_FIELD(_r, _f) (((_r) >> (_f##_S)) & (_f))
#define VALUE_GET_FIELD2(_r, _f) (((_r) & (_f))>> (_f##_S))
#define VALUE_SET_FIELD(_r, _f, _v) ((_r)=(((_r) & ~((_f) << (_f##_S)))|((_v)<<(_f##_S))))
#define VALUE_SET_FIELD2(_r, _f, _v) ((_r)=(((_r) & ~(_f))|((_v)<<(_f##_S))))
#define FIELD_TO_VALUE(_f, _v) (((_v)&(_f))<<_f##_S)
#define FIELD_TO_VALUE2(_f, _v) (((_v)<<_f##_S) & (_f))
#define REG_GET_FIELD(_r, _f) ((REG_READ(_r) >> (_f##_S)) & (_f))
#define REG_SET_FIELD(_r, _f, _v) (REG_WRITE((_r),((REG_READ(_r) & ~((_f) << (_f##_S)))|(((_v) & (_f))<<(_f##_S)))))


#define READ_PERI_REG(addr) (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) 
#define WRITE_PERI_REG(addr, val) (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val)   
#define CLEAR_PERI_REG_MASK(reg, mask) WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))))
#define SET_PERI_REG_MASK(reg, mask)   WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)))
#define GET_PERI_REG_MASK(reg, mask)   (READ_PERI_REG(reg) & (mask))
#define GET_PERI_REG_BITS(reg, hipos,lowpos)      ((READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1)) 
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) (WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|(((value) & bit_map)<<(shift)) ))
#define GET_PERI_REG_BITS2(reg, mask,shift)      ((READ_PERI_REG(reg)>>(shift))&(mask))
//}}

//Periheral Clock {{
#define  APB_CLK_FREQ_ROM                            13*1000000
#define  CPU_CLK_FREQ_ROM                            APB_CLK_FREQ_ROM
#define  CPU_CLK_FREQ                                APB_CLK_FREQ
#define  APB_CLK_FREQ                                40*1000000       //unit: Hz
#define  UART_CLK_FREQ                               APB_CLK_FREQ
//#define  WDT_CLK_FREQ                                APB_CLK_FREQ
#define  TIMER_CLK_FREQ                              (80000000>>4) //80MHz divided by 16
#define  SPI_CLK_DIV                                 4
//#define  RTC_CLK_FREQ                              32768           //unit:Hz
//#define  RTC_CLK_FREQ                                100000          //unit:Hz
//#define  CALIB_CLK_MHZ                               40
#define  TICKS_PER_US                                13              // CPU is 80MHz
//}}

#if 0
//Peripheral device base address define{{
#define DR_REG_DPORT_BASE			0x3ff00000
#define DR_REG_UART_BASE 0x60000000
#define DR_REG_SPI1_BASE 0x60002000 //no
#define DR_REG_SPI0_BASE 0x60003000 //no
#define DR_REG_GPIO_BASE 0x60004000 //no
#define DR_REG_FE2_BASE 0x60005000
#define DR_REG_FE_BASE 0x60006000
#define DR_REG_TIMER_BASE 0x60007000 //no
#define DR_REG_RTCCNTL_BASE 0x60008000
#define DR_REG_RTCIO_BASE 0x60008400

#define DR_REG_RTCMEM0_BASE 0x60021000
#define DR_REG_RTCMEM1_BASE 0x60022000
#define DR_REG_RTCMEM2_BASE 0x60023000

#define DR_REG_IO_MUX_BASE 0x60009000 //no
#define DR_REG_WDG_BASE 0x6000A000 //no
#define DR_REG_HINF_BASE 0x6000B000 //no
#define DR_REG_UHCI1_BASE 0x6000C000
//#define DR_REG_MISC_BASE 0x6000D000 //no use
#define DR_REG_I2C_BASE 0x6000E000 //no
#define DR_REG_I2S_BASE 0x6000F000 
#define DR_REG_UART1_BASE 0x60010000
#define DR_REG_BT_BASE 0x60011000
//#define DR_REG_BT_BUFFER_BASE 0x60012000 //no use
#define DR_REG_I2C_EXT_BASE 0x60013000 //no
#define DR_REG_UHCI0_BASE 0x60014000
#define DR_REG_SLCHOST_BASE 0x60015000
#define DR_REG_RMT_BASE 0x60016000
#define DR_REG_PCNT_BASE 0x60017000
#define DR_REG_SLC_BASE 0x60018000
#define DR_REG_LEDC_BASE 0x60019000
#define DR_REG_EFUSE_BASE 0x6001A000
#define DR_REG_SPI_ENCRYPT_BASE 0x6001B000
#define DR_REG_PWM_BASE 0x6001C000 //no
#define DR_REG_TIMERGROUP_BASE 0x6001D000 //no
#define DR_REG_TIMERGROUP1_BASE 0x6001E000 //no
#define DR_REG_BB_BASE 0x6001F000
#define DR_REG_GPIO_SD_BASE 0x60004f00
#else
#define DR_REG_DPORT_BASE                       0x3ff00000
#define DR_REG_UART_BASE 0x3ff40000
#define DR_REG_SPI1_BASE 0x3ff42000 //no
#define DR_REG_SPI0_BASE 0x3ff43000 //no
#define DR_REG_GPIO_BASE 0x3ff44000 //no
#define DR_REG_FE2_BASE 0x3ff45000
#define DR_REG_FE_BASE 0x3ff46000
#define DR_REG_TIMER_BASE 0x3ff47000 //no
#define DR_REG_RTCCNTL_BASE 0x3ff48000
#define DR_REG_RTCIO_BASE 0x3ff48400

#define DR_REG_RTCMEM0_BASE 0x3ff61000
#define DR_REG_RTCMEM1_BASE 0x3ff62000
#define DR_REG_RTCMEM2_BASE 0x3ff63000

#define DR_REG_IO_MUX_BASE 0x3ff49000 //no
#define DR_REG_WDG_BASE 0x3ff4A000 //no
#define DR_REG_HINF_BASE 0x3ff4B000 //no
#define DR_REG_UHCI1_BASE 0x3ff4C000
//#define DR_REG_MISC_BASE 0x6000D000 //no use
#define DR_REG_I2C_BASE 0x3ff4E000 //no
#define DR_REG_I2S_BASE 0x3ff4F000 
#define DR_REG_I2S1_BASE 0x3ff6D000 
#define DR_REG_UART1_BASE 0x3ff50000
#define DR_REG_BT_BASE 0x3ff51000
//#define DR_REG_BT_BUFFER_BASE 0x60012000 //no use
#define DR_REG_I2C_EXT_BASE 0x3ff53000 //no
#define DR_REG_UHCI0_BASE 0x3ff54000
#define DR_REG_SLCHOST_BASE 0x3ff55000
#define DR_REG_RMT_BASE 0x3ff56000
#define DR_REG_PCNT_BASE 0x3ff57000
#define DR_REG_SLC_BASE 0x3ff58000
#define DR_REG_LEDC_BASE 0x3ff59000
#define DR_REG_EFUSE_BASE 0x3ff5A000
#define DR_REG_SPI_ENCRYPT_BASE 0x3ff5B000
#define DR_REG_PWM_BASE 0x3ff5C000 //no
#define DR_REG_TIMERS_BASE 0x3ff5F000 //no
#define DR_REG_TIMERGROUP1_BASE 0x3ff5E000 //no
#define DR_REG_BB_BASE 0x3ff5F000
#define DR_REG_GPIO_SD_BASE 0x3ff44f00
#endif

//}}
#define REG_SPI_BASE(i)                     (DR_REG_SPI0_BASE - i*(0x1000))
#define PERIPHS_TIMER_BASEDDR			DR_REG_TIMER_BASE
#define PERIPHS_SPI_ENCRYPT_BASEADDR		DR_REG_SPI_ENCRYPT_BASE

#define UART0_UNHOLD_MASK 0x3
#define UART1_UNHOLD_MASK 0x60
#define SDIO_UNHOLD_MASK 0xfc
#define SPI_UNHOLD_MASK 0xfc

// TIMER reg {{
#define TIMER_REG_READ(addr)                        READ_PERI_REG(addr)
#define TIMER_REG_WRITE(addr, val)                WRITE_PERI_REG(addr, val)
#define TIMER_SET_REG_MASK(reg, mask)      WRITE_PERI_REG(reg, (READ_PERI_REG(reg)|(mask)))
/* Returns the current time according to the timer timer. */
#define NOW()                                                 TIMER_REG_READ(FRC2_COUNT_ADDRESS)
//load initial_value to timer1
#define FRC1_LOAD_ADDRESS                    (PERIPHS_TIMER_BASEDDR +0x00)
#define FRC1_LOAD_DATA_MSB                 22
#define FRC1_LOAD_DATA_LSB                  0
#define FRC1_LOAD_DATA_MASK              0x007fffff

//timer1's counter value(count from initial_value to 0)
#define FRC1_COUNT_ADDRESS                 (PERIPHS_TIMER_BASEDDR +0x04)
#define FRC1_COUNT_DATA_MSB              22
#define FRC1_COUNT_DATA_LSB               0
#define FRC1_COUNT_DATA_MASK           0x007fffff

#define FRC1_CTRL_ADDRESS                    (PERIPHS_TIMER_BASEDDR +0x08)
#define FRC1_CTRL_DATA_MSB                 7
#define FRC1_CTRL_DATA_LSB                  0
#define FRC1_CTRL_DATA_MASK              0x000000ff

//clear timer1's interrupt when write this address
#define FRC1_INT_ADDRESS                      (PERIPHS_TIMER_BASEDDR +0x0c)
#define FRC1_INT_CLR_MSB                      0
#define FRC1_INT_CLR_LSB                       0
#define FRC1_INT_CLR_MASK                   0x00000001

//only used for simulation
#define FRC1_TEST_ADDRESS                   (PERIPHS_TIMER_BASEDDR +0x10)
#define FRC1_TEST_MODE_MSB                0
#define FRC1_TEST_MODE_LSB                 0
#define FRC1_TEST_MODE_MASK             0x00000001

//load initial_value to timer2
#define FRC2_LOAD_ADDRESS                  (PERIPHS_TIMER_BASEDDR +0x20)
#define FRC2_LOAD_DATA_MSB               31
#define FRC2_LOAD_DATA_LSB                0
#define FRC2_LOAD_DATA_MASK            0xffffffff

//timer2's counter value(count from initial_value to 0)
#define FRC2_COUNT_ADDRESS                (PERIPHS_TIMER_BASEDDR +0x24)
#define FRC2_COUNT_DATA_MSB             31
#define FRC2_COUNT_DATA_LSB              0
#define FRC2_COUNT_DATA_MASK          0xffffffff

#define FRC2_CTRL_ADDRESS                    (PERIPHS_TIMER_BASEDDR +0x28)
#define FRC2_CTRL_DATA_MSB                 7
#define FRC2_CTRL_DATA_LSB                  0
#define FRC2_CTRL_DATA_MASK              0x000000ff
                                           
//clear interrupt when write this address  
#define FRC2_INT_ADDRESS                      (PERIPHS_TIMER_BASEDDR +0x2c)
#define FRC2_INT_CLR_MSB                      0
#define FRC2_INT_CLR_LSB                       0
#define FRC2_INT_CLR_MASK                   0x00000001

//set Alarm_value for timer2 to generate interrupt
#define FRC2_ALARM_ADDRESS               (PERIPHS_TIMER_BASEDDR +0x30)
#define FRC2_ALARM_DATA_MSB            31
#define FRC2_ALARM_DATA_LSB             0
#define FRC2_ALARM_DATA_MASK         0xffffffff
// }}

#define SPI_ENCRYPT_CNTL (PERIPHS_SPI_ENCRYPT_BASEADDR + 0x20)
#define SPI_ENCRYPT_CNTL_ENA				BIT(0)

#define SPI_ENCRYPT_ADDR (PERIPHS_SPI_ENCRYPT_BASEADDR + 0x24)

#define SPI_ENCRYPT_CHECKDONE (PERIPHS_SPI_ENCRYPT_BASEADDR + 0x28)
#define SPI_ENCRYPT_CHECKDONE_STATUS                       BIT(0)

#endif /* _ESP32_SOC_H_ */
