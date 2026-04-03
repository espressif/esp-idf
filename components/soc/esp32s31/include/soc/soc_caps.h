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
// #define SOC_ADC_SUPPORTED               1      // TODO: [ESP32S31] IDF-14741
// #define SOC_ANA_CMPR_SUPPORTED          1      // TODO: [ESP32S31] IDF-14787
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_UART_SUPPORTED              1         // TODO: [ESP32S31] IDF-14789
#define SOC_GDMA_SUPPORTED              1
// #define SOC_UHCI_SUPPORTED              1      // TODO: [ESP32S31] IDF-14791
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_AXI_GDMA_SUPPORTED          1
#define SOC_LP_AHB_GDMA_SUPPORTED       1
// #define SOC_DMA2D_SUPPORTED             1      // TODO: [ESP32S31] IDF-14762
#define SOC_GPTIMER_SUPPORTED           1
// #define SOC_PCNT_SUPPORTED              1      // TODO: [ESP32S31] IDF-14699
// #define SOC_LCDCAM_SUPPORTED            1      // TODO: [ESP32S31] IDF-14722
// #define SOC_LCDCAM_CAM_SUPPORTED        1      // TODO: [ESP32S31] IDF-14722
// #define SOC_LCDCAM_I80_LCD_SUPPORTED    1      // TODO: [ESP32S31] IDF-14722
// #define SOC_LCDCAM_RGB_LCD_SUPPORTED    1      // TODO: [ESP32S31] IDF-14722
#define SOC_MCPWM_SUPPORTED             1
// #define SOC_TWAI_SUPPORTED              1      // TODO: [ESP32S31] IDF-14719
// #define SOC_ETM_SUPPORTED               1      // TODO: [ESP32S31] IDF-14724
// #define SOC_PARLIO_SUPPORTED            1      // TODO: [ESP32S31] IDF-14711
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
// #define SOC_USB_OTG_SUPPORTED           1      // TODO: [ESP32S31] IDF-14701
// #define SOC_USB_SERIAL_JTAG_SUPPORTED   1      // TODO: [ESP32S31] IDF-14788
// #define SOC_TEMP_SENSOR_SUPPORTED       1      // TODO: [ESP32S31] IDF-14799
// #define SOC_SUPPORTS_SECURE_DL_MODE     1      // TODO: [ESP32S31] IDF-14629
// #define SOC_ULP_SUPPORTED               1      // TODO: [ESP32S31] IDF-14640
// #define SOC_LP_CORE_SUPPORTED           1      // TODO: [ESP32S31] IDF-14640
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1         // TODO: [ESP32S31] IDF-14688
#define SOC_EFUSE_SUPPORTED             1         // TODO: [ESP32S31] IDF-14688
// #define SOC_RTC_MEM_SUPPORTED           1      // TODO: [ESP32S31] IDF-14678
// #define SOC_RMT_SUPPORTED               1      // TODO: [ESP32S31] IDF-14794
// #define SOC_I2S_SUPPORTED               1      // TODO: [ESP32S31] IDF-14771
#define SOC_SDM_SUPPORTED               1
// #define SOC_GPSPI_SUPPORTED             1      // TODO: [ESP32S31] IDF-14734
// #define SOC_LEDC_SUPPORTED              1      // TODO: [ESP32S31] IDF-14709
// #define SOC_ISP_SUPPORTED               1      // TODO: [ESP32S31] IDF-14769
// #define SOC_I2C_SUPPORTED               1      // TODO: [ESP32S31] IDF-14726
#define SOC_SYSTIMER_SUPPORTED          1         // TODO: [ESP32S31] IDF-14693
// #define SOC_AES_SUPPORTED               1      // TODO: [ESP32S31] IDF-14633
// #define SOC_MPI_SUPPORTED               1      // TODO: [ESP32S31] IDF-14633
// #define SOC_SHA_SUPPORTED               1      // TODO: [ESP32S31] IDF-14630
// #define SOC_HMAC_SUPPORTED              1      // TODO: [ESP32S31] IDF-14621
// #define SOC_DIG_SIGN_SUPPORTED          1      // TODO: [ESP32S31] IDF-14624
// #define SOC_ECC_SUPPORTED               1      // TODO: [ESP32S31] IDF-14631
// #define SOC_ECC_EXTENDED_MODES_SUPPORTED   1   // TODO: [ESP32S31] IDF-14631
// #define SOC_FLASH_ENC_SUPPORTED         1         // TODO: [ESP32S31] IDF-14628
// #define SOC_SECURE_BOOT_SUPPORTED       1      // TODO: [ESP32S31] IDF-14629
// #define SOC_BOD_SUPPORTED               1      // TODO: [ESP32S31] IDF-14658
// #define SOC_APM_SUPPORTED               1      // TODO: [ESP32S31] IDF-14620
#define SOC_PAU_SUPPORTED                  1
#define SOC_PMU_SUPPORTED                  1
#define SOC_RTC_TIMER_SUPPORTED            1
// #define SOC_ULP_LP_UART_SUPPORTED       1      // TODO: [ESP32S31] IDF-14634
#define SOC_LP_GPIO_MATRIX_SUPPORTED    1
#define SOC_LP_PERIPHERALS_SUPPORTED    1
// #define SOC_LP_I2C_SUPPORTED            1      // TODO: [ESP32S31] IDF-14635
// #define SOC_LP_SPI_SUPPORTED            1      // TODO: [ESP32S31] IDF-14639
#define SOC_SPIRAM_SUPPORTED            1      // TODO: [ESP32S31] IDF-14718
#define SOC_PSRAM_DMA_CAPABLE           1
// #define SOC_SDMMC_HOST_SUPPORTED        1      // TODO: [ESP32S31] IDF-14705
// #define SOC_CLK_TREE_SUPPORTED          1      // TODO: [ESP32S31] IDF-14733
// #define SOC_ASSIST_DEBUG_SUPPORTED      1      // TODO: [ESP32S31] IDF-14675
// #define SOC_DEBUG_PROBE_SUPPORTED       1      // TODO: [ESP32S31] IDF-14798
#define SOC_WDT_SUPPORTED               1
#define SOC_RTC_WDT_SUPPORTED           1
#define SOC_SPI_FLASH_SUPPORTED         1         // TODO: [ESP32S31] IDF-14777
// #define SOC_TOUCH_SENSOR_SUPPORTED      1      // TODO: [ESP32S31] IDF-14796
// #define SOC_RNG_SUPPORTED               1      // TODO: [ESP32S31] IDF-14632
// #define SOC_PPA_SUPPORTED               1      // TODO: [ESP32S31] IDF-14769
// #define SOC_LIGHT_SLEEP_SUPPORTED       1      // TODO: [ESP32S31] IDF-14645
// #define SOC_DEEP_SLEEP_SUPPORTED        1      // TODO: [ESP32S31] IDF-14643
#define SOC_MODEM_CLOCK_SUPPORTED       1
// #define SOC_PM_SUPPORTED                1      // TODO: [ESP32S31] IDF-14648
// #define SOC_BITSCRAMBLER_SUPPORTED      1      // TODO: [ESP32S31] IDF-14714
// #define SOC_SIMD_INSTRUCTION_SUPPORTED  1      // TODO: [ESP32S31] IDF-14661
#define SOC_CORDIC_SUPPORTED            1
#define SOC_REGI2C_SUPPORTED            1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- ADC CAPS ----------------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_MAX_CHANNEL_NUM                 (10)
#define SOC_ADC_PERIPH_NUM                      (2)

/*-------------------------- CACHE CAPS --------------------------------------*/
// TODO: [ESP32S31] IDF-14651
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data
#define SOC_CACHE_WRITEBACK_SUPPORTED           1
#define SOC_CACHE_FREEZE_SUPPORTED              1
#define SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE      0

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
// #define SOC_GDMA_SUPPORT_SLEEP_RETENTION    1 // TODO: [ESP32S31] IDF-14760
#define SOC_GDMA_EXT_MEM_ENC_ALIGNMENT   (16)

/*-------------------------- APM CAPS ----------------------------------------*/
#define SOC_APM_CTRL_FILTER_SUPPORTED   1 /*!< Support for APM control filter */

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-S31 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 62
// #define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1 // TODO: [ESP32S31] IDF-14781
// #define SOC_GPIO_FLEX_GLITCH_FILTER_NUM    8 // TODO: [ESP32S31] IDF-14781
#define SOC_GPIO_SUPPORT_PIN_HYS_FILTER    1

// GPIO peripheral has the ETM extension
// #define SOC_GPIO_SUPPORT_ETM          1 // TODO: [ESP32S31] IDF-14786
// #define SOC_GPIO_ETM_EVENTS_PER_GROUP 8 // TODO: [ESP32S31] IDF-14786
// #define SOC_GPIO_ETM_TASKS_PER_GROUP  8 // TODO: [ESP32S31] IDF-14786

// GPIO0~7 on ESP32S31 can support chip deep sleep wakeup
// #define SOC_GPIO_SUPPORT_HP_PERIPH_PD_SLEEP_WAKEUP  (1) // TODO: [ESP32S31] IDF-14643
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP           SOC_GPIO_SUPPORT_HP_PERIPH_PD_SLEEP_WAKEUP
#define SOC_LP_IO_HAS_INDEPENDENT_WAKEUP_SOURCE     (1)

// LP IO peripherals have independent clock gating to manage
#define SOC_LP_IO_CLOCK_IS_INDEPENDENT      (1)

#define SOC_GPIO_VALID_GPIO_MASK        (((1ULL << SOC_GPIO_PIN_COUNT) - 1) & ~(0ULL | BIT29 | BIT41))
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#define SOC_GPIO_IN_RANGE_MAX           61
#define SOC_GPIO_OUT_RANGE_MAX          61

#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | 0xFF)
#define SOC_GPIO_DEEP_SLEEP_WAKE_SUPPORTED_PIN_CNT      (8)

// digital I/O pad powered by VDD3V3_CPU or VDD_SPI(GPIO_NUM_8~GPIO_NUM_61, excluding GPIO29/41 which are not bonded)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK (SOC_GPIO_VALID_GPIO_MASK & ~((1ULL << SOC_RTCIO_PIN_COUNT) - 1))

// Support to force hold all IOs
#define SOC_GPIO_SUPPORT_FORCE_HOLD              (1)
// Support to hold a single digital I/O when the digital domain is powered off
#define SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP  (1)

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT                 8
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED    1  /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                                * so the RTC GPIOs can be used as general purpose RTC GPIOs.
                                                */
#define SOC_RTCIO_HOLD_SUPPORTED            1
#define SOC_RTCIO_WAKE_SUPPORTED            1

/*-------------------------- Sigma Delta Modulator CAPS -----------------*/
#define SOC_SDM_SUPPORT_SLEEP_RETENTION     1

/*-------------------------- LEDC CAPS ---------------------------------------*/
// TODO: [ESP32S31] IDF-14709
#define SOC_LEDC_CHANNEL_NUM                (6)

/*-------------------------- MMU CAPS ----------------------------------------*/
// TODO: [ESP32S31] IDF-14669
#define SOC_MMU_PERIPH_NUM                    (2U)
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM     (2U)
#define SOC_MMU_DI_VADDR_SHARED               (1) /*!< D/I vaddr are shared */
#define SOC_MMU_PER_EXT_MEM_TARGET            (1) /*!< MMU is per physical external memory target (flash, psram) */

/*-------------------------- SPI CAPS ----------------------------------------*/
// TODO: [ESP32S31] IDF-14734
#define SOC_SPI_PERIPH_NUM              3
#define SOC_SPI_PERIPH_CS_NUM(i)        (((i)==0)? 2: (((i)==1)? 6: 3))
#define SOC_SPI_MAXIMUM_BUFFER_SIZE     64
#define SOC_SPI_MAX_BITWIDTH(host_id)   ((host_id == 2) ? 4 : 8) // Supported line mode: SPI3: 1, 2, 4, SPI1/2: 1, 2, 4, 8

/*-------------------------- SPIRAM CAPS ----------------------------------------*/
#define SOC_SPIRAM_XIP_SUPPORTED        1

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_TIMING_TUNING         (1)
#define SOC_MEMSPI_TIMING_TUNING_BY_DQS           (1)

#define SOC_MSPI_HAS_INDEPENT_IOMUX               1
#define SOC_MEMSPI_IS_INDEPENDENT                 1
#define SOC_MEMSPI_SUPPORT_CONTROL_DUMMY_OUT      1

#define SOC_MEMSPI_ENCRYPTION_ALIGNMENT           16    /*!< 16-byte alignment restriction to mem addr and size if encryption is enabled */

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
// TODO: [ESP32S31] IDF-14693

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_SUPPORT_ETM             (1)
// #define SOC_TIMER_SUPPORT_SLEEP_RETENTION (1) // TODO: [ESP32S31] IDF-14746

/*--------------------------- WATCHDOG CAPS ---------------------------------------*/
#define SOC_MWDT_SUPPORT_XTAL              (1)
// #define SOC_MWDT_SUPPORT_SLEEP_RETENTION   (1)

/*-------------------------- eFuse CAPS----------------------------*/
// TODO: [ESP32S31] IDF-14688
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1
/* Capability to disable the MSPI access in download mode */
#define SOC_EFUSE_DIS_DOWNLOAD_MSPI 1
#define SOC_EFUSE_ECDSA_KEY 1

/*-------------------------- Key Manager CAPS----------------------------*/
// TODO: [ESP32S31] IDF-14688
#define SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY    1 /*!< Key manager responsible to deploy ECDSA key */
// #define SOC_KEY_MANAGER_FE_KEY_DEPLOY       1 /*!< Key manager responsible to deploy Flash Encryption key */
/*-------------------------- Secure Boot CAPS----------------------------*/
// TODO: [ESP32S31] IDF-14629
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3

/*-------------------------- Flash Encryption CAPS----------------------------*/
// TODO: [ESP32S31] IDF-14628
#define SOC_FLASH_ENCRYPTION_XTS_AES        1
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)

/*-------------------------- UART CAPS ---------------------------------------*/
// TODO: [ESP32S31] IDF-14789
#define SOC_UART_NUM                    (4)
#define SOC_UART_HP_NUM                 (4)
#define SOC_UART_FIFO_LEN               (128)       /*!< The UART hardware FIFO length */
#define SOC_LP_UART_FIFO_LEN            (16)        /*!< The LP UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX            (5000000)   /*!< Max bit rate supported by UART */
#define SOC_UART_SUPPORT_RTC_CLK        (1)         /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK       (1)         /*!< Support XTAL clock as the clock source */
#define SOC_UART_SUPPORT_WAKEUP_INT     (1)         /*!< Support UART wakeup interrupt */
// #define SOC_UART_HAS_LP_UART            (1)         /*!< Support LP UART */
// #define SOC_UART_SUPPORT_SLEEP_RETENTION   (1)      /*!< Support back up registers before sleep */

#define SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN 5
#define SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE (1)

// /*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
// TODO: [ESP32S31] IDF-14733
#define SOC_MODEM_CLOCK_IS_INDEPENDENT            (1)

#define SOC_CLK_APLL_SUPPORTED                    (1)     /*!< Support Audio PLL */
#define SOC_CLK_MPLL_SUPPORTED                    (1)     /*!< Support MSPI PLL */
#define SOC_CLK_RC32K_SUPPORTED                   (1)     /*!< Support an internal 32kHz RC oscillator */

#define SOC_CLK_LP_FAST_SUPPORT_LP_PLL            (1)      /*!< Support LP_PLL clock as the LP_FAST clock source */
#define SOC_CLK_LP_FAST_SUPPORT_XTAL              (1)     /*!< Support XTAL clock as the LP_FAST clock source */

#define SOC_CLK_ANA_I2C_MST_HAS_ROOT_GATE         (1)     /*!< Any regi2c operation needs enable the analog i2c master clock first */

#define SOC_RCC_IS_INDEPENDENT                    1       /*!< Reset and Clock Control has own registers for each module */

/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_EXT1_WAKEUP      (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN   (1) /*!<Supports one bit per pin to configure the EXT1 trigger level */
#define SOC_PM_EXT1_WAKEUP_BY_PMU       (1)
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BEACON_WAKEUP    (1)
#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP    (1)     /*!<Supports waking up from touch pad trigger */
// #define SOC_PM_SUPPORT_CPU_PD           (1) // TODO: [ESP32S31] IDF-14647
#define SOC_PM_SUPPORT_XTAL32K_PD       (1)
#define SOC_PM_SUPPORT_RC32K_PD         (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)
// #define SOC_PM_SUPPORT_TOP_PD           (1)  // TODO: [ESP32S31] IDF-14647
#define SOC_PM_SUPPORT_HP_AON_PD        (1)
#define SOC_PM_SUPPORT_CNNT_PD          (1)
#define SOC_PM_SUPPORT_RTC_PERIPH_PD    (1)
#define SOC_PM_SUPPORT_MODEM_PD         (1) /*!<modem includes BLE and 15.4 and wifi*/
#define SOC_PM_SUPPORT_MAC_BB_PD        (1)

#define SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG      (1)

#define SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY   (1) /*!<Supports CRC only the stub code in RTC memory */

#define SOC_PM_CPU_RETENTION_BY_SW          (1)
#define SOC_PM_CACHE_RETENTION_BY_PAU       (1)
#define SOC_EXT_MEM_CACHE_TAG_IN_CPU_DOMAIN (1)

#define SOC_PM_PAU_LINK_NUM                 (5)
#define SOC_PM_PAU_REGDMA_LINK_CONFIGURABLE (1)
#define SOC_PM_PAU_REGDMA_LINK_IDX_WIFIMAC  (4) // The range of values for the link index is [0, SOC_PM_PAU_LINK_NUM)

#define SOC_PM_MODEM_RETENTION_BY_REGDMA           (1)
// #define SOC_PM_SUPPORT_PMU_MODEM_STATE      (1) // TODO: [ESP32S31] IDF-14582

#define SOC_PM_RETENTION_MODULE_NUM         (64)

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part
#define SOC_RTC_TIMER_V3                    1

/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_SWSYNC_CAN_PROPAGATE       (1)    ///< Software sync event can be routed to its output
#define SOC_MCPWM_SUPPORT_ETM                (1)    ///< Support ETM (Event Task Matrix)
#define SOC_MCPWM_SUPPORT_EVENT_COMPARATOR   (1)    ///< Support event comparator (based on ETM)
#define SOC_MCPWM_CAPTURE_CLK_FROM_GROUP     (1)    ///< Capture timer shares clock with other PWM timers
// #define SOC_MCPWM_SUPPORT_SLEEP_RETENTION    (1)    ///< Support back up registers before sleep
