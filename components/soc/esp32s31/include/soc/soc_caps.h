/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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

#define _SOC_CAPS_TARGET_IS_ESP32S31       1 // [gen_soc_caps:ignore]

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_ADC_SUPPORTED               1
#define SOC_ANA_CMPR_SUPPORTED          1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_UART_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_UHCI_SUPPORTED              1
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_AXI_GDMA_SUPPORTED          1
#define SOC_LP_AHB_GDMA_SUPPORTED       1
#define SOC_DMA2D_SUPPORTED             1
#define SOC_GPTIMER_SUPPORTED           1
#define SOC_LCDCAM_SUPPORTED            1
#define SOC_LCDCAM_CAM_SUPPORTED        1
#define SOC_LCDCAM_I80_LCD_SUPPORTED    1
#define SOC_LCDCAM_RGB_LCD_SUPPORTED    1
#define SOC_LCD_I80_SUPPORTED           1
#define SOC_LCD_RGB_SUPPORTED           1
#define SOC_PCNT_SUPPORTED              1
#define SOC_MCPWM_SUPPORTED             1
#define SOC_TWAI_SUPPORTED              1
#define SOC_TWAI_FD_SUPPORTED           1
#define SOC_ETM_SUPPORTED               1
#define SOC_PARLIO_SUPPORTED            1
#define SOC_PARLIO_LCD_SUPPORTED        1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_USB_OTG_SUPPORTED           1
#define SOC_TEMP_SENSOR_SUPPORTED       1
#define SOC_USB_SERIAL_JTAG_SUPPORTED   1
// #define SOC_SUPPORTS_SECURE_DL_MODE     1      // TODO: [ESP32S31] IDF-14629
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
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
#define SOC_BOD_SUPPORTED               1
// #define SOC_APM_SUPPORTED               1      // TODO: [ESP32S31] IDF-14620
#define SOC_PAU_SUPPORTED                  1
#define SOC_PMU_SUPPORTED                  1
#define SOC_RTC_TIMER_SUPPORTED            1
#define SOC_ULP_LP_UART_SUPPORTED       1
#define SOC_LP_GPIO_MATRIX_SUPPORTED    1
#define SOC_LP_PERIPHERALS_SUPPORTED    1
#define SOC_LP_I2C_SUPPORTED            1
#define SOC_LP_SPI_SUPPORTED            1
#define SOC_SPIRAM_SUPPORTED            1
#define SOC_PSRAM_DMA_CAPABLE           1
#define SOC_SDMMC_HOST_SUPPORTED        1
#define SOC_CLK_TREE_SUPPORTED          1
#define SOC_ASSIST_DEBUG_SUPPORTED      1
#define SOC_CPU_LOCKUP_DEBUG_SUPPORTED  1
#define SOC_DEBUG_PROBE_SUPPORTED       1
#define SOC_WDT_SUPPORTED               1
#define SOC_RTC_WDT_SUPPORTED           1
#define SOC_SPI_FLASH_SUPPORTED         1
#define SOC_SPI_FLASH_HAS_DEDICATED_LDO 1
#define SOC_TOUCH_SENSOR_SUPPORTED      1
#define SOC_GP_LDO_SUPPORTED            1
#define SOC_RNG_SUPPORTED               1
#define SOC_PPA_SUPPORTED               1
#define SOC_LIGHT_SLEEP_SUPPORTED       1
#define SOC_DEEP_SLEEP_SUPPORTED        1
#define SOC_MODEM_CLOCK_SUPPORTED       1
#define SOC_PM_SUPPORTED                1
#define SOC_BITSCRAMBLER_SUPPORTED      1
#define SOC_SIMD_INSTRUCTION_SUPPORTED  1
#define SOC_CORDIC_SUPPORTED            1
#define SOC_REGI2C_SUPPORTED            1
#define SOC_JPEG_CODEC_SUPPORTED        1
#define SOC_EMAC_SUPPORTED              1

/*-------------------------- EMAC CAPS ----------------------------------------*/
#define SOC_EMAC_SUPPORT_1000M                     (1)      /*!< EMAC Supports 1000Mbps mode */
#define SOC_EMAC_IEEE1588V2_SUPPORTED              (1)      /*!< EMAC Supports IEEE1588v2 time stamping */
#define SOC_EMAC_USE_MULTI_IO_MUX                  (1)      /*!< Multiple GPIO pad options exist to connect EMAC signal via IO_MUX */
#define SOC_EMAC_MII_USE_GPIO_MATRIX               (1)      /*!< EMAC MII signals are connected to GPIO pads via GPIO Matrix */
#define SOC_EMAC_DEDICATED_GPIO_CTRL_SUPPORTED     (1)      /*!< EMAC has dedicated CNNT-domain pad control registers */
// #define SOC_EMAC_SUPPORT_SLEEP_RETENTION           (1)   // TODO: [ESP32S31] IDF-14731 - implement sleep retention for EMAC

/*------------------------- Analog Comparator CAPS ---------------------------*/
#define SOC_ANA_CMPR_SUPPORT_ETM        (1)
#define SOC_ANA_CMPR_SUPPORT_ETM_SCAN   (1)

/*-------------------------- USB CAPS ----------------------------------------*/
#define SOC_USB_OTG_PERIPH_NUM          (1U)
#define SOC_USB_FSLS_PHY_NUM            (0U)
#define SOC_USB_UTMI_PHY_NUM            (1U)

#define SOC_PHY_SUPPORTED                1
#define SOC_WIFI_SUPPORTED               1
#define SOC_IEEE802154_SUPPORTED         1
#define SOC_BT_SUPPORTED                 1
#define SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT 1
/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M                        1
#define SOC_XTAL_CLOCK_PATH_DEPENDS_ON_TOP_DOMAIN   1

/*-------------------------- ADC CAPS ----------------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_MAX_CHANNEL_NUM                 (8)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (8)
#define SOC_ADC_ATTEN_NUM                       (1U)
#define SOC_ADC_RTC_MIN_BITWIDTH                (17)
#define SOC_ADC_RTC_MAX_BITWIDTH                (17)

#define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_DIGI_CONTROLLER_NUM             (2)
#define SOC_ADC_PATT_LEN_MAX                    (16)
#define SOC_ADC_DIGI_MIN_BITWIDTH               (17)
#define SOC_ADC_DIGI_MAX_BITWIDTH               (17)
#define SOC_ADC_DIGI_RESULT_BYTES               (4)
#define SOC_ADC_DIGI_DATA_BYTES_PER_CONV        (4)
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        (1)
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          (83333U)
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           (611U)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)

#define SOC_ADC_DIFF_SUPPORTED                  (1)

/*!< ADC power control is shared by PWDET, TempSensor */
#define SOC_ADC_SHARED_POWER                    1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_CACHE_WRITEBACK_SUPPORTED           1
#define SOC_CACHE_FREEZE_SUPPORTED              1

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
/* PIE coprocessor assembly is only supported with GCC compiler  */
#define SOC_CPU_HAS_PIE                 1

#define SOC_HP_CPU_HAS_MULTIPLE_CORES   1   // Convenience boolean macro used to determine if a target has multiple cores.

#define SOC_CPU_BREAKPOINTS_NUM             4
#define SOC_CPU_WATCHPOINTS_NUM             4
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x100   // bytes

#define SOC_CPU_HAS_PMA                 1
#define SOC_CPU_IDRAM_SPLIT_USING_PMP   1
#define SOC_CPU_PMP_REGION_GRANULARITY  128

#define SOC_CPU_HAS_LOCKUP_RESET        1

#define SOC_SIMD_PREFERRED_DATA_ALIGNMENT 16 // The preferred data alignment accepted by the SIMD instructions, in bytes

/*-------------------------- DMA Common CAPS ----------------------------------------*/
#define SOC_DMA_CAN_ACCESS_FLASH 1 /*!< DMA can access Flash memory */

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_AHB_GDMA_VERSION                2
#define SOC_GDMA_SUPPORT_ETM                1
#define SOC_GDMA_SUPPORT_SLEEP_RETENTION    1
#define SOC_GDMA_EXT_MEM_ENC_ALIGNMENT   (16)

/*-------------------------- MODEM CAPS --------------------------------------*/
#define SOC_MODEM_SUPPORT_ETM               1

/*-------------------------- APM CAPS ----------------------------------------*/
#define SOC_APM_CTRL_FILTER_SUPPORTED   1 /*!< Support for APM control filter */

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-S31 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 62
#define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1
#define SOC_GPIO_FLEX_GLITCH_FILTER_NUM    8
#define SOC_GPIO_SUPPORT_PIN_HYS_FILTER    1

// GPIO peripheral has the ETM extension
#define SOC_GPIO_SUPPORT_ETM          1

#define SOC_GPIO_NEED_SOFT_ISOLATE_DURING_PD        (1)
#define SOC_LP_IO_HAS_INDEPENDENT_WAKEUP_SOURCE     (1)

// LP IO peripherals have independent clock gating to manage
#define SOC_LP_IO_CLOCK_IS_INDEPENDENT      (1)

#define SOC_GPIO_VALID_GPIO_MASK        (((1ULL << SOC_GPIO_PIN_COUNT) - 1) & ~(0ULL | BIT29 | BIT41))
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#define SOC_GPIO_IN_RANGE_MAX           61
#define SOC_GPIO_OUT_RANGE_MAX          61

// digital I/O pad powered by VDD3V3_CPU or VDD_SPI(GPIO_NUM_8~GPIO_NUM_61, excluding GPIO29/41 which are not bonded)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK (SOC_GPIO_VALID_GPIO_MASK & ~((1ULL << SOC_RTCIO_PIN_COUNT) - 1))

// Support to force hold all IOs
#define SOC_GPIO_SUPPORT_FORCE_HOLD              (1)
// Support to hold a single digital I/O when the digital domain is powered off
#define SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP  (1)

// Clock-out routed to GPIO via the GPIO matrix
#define SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX         (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM            (3)
#define SOC_CLOCKOUT_SUPPORT_CHANNEL_DIVIDER     (1)

/*-------------------------- DEBUG PROBE CAPS ---------------------------------*/
#define SOC_DEBUG_PROBE_NUM_UNIT                 (2U)  // Unit 0: HP probe, Unit 1: LP probe
#define SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH         (16)  // Maximum width of the debug probe output in each unit

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT                 8
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED    1  /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                                * so the RTC GPIOs can be used as general purpose RTC GPIOs.
                                                */
#define SOC_RTCIO_HOLD_SUPPORTED            1
#define SOC_RTCIO_WAKE_SUPPORTED            1

/*-------------------------- Sigma Delta Modulator CAPS -----------------*/
#define SOC_SDM_SUPPORT_SLEEP_RETENTION     1

/*-------------------------- ETM CAPS -----------------------------------*/
#define SOC_ETM_SUPPORT_SLEEP_RETENTION     1

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK      (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK         (1)
#define SOC_LEDC_TIMER_NUM                  (4)
#define SOC_LEDC_CHANNEL_NUM                (8)
#define SOC_LEDC_TIMER_BIT_WIDTH            (20)
#define SOC_LEDC_SUPPORT_FADE_STOP          (1)
#define SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED (1)
#define SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX (16)
#define SOC_LEDC_FADE_PARAMS_BIT_WIDTH      (10)
#define SOC_LEDC_SUPPORT_SLEEP_RETENTION    (1)
#define SOC_LEDC_SUPPORT_ETM                (1)

/*--------------------------- RMT CAPS ---------------------------------------*/
#define SOC_RMT_MEM_WORDS_PER_CHANNEL         48 /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_PINGPONG           1  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT         1  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP     1  /*!< Hardware support of auto-stop in loop mode */
#define SOC_RMT_SUPPORT_DMA                   1  /*!< RMT peripheral can connect to DMA channel */
#define SOC_RMT_SUPPORT_SLEEP_RETENTION       1  /*!< The sleep retention feature can help back up RMT registers before sleep */

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

/*-------------------------- PCNT CAPS ---------------------------------------*/
#define SOC_PCNT_SUPPORT_RUNTIME_THRES_UPDATE 1
#define SOC_PCNT_SUPPORT_CLEAR_SIGNAL         1
#define SOC_PCNT_SUPPORT_STEP_NOTIFY          1

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PAGE_SIZE_CONFIGURABLE        (1)
#define SOC_MMU_PERIPH_NUM                    (2U)
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM     (2U)
#define SOC_MMU_DI_VADDR_SHARED               (1) /*!< D/I vaddr are shared */
#define SOC_MMU_PER_EXT_MEM_TARGET            (1) /*!< MMU is per physical external memory target (flash, psram) */

/*-------------------------- SDMMC CAPS -----------------------------------------*/
#define SOC_SDMMC_USE_IOMUX               1
#define SOC_SDMMC_NUM_SLOTS               2
#define SOC_SDMMC_DATA_WIDTH_MAX          4
/* Supported host clock delay phase number */
#define SOC_SDMMC_DELAY_PHASE_NUM         8
#define SOC_SDMMC_IO_UHS_POWER_EXTERNAL   1    ///< SDMMC IO power controlled by external power supply
#define SOC_SDMMC_PSRAM_DMA_CAPABLE       1    ///< SDMMC peripheral can do DMA transfer to/from PSRAMsts
#define SOC_SDMMC_UHS_I_SUPPORTED         1

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM                  3
#define SOC_SPI_MAXIMUM_BUFFER_SIZE         64
#define SOC_SPI_SUPPORT_SLEEP_RETENTION     1
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1
#define SOC_SPI_SUPPORT_OCT                 1

/*-------------------------- SPIRAM CAPS ----------------------------------------*/
#define SOC_SPIRAM_XIP_SUPPORTED        1

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)
#define SOC_SPI_MEM_SUPPORT_TIMING_TUNING         (1)
#define SOC_MEMSPI_TIMING_TUNING_BY_DQS           (1)
#define SOC_SPI_MEM_SUPPORT_CACHE_32BIT_ADDR_MAP          (1)
#define SOC_SPI_MEM_SUPPORT_TSUS_TRES_SEPERATE_CTR        (1)

#define SOC_PSRAM_HAS_DEDICATED_LDO               1
#define SOC_MEMSPI_IS_INDEPENDENT                 1
#define SOC_PSRAM_MEMSPI_IS_INDEPENDENT           1

#define SOC_MEMSPI_ENCRYPTION_ALIGNMENT           16    /*!< 16-byte alignment restriction to mem addr and size if encryption is enabled */

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_SUPPORT_ETM            1  // Systimer comparator can generate ETM event

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_SUPPORT_ETM             (1)
#define SOC_TIMER_SUPPORT_SLEEP_RETENTION (1)

/*--------------------------- WATCHDOG CAPS ---------------------------------------*/
#define SOC_MWDT_SUPPORT_XTAL              (1)
#define SOC_MWDT_SUPPORT_SLEEP_RETENTION   (1)

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM             2U
#define SOC_TWAI_MASK_FILTER_NUM            3U
#define SOC_TWAI_RANGE_FILTER_NUM           1U
#define SOC_TWAI_SUPPORT_SLEEP_RETENTION    1

/*-------------------------- PARLIO CAPS --------------------------------------*/
#define SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH      16 /*!< Number of data lines of the TX unit */
#define SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH      16 /*!< Number of data lines of the RX unit */
#define SOC_PARLIO_TX_CLK_SUPPORT_GATING       1  /*!< Support gating TX clock */
#define SOC_PARLIO_RX_CLK_SUPPORT_GATING       1  /*!< Support gating RX clock */
#define SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION 1 /*!< Support loop transmission */
#define SOC_PARLIO_SUPPORT_SLEEP_RETENTION     1  /*!< Support back up registers before sleep */
#define SOC_PARLIO_SUPPORT_I80_LCD             1  /*!< Support to drive I80 interfaced LCD */

/*-------------------------- AES CAPS ----------------------------------------*/
#define SOC_AES_GDMA                            (1)
#define SOC_AES_SUPPORT_DMA                     (1)
#define SOC_AES_SUPPORT_AES_128                 (1)
#define SOC_AES_SUPPORT_AES_256                 (1)
// TODO: [ESP32S31] IDF-14633 SOC_AES_SUPPORT_GCM not enabled: GCM control registers (AAD_BLOCK_NUM, REMAINDER_BIT_NUM, CONTINUE) non-functional on v0.0 silicon
#define SOC_AES_SUPPORT_PSEUDO_ROUND_FUNCTION   (1)

/*-------------------------- SHA CAPS ----------------------------------------*/
#define SOC_SHA_GDMA                    (1)
#define SOC_SHA_DMA_MAX_BUFFER_SIZE     (3968)
#define SOC_SHA_SUPPORT_DMA             (1)
#define SOC_SHA_SUPPORT_RESUME          (1)
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)
#define SOC_SHA_SUPPORT_SHA512_224      (1)
#define SOC_SHA_SUPPORT_SHA512_256      (1)
#define SOC_SHA_SUPPORT_SHA512_T        (1)

/*--------------------------- MPI CAPS ---------------------------------------*/
#define SOC_MPI_MEM_BLOCKS_NUM          (4)
#define SOC_MPI_OPERATIONS_NUM          (3)

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN             (4096)

/*--------------------------- ECC CAPS ---------------------------------------*/
#define SOC_ECC_CONSTANT_TIME_POINT_MUL           1
#define SOC_ECC_SUPPORT_CURVE_P384      (1)

/*--------------------------- ECDSA CAPS ---------------------------------------*/
#define SOC_ECDSA_SUPPORT_EXPORT_PUBKEY              (1)
#define SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE         (1)
#define SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP      (1)
#define SOC_ECDSA_SUPPORT_CURVE_P384                 (1)
#define SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES (1)  /*!< Support individual key purposes for different ECDSA curves (P192, P256, P384) */

/*-------------------------- Digital Signature CAPS ----------------------------------------*/
#define SOC_DS_SIGNATURE_MAX_BIT_LEN    (4096)
#define SOC_DS_KEY_PARAM_MD_IV_LENGTH   (16)
#define SOC_DS_KEY_CHECK_MAX_WAIT_US    (1100)

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1
/* Capability to disable the MSPI access in download mode */
#define SOC_EFUSE_DIS_DOWNLOAD_MSPI 1
// ECDSA_P256_KEY
#define SOC_EFUSE_ECDSA_KEY 1
#define SOC_EFUSE_ECDSA_KEY_P192 1
#define SOC_EFUSE_ECDSA_KEY_P384 1
#define SOC_EFUSE_XTS_AES_KEY_128 1
#define SOC_EFUSE_XTS_AES_KEY_256 1

/*-------------------------- HUK CAPS----------------------------*/
#define SOC_HUK_SUPPORTED                       1

/*-------------------------- Key Manager CAPS----------------------------*/
#define SOC_KEY_MANAGER_SUPPORTED                   1
#define SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT      1 /*!< Key manager supports key deployment */
#define SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY            1 /*!< Key manager responsible to deploy ECDSA key */
#define SOC_KEY_MANAGER_FE_KEY_DEPLOY               1 /*!< Key manager responsible to deploy Flash Encryption key */
#define SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128   1 /*!< Key manager responsible to deploy the XTS-AES-128 key */
#define SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256   1 /*!< Key manager responsible to deploy the XTS-AES-256 key */
#define SOC_KEY_MANAGER_HMAC_KEY_DEPLOY             1 /*!< Key manager responsible to deploy HMAC key */
#define SOC_KEY_MANAGER_DS_KEY_DEPLOY               1 /*!< Key manager responsible to deploy DS key */

/*--------------------------- CAM ---------------------------------*/
#define SOC_LCDCAM_CAM_SUPPORT_RGB_YUV_CONV         (1)

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              1
#define SOC_SECURE_BOOT_V2_ECC              0
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   1
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  1

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTION_XTS_AES                       1
#define SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS               1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128                   1  /* SOC_EFUSE_XTS_AES_KEY_128 (1) || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 (1) */
#define SOC_FLASH_ENCRYPTION_XTS_AES_256                   1  /* SOC_EFUSE_XTS_AES_KEY_256 (1) || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 (1) */
#define SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND  1
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)

/*------------------------Bootloader CAPS---------------------------------*/
/* Support Recovery Bootloader */
#define SOC_RECOVERY_BOOTLOADER_SUPPORTED             (1)
/* Support Anti-rollback */
#define SOC_BOOTLOADER_ANTI_ROLLBACK_SUPPORTED        (1)

/*-------------------------- UART CAPS ---------------------------------------*/
#define SOC_UART_NUM                    (5)
#define SOC_UART_HP_NUM                 (4)
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

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*-------------------------- EXTERNAL COEXISTENCE CAPS -------------------------------------*/
#define SOC_EXTERNAL_COEX_ADVANCE              (1) /*!< HARDWARE  EXTERNAL COEXISTENCE CAPS */

// /*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
#define SOC_MODEM_CLOCK_IS_INDEPENDENT            (1)
#define SOC_MODEM_APB_CLOCK_IS_INDEPENDENT        (1)
#define SOC_MODEM_CLOCK_SOC_PLL_SOURCE_CG_SUPPORTED (1)
#define SOC_MODEM_CLOCK_WIFI_BB_80X1_AS_APB       (1)

#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_APLL_SUPPORTED                    (1)     /*!< Support Audio PLL */
#define SOC_CLK_MPLL_SUPPORTED                    (1)     /*!< Support MSPI PLL */
#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */
#define SOC_CLK_RC32K_SUPPORTED                   (1)     /*!< Support an internal 32kHz RC oscillator */

#define SOC_CLK_LP_FAST_SUPPORT_XTAL              (1)     /*!< Support XTAL clock as the LP_FAST clock source */

#define SOC_CLK_ANA_I2C_MST_HAS_ROOT_GATE         (1)     /*!< Any regi2c operation needs enable the analog i2c master clock first */

#define SOC_RCC_IS_INDEPENDENT                    1       /*!< Reset and Clock Control has own registers for each module */
/*-------------------------- Memory CAPS --------------------------*/
#define SOC_ASYNCHRONOUS_BUS_ERROR_MODE           (1)

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_VERSION                    (3)     /*!< Hardware version of touch sensor */
#define SOC_TOUCH_MIN_CHAN_ID                       (0U)    /*!< Touch minimum channel number */
#define SOC_TOUCH_MAX_CHAN_ID                       (13)    /*!< Touch maximum channel number */

/* Touch Sensor Features */
#define SOC_TOUCH_SUPPORT_SLEEP_WAKEUP              (1)     /*!< Touch sensor supports sleep awake */
#define SOC_TOUCH_SUPPORT_BENCHMARK                 (1)     /*!< Touch sensor supports benchmark configuration */
#define SOC_TOUCH_SUPPORT_WATERPROOF                (1)     /*!< Touch sensor supports waterproof */
#define SOC_TOUCH_SUPPORT_PROX_SENSING              (1)     /*!< Touch sensor supports proximity sensing */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM             (3)     /*!< Support touch proximity channel number. */
#define SOC_TOUCH_SAMPLE_CFG_NUM                    (3)     /*!< The sample configurations number in total, each sampler can be used to sample on one frequency */

/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_BT_WAKEUP        (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP      (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN   (1) /*!<Supports one bit per pin to configure the EXT1 trigger level */
#define SOC_PM_EXT1_WAKEUP_BY_PMU       (1)
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BEACON_WAKEUP    (1)
#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP    (1)     /*!<Supports waking up from touch pad trigger */
#define SOC_PM_SUPPORT_CPU_PD           (1)
#define SOC_PM_SUPPORT_XTAL32K_PD       (1)
#define SOC_PM_SUPPORT_RC32K_PD         (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)
#define SOC_PM_SUPPORT_TOP_PD           (1)
#define SOC_PM_SUPPORT_HP_AON_PD        (1)
#define SOC_PM_SUPPORT_CNNT_PD          (1)
#define SOC_PM_SUPPORT_RTC_PERIPH_PD    (1)
#define SOC_PM_SUPPORT_MODEM_PD         (1) /*!<modem includes BLE and 15.4 and wifi*/
#define SOC_PM_SUPPORT_MAC_BB_PD        (1)

#define SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG      (1)

#define SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY   (1) /*!<Supports CRC only the stub code in RTC memory */

#define SOC_PM_CPU_RETENTION_BY_SW          (1)
#define SOC_PM_FPU_RETENTION_BY_SW          (1)
#define SOC_PM_CACHE_RETENTION_BY_PAU       (1)
#define SOC_EXT_MEM_CACHE_TAG_IN_CPU_DOMAIN (1)

#define SOC_PM_PAU_LINK_NUM                 (5)
#define SOC_PM_PAU_REGDMA_LINK_CONFIGURABLE (1)
#define SOC_PM_PAU_REGDMA_LINK_IDX_WIFIMAC  (4) // The range of values for the link index is [0, SOC_PM_PAU_LINK_NUM)
/** Workaround: software-triggered modem PHY retention uses dedicated WiFi MAC REGDMA, not entry link_sel */
#define SOC_PM_PAU_REGDMA_MODEM_WIFIMAC_WORKAROUND (1)

#define SOC_PM_MODEM_RETENTION_BY_REGDMA           (1)
// #define SOC_PM_SUPPORT_PMU_MODEM_STATE      (1) // TODO: [ESP32S31] IDF-14582

#define SOC_PM_RETENTION_MODULE_NUM         (64)

/*-------------------------- LP_CORE CAPS ------------------------------------*/
#define SOC_LP_MAILBOX_SUPPORTED                    (1) /*!< LP Core supports LP-mailbox */
#define SOC_LP_CORE_SUPPORT_ETM                     (1) /*!< LP Core supports ETM wakeup */
#define SOC_LP_CORE_CONFIGURABLE_BOOT_ADDR          (1) /*!< LP Core has no LP ROM; HP must write the reset_vector address (LP_RAM_BASE+0x80) to LP_SYS.lp_core_boot_addr before triggering LP wake */
#define SOC_LP_CORE_SUPPORT_I2C                     (1) /*!< LP Core supports I2C */
#define SOC_LP_CORE_HW_AUTO_CLRWAKEUPCAUSE          (1) /*!< LP core requests sleep, PMU clears both HP and LP wakeup causes */
#define SOC_LP_CORE_HAS_PMP                         (1) /*!< LP Core RISC-V has 16 PMP entries (128-byte granularity, RISC-V v1.10) */

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part
#define SOC_RTC_TIMER_V3                    1

/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_SWSYNC_CAN_PROPAGATE       (1)    ///< Software sync event can be routed to its output
#define SOC_MCPWM_SUPPORT_ETM                (1)    ///< Support ETM (Event Task Matrix)
#define SOC_MCPWM_SUPPORT_EVENT_COMPARATOR   (1)    ///< Support event comparator (based on ETM)
#define SOC_MCPWM_CAPTURE_CLK_FROM_GROUP     (1)    ///< Capture timer shares clock with other PWM timers
#define SOC_MCPWM_SUPPORT_SLEEP_RETENTION    (1)    ///< Support back up registers before sleep

/*--------------------------- JPEG --------------------------------*/
#define SOC_JPEG_DECODE_SUPPORTED                 (1)
#define SOC_JPEG_ENCODE_SUPPORTED                 (1)

/*------------------------------------ WI-FI CAPS ------------------------------------*/
#define SOC_WIFI_HW_TSF                     (1)    /*!< Support hardware TSF */
#define SOC_WIFI_FTM_SUPPORT                (1)    /*!< Support FTM */
#define SOC_WIFI_GCMP_SUPPORT               (1)    /*!< Support GCMP(GCMP128 and GCMP256) */
#define SOC_WIFI_WAPI_SUPPORT               (1)    /*!< Support WAPI */
#define SOC_WIFI_CSI_SUPPORT                (1)    /*!< Support CSI */
#define SOC_WIFI_MESH_SUPPORT               (1)    /*!< Support WIFI MESH */
#define SOC_WIFI_HE_SUPPORT                 (1)    /*!< Support Wi-Fi 6 */
#define SOC_WIFI_MAC_VERSION_NUM            (3)    /*!< Wi-Fi MAC version num is 3 */
#define SOC_WIFI_NAN_SUPPORT                (1)    /*!< Support WIFI Aware (NAN) */

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*---------------------------------- Bluetooth CAPS ----------------------------------*/
#define SOC_BT_CLASSIC_SUPPORTED                    (1)     /*!< Support Bluetooth Classic hardware */
#define SOC_BLE_SUPPORTED                           (1)     /*!< Support Bluetooth Low Energy hardware */
#define SOC_BLE_MESH_SUPPORTED                      (1)     /*!< Support BLE MESH */
#define SOC_BLE_ISO_SUPPORTED                       (1)     /*!< Support BLE ISO */
#define SOC_BLE_AUDIO_SUPPORTED                     (1)     /*!< Support BLE Audio */
#define SOC_ESP_NIMBLE_CONTROLLER                   (1)     /*!< Support BLE EMBEDDED controller V1 */
#define SOC_BLE_50_SUPPORTED                        (1)     /*!< Support Bluetooth 5.0 */
#define SOC_BLE_DEVICE_PRIVACY_SUPPORTED            (1)     /*!< Support BLE device privacy mode */
#define SOC_BLE_POWER_CONTROL_SUPPORTED             (1)     /*!< Support Bluetooth Power Control */
#define SOC_BLE_MULTI_CONN_OPTIMIZATION             (1)     /*!< Support multiple connections optimization */
#define SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED          (1)     /*!< Support For BLE Periodic Adv Enhancements */
#define SOC_BLUFI_SUPPORTED                         (1)     /*!< Support BLUFI */
#define SOC_BLE_CTE_SUPPORTED                       (1)     /*!< Support Bluetooth LE Constant Tone Extension (CTE) */
#define SOC_BLE_SUBRATE_SUPPORTED                   (1)     /*!< Support Bluetooth LE Connection Subrating */
#define SOC_BLE_PERIODIC_ADV_WITH_RESPONSE          (1)     /*!< Support Bluetooth LE Periodic Advertising with Response (PAwR) */

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_HW_VERSION_2                  (1)
#define SOC_I2S_SUPPORTS_ETM                  (1)
#define SOC_I2S_SUPPORTS_APLL                 (1)
#define SOC_I2S_SUPPORTS_RC_FAST              (1)     // Support RC_FAST as I2S clock source
#define SOC_I2S_SUPPORTS_EXTERNAL             (1)     // Support External clock source
#define SOC_I2S_SUPPORTS_PCM                  (1)
#define SOC_I2S_SUPPORTS_PDM                  (1)
#define SOC_I2S_SUPPORTS_PDM_TX               (1)     // Support to output raw PDM format data
#define SOC_I2S_SUPPORTS_PCM2PDM              (1)     // Support to write PCM format but output PDM format data with the help of PCM to PDM filter
#define SOC_I2S_SUPPORTS_PDM_RX               (1)     // Support to input raw PDM format data
#define SOC_I2S_SUPPORTS_PDM2PCM              (1)     // Support to input PDM format but read PCM format data with the help of PDM to PCM filter (only on I2S0)
#define SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER     (1)
#define SOC_I2S_SUPPORTS_TX_SYNC_CNT          (1)     // Support TX synchronization count (ideal_cnt)
#define SOC_I2S_SUPPORTS_RX_RECOMB            (1)     // Support RX recomb for DMA data format reorganization
#define SOC_I2S_SUPPORTS_TDM                  (1)
#define SOC_I2S_SUPPORTS_BT_DEST              (1)     // Support routing I2S TX/RX data to Bluetooth (Classic sync link / voice path), not via DMA
#define SOC_I2S_PDM_MAX_TX_LINES              (2)     // On I2S0
#define SOC_I2S_PDM_MAX_RX_LINES              (4)     // On I2S0

/*---------------------------------- ASRC CAPS ----------------------------------*/
#define SOC_ASRC_SUPPORTED (1)

/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_INTR_SUPPORT                  (1)
#define SOC_TSENS_IS_INDEPENDENT_FROM_ADC                    (1)  /*!< Temperature sensor is a separate module, not share regs with ADC */
#define SOC_TEMPERATURE_SENSOR_SUPPORT_ETM                   (1)
// temperature sensor on esp32s31 in under low power domain.
#define SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION       (1)
