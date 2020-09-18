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

/*
 * Soc capabilities file, describing the following chip attributes:
 * - Peripheral or feature supported or not
 * - Number of resources (peripheral, channel, etc.)
 * - Maximum / Minimum value of HW, including: buffer/fifo size, length of transaction, frequency
 *   supported, etc.
 *
 * For boolean definitions:
 * - if true: `#define MODULE_[SUBMODULE_]SUPPORT_FEATURE 1`.
 *   (`#define` blank string causes error when checking by `#if x`)
 * - if false: not define anything at all.
 *   (`#ifdef x` is true even when `#define 0` is defined before.)
 *
 * The code depending on this file uses these boolean definitions as `#if x` or `#if !x`.
 * (`#ifdef/ifndef x` is not compatible with `#define x 0`. Though we don't suggest to use `#define
 * x 0`, it's still a risk.)
 *
 * ECO & exceptions:
 * For ECO-ed booleans, `#define x "Not determined"` for them. This will cause error when used by
 * `#if x` and `#if !x`, making these missing definitions more obvious.
 */

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_TWAI_SUPPORTED 1
#define SOC_CP_DMA_SUPPORTED 1
#define SOC_CPU_CORES_NUM 1
#define SOC_SUPPORTS_SECURE_DL_MODE 1
#define SOC_RISCV_COPROC_SUPPORTED 1
#define SOC_USB_SUPPORTED 1
#define SOC_PCNT_SUPPORTED 1

#define SOC_CACHE_SUPPORT_WRAP    1

/*-------------------------- ADC CAPS ----------------------------------------*/
#define SOC_ADC_PERIPH_NUM              (2)
#define SOC_ADC_PATT_LEN_MAX            (16)

#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM) (10)
#define SOC_ADC_MAX_CHANNEL_NUM         (10)

/**
 * Check if adc support digital controller (DMA) mode.
 * @value
 *      - 1 : support;
 *      - 0 : not support;
 */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM) ((PERIPH_NUM==0)? 1: 1)

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CP-DMA CAPS -------------------------------------*/
#define SOC_CP_DMA_MAX_BUFFER_SIZE (4095) /*!< Maximum size of the buffer that can be attached to descriptor */

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_BREAKPOINTS_NUM         2
#define SOC_CPU_WATCHPOINTS_NUM         2

#define SOC_CPU_WATCHPOINT_SIZE         64 // bytes

/*-------------------------- DAC CAPS ----------------------------------------*/
#define SOC_DAC_PERIPH_NUM      2

#define SOC_DAC_RESOLUTION      8 // DAC resolution ratio 8 bit

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-S2 has 1 GPIO peripheral
#define SOC_GPIO_PORT           (1)
#define SOC_GPIO_PIN_COUNT      (48)

// On ESP32 those PADs which have RTC functions must set pullup/down/capability via RTC register.
// On ESP32-S2, Digital IOs have their own registers to control pullup/down/capability, independent with RTC registers.
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT (1)
// Force hold is a new function of ESP32-S2
#define SOC_GPIO_SUPPORT_FORCE_HOLD      (1)

// 0~47 except from 22~25, 47 are valid
#define SOC_GPIO_VALID_GPIO_MASK             (0xFFFFFFFFFFFFULL & ~(0ULL | BIT22 | BIT23 | BIT24 | BIT25 | BIT47))
// GPIO 46, 47 are input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK     (SOC_GPIO_VALID_GPIO_MASK & ~(0ULL | BIT46 | BIT47))

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-S2 have 2 I2C.
#define SOC_I2C_NUM            (2)

#define SOC_I2C_FIFO_LEN       (32) /*!< I2C hardware FIFO depth */

//ESP32-S2 support hardware FSM reset
#define SOC_I2C_SUPPORT_HW_FSM_RST  (1)
//ESP32-S2 support hardware clear bus
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
// ESP32-S2 have 2 I2S
#define SOC_I2S_NUM            (1)

#define SOC_I2S_SUPPORTS_DMA_EQUAL (1) // ESP32-S2 need dma equal

#define SOC_I2S_MAX_BUFFER_SIZE               (4 * 1024 * 1024) //the maximum RAM can be allocated

#define SOC_I2S_APLL_MIN_FREQ                     (250000000)
#define SOC_I2S_APLL_MAX_FREQ                     (500000000)
#define SOC_I2S_APLL_MIN_RATE                 (10675) //in Hz, I2S Clock rate limited by hardware

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_XTAL_CLOCK  (1)

/*-------------------------- MPU CAPS ----------------------------------------*/
//TODO: correct the caller and remove unsupported lines
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- PCNT CAPS ---------------------------------------*/
// ESP32-S2 have 1 PCNT peripheral
#define SOC_PCNT_PORT_NUM      (1)
#define SOC_PCNT_UNIT_NUM      (4) // ESP32-S2 only have 4 unit
#define SOC_PCNT_UNIT_CHANNEL_NUM (2)

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_CHANNEL_MEM_WORDS (64)       /*!< Each channel owns 64 words memory (1 word = 4 Bytes) */
#define SOC_RMT_CHANNELS_NUM (4)             /*!< Total 4 channels */
#define SOC_RMT_SUPPORT_RX_PINGPONG (1)      /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_RX_DEMODULATION  (1) /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT (1)    /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_GROUP (1)         /*!< Support a group of TX channels to transmit simultaneously */

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT   22

/*-------------------------- SIGMA DELTA CAPS --------------------------------*/
// ESP32-S2 have 1 SIGMADELTA peripheral.
#define SOC_SIGMADELTA_NUM            1

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM      3
#define SOC_SPI_DMA_CHAN_NUM    3
#define SOC_SPI_PERIPH_CS_NUM(i)    (((i)==0)? 2: (((i)==1)? 6: 3))

#define SOC_SPI_MAXIMUM_BUFFER_SIZE     72

//#define SOC_SPI_SUPPORT_AS_CS         //don't support to toggle the CS while the clock toggles
#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_CONTINUOUS_TRANS    1
/// The SPI Slave half duplex mode has been updated greatly in ESP32-S2
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1

// Peripheral supports DIO, DOUT, QIO, or QOUT
// VSPI (SPI3) only support 1-bit mode
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)          ((host_id) != 2)

// Peripheral supports output given level during its "dummy phase"
// Only SPI1 supports this feature
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUTPUT(host_id)    ((host_id) == 0)

#define SOC_MEMSPI_IS_INDEPENDENT 1

/*-------------------------- SYS TIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM (1)   // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM (3)     // Number of alarm units

#define SOC_SYSTIMER_BIT_WIDTH_LO (32) // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI (32) // Bit width of systimer high part

/*-------------------------- TIMER GROUP CAPS --------------------------------*/
#define SOC_TIMER_GROUP_SUPPORT_XTAL        1

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_NUM                (15)    /*! 15 Touch channels */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM     (3)  /* Sopport touch proximity channel number. */

#define SOC_TOUCH_PAD_THRESHOLD_MAX         (0x1FFFFF)  /*!<If set touch threshold max value, The touch sensor can't be in touched status */
#define SOC_TOUCH_PAD_MEASURE_WAIT_MAX      (0xFF)  /*!<The timer frequency is 8Mhz, the max value is 0xff */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_BRP_MIN                2
#define SOC_TWAI_BRP_MAX                32768

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-S2 have 2 UART.
#define SOC_UART_NUM           (2)

#define SOC_UART_FIFO_LEN          (128)      /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX       (5000000)  /*!< Max bit rate supported by UART */

/*-------------------------- USB CAPS ----------------------------------------*/
#define SOC_USB_PERIPH_NUM 1

/* ---------------------------- Compatibility ------------------------------- */
// No contents