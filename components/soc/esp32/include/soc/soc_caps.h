
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
 * For ECO-ed booleans, `#define x "Not determined"` for them. This will cause error when used by
 * `#if x` and `#if !x`, making these missing definitions more obvious.
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

#ifdef __has_include
#  if __has_include("sdkconfig.h")
#    include "sdkconfig.h"
#    define SOC_CAPS_ECO_VER    CONFIG_ESP32_REV_MIN_FULL
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
#define SOC_BROWNOUT_RESET_SUPPORTED    "Not determined" // [gen_soc_caps:ignore]
#define SOC_DPORT_WORKAROUND            "Not determined" // [gen_soc_caps:ignore]
#endif

#define _SOC_CAPS_TARGET_IS_ESP32 1 // [gen_soc_caps:ignore]

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_CAPS_ECO_VER_MAX        301

#define SOC_ADC_SUPPORTED           1
#define SOC_DAC_SUPPORTED           1
#define SOC_UART_SUPPORTED          1
#define SOC_MCPWM_SUPPORTED         1
#define SOC_GPTIMER_SUPPORTED       1
#define SOC_SDMMC_HOST_SUPPORTED    1
#define SOC_BT_SUPPORTED            1
#define SOC_PCNT_SUPPORTED          1
#define SOC_PHY_SUPPORTED           1
#define SOC_WIFI_SUPPORTED          1
#define SOC_SDIO_SLAVE_SUPPORTED    1
#define SOC_TWAI_SUPPORTED          1
#define SOC_EFUSE_SUPPORTED         1
#define SOC_EMAC_SUPPORTED          1
#define SOC_ULP_SUPPORTED           1
#define SOC_CCOMP_TIMER_SUPPORTED   1
#define SOC_RTC_FAST_MEM_SUPPORTED  1
#define SOC_RTC_SLOW_MEM_SUPPORTED  1
#define SOC_RTC_MEM_SUPPORTED       1
#define SOC_RTC_TIMER_V1_SUPPORTED  1
#define SOC_I2S_SUPPORTED           1
#define SOC_I2S_I80_LCD_SUPPORTED   1
#define SOC_LCD_I80_SUPPORTED       1
#define SOC_RMT_SUPPORTED           1
#define SOC_SDM_SUPPORTED           1
#define SOC_GPSPI_SUPPORTED         1
#define SOC_LEDC_SUPPORTED          1
#define SOC_I2C_SUPPORTED           1
#define SOC_SUPPORT_COEXISTENCE     1
#define SOC_AES_SUPPORTED           1
#define SOC_MPI_SUPPORTED           1
#define SOC_SHA_SUPPORTED           1
#define SOC_FLASH_ENC_SUPPORTED     1
#define SOC_SECURE_BOOT_SUPPORTED   1
#define SOC_TOUCH_SENSOR_SUPPORTED  1
#define SOC_BOD_SUPPORTED           1
#define SOC_ULP_FSM_SUPPORTED       1
#define SOC_CLK_TREE_SUPPORTED      1
#define SOC_MPU_SUPPORTED           1
#define SOC_WDT_SUPPORTED           1
#define SOC_SPI_FLASH_SUPPORTED     1
#define SOC_RNG_SUPPORTED           1
#define SOC_LIGHT_SLEEP_SUPPORTED   1
#define SOC_DEEP_SLEEP_SUPPORTED    1
#define SOC_LP_PERIPH_SHARE_INTERRUPT   1   // LP peripherals sharing the same interrupt source
#define SOC_PM_SUPPORTED            1
#define SOC_SPI_EXTERNAL_NOR_FLASH_SUPPORTED    1

#if SOC_CAPS_ECO_VER < 200
#define SOC_DPORT_WORKAROUND                   1  // [gen_soc_caps:ignore]
#endif // SOC_CAPS_ECO_VER < 200
#define SOC_DPORT_WORKAROUND_DIS_INTERRUPT_LVL (5U)

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_26M            1
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- ADC CAPS ----------------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_RTC_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        ((UNIT == 0) ? 1 : 0)
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         ((PERIPH_NUM==0)? 8: 10)
#define SOC_ADC_MAX_CHANNEL_NUM                 (10)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (2)
#define SOC_ADC_PATT_LEN_MAX                    (16) //Two pattern table, each contains 16 items. Each item takes 1 byte. But only support ADC1 using DMA mode
#define SOC_ADC_DIGI_MIN_BITWIDTH               (9)
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_RESULT_BYTES               (2)
#define SOC_ADC_DIGI_DATA_BYTES_PER_CONV        (4)
#define SOC_ADC_DIGI_MONITOR_NUM                (0U) // to reference `IDF_TARGET_SOC_ADC_DIGI_MONITOR_NUM` in document
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          (2000000)
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           (20000)

/*!< RTC */
#define SOC_ADC_RTC_MIN_BITWIDTH                (9)
#define SOC_ADC_RTC_MAX_BITWIDTH                (12)

/*!< ADC power control is shared by PWDET */
#define SOC_ADC_SHARED_POWER                    1

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#if SOC_CAPS_ECO_VER >= 100
#define SOC_BROWNOUT_RESET_SUPPORTED 1
#endif

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data within one core
#define SOC_IDCACHE_PER_CORE                    1   //Independent Cache unit pre core

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               2
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FPU                 1
#define SOC_HP_CPU_HAS_MULTIPLE_CORES   1   // Convenience boolean macro used to determine if a target has multiple cores.

#define SOC_CPU_BREAKPOINTS_NUM             2
#define SOC_CPU_WATCHPOINTS_NUM             2
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x40 // bytes

/*-------------------------- DAC CAPS ----------------------------------------*/
#define SOC_DAC_CHAN_NUM              2
#define SOC_DAC_RESOLUTION              8 // DAC resolution ratio 8 bit
#define SOC_DAC_DMA_16BIT_ALIGN         1 // The DMA data should left shift 8 bit to be aligned with 16 bit

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32 has 1 GPIO peripheral
#define SOC_GPIO_PORT                   (1U)
#define SOC_GPIO_PIN_COUNT              40

// 0~39 valid except 24, 28~31
#define SOC_GPIO_VALID_GPIO_MASK        (0xFFFFFFFFFFULL & ~(0ULL | BIT24 | BIT28 | BIT29 | BIT30 | BIT31))
// GPIO >= 34 are input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK (SOC_GPIO_VALID_GPIO_MASK & ~(0ULL | BIT34 | BIT35 | BIT36 | BIT37 | BIT38 | BIT39))

#define SOC_GPIO_IN_RANGE_MAX           39
#define SOC_GPIO_OUT_RANGE_MAX          33

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM: 1, 3, 5, 6, 7, 8, 9, 10, 11, 16, 17, 18, 19, 21, 22, 23)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0xEF0FEAULL

// The Clock Out signal is binding to the pin's IO_MUX function
#define SOC_GPIO_CLOCKOUT_BY_IO_MUX    (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM  (3)

/*-------------------------- I2C CAPS ----------------------------------------*/
#define SOC_I2C_NUM                (2U)
#define SOC_HP_I2C_NUM             (2U)

#define SOC_I2C_SUPPORT_APB        (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR (1)

#define SOC_I2C_SUPPORT_SLAVE      (1)

// On ESP32, the stop bit should be independent, we can't put trans data and stop command together
#define SOC_I2C_STOP_INDEPENDENT   (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
// ESP32 has 2 I2S
#define SOC_I2S_HW_VERSION_1        (1)
#define SOC_I2S_SUPPORTS_APLL       (1)
#define SOC_I2S_SUPPORTS_PDM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)     // Support to output raw PDM format data
#define SOC_I2S_SUPPORTS_PCM2PDM    (1)     // Support to write PCM format but output PDM format data with the help of PCM to PDM filter
#define SOC_I2S_SUPPORTS_PDM_RX     (1)     // Support to input raw PDM format data
#define SOC_I2S_SUPPORTS_PDM2PCM    (1)     // Support to input PDM format but read PCM format data with the help of PDM to PCM filter
#define SOC_I2S_PDM_MAX_TX_LINES    (1U)
#define SOC_I2S_PDM_MAX_RX_LINES    (1U)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_HAS_TIMER_SPECIFIC_MUX  (1)
#define SOC_LEDC_SUPPORT_APB_CLOCK       (1)
#define SOC_LEDC_SUPPORT_REF_TICK        (1)
#define SOC_LEDC_SUPPORT_HS_MODE         (1)
#define SOC_LEDC_TIMER_NUM               (4)
#define SOC_LEDC_CHANNEL_NUM             (8)
#define SOC_LEDC_TIMER_BIT_WIDTH         (20)

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PERIPH_NUM                      2
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM       3

/*-------------------------- MPU CAPS ----------------------------------------*/
//TODO: correct the caller and remove unsupported lines
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_MEM_WORDS_PER_CHANNEL   64 /*!< Each channel owns 64 words memory */

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT 18
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED 1
#define SOC_RTCIO_HOLD_SUPPORTED 1
#define SOC_RTCIO_WAKE_SUPPORTED 1

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM              3
#define SOC_SPI_PERIPH_CS_NUM(i)        3

#define SOC_SPI_HD_BOTH_INOUT_SUPPORTED 1  //Support enabling MOSI and MISO phases together under Halfduplex mode
#define SOC_SPI_MAXIMUM_BUFFER_SIZE     64
#define SOC_SPI_MAX_BITWIDTH(host_id)   (4) // Supported line mode: DIO, DOUT, QIO, or QOUT

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part
#define SOC_RTC_TIMER_SUPPORTED             SOC_RTC_TIMER_V1_SUPPORTED

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_VERSION            (1U)     /*!<Hardware version of touch sensor */
#define SOC_TOUCH_MIN_CHAN_ID               (0U)    /*!< Touch minimum channel number */
#define SOC_TOUCH_MAX_CHAN_ID               (9)     /*!< Touch maximum channel number */
#define SOC_TOUCH_SUPPORT_SLEEP_WAKEUP      (1)
#define SOC_TOUCH_SAMPLE_CFG_NUM            (1U)    /*!< The sample configuration number in total, each sampler can be used to sample on one frequency */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM         1U
#define SOC_TWAI_MASK_FILTER_NUM        1U

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32 have 3 UART.
#define SOC_UART_NUM                (3)
#define SOC_UART_HP_NUM             (3)
#define SOC_UART_SUPPORT_APB_CLK    (1)         /*!< Support APB as the clock source */
#define SOC_UART_SUPPORT_REF_TICK   (1)         /*!< Support REF_TICK as the clock source */
#define SOC_UART_FIFO_LEN           (128)       /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)   /*!< Max bit rate supported by UART */

#define SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE (1)

/*-------------------------- SPIRAM CAPS -------------------------------------*/
#define SOC_SPIRAM_SUPPORTED    1

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE         (1)

/*--------------------------- SHA CAPS ---------------------------------------*/
/* ESP32 style SHA engine, where multiple states can be stored in parallel */
#define SOC_SHA_SUPPORT_PARALLEL_ENG    (1)

/* ESP32's SHA peripheral processes and stores data in big-endian format */
#define SOC_SHA_ENDIANNESS_BE           (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)

/*--------------------------- MPI CAPS ---------------------------------------*/
#define SOC_MPI_MEM_BLOCKS_NUM (4)
#define SOC_MPI_OPERATIONS_NUM (1U)

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)

/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_192 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- Secure Boot CAPS----------------------------*/
/*
 * ESP32 ECO3 revision also supports `SOC_SECURE_BOOT_V2_RSA` but for that we will need selected ECO
 * version from `Kconfig`, which is not possible while generating `Kconfig.soc_caps.in` from this header.
 * Hence, for now we are handling this special capability in bootloader "security" configuration itself.
 */
#define SOC_SECURE_BOOT_V1                  1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   (1U)

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (32)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

/*-------------------------- Power Management CAPS ---------------------------*/
#define SOC_PM_SUPPORT_EXT0_WAKEUP                (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP                (1)
#define SOC_PM_SUPPORT_EXT_WAKEUP                 (1)     /*!<Compatible to the old version of IDF */

#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP        (1)     /*!<Supports waking up from touch pad trigger */
#define SOC_PM_SUPPORT_RTC_PERIPH_PD              (1)
#define SOC_PM_SUPPORT_RTC_FAST_MEM_PD            (1)
#define SOC_PM_SUPPORT_RTC_SLOW_MEM_PD            (1)
#define SOC_PM_SUPPORT_RC_FAST_PD                 (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD                 (1)
#define SOC_PM_SUPPORT_MODEM_PD                   (1)     /*!<Modem here includes wifi and btdm */

#define SOC_CONFIGURABLE_VDDSDIO_SUPPORTED        (1)
#define SOC_PM_MODEM_PD_BY_SW                     (1)
/*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
#define SOC_CLK_APLL_SUPPORTED                    (1)

#define SOC_CLK_RC_FAST_D256_SUPPORTED            (1)
#define SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256     (1)
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */

#define SOC_CLK_LP_FAST_SUPPORT_XTAL_D4           (1)     /*!< Support XTAL_D4 clock as the LP_FAST clock source */

/*-------------------------- SDMMC CAPS -----------------------------------------*/

/* On ESP32, clock/cmd/data pins use IO MUX.
 * Card detect, write protect, interrupt use GPIO Matrix on all chips.
 */
#define SOC_SDMMC_USE_IOMUX          1
#define SOC_SDMMC_NUM_SLOTS          2
#define SOC_SDMMC_DATA_WIDTH_MAX     8

/*-------------------------- WI-FI HARDWARE CAPS -------------------------------*/
#define SOC_WIFI_WAPI_SUPPORT                   (1)    /*!< Support WAPI */
#define SOC_WIFI_CSI_SUPPORT                    (1)    /*!< Support CSI */
#define SOC_WIFI_MESH_SUPPORT                   (1)    /*!< Support WIFI MESH */
#define SOC_WIFI_SUPPORT_VARIABLE_BEACON_WINDOW (1)    /*!< Support delta early time for rf phy on/off */
#define SOC_WIFI_NAN_SUPPORT                    (1)    /*!< Support WIFI Aware (NAN) */

/*---------------------------------- Bluetooth CAPS ----------------------------------*/
#define SOC_BLE_SUPPORTED               (1)    /*!< Support Bluetooth Low Energy hardware */
#define SOC_BLE_MESH_SUPPORTED          (1)    /*!< Support BLE MESH */
#define SOC_BT_CLASSIC_SUPPORTED        (1)    /*!< Support Bluetooth Classic hardware */
#define SOC_BLE_DEVICE_PRIVACY_SUPPORTED (0)   /*!< Support BLE device privacy mode */
#define SOC_BLUFI_SUPPORTED             (1)    /*!< Support BLUFI */
#define SOC_BT_H2C_ENC_KEY_CTRL_ENH_VSC_SUPPORTED (1) /*!< Support Bluetooth Classic encryption key size configuration through vendor-specific HCI command */
#define SOC_BLE_MULTI_CONN_OPTIMIZATION (1)    /*!< Support multiple connections optimization */

/*-------------------------- ULP CAPS ----------------------------------------*/
#define SOC_ULP_HAS_ADC                     (1)    /* ADC can be accessed from ULP */

/*------------------------------------- PHY CAPS -------------------------------------*/
#define SOC_PHY_COMBO_MODULE                  (1) /*!< Support Wi-Fi, BT and BLE*/

/*--------------------------- EMAC --------------------------------*/
#define SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK     (1) /*!< REF RMII CLK output is looped back internally */
