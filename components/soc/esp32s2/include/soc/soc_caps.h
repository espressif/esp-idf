/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
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
 * For ECO-ed booleans, `#define x "Not determined" // [gen_soc_caps:ignore]` for them. This will cause error when used by
 * `#if x` and `#if !x`, making these missing definitions more obvious. Ignore pragma ensures those values are not converted into config options.
 *
 * These defines are parsed and imported as kconfig variables via the script
 * `tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py`
 *
 * If this file is changed the script will automatically run the script
 * and generate the kconfig variables as part of the pre-commit hooks.
 *
 * It can also be run manually. For more information, see `${IDF_PATH}/tools/gen_soc_caps_kconfig/README.md`
 */

#pragma once

#if __has_include("soc/soc_caps_eval.h")
#include "soc/soc_caps_eval.h"
#endif

#define _SOC_CAPS_TARGET_IS_ESP32S2     1 // [gen_soc_caps:ignore]

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_ADC_SUPPORTED               1
#define SOC_DAC_SUPPORTED               1
#define SOC_UART_SUPPORTED              1
#define SOC_TWAI_SUPPORTED              1
#define SOC_CP_DMA_SUPPORTED            1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_GPTIMER_SUPPORTED           1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_ULP_FSM_SUPPORTED           1
#define SOC_RISCV_COPROC_SUPPORTED      1
#define SOC_USB_OTG_SUPPORTED           1
#define SOC_PCNT_SUPPORTED              1
#define SOC_PHY_SUPPORTED               1
#define SOC_WIFI_SUPPORTED              1
#define SOC_ULP_SUPPORTED               1
#define SOC_CCOMP_TIMER_SUPPORTED       1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1
#define SOC_EFUSE_SUPPORTED             1
#define SOC_TEMP_SENSOR_SUPPORTED       1
#define SOC_CACHE_SUPPORT_WRAP          1
#define SOC_RTC_FAST_MEM_SUPPORTED      1
#define SOC_RTC_SLOW_MEM_SUPPORTED      1
#define SOC_RTC_MEM_SUPPORTED           1
#define SOC_RTC_TIMER_V1_SUPPORTED      1
#define SOC_PSRAM_DMA_CAPABLE           1
#define SOC_XT_WDT_SUPPORTED            1
#define SOC_I2S_SUPPORTED               1
#define SOC_I2S_I80_LCD_SUPPORTED       1
#define SOC_LCD_I80_SUPPORTED           1
#define SOC_RMT_SUPPORTED               1
#define SOC_SDM_SUPPORTED               1
#define SOC_GPSPI_SUPPORTED             1
#define SOC_LEDC_SUPPORTED              1
#define SOC_I2C_SUPPORTED               1
#define SOC_SYSTIMER_SUPPORTED          1
#define SOC_SUPPORT_COEXISTENCE         0
#define SOC_AES_SUPPORTED               1
#define SOC_MPI_SUPPORTED               1
#define SOC_SHA_SUPPORTED               1
#define SOC_HMAC_SUPPORTED              1
#define SOC_DIG_SIGN_SUPPORTED          1
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
#define SOC_MEMPROT_SUPPORTED           1
#define SOC_TOUCH_SENSOR_SUPPORTED      1
#define SOC_BOD_SUPPORTED               1
#define SOC_CLK_TREE_SUPPORTED          1
#define SOC_MPU_SUPPORTED               1
#define SOC_WDT_SUPPORTED               1
#define SOC_SPI_FLASH_SUPPORTED         1
#define SOC_RNG_SUPPORTED               1
#define SOC_LIGHT_SLEEP_SUPPORTED       1
#define SOC_DEEP_SLEEP_SUPPORTED        1
#define SOC_LP_PERIPH_SHARE_INTERRUPT   1   // LP peripherals sharing the same interrupt source
#define SOC_PM_SUPPORTED                1
#define SOC_SPI_EXTERNAL_NOR_FLASH_SUPPORTED    1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- ADC CAPS ----------------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_RTC_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_ARBITER_SUPPORTED               1
#define SOC_ADC_DIG_IIR_FILTER_SUPPORTED        1
#define SOC_ADC_DIG_IIR_FILTER_UNIT_BINDED      1    //ADC filter is binded with the ADC unit
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        1    //Digital controller supported ADC unit
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_CHANNEL_NUM(UNIT)               (10)
#define SOC_ADC_MAX_CHANNEL_NUM                 (10)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (2)
#define SOC_ADC_PATT_LEN_MAX                    (32) /*!< Two pattern table, each contains 16 items. Each item takes 1 byte */
#define SOC_ADC_DIGI_MIN_BITWIDTH               (12)
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_IIR_FILTER_NUM             (2)
#define SOC_ADC_DIGI_RESULT_BYTES               (2)
#define SOC_ADC_DIGI_DATA_BYTES_PER_CONV        (2)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interval<= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_RTC_MIN_BITWIDTH                (13)
#define SOC_ADC_RTC_MAX_BITWIDTH                (13)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/
#define SOC_ADC_SELF_HW_CALI_SUPPORTED          (1) /*!< support HW offset self calibration */

/*!< ADC power control is shared by PWDET, TempSensor */
#define SOC_ADC_SHARED_POWER                    1

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_CACHE_WRITEBACK_SUPPORTED           1

/*-------------------------- CP-DMA CAPS -------------------------------------*/
#define SOC_CP_DMA_MAX_BUFFER_SIZE (4095) /*!< Maximum size of the buffer that can be attached to descriptor */

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               (1U)
#define SOC_CPU_INTR_NUM                32

#define SOC_CPU_BREAKPOINTS_NUM             2
#define SOC_CPU_WATCHPOINTS_NUM             2
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x40 // bytes

/*-------------------------- DAC CAPS ----------------------------------------*/
#define SOC_DAC_CHAN_NUM      2
#define SOC_DAC_RESOLUTION      8 // DAC resolution ratio 8 bit

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-S2 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 47
#define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1

// Force hold is a new function of ESP32-S2
#define SOC_GPIO_SUPPORT_FORCE_HOLD      (1)

// 0~46 valid except 22~25
#define SOC_GPIO_VALID_GPIO_MASK             (0x7FFFFFFFFFFFULL & ~(0ULL | BIT22 | BIT23 | BIT24 | BIT25))
// GPIO 46 is input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK     (SOC_GPIO_VALID_GPIO_MASK & ~(0ULL | BIT46))

#define SOC_GPIO_IN_RANGE_MAX           46
#define SOC_GPIO_OUT_RANGE_MAX          45

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_26~GPIO_NUM_46)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK  0x00007FFFFC000000ULL

// The Clock Out signal is binding to the pin's IO_MUX function
#define SOC_GPIO_CLOCKOUT_BY_IO_MUX    (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM  (3)

/*-------------------------- Dedicated GPIO CAPS ---------------------------------------*/
#define SOC_DEDIC_GPIO_HAS_INTERRUPT    (1) /*!< Dedicated GPIO has its own interrupt source */

/*-------------------------- I2C CAPS ----------------------------------------*/
#define SOC_I2C_NUM                             (2U)
#define SOC_HP_I2C_NUM                          (2U)

#define SOC_I2C_SUPPORT_REF_TICK                (1)
#define SOC_I2C_SUPPORT_APB                     (1)

#define SOC_I2C_SUPPORT_SLAVE                   (1)
#define SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE     (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
// ESP32-S2 has 1 I2S
#define SOC_I2S_HW_VERSION_1        (1)
#define SOC_I2S_SUPPORTS_APLL       (1)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_HAS_TIMER_SPECIFIC_MUX  (1)
#define SOC_LEDC_SUPPORT_APB_CLOCK       (1)
#define SOC_LEDC_SUPPORT_REF_TICK        (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK      (1)
#define SOC_LEDC_TIMER_NUM               (4)
#define SOC_LEDC_CHANNEL_NUM             (8)
#define SOC_LEDC_TIMER_BIT_WIDTH         (14)
#define SOC_LEDC_SUPPORT_FADE_STOP       (1)

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM        5
#define SOC_MMU_PERIPH_NUM                       (1U)

/*-------------------------- MPU CAPS ----------------------------------------*/
//TODO: correct the caller and remove unsupported lines
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_MEM_WORDS_PER_CHANNEL         64 /*!< Each channel owns 64 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT         1  /*!< Support transmitting specified number of cycles in loop mode */

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT   22
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED 1  /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                             * so it supports unique IOMUX configuration (including IE, OE, PU, PD, DRV etc.)
                                             * when the pins are switched to RTC function.
                                             */
#define SOC_RTCIO_HOLD_SUPPORTED 1
#define SOC_RTCIO_WAKE_SUPPORTED 1
// LP IO peripherals have independent clock gating to manage
#define SOC_LP_IO_CLOCK_IS_INDEPENDENT 1

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM                  3
#define SOC_SPI_PERIPH_CS_NUM(i)            (((i)==0)? 2: (((i)==1)? 6: 3))
#define SOC_SPI_MAXIMUM_BUFFER_SIZE         72
/// The SPI Slave half duplex mode has been updated greatly in ESP32-S2
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1
#define SOC_SPI_HD_BOTH_INOUT_SUPPORTED     1   //Support enabling MOSI and MISO phases together under Halfduplex mode
#define SOC_SPI_SUPPORT_OCT                 1
#define SOC_SPI_MAX_BITWIDTH(host_id)       ((host_id == 2) ? 1 : 8) // Supported line mode: SPI3: 1, SPI1/2: 1, 2, 4, 8
#define SOC_SPI_SCT_SUPPORTED(host_id)      ((host_id) == 1)

// Peripheral supports output given level during its "dummy phase"
// Only SPI1 supports this feature
#define SOC_MEMSPI_SUPPORT_CONTROL_DUMMY_OUT      1
#define SOC_MEMSPI_IS_INDEPENDENT                 1
#define SOC_MEMSPI_SRC_FREQ_80M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_40M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_26M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_20M_SUPPORTED         1

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM  (1U)  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM    3  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO 32 // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI 32 // Bit width of systimer high part

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part
#define SOC_RTC_TIMER_SUPPORTED             SOC_RTC_TIMER_V1_SUPPORTED

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_VERSION            (2)     /*!< Hardware version of touch sensor */
#define SOC_TOUCH_MIN_CHAN_ID               (1U)     /*!< Touch minimum channel number, (0 is internal denoise channel) */
#define SOC_TOUCH_MAX_CHAN_ID               (14)    /*!< Touch maximum channel number */
#define SOC_TOUCH_SUPPORT_BENCHMARK         (1)     /*!< Touch sensor supports benchmark configuration */
#define SOC_TOUCH_SUPPORT_SLEEP_WAKEUP      (1)     /*!< Touch sensor supports sleep awake */
#define SOC_TOUCH_SUPPORT_WATERPROOF        (1)     /*!< Touch sensor supports waterproof */
#define SOC_TOUCH_SUPPORT_PROX_SENSING      (1)     /*!< Touch sensor supports proximity sensing */
#define SOC_TOUCH_SUPPORT_DENOISE_CHAN      (1)     /*!< Touch sensor supports denoise channel */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM     (3)     /*!< Support touch proximity channel number. */

#define SOC_TOUCH_SAMPLE_CFG_NUM            (1U)    /*!< The sample configuration number in total, each sampler can be used to sample on one frequency */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM         1U
#define SOC_TWAI_MASK_FILTER_NUM        1U

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-S2 has 2 UART.
#define SOC_UART_NUM                (2)
#define SOC_UART_HP_NUM             (2)
#define SOC_UART_SUPPORT_WAKEUP_INT (1)         /*!< Support UART wakeup interrupt */
#define SOC_UART_SUPPORT_APB_CLK    (1)         /*!< Support APB as the clock source */
#define SOC_UART_SUPPORT_REF_TICK   (1)         /*!< Support REF_TICK as the clock source */
#define SOC_UART_FIFO_LEN           (128)       /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)   /*!< Max bit rate supported by UART */

#define SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE (1)

/*-------------------------- SPIRAM CAPS -------------------------------------*/
#define SOC_SPIRAM_SUPPORTED          1
#define SOC_SPIRAM_XIP_SUPPORTED      1

/*-------------------------- USB CAPS ----------------------------------------*/
#define SOC_USB_OTG_PERIPH_NUM          (1U)

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
#define SOC_SHA_SUPPORT_SHA512          (1)
#define SOC_SHA_SUPPORT_SHA512_224      (1)
#define SOC_SHA_SUPPORT_SHA512_256      (1)
#define SOC_SHA_SUPPORT_SHA512_T        (1)

/*--------------------------- MPI CAPS ---------------------------------------*/
#define SOC_MPI_MEM_BLOCKS_NUM (4)
#define SOC_MPI_OPERATIONS_NUM (3)

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)

/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)
#define SOC_AES_SUPPORT_GCM     (1)

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_DOWNLOAD_ICACHE 1
#define SOC_EFUSE_DIS_DOWNLOAD_DCACHE 1
#define SOC_EFUSE_HARD_DIS_JTAG 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1
#define SOC_EFUSE_DIS_BOOT_REMAP 1
#define SOC_EFUSE_DIS_LEGACY_SPI_BOOT 1
#define SOC_EFUSE_DIS_ICACHE 1
#define SOC_EFUSE_XTS_AES_KEY_128 1
#define SOC_EFUSE_XTS_AES_KEY_256 1

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   1
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  1

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)
#define SOC_FLASH_ENCRYPTION_XTS_AES        1
#define SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS 1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128    1
#define SOC_FLASH_ENCRYPTION_XTS_AES_256    1

/*-------------------------- MEMPROT CAPS ------------------------------------*/
#define SOC_MEMPROT_CPU_PREFETCH_PAD_SIZE   16
#define SOC_MEMPROT_MEM_ALIGN_SIZE          4

/* Has "crypto DMA", which is shared with SHA */
#define SOC_AES_CRYPTO_DMA      (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_192 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE          (1)
#define SOC_SPI_MEM_SUPPORT_WRAP                          (1)

/*-------------------------- Power Management CAPS ---------------------------*/
#define SOC_PM_SUPPORT_EXT0_WAKEUP                (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP                (1)
#define SOC_PM_SUPPORT_EXT_WAKEUP                 (1)     /*!<Compatible to the old version of IDF */
#define SOC_PM_SUPPORT_WIFI_WAKEUP                (1)
#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP        (1)     /*!<Supports waking up from touch pad trigger */

#define SOC_PM_SUPPORT_WIFI_PD                    (1)
#define SOC_PM_SUPPORT_RTC_PERIPH_PD              (1)
#define SOC_PM_SUPPORT_RTC_FAST_MEM_PD            (1)
#define SOC_PM_SUPPORT_RTC_SLOW_MEM_PD            (1)
#define SOC_PM_SUPPORT_RC_FAST_PD                 (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD                 (1)

#define SOC_CONFIGURABLE_VDDSDIO_SUPPORTED        (1)
#define SOC_PM_MODEM_PD_BY_SW                     (1)

/*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
#define SOC_CLK_APLL_SUPPORTED                    (1)

#define SOC_CLK_RC_FAST_D256_SUPPORTED            (1)
#define SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256     (1)
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */

#define SOC_CLK_LP_FAST_SUPPORT_XTAL_D4           (1)     /*!< Support XTAL_D4 clock as the LP_FAST clock source */

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)
/* ---------------------------- Compatibility ------------------------------- */
// No contents

/*-------------------------- EXTERNAL COEXISTENCE CAPS -------------------------------------*/
#define SOC_EXTERNAL_COEX_ADVANCE              (0) /*!< HARDWARE ADVANCED EXTERNAL COEXISTENCE CAPS */
#define SOC_EXTERNAL_COEX_LEADER_TX_LINE       (1) /*!< EXTERNAL COEXISTENCE TX LINE CAPS */

/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC                (1)

/*------------------------------------ WI-FI CAPS ------------------------------------*/
#define SOC_WIFI_HW_TSF                     (1)    /*!< Support hardware TSF */
#define SOC_WIFI_FTM_SUPPORT                (1)    /*!< Support FTM */
#define SOC_WIFI_WAPI_SUPPORT               (1)    /*!< Support WAPI */
#define SOC_WIFI_CSI_SUPPORT                (1)    /*!< Support CSI */
#define SOC_WIFI_MESH_SUPPORT               (1)    /*!< Support WIFI MESH */
#define SOC_WIFI_SUPPORT_VARIABLE_BEACON_WINDOW   (1)    /*!< Support delta early time for rf phy on/off */
#define SOC_WIFI_NAN_SUPPORT                (1)    /*!< Support WIFI Aware (NAN) */
/*-------------------------- ULP CAPS ----------------------------------------*/
#define SOC_ULP_HAS_ADC                     (1)    /* ADC can be accessed from ULP */

/*------------------------------------- PHY CAPS -------------------------------------*/
#define SOC_PHY_COMBO_MODULE                  (0) /*!< Only support Wi-Fi*/
