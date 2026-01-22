/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

/*
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

#define _SOC_CAPS_TARGET_IS_ESP32P4     1 // [gen_soc_caps:ignore]

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_ADC_SUPPORTED               1
#define SOC_ANA_CMPR_SUPPORTED          1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_UART_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_UHCI_SUPPORTED              1
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_AXI_GDMA_SUPPORTED          1
#define SOC_DW_GDMA_SUPPORTED           1
#define SOC_DMA2D_SUPPORTED             1
#define SOC_GPTIMER_SUPPORTED           1
#define SOC_PCNT_SUPPORTED              1
#define SOC_LCDCAM_CAM_SUPPORTED        1
#define SOC_LCDCAM_I80_LCD_SUPPORTED    1
#define SOC_LCDCAM_RGB_LCD_SUPPORTED    1
#define SOC_LCD_I80_SUPPORTED           1
#define SOC_LCD_RGB_SUPPORTED           1
#define SOC_MIPI_CSI_SUPPORTED          1
#define SOC_MIPI_DSI_SUPPORTED          1
#define SOC_MCPWM_SUPPORTED             1
#define SOC_TWAI_SUPPORTED              1
#define SOC_ETM_SUPPORTED               1
#define SOC_PARLIO_SUPPORTED            1
#define SOC_PARLIO_LCD_SUPPORTED        1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_EMAC_SUPPORTED              1
#define SOC_USB_OTG_SUPPORTED           1
#define SOC_WIRELESS_HOST_SUPPORTED     1
#define SOC_USB_SERIAL_JTAG_SUPPORTED   1
#define SOC_TEMP_SENSOR_SUPPORTED       1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_ULP_SUPPORTED               1
#define SOC_LP_CORE_SUPPORTED           1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1
#define SOC_EFUSE_SUPPORTED             1
#define SOC_RTC_FAST_MEM_SUPPORTED      1
#define SOC_RTC_MEM_SUPPORTED           1
#define SOC_RMT_SUPPORTED               1
#define SOC_I2S_SUPPORTED               1
#define SOC_SDM_SUPPORTED               1
#define SOC_GPSPI_SUPPORTED             1
#define SOC_LEDC_SUPPORTED              1
#define SOC_ISP_SUPPORTED               1
#define SOC_I2C_SUPPORTED               1
#define SOC_SYSTIMER_SUPPORTED          1
#define SOC_AES_SUPPORTED               1
#define SOC_MPI_SUPPORTED               1
#define SOC_SHA_SUPPORTED               1
#define SOC_HMAC_SUPPORTED              1
#define SOC_DIG_SIGN_SUPPORTED          1
#define SOC_ECC_SUPPORTED               1
#define SOC_ECC_EXTENDED_MODES_SUPPORTED   1
#define SOC_ECDSA_SUPPORTED             1
#define SOC_KEY_MANAGER_SUPPORTED       1
#define SOC_HUK_SUPPORTED               1
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
#define SOC_BOD_SUPPORTED               1
#define SOC_VBAT_SUPPORTED              1
#define SOC_APM_SUPPORTED               1
#define SOC_PMU_SUPPORTED               1
#define SOC_PMU_PVT_SUPPORTED           1
#define SOC_PVT_EN_WITH_SLEEP           1
#define SOC_PVT_RETENTION_BY_REGDMA     1
#define SOC_DCDC_SUPPORTED              1
#define SOC_PAU_SUPPORTED               1     //TODO: IDF-7531
#define SOC_RTC_TIMER_V2_SUPPORTED      1
#define SOC_ULP_LP_UART_SUPPORTED       1
#define SOC_LP_GPIO_MATRIX_SUPPORTED    1
#define SOC_LP_PERIPHERALS_SUPPORTED    1
#define SOC_LP_I2C_SUPPORTED            1
#define SOC_LP_I2S_SUPPORTED            1
#define SOC_LP_SPI_SUPPORTED            1
#define SOC_LP_ADC_SUPPORTED            1
#define SOC_LP_VAD_SUPPORTED            1
#define SOC_LP_MAILBOX_SUPPORTED        1
#define SOC_SPIRAM_SUPPORTED            1
#define SOC_PSRAM_DMA_CAPABLE           1
#define SOC_SDMMC_HOST_SUPPORTED        1
#define SOC_CLK_TREE_SUPPORTED          1
#define SOC_ASSIST_DEBUG_SUPPORTED      1
#define SOC_DEBUG_PROBE_SUPPORTED       1
#define SOC_WDT_SUPPORTED               1
#define SOC_SPI_FLASH_SUPPORTED         1
#define SOC_TOUCH_SENSOR_SUPPORTED      1
#define SOC_RNG_SUPPORTED               1
#define SOC_GP_LDO_SUPPORTED            1 // General purpose LDO
#define SOC_PPA_SUPPORTED               1
#define SOC_LIGHT_SLEEP_SUPPORTED       1
#define SOC_DEEP_SLEEP_SUPPORTED        1
#define SOC_PM_SUPPORTED                1
#define SOC_BITSCRAMBLER_SUPPORTED      1
#define SOC_SIMD_INSTRUCTION_SUPPORTED  1
#define SOC_I3C_MASTER_SUPPORTED        1
#define SOC_SPI_EXTERNAL_NOR_FLASH_SUPPORTED    1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)
#define SOC_AES_SUPPORT_GCM     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GDMA            (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

#define SOC_AES_SUPPORT_PSEUDO_ROUND_FUNCTION (1)

/*-------------------------- ADC CAPS -------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_RTC_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
// #define SOC_ADC_DIG_IIR_FILTER_SUPPORTED        1
// #define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        1    //Digital controller supported ADC unit
#define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         ((PERIPH_NUM==0)? 8: 6)
#define SOC_ADC_MAX_CHANNEL_NUM                 (8)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (2)
#define SOC_ADC_PATT_LEN_MAX                    (16) /*!< Four pattern tables, each contains 4 items. Each item takes 1 byte */
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_MIN_BITWIDTH               (12)
#define SOC_ADC_DIGI_IIR_FILTER_NUM             (2)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)
#define SOC_ADC_DIGI_RESULT_BYTES               (4)
#define SOC_ADC_DIGI_DATA_BYTES_PER_CONV        (4)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interval <= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_RTC_MIN_BITWIDTH                (12)
#define SOC_ADC_RTC_MAX_BITWIDTH                (12)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/
#define SOC_ADC_SELF_HW_CALI_SUPPORTED          (1) /*!< support HW offset self calibration */
#define SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED    (1) /*!< support channel compensation to the HW offset calibration */

/*!< ADC power control is shared by PWDET, TempSensor */
#define SOC_ADC_SHARED_POWER                    1

// ESP32P4-TODO: Copy from esp32c6, need check
/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (0)

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data
#define SOC_CACHE_WRITEBACK_SUPPORTED           1
#define SOC_CACHE_FREEZE_SUPPORTED              1
#define SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE      1

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               (2U)
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FLEXIBLE_INTC       1
#define SOC_INT_CLIC_SUPPORTED          1
#define SOC_INT_HW_NESTED_SUPPORTED     1       // Support for hardware interrupts nesting
#define SOC_BRANCH_PREDICTOR_SUPPORTED  1
#define SOC_CPU_COPROC_NUM              3
#define SOC_CPU_HAS_FPU                 1
#define SOC_CPU_HAS_FPU_EXT_ILL_BUG     1       // EXT_ILL CSR doesn't support FLW/FSW
#define SOC_CPU_HAS_HWLOOP              1
#define SOC_CPU_HAS_HWLOOP_STATE_BUG    1       // HWLOOP state doesn't go to DIRTY after executing the last instruction of a loop
/* PIE coprocessor assembly is only supported with GCC compiler  */
#define SOC_CPU_HAS_PIE                 1

#define SOC_HP_CPU_HAS_MULTIPLE_CORES   1   // Convenience boolean macro used to determine if a target has multiple cores.

#define SOC_CPU_BREAKPOINTS_NUM             3
#define SOC_CPU_WATCHPOINTS_NUM             3
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x100   // bytes

#define SOC_CPU_HAS_PMA                 1
#define SOC_CPU_IDRAM_SPLIT_USING_PMP   1
#define SOC_CPU_PMP_REGION_GRANULARITY  128

#define SOC_CPU_HAS_LOCKUP_RESET        1

#define SOC_CPU_HAS_ZC_EXTENSIONS       1

#define SOC_CPU_ZCMP_WORKAROUND         1
#define SOC_CPU_ZCMP_PUSH_REVERSED      1
#define SOC_CPU_ZCMP_POPRET_ISSUE       1

#define SOC_SIMD_PREFERRED_DATA_ALIGNMENT 16 // The preferred data alignment accepted by the SIMD instructions, in bytes

/*-------------------------- DIGITAL SIGNATURE CAPS ----------------------------------------*/
/** The maximum length of a Digital Signature in bits. */
#define SOC_DS_SIGNATURE_MAX_BIT_LEN (4096)

/** Initialization vector (IV) length for the RSA key parameter message digest (MD) in bytes. */
#define SOC_DS_KEY_PARAM_MD_IV_LENGTH (16)

/** Maximum wait time for DS parameter decryption key. If overdue, then key error.
    See TRM DS chapter for more details */
#define SOC_DS_KEY_CHECK_MAX_WAIT_US (1100)

/*-------------------------- DMA Common CAPS ----------------------------------------*/
#define SOC_DMA_CAN_ACCESS_FLASH 1 /*!< DMA can access Flash memory */

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_AHB_GDMA_VERSION                2
#define SOC_GDMA_SUPPORT_CRC                1
#define SOC_GDMA_SUPPORT_ETM                1
#define SOC_GDMA_SUPPORT_SLEEP_RETENTION    1
#define SOC_GDMA_EXT_MEM_ENC_ALIGNMENT   (16)

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-P4 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 55
#define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1
#define SOC_GPIO_FLEX_GLITCH_FILTER_NUM    8
#define SOC_GPIO_SUPPORT_PIN_HYS_FILTER    1

// GPIO peripheral has the ETM extension
#define SOC_GPIO_SUPPORT_ETM          1

// Target has the full LP IO subsystem

// GPIO0~15 on ESP32P4 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP   (1)
#define SOC_LP_IO_HAS_INDEPENDENT_WAKEUP_SOURCE   (1)

// LP IO peripherals have independent clock gating to manage
#define SOC_LP_IO_CLOCK_IS_INDEPENDENT      (1)

#define SOC_GPIO_VALID_GPIO_MASK        (0x007FFFFFFFFFFFFF)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#define SOC_GPIO_IN_RANGE_MAX           54
#define SOC_GPIO_OUT_RANGE_MAX          54

#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | 0xFFFF)
#define SOC_GPIO_DEEP_SLEEP_WAKE_SUPPORTED_PIN_CNT      (16)

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_16~GPIO_NUM_54)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x007FFFFFFFFF0000ULL

// Support to force hold all IOs
#define SOC_GPIO_SUPPORT_FORCE_HOLD              (1)
// Support to hold a single digital I/O when the digital domain is powered off
#define SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP  (1)    // Supported only on ESP32P4 rev >= 3.0 (see DIG-399)

// The Clock Out signal is route to the pin by GPIO matrix
#define SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX         (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM            (2)
#define SOC_CLOCKOUT_SUPPORT_CHANNEL_DIVIDER     (1)

#define SOC_DEBUG_PROBE_NUM_UNIT                 (1U)  // Number of debug probe units
#define SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH         (16) // Maximum width of the debug probe output in each unit

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT                 16
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED    1   /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                                 * so it supports unique IOMUX configuration (including IE, OE, PU, PD, DRV etc.)
                                                 * when the pins are switched to RTC function.
                                                 */
#define SOC_RTCIO_HOLD_SUPPORTED            1
#define SOC_RTCIO_WAKE_SUPPORTED            1

/*-------------------------- Sigma Delta Modulator CAPS -----------------*/
#define SOC_SDM_SUPPORT_SLEEP_RETENTION 1

/*-------------------------- ETM CAPS -----------------------------------*/
#define SOC_ETM_SUPPORT_SLEEP_RETENTION 1

/*------------------------- Analog Comparator CAPS ---------------------------*/
#define SOC_ANA_CMPR_NUM                       (2)
#define SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE      (1)  // Support positive/negative/any cross interrupt
#define SOC_ANA_CMPR_SUPPORT_ETM               (1)

/*-------------------------- I2C CAPS ----------------------------------------*/
#define SOC_I2C_NUM                             (3U)
#define SOC_HP_I2C_NUM                          (2U)
#define SOC_LP_I2C_NUM                          (1U)

#define SOC_I2C_SUPPORT_XTAL                    (1)
#define SOC_I2C_SUPPORT_RTC                     (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR              (1)

#define SOC_I2C_SUPPORT_SLAVE                   (1)
#define SOC_I2C_SLAVE_SUPPORT_BROADCAST         (1)
#define SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE     (1)

#define SOC_I2C_SUPPORT_SLEEP_RETENTION         (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_HW_VERSION_2        (1)
#define SOC_I2S_SUPPORTS_ETM        (1)
#define SOC_I2S_SUPPORTS_APLL       (1)
#define SOC_I2S_SUPPORTS_PCM        (1)
#define SOC_I2S_SUPPORTS_PDM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)     // Support to output raw PDM format data
#define SOC_I2S_SUPPORTS_PCM2PDM    (1)     // Support to write PCM format but output PDM format data with the help of PCM to PDM filter
#define SOC_I2S_SUPPORTS_PDM_RX     (1)     // Support to input raw PDM format data
#define SOC_I2S_SUPPORTS_PDM2PCM    (1)     // Support to input PDM format but read PCM format data with the help of PDM to PCM filter
#define SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER (1)
#define SOC_I2S_SUPPORTS_TX_SYNC_CNT      (1)
#define SOC_I2S_SUPPORTS_TDM        (1)
#define SOC_I2S_PDM_MAX_TX_LINES    (2)     // On I2S0
#define SOC_I2S_PDM_MAX_RX_LINES    (4)     // On I2S0

/*-------------------------- LP_I2S CAPS -------------------------------------*/
#define SOC_LP_I2S_NUM              (1U)

/*-------------------------- ISP CAPS ----------------------------------------*/
#define SOC_ISP_BF_SUPPORTED                     1
#define SOC_ISP_BLC_SUPPORTED                    1
#define SOC_ISP_CCM_SUPPORTED                    1
#define SOC_ISP_COLOR_SUPPORTED                  1
#define SOC_ISP_CROP_SUPPORTED                   1
#define SOC_ISP_DEMOSAIC_SUPPORTED               1
#define SOC_ISP_DVP_SUPPORTED                    1
#define SOC_ISP_LSC_SUPPORTED                    1
#define SOC_ISP_SHARPEN_SUPPORTED                1
#define SOC_ISP_WBG_SUPPORTED                    1
#define SOC_ISP_SHARE_CSI_BRG                    1
#define SOC_ISP_AE_BLOCK_X_NUMS                  5
#define SOC_ISP_AE_BLOCK_Y_NUMS                  5
#define SOC_ISP_AF_WINDOW_NUMS                   3
#define SOC_ISP_AWB_WINDOW_X_NUMS                5
#define SOC_ISP_AWB_WINDOW_Y_NUMS                5
#define SOC_ISP_BF_TEMPLATE_X_NUMS               3
#define SOC_ISP_BF_TEMPLATE_Y_NUMS               3
#define SOC_ISP_CCM_DIMENSION                    3
#define SOC_ISP_DEMOSAIC_GRAD_RATIO_INT_BITS     2
#define SOC_ISP_DEMOSAIC_GRAD_RATIO_DEC_BITS     4
#define SOC_ISP_DEMOSAIC_GRAD_RATIO_RES_BITS     26
#define SOC_ISP_SHARPEN_TEMPLATE_X_NUMS          3
#define SOC_ISP_SHARPEN_TEMPLATE_Y_NUMS          3
#define SOC_ISP_SHARPEN_H_FREQ_COEF_INT_BITS     3
#define SOC_ISP_SHARPEN_H_FREQ_COEF_DEC_BITS     5
#define SOC_ISP_SHARPEN_H_FREQ_COEF_RES_BITS     24
#define SOC_ISP_SHARPEN_M_FREQ_COEF_INT_BITS     3
#define SOC_ISP_SHARPEN_M_FREQ_COEF_DEC_BITS     5
#define SOC_ISP_SHARPEN_M_FREQ_COEF_RES_BITS     24
#define SOC_ISP_HIST_BLOCK_X_NUMS                5
#define SOC_ISP_HIST_BLOCK_Y_NUMS                5
#define SOC_ISP_HIST_SEGMENT_NUMS                16
#define SOC_ISP_HIST_INTERVAL_NUMS               15
#define SOC_ISP_LSC_GRAD_RATIO_INT_BITS          2
#define SOC_ISP_LSC_GRAD_RATIO_DEC_BITS          8
#define SOC_ISP_LSC_GRAD_RATIO_RES_BITS          22

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK      (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK         (1)
#define SOC_LEDC_TIMER_NUM                  (4)
#define SOC_LEDC_CHANNEL_NUM                (8)
#define SOC_LEDC_TIMER_BIT_WIDTH            (20)
#define SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED (1)
#define SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX (16)
#define SOC_LEDC_SUPPORT_FADE_STOP          (1)
#define SOC_LEDC_FADE_PARAMS_BIT_WIDTH      (10)
#define SOC_LEDC_SUPPORT_SLEEP_RETENTION    (1)
#define SOC_LEDC_SUPPORT_ETM                (1)

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PERIPH_NUM                    (2U)
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM     (2U)
#define SOC_MMU_DI_VADDR_SHARED               (1) /*!< D/I vaddr are shared */
#define SOC_MMU_PER_EXT_MEM_TARGET            (1) /*!< MMU is per physical external memory target (flash, psram) */

/*-------------------------- MPU CAPS ----------------------------------------*/
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- PCNT CAPS ---------------------------------------*/
#define SOC_PCNT_SUPPORT_RUNTIME_THRES_UPDATE 1
#define SOC_PCNT_SUPPORT_CLEAR_SIGNAL         1  /*!< Support clear signal input */

/*--------------------------- RMT CAPS ---------------------------------------*/
#define SOC_RMT_MEM_WORDS_PER_CHANNEL         48 /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_PINGPONG           1  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT         1  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP     1  /*!< Hardware support of auto-stop in loop mode */
#define SOC_RMT_SUPPORT_DMA                   1  /*!< RMT peripheral can connect to DMA channel */
#define SOC_RMT_SUPPORT_SLEEP_RETENTION       1  /*!< The sleep retention feature can help back up RMT registers before sleep */

/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_SWSYNC_CAN_PROPAGATE       (1)    ///< Software sync event can be routed to its output
#define SOC_MCPWM_SUPPORT_ETM                (1)    ///< Support ETM (Event Task Matrix)
#define SOC_MCPWM_SUPPORT_EVENT_COMPARATOR   (1)    ///< Support event comparator (based on ETM)
#define SOC_MCPWM_CAPTURE_CLK_FROM_GROUP     (1)    ///< Capture timer shares clock with other PWM timers
#define SOC_MCPWM_SUPPORT_SLEEP_RETENTION    (1)    ///< Support back up registers before sleep

/*-------------------------- USB CAPS ----------------------------------------*/
// USB Serial JTAG Caps
// #define SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP     (1)     /*!< Support to maintain minimum usb communication during light sleep */ // TODO: IDF-6395

// USB OTG Caps
#define SOC_USB_OTG_PERIPH_NUM          (2U)

// USB PHY Caps
#define SOC_USB_UTMI_PHY_NUM            (1U)
#define SOC_USB_UTMI_PHY_NO_POWER_OFF_ISO    1

/*-------------------------- PARLIO CAPS --------------------------------------*/
#define SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH    16  /*!< Number of data lines of the TX unit */
#define SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH    16  /*!< Number of data lines of the RX unit */
#define SOC_PARLIO_TX_CLK_SUPPORT_GATING     1  /*!< Support gating TX clock */
#define SOC_PARLIO_RX_CLK_SUPPORT_GATING     1  /*!< Support gating RX clock */
#define SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION 1  /*!< Support loop transmission */
#define SOC_PARLIO_SUPPORT_SLEEP_RETENTION   1   /*!< Support back up registers before sleep */
#define SOC_PARLIO_SUPPORT_I80_LCD           1   /*!< Support to drive I80 interfaced LCD */

/*--------------------------- MPI CAPS ---------------------------------------*/
#define SOC_MPI_MEM_BLOCKS_NUM (4)
#define SOC_MPI_OPERATIONS_NUM (3)

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)

/*-------------------------- SDMMC CAPS -----------------------------------------*/

/**
 * Card detect, write protect, interrupt use GPIO Matrix on all chips.
 * Slot 0 clock/cmd/data pins use IOMUX
 * Slot 1 clock/cmd/data pins use GPIO Matrix
 */
#define SOC_SDMMC_USE_IOMUX          1
#define SOC_SDMMC_USE_GPIO_MATRIX    1
#define SOC_SDMMC_NUM_SLOTS          2
#define SOC_SDMMC_DATA_WIDTH_MAX     8
/* Supported host clock delay phase number */
#define SOC_SDMMC_DELAY_PHASE_NUM    8
#define SOC_SDMMC_IO_POWER_EXTERNAL  1    ///< SDMMC IO power controlled by external power supply
#define SOC_SDMMC_PSRAM_DMA_CAPABLE  1    ///< SDMMC peripheral can do DMA transfer to/from PSRAM
#define SOC_SDMMC_UHS_I_SUPPORTED    1

/*--------------------------- SHA CAPS ---------------------------------------*/

/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SOC_SHA_DMA_MAX_BUFFER_SIZE     (3968)
#define SOC_SHA_SUPPORT_DMA             (1)

/* The SHA engine is able to resume hashing from a user */
#define SOC_SHA_SUPPORT_RESUME          (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_SHA_GDMA             (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)
#define SOC_SHA_SUPPORT_SHA512_224      (1)
#define SOC_SHA_SUPPORT_SHA512_256      (1)
#define SOC_SHA_SUPPORT_SHA512_T        (1)

/*--------------------------- ECC CAPS ---------------------------------------*/
#define SOC_ECC_CONSTANT_TIME_POINT_MUL           1
#define SOC_ECC_SUPPORT_CURVE_P384      (1)

/*--------------------------- ECDSA CAPS ---------------------------------------*/
#define SOC_ECDSA_SUPPORT_EXPORT_PUBKEY     (1)
#define SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE   (1)
#define SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP (1)
#define SOC_ECDSA_USES_MPI                  (1)
#define SOC_ECDSA_SUPPORT_CURVE_P384 (1)
#define SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES (1)  /*!< Support individual key purposes for different ECDSA curves (P192, P256, P384) */

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM                  3
#define SOC_SPI_PERIPH_CS_NUM(i)            6
#define SOC_SPI_MAXIMUM_BUFFER_SIZE         64
#define SOC_SPI_SUPPORT_SLEEP_RETENTION     1
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1
#define SOC_SPI_SUPPORT_OCT                 1
#define SOC_SPI_MAX_BITWIDTH(host_id)       ((host_id == 2) ? 4 : 8) // Supported line mode: SPI3: 1, 2, 4, SPI1/2: 1, 2, 4, 8

/*-------------------------- LP SPI CAPS ----------------------------------------*/
#define SOC_LP_SPI_MAXIMUM_BUFFER_SIZE     64

/*-------------------------- SPIRAM CAPS ----------------------------------------*/
#define SOC_SPIRAM_XIP_SUPPORTED        1

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)
// #define SOC_SPI_MEM_SUPPORT_WRAP                          (1) // IDFCI-2073 The feature cannot be treated as supported on P4
#define SOC_SPI_MEM_SUPPORT_TIMING_TUNING                 (1)
#define SOC_MEMSPI_TIMING_TUNING_BY_DQS                   (1)
#define SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY           (1)
#define SOC_SPI_MEM_SUPPORT_CACHE_32BIT_ADDR_MAP          (1)
#define SOC_SPI_MEM_PSRAM_FREQ_AXI_CONSTRAINED            (1)
#define SOC_SPI_MEM_SUPPORT_TSUS_TRES_SEPERATE_CTR        (1)

#define SOC_MSPI_HAS_INDEPENT_IOMUX               1
#define SOC_MEMSPI_IS_INDEPENDENT                 1
#define SOC_MEMSPI_SUPPORT_CONTROL_DUMMY_OUT      1

#define SOC_SPI_MEM_FLASH_SUPPORT_HPM                         (1) /*!< Support High Performance Mode */

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM            2  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM              3  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO           32 // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI           20 // Bit width of systimer high part
#define SOC_SYSTIMER_FIXED_DIVIDER          1  // Clock source divider is fixed: 2.5
#define SOC_SYSTIMER_SUPPORT_RC_FAST        1  // Systimer can use RC_FAST clock source
#define SOC_SYSTIMER_INT_LEVEL              1  // Systimer peripheral uses level interrupt
#define SOC_SYSTIMER_ALARM_MISS_COMPENSATE  1  // Systimer peripheral can generate interrupt immediately if t(target) > t(current)
#define SOC_SYSTIMER_SUPPORT_ETM            1  // Systimer comparator can generate ETM event

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part
#define SOC_RTC_TIMER_SUPPORTED             SOC_RTC_TIMER_V2_SUPPORTED

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_SUPPORT_ETM             1
#define SOC_TIMER_SUPPORT_SLEEP_RETENTION 1

/*--------------------------- WATCHDOG CAPS ---------------------------------------*/
#define SOC_MWDT_SUPPORT_XTAL              (1)
#define SOC_MWDT_SUPPORT_SLEEP_RETENTION   (1)

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_VERSION                    (3)     /*!< Hardware version of touch sensor */
#define SOC_TOUCH_MIN_CHAN_ID                       (1U)    /*!< Touch minimum channel number */
#define SOC_TOUCH_MAX_CHAN_ID                       (14)    /*!< Touch maximum channel number */

/* Touch Sensor Features */
#define SOC_TOUCH_SUPPORT_SLEEP_WAKEUP              (1)     /*!< Touch sensor supports sleep awake */
#define SOC_TOUCH_SUPPORT_BENCHMARK                 (1)     /*!< Touch sensor supports benchmark configuration */
#define SOC_TOUCH_SUPPORT_WATERPROOF                (1)     /*!< Touch sensor supports waterproof */
#define SOC_TOUCH_SUPPORT_PROX_SENSING              (1)     /*!< Touch sensor supports proximity sensing */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM             (3)     /*!< Support touch proximity channel number. */
#define SOC_TOUCH_SAMPLE_CFG_NUM                    (3)     /*!< The sample configurations number in total, each sampler can be used to sample on one frequency */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM             3
#define SOC_TWAI_MASK_FILTER_NUM            1U
#define SOC_TWAI_SUPPORT_SLEEP_RETENTION    1

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1
/* Capability to disable the MSPI access in download mode */
#define SOC_EFUSE_DIS_DOWNLOAD_MSPI 1
// ECDSA_P256_KEY
#define SOC_EFUSE_ECDSA_KEY 1
#define SOC_EFUSE_XTS_AES_KEY_128 1
#define SOC_EFUSE_XTS_AES_KEY_256 1
#define SOC_EFUSE_ECDSA_KEY_P192 1
#define SOC_EFUSE_ECDSA_KEY_P384 1

/*-------------------------- Key Manager CAPS----------------------------*/
#define SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT  1 /*!< Key manager supports key deployment */
#define SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY        1 /*!< Key manager responsible to deploy ECDSA key */
#define SOC_KEY_MANAGER_FE_KEY_DEPLOY           1 /*!< Key manager responsible to deploy Flash Encryption key */
#define SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128   1 /*!< Key manager responsible to deploy the XTS-AES-128 key */
#define SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256   1 /*!< Key manager responsible to deploy the XTS-AES-256 key */
#define SOC_KEY_MANAGER_HMAC_KEY_DEPLOY         1 /*!< Key manager responsible to deploy HMAC key */
#define SOC_KEY_MANAGER_DS_KEY_DEPLOY           1 /*!< Key manager responsible to deploy DS key */

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              1
#define SOC_SECURE_BOOT_V2_ECC              1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   1
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  1

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)
#define SOC_FLASH_ENCRYPTION_XTS_AES        1
#define SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS 1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128    1  /* SOC_EFUSE_XTS_AES_KEY_128 (1) || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 (1) */
#define SOC_FLASH_ENCRYPTION_XTS_AES_256    1  /* SOC_EFUSE_XTS_AES_KEY_256 (1) || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 (1) */
#define SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND  1 /*!< Only available in chip version above 3.0 */
/*-------------------------- MEMPROT CAPS ------------------------------------*/

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-P4 has 6 UARTs (5 HP UART, and 1 LP UART)
#define SOC_UART_NUM                    (6)
#define SOC_UART_HP_NUM                 (5)
#define SOC_UART_LP_NUM                 (1U)
#define SOC_UART_FIFO_LEN               (128)       /*!< The UART hardware FIFO length */
#define SOC_LP_UART_FIFO_LEN            (16)        /*!< The LP UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX            (5000000)   /*!< Max bit rate supported by UART */
#define SOC_UART_SUPPORT_RTC_CLK        (1)         /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK       (1)         /*!< Support XTAL clock as the clock source */
#define SOC_UART_SUPPORT_WAKEUP_INT     (1)         /*!< Support UART wakeup interrupt */
#define SOC_UART_HAS_LP_UART            (1)         /*!< Support LP UART */
#define SOC_UART_SUPPORT_SLEEP_RETENTION   (1)      /*!< Support back up registers before sleep */

#define SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN 5
#define SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE (1)
#define SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE   (1)
#define SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE     (1)
#define SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE      (1)

/*-------------------------- LP_VAD CAPS -------------------------------------*/
#define SOC_LP_I2S_SUPPORT_VAD           (1)

/*--------------------------- UHCI CAPS -------------------------------------*/
#define SOC_UHCI_NUM               (1UL)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_EXT1_WAKEUP      (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN   (1) /*!<Supports one bit per pin to configure the EXT1 trigger level */
#define SOC_PM_EXT1_WAKEUP_BY_PMU       (1)
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP    (1)     /*!<Supports waking up from touch pad trigger */
#define SOC_PM_SUPPORT_LP_UART_WAKEUP   (1)
#define SOC_PM_SUPPORT_CPU_PD           (1)
#define SOC_PM_SUPPORT_XTAL32K_PD       (1)
#define SOC_PM_SUPPORT_RC32K_PD         (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)
#define SOC_PM_SUPPORT_TOP_PD           (1)
#define SOC_PM_SUPPORT_CNNT_PD          (1)
#define SOC_PM_SUPPORT_RTC_PERIPH_PD    (1)

#define SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY   (1) /*!<Supports CRC only the stub code in RTC memory */

#define SOC_PM_CPU_RETENTION_BY_SW      (1)
#define SOC_PM_CACHE_RETENTION_BY_PAU   (1)

#define SOC_PM_PAU_LINK_NUM             (4)
#define SOC_PM_PAU_REGDMA_LINK_MULTI_ADDR   (1)
#define SOC_PAU_IN_TOP_DOMAIN           (1)

#define SOC_PM_PAU_REGDMA_UPDATE_CACHE_BEFORE_WAIT_COMPARE  (1)
#define SOC_SLEEP_SYSTIMER_STALL_WORKAROUND 1    //TODO IDF-11381: replace with all xtal field clk gate control
#define SOC_SLEEP_TGWDT_STOP_WORKAROUND     1    //TODO IDF-11381: replace with all xtal field clk gate control

#define SOC_PM_RETENTION_MODULE_NUM         (64)

/*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_APLL_SUPPORTED                    (1)     /*!< Support Audio PLL */
#define SOC_CLK_MPLL_SUPPORTED                    (1)     /*!< Support MSPI PLL */
#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */
#define SOC_CLK_RC32K_SUPPORTED                   (1)     /*!< Support an internal 32kHz RC oscillator */

#define SOC_CLK_LP_FAST_SUPPORT_LP_PLL            (1)      /*!< Support LP_PLL clock as the LP_FAST clock source */
#define SOC_CLK_LP_FAST_SUPPORT_XTAL              (1)     /*!< Support XTAL clock as the LP_FAST clock source */

#define SOC_PERIPH_CLK_CTRL_SHARED                (1)     /*!< Peripheral clock control (e.g. set clock source) is shared between various peripherals */

/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_INTR_SUPPORT                  (1)
#define SOC_TSENS_IS_INDEPENDENT_FROM_ADC                    (1)  /*!< Temperature sensor is a separate module, not share regs with ADC */
#define SOC_TEMPERATURE_SENSOR_SUPPORT_ETM                   (1)
// temperature sensor on esp32p4 in under low power domain.
#define SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION       (1)

/*-------------------------- Memory CAPS --------------------------*/
#define SOC_MEM_TCM_SUPPORTED                      (1)
#define SOC_ASYNCHRONOUS_BUS_ERROR_MODE            (1)
/*--------------------------- EMAC --------------------------------*/
#define SOC_EMAC_IEEE1588V2_SUPPORTED              (1)      /*!< EMAC Supports IEEE1588v2 time stamping */
#define SOC_EMAC_USE_MULTI_IO_MUX                  (1)      /*!< Multiple GPIO pad options exist to connect EMAC signal via IO_MUX */
#define SOC_EMAC_MII_USE_GPIO_MATRIX               (1)      /*!< EMAC MII signals are connected to GPIO pads via GPIO Matrix */
#define SOC_EMAC_SUPPORT_SLEEP_RETENTION           (1)      /*!< EMAC supports register backup/restore in sleep mode */

/*--------------------------- JPEG --------------------------------*/
#define SOC_JPEG_CODEC_SUPPORTED                  (1)
#define SOC_JPEG_DECODE_SUPPORTED                 (1)
#define SOC_JPEG_ENCODE_SUPPORTED                 (1)

/*--------------------------- CAM ---------------------------------*/
#define SOC_LCDCAM_CAM_SUPPORT_RGB_YUV_CONV         (1)

/*------------------------------------- ULP CAPS -------------------------------------*/
#define SOC_LP_CORE_SUPPORT_ETM                     (1) /*!< LP Core supports ETM */
#define SOC_LP_CORE_SUPPORT_LP_ADC                  (1) /*!< LP ADC can be accessed from the LP-Core */
#define SOC_LP_CORE_SUPPORT_STORE_LOAD_EXCEPTIONS   (1) /*!< LP Core will raise exceptions if accessing invalid addresses */
