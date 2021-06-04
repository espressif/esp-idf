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

#ifdef __has_include
#  if __has_include("sdkconfig.h")
#    include "sdkconfig.h"
#    define SOC_CAPS_ECO_VER    CONFIG_ESP32_REV_MIN
#  endif
#endif

#if __DOXYGEN__ && !defined(SOC_CAPS_ECO_VER)
#define SOC_CAPS_ECO_VER    SOC_CAPS_ECO_VER_MAX
#endif

#ifndef SOC_CAPS_ECO_VER
#warning ECO version not determined. Some ECO related caps will not be available.
#warning Define SOC_CAPS_ECO_VER before including this header.

// Define warning strings here for ECO-ed features to show error when they are used without being
// defined correctly
#define SOC_BROWNOUT_RESET_SUPPORTED    "Not determined"
#define SOC_TWAI_BRP_DIV_SUPPORTED      "Not determined"
#endif

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_CAPS_ECO_VER_MAX        3

#define SOC_DAC_SUPPORTED           1
#define SOC_MCPWM_SUPPORTED         1
#define SOC_SDMMC_HOST_SUPPORTED    1
#define SOC_BT_SUPPORTED            1
#define SOC_PCNT_SUPPORTED          1
#define SOC_SDIO_SLAVE_SUPPORTED    1
#define SOC_TWAI_SUPPORTED          1
#define SOC_EMAC_SUPPORTED          1
#define SOC_RISCV_COPROC_SUPPORTED  0   //TODO: correct the caller and remove this line
#define SOC_CPU_CORES_NUM           2
#define SOC_ULP_SUPPORTED           1
#define SOC_RTC_SLOW_MEM_SUPPORTED  1
#define SOC_CCOMP_TIMER_SUPPORTED   1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS 1

/*-------------------------- ADC CAPS ----------------------------------------*/
#define SOC_ADC_PERIPH_NUM              (2)
#define SOC_ADC_PATT_LEN_MAX            (16)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM) ((PERIPH_NUM==0)? 8: 10)
#define SOC_ADC_MAX_CHANNEL_NUM         (10)
#define SOC_ADC_MAX_BITWIDTH            (12)

/**
 * Check if adc support digital controller (DMA) mode.
 * @value
 *      - 1 : support;
 *      - 0 : not support;
 */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM) ((PERIPH_NUM==0)? 1: 0)
#define SOC_ADC_SUPPORT_RTC_CTRL        1

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#if SOC_CAPS_ECO_VER >= 1
#define SOC_BROWNOUT_RESET_SUPPORTED 1
#endif

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_BREAKPOINTS_NUM         2
#define SOC_CPU_WATCHPOINTS_NUM         2

#define SOC_CPU_WATCHPOINT_SIZE         64 // bytes

#define SOC_CPU_HAS_FPU             1

/*-------------------------- DAC CAPS ----------------------------------------*/
#define SOC_DAC_PERIPH_NUM      2
#define SOC_DAC_RESOLUTION      8 // DAC resolution ratio 8 bit

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32 has 1 GPIO peripheral
#define SOC_GPIO_PORT                   (1)
#define SOC_GPIO_PIN_COUNT              40

// SOC_GPIO_SUPPORT_RTC_INDEPENDENT not defined. On ESP32 those PADs which have RTC functions must
// set pullup/down/capability via RTC register. On ESP32-S2, Digital IOs have their own registers to
// control pullup/down/capability, independent with RTC registers.

// 0~39 except from 20, 24, 28~31 are valid
#define SOC_GPIO_VALID_GPIO_MASK        (0xFFFFFFFFFFULL & ~(0ULL | BIT20 | BIT24 | BIT28 | BIT29 | BIT30 | BIT31))
// GPIO >= 34 are input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK (SOC_GPIO_VALID_GPIO_MASK & ~(0ULL | BIT34 | BIT35 | BIT36 | BIT37 | BIT38 | BIT39))

// Support to configure slept status
#define SOC_GPIO_SUPPORT_SLP_SWITCH  (1)

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32 have 2 I2C.
#define SOC_I2C_NUM             (2)

#define SOC_I2C_FIFO_LEN        (32) /*!< I2C hardware FIFO depth */

#define SOC_I2C_SUPPORT_APB     (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
// ESP32 have 2 I2S
#define SOC_I2S_NUM                 (2)

#define SOC_I2S_SUPPORTS_PDM        (1) // ESP32 support PDM
#define SOC_I2S_SUPPORTS_ADC_DAC    (1) // ESP32 support ADC and DAC

#define SOC_I2S_MAX_BUFFER_SIZE     (4 * 1024 * 1024) //the maximum RAM can be allocated

#define SOC_I2S_APLL_MIN_FREQ       (250000000)
#define SOC_I2S_APLL_MAX_FREQ       (500000000)
#define SOC_I2S_APLL_MIN_RATE       (10675) //in Hz, I2S Clock rate limited by hardware

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_HS_MODE  (1)
#define SOC_LEDC_SUPPORT_HS_MODE     (1)
#define SOC_LEDC_CHANNEL_NUM         (8)
#define SOC_LEDC_TIMER_BIT_WIDE_NUM  (20)

/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_GROUPS                     (2)    ///< 2 MCPWM groups on the chip (i.e., the number of independent MCPWM peripherals)
#define SOC_MCPWM_TIMERS_PER_GROUP           (3)    ///< The number of timers that each group has
#define SOC_MCPWM_OPERATORS_PER_GROUP        (3)    ///< The number of operators that each group has
#define SOC_MCPWM_COMPARATORS_PER_OPERATOR   (2)    ///< The number of comparators that each operator has
#define SOC_MCPWM_GENERATORS_PER_OPERATOR    (2)    ///< The number of generators that each operator has
#define SOC_MCPWM_FAULT_DETECTORS_PER_GROUP  (3)    ///< The number of fault signal detectors that each group has
#define SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP   (1)    ///< The number of capture timers that each group has
#define SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER (3)    ///< The number of capture channels that each capture timer has
#define SOC_MCPWM_EXT_SYNCERS_PER_GROUP      (3)    ///< The number of external syncers that each group has
#define SOC_MCPWM_BASE_CLK_HZ       (160000000ULL)  ///< Base Clock frequency of 160MHz

/*-------------------------- MPU CAPS ----------------------------------------*/
//TODO: correct the caller and remove unsupported lines
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- PCNT CAPS ---------------------------------------*/
// ESP32 have 1 PCNT peripheral
#define SOC_PCNT_PORT_NUM      (1)
#define SOC_PCNT_UNIT_NUM      (8)
#define SOC_PCNT_UNIT_CHANNEL_NUM (2)

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_GROUPS                  (1)  /*!< One RMT group */
#define SOC_RMT_TX_CANDIDATES_PER_GROUP (8)  /*!< Number of channels that capable of Transmit in each group */
#define SOC_RMT_RX_CANDIDATES_PER_GROUP (8)  /*!< Number of channels that capable of Receive in each group */
#define SOC_RMT_CHANNELS_PER_GROUP      (8)  /*!< Total 8 channels */
#define SOC_RMT_MEM_WORDS_PER_CHANNEL   (64) /*!< Each channel owns 64 words memory */
#define SOC_RMT_SUPPORT_REF_TICK        (1)  /*!< Support set REF_TICK as the RMT clock source */
#define SOC_RMT_CHANNEL_CLK_INDEPENDENT (1)  /*!< Can select different source clock for each channel */

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT 18
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED 1
#define SOC_RTCIO_HOLD_SUPPORTED 1
#define SOC_RTCIO_WAKE_SUPPORTED 1

/*-------------------------- SIGMA DELTA CAPS --------------------------------*/
#define SOC_SIGMADELTA_NUM            1
#define SOC_SIGMADELTA_CHANNEL_NUM (8) // 8 channels

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM          3
#define SOC_SPI_DMA_CHAN_NUM        2

#define SOC_SPI_PERIPH_CS_NUM(i)    3

#define SOC_SPI_MAXIMUM_BUFFER_SIZE 64
#define SOC_SPI_MAX_PRE_DIVIDER     8192

#define SOC_SPI_SUPPORT_AS_CS       1 //Support to toggle the CS while the clock toggles

// Peripheral supports DIO, DOUT, QIO, or QOUT
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(spi_host)         ({(void)spi_host; 1;})

/*-------------------------- TIMER GROUP CAPS --------------------------------*/
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (2)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (64)
#define SOC_TIMER_GROUP_TOTAL_TIMERS (SOC_TIMER_GROUPS * SOC_TIMER_GROUP_TIMERS_PER_GROUP)

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_NUM                (10)

#define SOC_TOUCH_PAD_MEASURE_WAIT_MAX      (0xFF)  /*!<The timer frequency is 8Mhz, the max value is 0xff */
#define SOC_TOUCH_PAD_THRESHOLD_MAX         (0)     /*!<If set touch threshold max value, The touch sensor can't be in touched status */
#define SOC_TOUCH_PAD_WAKE_SUPPORTED        (1)     /*!<Supports waking up from touch pad trigger */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_BRP_MIN                        2
#if SOC_CAPS_ECO_VER >= 2
#  define SOC_TWAI_BRP_MAX              256
#  define SOC_TWAI_BRP_DIV_SUPPORTED    1
#  define SOC_TWAI_BRP_DIV_THRESH       128
#else
#  define SOC_TWAI_BRP_MAX              128
#endif
#define SOC_TWAI_SUPPORT_MULTI_ADDRESS_LAYOUT   1

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32 have 3 UART.
#define SOC_UART_NUM                (3)
#define SOC_UART_SUPPORT_REF_TICK   (1)         /*!< Support REF_TICK as the clock source */
#define SOC_UART_FIFO_LEN           (128)       /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)   /*!< Max bit rate supported by UART */


/*-------------------------- SPIRAM CAPS -------------------------------------*/
#define SOC_SPIRAM_SUPPORTED    1

/*--------------------------- SHA CAPS ---------------------------------------*/
/* ESP32 style SHA engine, where multiple states can be stored in parallel */
#define SOC_SHA_SUPPORT_PARALLEL_ENG    (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)


/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)


/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_192 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (32)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

/*-------------------------- Power Management CAPS ---------------------------*/
#define SOC_PM_SUPPORT_EXT_WAKEUP       (1)

/* ---------------------------- Compatibility ------------------------------- */
#define SOC_CAN_SUPPORTED                   SOC_TWAI_SUPPORTED
#define CAN_BRP_MIN                         SOC_TWAI_BRP_MIN
#define CAN_BRP_MAX                         SOC_TWAI_BRP_MAX
#define CAN_SUPPORT_MULTI_ADDRESS_LAYOUT    SOC_TWAI_SUPPORT_MULTI_ADDRESS_LAYOUT
#if SOC_CAPS_ECO_VER >= 2
#  define CAN_BRP_DIV_SUPPORTED             SOC_TWAI_BRP_DIV_SUPPORTED
#  define CAN_BRP_DIV_THRESH                SOC_TWAI_BRP_DIV_THRESH
#endif

/*-------------------------- SDMMC CAPS -----------------------------------------*/

/* On ESP32, clock/cmd/data pins use IO MUX.
 * Card detect, write protect, interrupt use GPIO Matrix on all chips.
 */
#define SOC_SDMMC_USE_IOMUX  1
#define SOC_SDMMC_NUM_SLOTS  2
