/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_ADC_SUPPORTED               1
#define SOC_ANA_CMPR_SUPPORTED          1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_UART_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_GPTIMER_SUPPORTED           1
// #define SOC_BT_SUPPORTED                1    // TODO: IDF-13139, re-enable for c61 eco3
//  \#define SOC_IEEE802154_SUPPORTED        1
#define SOC_USB_SERIAL_JTAG_SUPPORTED   1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_TEMP_SENSOR_SUPPORTED       1
// #define SOC_PHY_SUPPORTED               1    //TODO: IDF-13141, re-open for c61 eco3
// #define SOC_WIFI_SUPPORTED              1    //TODO: IDF-13138, re-open on c61 eco3
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1
#define SOC_EFUSE_SUPPORTED             1
#define SOC_I2S_SUPPORTED               1
#define SOC_GPSPI_SUPPORTED             1
#define SOC_I2C_SUPPORTED               1
#define SOC_LEDC_SUPPORTED              1
#define SOC_SYSTIMER_SUPPORTED          1
//  \#define SOC_SUPPORT_COEXISTENCE         1
#define SOC_SHA_SUPPORTED               1
#define SOC_ECC_SUPPORTED               1
#define SOC_ECC_EXTENDED_MODES_SUPPORTED   1
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
#define SOC_BOD_SUPPORTED               1
#define SOC_APM_SUPPORTED               1 /*!< Support for APM peripheral */
#define SOC_PMU_SUPPORTED               1
#define SOC_LP_TIMER_SUPPORTED          1
#define SOC_LP_AON_SUPPORTED            1
#define SOC_CLK_TREE_SUPPORTED          1
#define SOC_ASSIST_DEBUG_SUPPORTED      1
#define SOC_WDT_SUPPORTED               1
#define SOC_SPI_FLASH_SUPPORTED         1       //TODO: [ESP32C61] IDF-9314
//  \#define SOC_RNG_SUPPORTED               1    //TODO: [ESP32C61] IDF-9236
#define SOC_MODEM_CLOCK_SUPPORTED       1
#define SOC_REG_I2C_SUPPORTED           1
#define SOC_ETM_SUPPORTED               1
//  \#define SOC_SDIO_SLAVE_SUPPORTED        0
#define SOC_PAU_SUPPORTED               1
#define SOC_LIGHT_SLEEP_SUPPORTED       1
#define SOC_DEEP_SLEEP_SUPPORTED        1
#define SOC_PM_SUPPORTED                1
#define SOC_ECDSA_SUPPORTED             1
#define SOC_SPIRAM_SUPPORTED            1
/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M                        1
#define SOC_XTAL_CLOCK_PATH_DEPENDS_ON_TOP_DOMAIN   1

/*-------------------------- ADC CAPS -------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_IIR_FILTER_SUPPORTED        1
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        1    //Digital controller supported ADC unit
#define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_PERIPH_NUM                      (1U)
#define SOC_ADC_MAX_CHANNEL_NUM                 (4)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (4)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (1U)
#define SOC_ADC_PATT_LEN_MAX                    (8) /*!< Two pattern tables, each contains 4 items. Each item takes 1 byte */
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

// /*!< Calibration */  // TODO: [ESP32C61] IDF-9303
//  \#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/
//  \#define SOC_ADC_SELF_HW_CALI_SUPPORTED          (1) /*!< support HW offset self calibration */
//  \#define SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED (1) /*!< support channel compensation to the HW offset calibration */

/*!< Interrupt */
#define SOC_ADC_TEMPERATURE_SHARE_INTR          (1)

/*!< ADC power control is shared by PWDET */
#define SOC_ADC_SHARED_POWER                    1

/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (0)

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- RNG CAPS -----------------------------------*/
#define SOC_RNG_SUPPORTED            1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data
#define SOC_CACHE_WRITEBACK_SUPPORTED           1
#define SOC_CACHE_FREEZE_SUPPORTED              1

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               (1U)
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FLEXIBLE_INTC       1
#define SOC_CPU_SUPPORT_WFE             1
#define SOC_INT_PLIC_SUPPORTED          0       //riscv platform-level interrupt controller
#define SOC_INT_CLIC_SUPPORTED          1
#define SOC_INT_HW_NESTED_SUPPORTED     1       // Support for hardware interrupts nesting
#define SOC_BRANCH_PREDICTOR_SUPPORTED  1

#define SOC_CPU_BREAKPOINTS_NUM             4
#define SOC_CPU_WATCHPOINTS_NUM             4
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x80000000 // bytes

#define SOC_CPU_HAS_PMA                 1
#define SOC_CPU_IDRAM_SPLIT_USING_PMP   1
#define SOC_CPU_PMP_REGION_GRANULARITY  128

#define SOC_CPU_HAS_LOCKUP_RESET        1

/*-------------------------- DMA Common CAPS ----------------------------------------*/
#define SOC_DMA_CAN_ACCESS_FLASH 1 /*!< DMA can access Flash memory */

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_AHB_GDMA_VERSION            2U
#define SOC_GDMA_NUM_GROUPS_MAX         1U
#define SOC_GDMA_PAIRS_PER_GROUP_MAX    2
#define SOC_GDMA_SUPPORT_ETM            1  // Support ETM submodule
#define SOC_GDMA_SUPPORT_SLEEP_RETENTION    1

/*-------------------------- ETM CAPS --------------------------------------*/
#define SOC_ETM_GROUPS                  1U  // Number of ETM groups
#define SOC_ETM_CHANNELS_PER_GROUP      50  // Number of ETM channels in the group
#define SOC_ETM_SUPPORT_SLEEP_RETENTION 1   // Support sleep retention

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-C61 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 25
#define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1
#define SOC_GPIO_SUPPORT_PIN_HYS_FILTER    1

// GPIO peripheral has the ETM extension
#define SOC_GPIO_SUPPORT_ETM          1

// Target has the full LP IO subsystem
// On ESP32-C61, Digital IOs have their own registers to control pullup/down capability, independent of LP registers.
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT    (1)

// LP IO peripherals have independent clock gating to manage
#define SOC_LP_IO_CLOCK_IS_INDEPENDENT      (1)

#define SOC_GPIO_VALID_GPIO_MASK        ((1ULL<<SOC_GPIO_PIN_COUNT) - 1)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#define SOC_GPIO_IN_RANGE_MAX           24
#define SOC_GPIO_OUT_RANGE_MAX          24

// GPIO0~6 on ESP32C61 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP   (1)
#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6)
#define SOC_GPIO_DEEP_SLEEP_WAKE_SUPPORTED_PIN_CNT      (7)

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_7~GPIO_NUM_24)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x1FFFF80ULL

// Support to force hold all IOs
#define SOC_GPIO_SUPPORT_FORCE_HOLD              (1)
// "LP"_IOs and DIG_IOs can be hold during deep sleep and after waking up
#define SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP         (1)
// Support to hold a single digital I/O when the digital domain is powered off
#define SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP  (1)

// The Clock Out signal is route to the pin by GPIO matrix
//  \#define SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX    (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM           (3)

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT                 7
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED    1  /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                                 * so it supports unique IOMUX configuration (including IE, OE, PU, PD, DRV etc.)
                                                 * when the pins are switched to RTC function.
                                                 */
#define SOC_RTCIO_HOLD_SUPPORTED            1
#define SOC_RTCIO_WAKE_SUPPORTED            1
#define SOC_RTCIO_EDGE_WAKE_SUPPORTED       1

/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_PERIPH_ALWAYS_ENABLE  (1) /*!< The dedicated GPIO (a.k.a. fast GPIO) is featured by some customized CPU instructions, which is always enabled */

/*------------------------- Analog Comparator CAPS ---------------------------*/
#define SOC_ANA_CMPR_NUM                       (1U)
#define SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE      (1)  // Support positive/negative/any cross interrupt
#define SOC_ANA_CMPR_SUPPORT_ETM               (1)

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-C61 has 1 I2C
#define SOC_I2C_NUM                 (1U)
#define SOC_HP_I2C_NUM              (1U)

#define SOC_I2C_FIFO_LEN            (32) /*!< I2C hardware FIFO depth */
#define SOC_I2C_CMD_REG_NUM         (8)  /*!< Number of I2C command registers */
#define SOC_I2C_SUPPORT_SLAVE       (1)

#define SOC_I2C_SUPPORT_HW_FSM_RST  (1)
#define SOC_I2C_SUPPORT_XTAL        (1)
#define SOC_I2C_SUPPORT_RTC         (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR   (1)
#define SOC_I2C_SLAVE_SUPPORT_BROADCAST    (1)
#define SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE    (1)
#define SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS   (1)
#define SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH    (1)
#define SOC_I2C_SUPPORT_SLEEP_RETENTION        (1)

// /*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_NUM                     (1U)
#define SOC_I2S_HW_VERSION_2            (1)
#define SOC_I2S_SUPPORTS_ETM            (1)
#define SOC_I2S_SUPPORTS_XTAL           (1)
#define SOC_I2S_SUPPORTS_PLL_F160M      (1)
#define SOC_I2S_SUPPORTS_PLL_F120M      (1)
#define SOC_I2S_SUPPORTS_PCM            (1)
#define SOC_I2S_SUPPORTS_PDM            (1)
#define SOC_I2S_SUPPORTS_PDM_TX         (1)     // Support to output raw PDM format data
#define SOC_I2S_SUPPORTS_PCM2PDM        (1)     // Support to write PCM format but output PDM format data with the help of PCM to PDM filter
#define SOC_I2S_SUPPORTS_PDM_RX         (1)     // Support to input raw PDM format data
#define SOC_I2S_SUPPORTS_TX_SYNC_CNT    (1)
#define SOC_I2S_PDM_MAX_TX_LINES        (2)
#define SOC_I2S_PDM_MAX_RX_LINES        (1U)
#define SOC_I2S_SUPPORTS_TDM            (1)
#define SOC_I2S_TDM_FULL_DATA_WIDTH     (1)     /*!< No limitation to data bit width when using multiple slots */
#define SOC_I2S_SUPPORT_SLEEP_RETENTION (1)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK      (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK         (1)
#define SOC_LEDC_TIMER_NUM                  (4)
#define SOC_LEDC_CHANNEL_NUM                (6)
#define SOC_LEDC_TIMER_BIT_WIDTH            (20)
#define SOC_LEDC_SUPPORT_FADE_STOP          (1)
#define SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED (1)
#define SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX (16)
#define SOC_LEDC_FADE_PARAMS_BIT_WIDTH      (10)
#define SOC_LEDC_SUPPORT_SLEEP_RETENTION    (1)

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PAGE_SIZE_CONFIGURABLE        (1)
#define SOC_MMU_PAGE_SIZE_8KB_SUPPORTED       (1)
#define SOC_MMU_PERIPH_NUM                    (1U)
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM     (1U)
#define SOC_MMU_DI_VADDR_SHARED               (1) /*!< D/I vaddr are shared */

/*-------------------------- MPU CAPS ----------------------------------------*/
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*------------------------ USB SERIAL JTAG CAPS ------------------------------*/
//  \#define SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP     (1)     /*!< Support to maintain minimum usb communication during light sleep */ // TODO: IDF-6395

// TODO: IDF-5353 (Copy from esp32c3, need check)
/*--------------------------- SHA CAPS ---------------------------------------*/

/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SOC_SHA_DMA_MAX_BUFFER_SIZE     (3968)
#define SOC_SHA_SUPPORT_DMA             (1)

// /* The SHA engine is able to resume hashing from a user */
#define SOC_SHA_SUPPORT_RESUME          (1)

// /* Has a centralized DMA, which is shared with all peripherals */
#define SOC_SHA_GDMA             (1)

// /* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)

/*--------------------------- ECC CAPS ---------------------------------------*/
#define SOC_ECC_CONSTANT_TIME_POINT_MUL           1

/*--------------------------- ECDSA CAPS ---------------------------------------*/
#define SOC_ECDSA_SUPPORT_EXPORT_PUBKEY     (1)
#define SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE   (1)

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM                  2
#define SOC_SPI_PERIPH_CS_NUM(i)            6
#define SOC_SPI_MAX_CS_NUM                  6
#define SOC_SPI_MAX_PRE_DIVIDER             16
#define SOC_SPI_MAXIMUM_BUFFER_SIZE         64

#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1
#define SOC_SPI_SUPPORT_SLEEP_RETENTION     1
#define SOC_SPI_SUPPORT_CLK_XTAL            1
#define SOC_SPI_SUPPORT_CLK_PLL             1
#define SOC_SPI_SUPPORT_CLK_RC_FAST         1

// Peripheral supports DIO, DOUT, QIO, or QOUT
// host_id = 0 -> SPI0/SPI1, host_id = 1 -> SPI2,
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})
#define SOC_MEMSPI_IS_INDEPENDENT 1

/*-------------------------- SPIRAM CAPS ----------------------------------------*/
#define SOC_SPIRAM_XIP_SUPPORTED        1

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)
#define SOC_SPI_MEM_SUPPORT_WRAP                          (1)

#define SOC_MEMSPI_SRC_FREQ_80M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_40M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_20M_SUPPORTED         1

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

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (1U)
#define SOC_TIMER_GROUP_TOTAL_TIMERS      (2)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (54)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_SUPPORT_RC_FAST   (1)
#define SOC_TIMER_SUPPORT_SLEEP_RETENTION (1)
#define SOC_TIMER_SUPPORT_ETM             (1)

/*--------------------------- WATCHDOG CAPS ---------------------------------------*/
// #define SOC_MWDT_SUPPORT_XTAL              (1)
#define SOC_MWDT_SUPPORT_SLEEP_RETENTION   (1)

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_DOWNLOAD_ICACHE 0
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_SOFT_DIS_JTAG 0
#define SOC_EFUSE_DIS_ICACHE 1
#define SOC_EFUSE_ECDSA_KEY 1

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              0
#define SOC_SECURE_BOOT_V2_ECC              1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   1
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  1

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)
#define SOC_FLASH_ENCRYPTION_XTS_AES        1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128    1
#define SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND  1

/*-------------------------- APM CAPS ----------------------------------------*/
#define SOC_APM_CTRL_FILTER_SUPPORTED   1 /*!< Support for APM control filter */

/*------------------------ Anti DPA (Security) CAPS --------------------------*/
#define SOC_CRYPTO_DPA_PROTECTION_SUPPORTED     1

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-C61 has 3 UARTs (3 HP UART)
#define SOC_UART_NUM                    (3)
#define SOC_UART_HP_NUM                 (3)
#define SOC_UART_FIFO_LEN               (128)       /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX            (5000000)   /*!< Max bit rate supported by UART */
#define SOC_UART_SUPPORT_PLL_F80M_CLK   (1)         /*!< Support PLL_F80M as the clock source */
#define SOC_UART_SUPPORT_RTC_CLK        (1)         /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK       (1)         /*!< Support XTAL clock as the clock source */
#define SOC_UART_SUPPORT_WAKEUP_INT     (1)         /*!< Support UART wakeup interrupt */
#define SOC_UART_SUPPORT_SLEEP_RETENTION   (1)      /*!< Support back up registers before sleep */

// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)

#define SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN 5
#define SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE (1)
#define SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE   (1)
#define SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE     (1)
#define SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE      (1)

// TODO: IDF-5679 (Copy from esp32c3, need check)
/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*-------------------------- EXTERNAL COEXISTENCE CAPS -------------------------------------*/
#define SOC_EXTERNAL_COEX_ADVANCE              (1) /*!< HARDWARE ADVANCED EXTERNAL COEXISTENCE CAPS */
#define SOC_EXTERNAL_COEX_LEADER_TX_LINE       (0) /*!< EXTERNAL COEXISTENCE TX LINE CAPS */

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

// TODO: IDF-5679 (Copy from esp32c3, need check)
/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

// TODO: IDF-5351 (Copy from esp32c3, need check)
/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BEACON_WAKEUP    (1)
#define SOC_PM_SUPPORT_BT_WAKEUP        (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP      (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN   (1) /*!<Supports one bit per pin to configure the EXT1 trigger level */
#define SOC_PM_SUPPORT_CPU_PD           (1)
#define SOC_PM_SUPPORT_MODEM_PD         (1)
#define SOC_PM_SUPPORT_XTAL32K_PD       (1)
#define SOC_PM_SUPPORT_RC32K_PD         (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)
#define SOC_PM_SUPPORT_TOP_PD           (1)
#define SOC_PM_SUPPORT_HP_AON_PD        (1)
#define SOC_PM_SUPPORT_MAC_BB_PD        (1)
#define SOC_PM_SUPPORT_RTC_PERIPH_PD    (1)

#define SOC_PM_SUPPORT_PMU_MODEM_STATE  (1)
/* macro redefine for pass esp_wifi headers md5sum check */
#define MAC_SUPPORT_PMU_MODEM_STATE     SOC_PM_SUPPORT_PMU_MODEM_STATE

#define SOC_PM_SUPPORT_PMU_CLK_ICG          (1)

#define SOC_PM_CPU_RETENTION_BY_SW          (1)
#define SOC_PM_MODEM_RETENTION_BY_REGDMA    (1)
#define SOC_EXT_MEM_CACHE_TAG_IN_CPU_DOMAIN (1)

#define SOC_PM_PAU_LINK_NUM                 (4)
#define SOC_PM_PAU_REGDMA_LINK_MULTI_ADDR   (1)
#define SOC_PM_PAU_REGDMA_LINK_WIFIMAC      (1)

#define SOC_PM_PAU_REGDMA_UPDATE_CACHE_BEFORE_WAIT_COMPARE  (1)
#define SOC_PM_PMU_MIN_SLP_SLOW_CLK_CYCLE_FIXED    (1)

#define SOC_PM_RETENTION_MODULE_NUM         (32)

/*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)
#define SOC_MODEM_CLOCK_IS_INDEPENDENT            (1)

#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */
#define SOC_CLK_OSC_SLOW_SUPPORTED                (1)     /*!< Support to connect an external oscillator, not a crystal */
#define SOC_CLK_LP_FAST_SUPPORT_XTAL              (1)     /*!< Support XTAL clock as the LP_FAST clock source */
#define SOC_CLK_LP_FAST_SUPPORT_XTAL_D2           (1)     /*!< Support XTAL_D2 clock as the LP_FAST clock source */

#define SOC_RCC_IS_INDEPENDENT                    1       /*!< Reset and Clock Control is independent, thanks to the PCR registers */

/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC                (1)
#define SOC_TEMPERATURE_SENSOR_SUPPORT_XTAL                   (1)
#define SOC_TEMPERATURE_SENSOR_INTR_SUPPORT                   (1)
#define SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION        (1)
#define SOC_TEMPERATURE_SENSOR_UNDER_PD_TOP_DOMAIN            (1)

/*------------------------------------ WI-FI CAPS ------------------------------------*/
//TODO: IDF-13138, re-open on c61 eco3
// #define SOC_WIFI_HW_TSF                     (1)    /*!< Support hardware TSF */
// #define SOC_WIFI_FTM_SUPPORT                (0)    /*!< Support FTM */
// #define SOC_WIFI_GCMP_SUPPORT               (1)    /*!< Support GCMP(GCMP128 and GCMP256) */
// #define SOC_WIFI_WAPI_SUPPORT               (1)    /*!< Support WAPI */
// #define SOC_WIFI_CSI_SUPPORT                (1)    /*!< Support CSI */
// #define SOC_WIFI_MESH_SUPPORT               (1)    /*!< Support WIFI MESH */
// #define SOC_WIFI_HE_SUPPORT                 (1)    /*!< Support Wi-Fi 6 */
// #define SOC_WIFI_MAC_VERSION_NUM            (3)    /*!< Wi-Fi MAC version num is 3 */
// #define SOC_WIFI_NAN_SUPPORT                (1)    /*!< Support WIFI Aware (NAN) */

// /*---------------------------------- Bluetooth CAPS ----------------------------------*/
// TODO: IDF-13139, re-enable for c61 eco3
// #define SOC_BLE_SUPPORTED                   (1)    /*!< Support Bluetooth Low Energy hardware */
// #define SOC_BLE_MESH_SUPPORTED              (1)    /*!< Support BLE MESH */
// #define SOC_ESP_NIMBLE_CONTROLLER           (1)    /*!< Support BLE EMBEDDED controller V1 */
// #define SOC_BLE_50_SUPPORTED                (1)    /*!< Support Bluetooth 5.0 */
// #define SOC_BLE_DEVICE_PRIVACY_SUPPORTED    (1)   /*!< Support BLE device privacy mode */
// #define SOC_BLE_POWER_CONTROL_SUPPORTED     (1)    /*!< Support Bluetooth Power Control */
// #define SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED  (1)    /*!< Support For BLE Periodic Adv Enhancements */
// #define SOC_BLUFI_SUPPORTED                 (1)    /*!< Support BLUFI */
// #define SOC_BLE_MULTI_CONN_OPTIMIZATION     (1)    /*!< Support multiple connections optimization */
// #define SOC_BLE_CTE_SUPPORTED               (1)    /*!< Support Bluetooth LE Constant Tone Extension (CTE) */

/*------------------------------------- PHY CAPS -------------------------------------*/
// #define SOC_PHY_COMBO_MODULE                  (1) /*!< Support Wi-Fi, BLE and 15.4*/

/*------------------------------------- No Reset CAPS -------------------------------------*/
//  \#define SOC_CAPS_NO_RESET_BY_ANA_BOD           (1)   //TODO: [ESP32C61] IDF-9254
