/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#define SOC_ADC_SUPPORTED               1
#define SOC_DAC_SUPPORTED               1
#define SOC_TWAI_SUPPORTED              1
#define SOC_CP_DMA_SUPPORTED            1
#define SOC_CPU_CORES_NUM               1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_RISCV_COPROC_SUPPORTED      1
#define SOC_USB_OTG_SUPPORTED           1
#define SOC_PCNT_SUPPORTED              1
#define SOC_ULP_SUPPORTED               1
#define SOC_RTC_SLOW_MEM_SUPPORTED      1
#define SOC_CCOMP_TIMER_SUPPORTED       1
#define SOC_DIG_SIGN_SUPPORTED          1
#define SOC_HMAC_SUPPORTED              1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS 3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS 1
#define SOC_TEMP_SENSOR_SUPPORTED       1
#define SOC_CACHE_SUPPORT_WRAP          1
#define SOC_FLASH_ENCRYPTION_XTS_AES      1
#define SOC_FLASH_ENCRYPTION_XTS_AES_256 1
#define SOC_PSRAM_DMA_CAPABLE           1
#define SOC_XT_WDT_SUPPORTED            1

/*-------------------------- ADC CAPS ----------------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_RTC_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_ARBITER_SUPPORTED               1
#define SOC_ADC_FILTER_SUPPORTED                1
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (10)
#define SOC_ADC_MAX_CHANNEL_NUM                 (10)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (2)
#define SOC_ADC_PATT_LEN_MAX                    (32) /*!< Two pattern table, each contains 16 items. Each item takes 1 byte */
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_RESULT_BYTES               (2)
#define SOC_ADC_DIGI_DATA_BYTES_PER_CONV        (2)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interva <= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_MAX_BITWIDTH                    (13)
#define SOC_RTC_SLOW_CLOCK_SUPPORT_8MD256       (1)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/

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
#define SOC_GPIO_PIN_COUNT      (47)

// On ESP32-S2 those PADs which have RTC functions must set pullup/down/capability via RTC register.
// On ESP32-S2, Digital IOs have their own registers to control pullup/down/capability, independent with RTC registers.
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT (1)
// Force hold is a new function of ESP32-S2
#define SOC_GPIO_SUPPORT_FORCE_HOLD      (1)

// 0~46 except from 22~25 are valid
#define SOC_GPIO_VALID_GPIO_MASK             (0x7FFFFFFFFFFFULL & ~(0ULL | BIT22 | BIT23 | BIT24 | BIT25))
// GPIO 46 is input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK     (SOC_GPIO_VALID_GPIO_MASK & ~(0ULL | BIT46))

// Support to configure slept status
#define SOC_GPIO_SUPPORT_SLP_SWITCH  (1)

/*-------------------------- Dedicated GPIO CAPS ---------------------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_GPIO_ALLOW_REG_ACCESS (1) /*!< Allow access dedicated GPIO channel by register */
#define SOC_DEDIC_GPIO_HAS_INTERRUPT    (1) /*!< Dedicated GPIO has its own interrupt source */
#define SOC_DEDIC_GPIO_OUT_AUTO_ENABLE  (1) /*!< Dedicated GPIO output attribution is enabled automatically */

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-S2 have 2 I2C.
#define SOC_I2C_NUM            (2)

#define SOC_I2C_FIFO_LEN       (32) /*!< I2C hardware FIFO depth */

// FSM_RST only resets the FSM, not using it. So SOC_I2C_SUPPORT_HW_FSM_RST not defined.
//ESP32-S2 support hardware clear bus
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_REF_TICK   (1)
#define SOC_I2C_SUPPORT_APB        (1)

/*-------------------------- APLL CAPS ----------------------------------------*/
#define SOC_CLK_APLL_SUPPORTED       (1)
// apll_multiplier_out = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)
#define SOC_APLL_MULTIPLIER_OUT_MIN_HZ (350000000) // 350 MHz
#define SOC_APLL_MULTIPLIER_OUT_MAX_HZ (500000000) // 500 MHz
#define SOC_APLL_MIN_HZ    (5303031)   // 5.303031 MHz
#define SOC_APLL_MAX_HZ    (125000000) // 125MHz

/*-------------------------- I2S CAPS ----------------------------------------*/
// ESP32-S2 have 1 I2S
#define SOC_I2S_NUM                (1)
#define SOC_I2S_SUPPORTS_APLL      (1)// ESP32-S2 support APLL
#define SOC_I2S_SUPPORTS_DMA_EQUAL (1)
#define SOC_I2S_LCD_I80_VARIANT    (1)

/*-------------------------- LCD CAPS ----------------------------------------*/
/* Notes: On esp32-s2, LCD intel 8080 timing is generated by I2S peripheral */
#define SOC_LCD_I80_SUPPORTED      (1)  /*!< Intel 8080 LCD is supported */
#define SOC_LCD_I80_BUSES          (1)  /*!< Only I2S0 has LCD mode */
#define SOC_LCD_I80_BUS_WIDTH      (24) /*!< Intel 8080 bus width */

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_HAS_TIMER_SPECIFIC_MUX  (1)
#define SOC_LEDC_SUPPORT_REF_TICK    (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK  (1)
#define SOC_LEDC_CHANNEL_NUM         (8)
#define SOC_LEDC_TIMER_BIT_WIDE_NUM  (14)

/*-------------------------- MPU CAPS ----------------------------------------*/
//TODO: correct the caller and remove unsupported lines
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- PCNT CAPS ---------------------------------------*/
#define SOC_PCNT_GROUPS               (1)
#define SOC_PCNT_UNITS_PER_GROUP      (4)
#define SOC_PCNT_CHANNELS_PER_UNIT    (2)
#define SOC_PCNT_THRES_POINT_PER_UNIT (2)

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_GROUPS                  (1)  /*!< One RMT group */
#define SOC_RMT_TX_CANDIDATES_PER_GROUP (4)  /*!< Number of channels that capable of Transmit in each group */
#define SOC_RMT_RX_CANDIDATES_PER_GROUP (4)  /*!< Number of channels that capable of Receive in each group */
#define SOC_RMT_CHANNELS_PER_GROUP      (4)  /*!< Total 4 channels */
#define SOC_RMT_MEM_WORDS_PER_CHANNEL   (64) /*!< Each channel owns 64 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_DEMODULATION (1)  /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT   (1)  /*!< Support transmiting specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_SYNCHRO      (1)  /*!< Support coordinate a group of TX channels to start simultaneously */
#define SOC_RMT_SUPPORT_REF_TICK        (1)  /*!< Support set REF_TICK as the RMT clock source */
#define SOC_RMT_CHANNEL_CLK_INDEPENDENT (1)  /*!< Can select different source clock for each channel */

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT   22
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED 1
#define SOC_RTCIO_HOLD_SUPPORTED 1
#define SOC_RTCIO_WAKE_SUPPORTED 1


/*-------------------------- SIGMA DELTA CAPS --------------------------------*/
#define SOC_SIGMADELTA_NUM            1
#define SOC_SIGMADELTA_CHANNEL_NUM (8) // 8 channels

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM          3
#define SOC_SPI_DMA_CHAN_NUM        3
#define SOC_SPI_PERIPH_CS_NUM(i)    (((i)==0)? 2: (((i)==1)? 6: 3))
#define SOC_SPI_MAX_CS_NUM          6

#define SOC_SPI_MAXIMUM_BUFFER_SIZE     72
#define SOC_SPI_MAX_PRE_DIVIDER         8192

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
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUTPUT             1

#define SOC_MEMSPI_IS_INDEPENDENT 1
#define SOC_SPI_SUPPORT_OCT 1

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM  (1)  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM    (3)  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO (32) // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI (32) // Bit width of systimer high part

/*-------------------------- TIMER GROUP CAPS --------------------------------*/
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (64)
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (2)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS (SOC_TIMER_GROUPS * SOC_TIMER_GROUP_TIMERS_PER_GROUP)

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_VERSION_2                 (1)     /*!<Hardware version of touch sensor */
#define SOC_TOUCH_SENSOR_NUM                (15)    /*!<15 Touch channels */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM     (3)     /* Sopport touch proximity channel number. */

#define SOC_TOUCH_PAD_THRESHOLD_MAX         (0x1FFFFF)  /*!<If set touch threshold max value, The touch sensor can't be in touched status */
#define SOC_TOUCH_PAD_MEASURE_WAIT_MAX      (0xFF)  /*!<The timer frequency is 8Mhz, the max value is 0xff */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_BRP_MIN                2
#define SOC_TWAI_BRP_MAX                32768
#define SOC_TWAI_SUPPORTS_RX_STATUS     1

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-S2 has 2 UART.
#define SOC_UART_NUM                (2)
#define SOC_UART_SUPPORT_REF_TICK   (1)         /*!< Support REF_TICK as the clock source */
#define SOC_UART_FIFO_LEN           (128)       /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)   /*!< Max bit rate supported by UART */

/*-------------------------- SPIRAM CAPS -------------------------------------*/
#define SOC_SPIRAM_SUPPORTED    1

/*-------------------------- USB CAPS ----------------------------------------*/
#define SOC_USB_PERIPH_NUM 1


/*--------------------------- SHA CAPS ---------------------------------------*/
/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SOC_SHA_DMA_MAX_BUFFER_SIZE     (3968)
#define SOC_SHA_SUPPORT_DMA             (1)

/* The SHA engine is able to resume hashing from a user supplied context */
#define SOC_SHA_SUPPORT_RESUME          (1)

/* Has "crypto DMA", which is shared with AES */
#define SOC_SHA_CRYPTO_DMA              (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)
#define SOC_SHA_SUPPORT_SHA512_224      (1)
#define SOC_SHA_SUPPORT_SHA512_256      (1)
#define SOC_SHA_SUPPORT_SHA512_T        (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)


/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)


/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)
#define SOC_AES_SUPPORT_GCM     (1)

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)

/* Has "crypto DMA", which is shared with SHA */
#define SOC_AES_CRYPTO_DMA      (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_192 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- WI-FI HARDWARE TSF CAPS -------------------------------*/
#define SOC_WIFI_HW_TSF                 (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
/*-------------------------- Power Management CAPS ---------------------------*/
#define SOC_PM_SUPPORT_EXT_WAKEUP       (1)
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP        (1)     /*!<Supports waking up from touch pad trigger */

#define SOC_PM_SUPPORT_RTC_PERIPH_PD              (1)
#define SOC_PM_SUPPORT_WIFI_PD          (1)


/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)
/* ---------------------------- Compatibility ------------------------------- */
// No contents
