/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_UART_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_GPTIMER_SUPPORTED           1
#define SOC_PHY_SUPPORTED               1
#define SOC_BT_SUPPORTED                1
#define SOC_WIFI_SUPPORTED              1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     0
#define SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK 1
#define SOC_EFUSE_SUPPORTED             1
#define SOC_TEMP_SENSOR_SUPPORTED       1
#define SOC_LEDC_SUPPORTED              1
#define SOC_I2C_SUPPORTED               1
#define SOC_GPSPI_SUPPORTED             1
#define SOC_SHA_SUPPORTED               1
#define SOC_ECC_SUPPORTED               1
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
#define SOC_SYSTIMER_SUPPORTED          1
#define SOC_BOD_SUPPORTED               1
#define SOC_CLK_TREE_SUPPORTED          1
#define SOC_ASSIST_DEBUG_SUPPORTED      1
#define SOC_WDT_SUPPORTED               1
#define SOC_SPI_FLASH_SUPPORTED         1
#define SOC_RNG_SUPPORTED               1
#define SOC_LIGHT_SLEEP_SUPPORTED       1
#define SOC_DEEP_SLEEP_SUPPORTED        1
#define SOC_LP_PERIPH_SHARE_INTERRUPT   1   // LP peripherals sharing the same interrupt source

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_26M            1
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- ADC CAPS -------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_IIR_FILTER_SUPPORTED        1
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        1    //Digital controller supported ADC unit
#define SOC_ADC_PERIPH_NUM                      (1U)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (5)
#define SOC_ADC_MAX_CHANNEL_NUM                 (5)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (1U)
#define SOC_ADC_PATT_LEN_MAX                    (8) /*!< One pattern table, each contains 8 items. Each item takes 1 byte */
#define SOC_ADC_DIGI_MIN_BITWIDTH               (12)
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_IIR_FILTER_NUM             (2)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interval<= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_RTC_MIN_BITWIDTH                (12)
#define SOC_ADC_RTC_MAX_BITWIDTH                (12)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/
#define SOC_ADC_SELF_HW_CALI_SUPPORTED          (1) /*!< support HW offset self calibration */

/*!< ADC power control is shared by PWDET, TempSensor */
#define SOC_ADC_SHARED_POWER                    1

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               (1U)
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FLEXIBLE_INTC       1
#define SOC_CPU_HAS_CSR_PC              1

#define SOC_CPU_BREAKPOINTS_NUM             2
#define SOC_CPU_WATCHPOINTS_NUM             2
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x80000000 // bytes

#define SOC_CPU_IDRAM_SPLIT_USING_PMP   1

/*-------------------------- ECC CAPS --------------------------*/
#define SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK  1  // C2 ECC peripheral has a bug in ECC point verification, if value of K is zero the verification fails

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_AHB_GDMA_VERSION            1U
#define SOC_GDMA_NUM_GROUPS_MAX         1U
#define SOC_GDMA_PAIRS_PER_GROUP_MAX    1U

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-C2 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 21
#define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1
#define SOC_GPIO_FILTER_CLK_SUPPORT_APB    1

// Target has no full RTC IO subsystem, GPIO0~5 remain RTC function (powered by VDD3V3_RTC, and can be used as deep-sleep wakeup pins)

// Force hold is a new function of ESP32-C2
#define SOC_GPIO_SUPPORT_FORCE_HOLD         (1)
// GPIO0~5 on ESP32-C2 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP   (1)

#define SOC_GPIO_VALID_GPIO_MASK        ((1U<<SOC_GPIO_PIN_COUNT) - 1)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#define SOC_GPIO_IN_RANGE_MAX           20
#define SOC_GPIO_OUT_RANGE_MAX          20

#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5)

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_6~GPIO_NUM_20)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x00000000001FFFC0ULL

// The Clock Out signal is route to the pin by GPIO matrix
#define SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX    (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM       (3)

/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_PERIPH_ALWAYS_ENABLE  (1) /*!< The dedicated GPIO (a.k.a. fast GPIO) is featured by some customized CPU instructions, which is always enabled */

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-C2 has 1 I2C
#define SOC_I2C_NUM                 (1U)
#define SOC_HP_I2C_NUM              (1U)

#define SOC_I2C_FIFO_LEN            (16) /*!< I2C hardware FIFO depth */
#define SOC_I2C_CMD_REG_NUM         (8)  /*!< Number of I2C command registers */

// FSM_RST only resets the FSM, not using it. So SOC_I2C_SUPPORT_HW_FSM_RST not defined.
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_XTAL        (1)
#define SOC_I2C_SUPPORT_RTC         (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR  (1)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK  (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK     (1)
#define SOC_LEDC_CHANNEL_NUM            (6)
#define SOC_LEDC_TIMER_BIT_WIDTH        (14)
#define SOC_LEDC_SUPPORT_FADE_STOP      (1)

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PAGE_SIZE_CONFIGURABLE          (1)
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM       (1U)
#define SOC_MMU_PERIPH_NUM                      (1U)

/*-------------------------- MPU CAPS ----------------------------------------*/
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- RTC CAPS --------------------------------------*/
#define SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH       (128)
#define SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM        (108)
#define SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN      (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)
#define SOC_RTC_CNTL_CPU_PD_DMA_BLOCK_SIZE      (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)

#define SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE  (SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM * (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3))

/*-------------------------- RTCIO CAPS --------------------------------------*/
/* No dedicated RTCIO subsystem on ESP32-C2. RTC functions are still supported
 * for hold, wake & 32kHz crystal functions - via rtc_cntl_reg */
#define SOC_RTCIO_PIN_COUNT    (0U)

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (3072)

/*--------------------------- SHA CAPS ---------------------------------------*/

/* Due to very limited availability of the DMA channels, DMA support for the SHA peripheral is disabled */
// #define SOC_SHA_SUPPORT_DMA             (1)

/* The SHA engine is able to resume hashing from a user */
#define SOC_SHA_SUPPORT_RESUME          (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM          2
#define SOC_SPI_PERIPH_CS_NUM(i)    6
#define SOC_SPI_MAX_CS_NUM          6

#define SOC_SPI_MAXIMUM_BUFFER_SIZE     64

#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_CONTINUOUS_TRANS    1
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1
#define SOC_SPI_SUPPORT_CLK_XTAL            1
#define SOC_SPI_SUPPORT_CLK_PLL_F40M        1

// Peripheral supports DIO, DOUT, QIO, or QOUT
// host_id = 0 -> SPI0/SPI1, host_id = 1 -> SPI2,
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})

// Peripheral supports output given level during its "dummy phase"
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUT 1

#define SOC_SPI_SCT_SUPPORTED                     1
#define SOC_SPI_SCT_SUPPORTED_PERIPH(PERIPH_NUM)  ((PERIPH_NUM==1) ? 1 : 0)    //Support Segmented-Configure-Transfer
#define SOC_SPI_SCT_REG_NUM                       14
#define SOC_SPI_SCT_BUFFER_NUM_MAX                (1 + SOC_SPI_SCT_REG_NUM)  //1-word-bitmap + 14-word-regs
#define SOC_SPI_SCT_CONF_BITLEN_MAX               0x3FFFA       //18 bits wide reg

#define SOC_MEMSPI_IS_INDEPENDENT 1
#define SOC_SPI_MAX_PRE_DIVIDER 16

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)
#define SOC_SPI_MEM_SUPPORT_WRAP                          (1)

#define SOC_MEMSPI_SRC_FREQ_60M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_30M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_20M_SUPPORTED         1
#define SOC_MEMSPI_SRC_FREQ_15M_SUPPORTED         1

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM            2  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM              3  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO           32 // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI           20 // Bit width of systimer high part
#define SOC_SYSTIMER_FIXED_DIVIDER          1  // Clock source divider is fixed: 2.5
#define SOC_SYSTIMER_INT_LEVEL              1  // Systimer peripheral uses level interrupt
#define SOC_SYSTIMER_ALARM_MISS_COMPENSATE  1  // Systimer peripheral can generate interrupt immediately if t(target) > t(current)

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_GROUPS                  (1U)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (1U)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (54)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS      (1U)

/*--------------------------- WATCHDOG CAPS ---------------------------------------*/
#define SOC_MWDT_SUPPORT_XTAL              (1)

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_DOWNLOAD_ICACHE 1
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_ECC              1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   (1U)

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (32)
#define SOC_FLASH_ENCRYPTION_XTS_AES             1
#define SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS     1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128         1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128_DERIVED 1

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-C2 has 2 UARTs
#define SOC_UART_NUM                (2)
#define SOC_UART_HP_NUM             (2)
#define SOC_UART_FIFO_LEN           (128)      /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (2500000)  /*!< Max bit rate supported by UART */
#define SOC_UART_SUPPORT_WAKEUP_INT (1)         /*!< Support UART wakeup interrupt */
#define SOC_UART_SUPPORT_PLL_F40M_CLK (1)   /*!< Support APB as the clock source */
#define SOC_UART_SUPPORT_RTC_CLK    (1)     /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK   (1)     /*!< Support XTAL clock as the clock source */

// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)

/*-------------------------- COEXISTENCE CAPS -------------------------------*/
#define SOC_SUPPORT_COEXISTENCE     (1)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*-------------------------- EXTERNAL COEXISTENCE CAPS -------------------------------------*/
#define SOC_EXTERNAL_COEX_ADVANCE              (1) /*!< HARDWARE ADVANCED EXTERNAL COEXISTENCE CAPS */
#define SOC_EXTERNAL_COEX_LEADER_TX_LINE       (0) /*!< EXTERNAL COEXISTENCE TX LINE CAPS */

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BT_WAKEUP        (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)

/*--------------------------- CLOCK SUBSYSTEM CAPS -------------------------- */
#define SOC_CLK_RC_FAST_D256_SUPPORTED            (1)
#define SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256     (1)
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_OSC_SLOW_SUPPORTED                (1)     /*!< ESP32C2 only supports to connect an external oscillator, not a crystal */

/*------------------------------------ WI-FI CAPS ------------------------------------*/
#define SOC_WIFI_HW_TSF                           (1)    /*!< Support hardware TSF */
#define SOC_WIFI_FTM_SUPPORT                      (1)    /*!< Support FTM */
#define SOC_WIFI_SUPPORT_VARIABLE_BEACON_WINDOW   (1)    /*!< Support delta early time for rf phy on/off */
#define SOC_WIFI_PHY_NEEDS_USB_WORKAROUND         (1)    /*!< SoC has WiFi and USB PHYs interference, needs a workaround */

/*---------------------------------- Bluetooth CAPS ----------------------------------*/
#define SOC_BLE_SUPPORTED               (1)    /*!< Support Bluetooth Low Energy hardware */
#define SOC_BLE_MESH_SUPPORTED          (0)    /*!< Support BLE MESH */
#define SOC_ESP_NIMBLE_CONTROLLER       (1)    /*!< Support BLE EMBEDDED controller V1 */
#define SOC_BLE_50_SUPPORTED            (1)    /*!< Support Bluetooth 5.0 */
#define SOC_BLE_DEVICE_PRIVACY_SUPPORTED (1)   /*!< Support BLE device privacy mode */
#define SOC_BLUFI_SUPPORTED             (1)    /*!< Support BLUFI */
#define SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED  (1)    /*!< Support For BLE Periodic Adv Enhancements */

/*------------------------------------- PHY CAPS -------------------------------------*/
#define SOC_PHY_IMPROVE_RX_11B              (1)
#define SOC_PHY_COMBO_MODULE                  (1) /*!< Support Wi-Fi and BLE*/
