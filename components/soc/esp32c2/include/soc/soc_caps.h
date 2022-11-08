/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

/*
 * These defines are parsed and imported as kconfig variables via the script
 * `tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py`
 *
 * If this file is changed the script will automatically run the script
 * and generate the kconfig variables as part of the pre-commit hooks.
 *
 * It can also be ran manually with `./tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py 'components/soc/esp32c3/include/soc/'`
 *
 * For more information see `tools/gen_soc_caps_kconfig/README.md`
 *
*/

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_ADC_SUPPORTED               1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_GDMA_SUPPORTED              1
#define SOC_BT_SUPPORTED                1
#define SOC_WIFI_SUPPORTED              1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     0
#define SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK 1
#define SOC_TEMP_SENSOR_SUPPORTED       1
#define SOC_LEDC_SUPPORTED              1
#define SOC_I2C_SUPPORTED               1
#define SOC_SHA_SUPPORTED               1
#define SOC_ECC_SUPPORTED               1
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
#define SOC_SYSTIMER_SUPPORTED          1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_26M            1
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- ADC CAPS -------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_FILTER_SUPPORTED                1
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_PERIPH_NUM                      (1U)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (5)
#define SOC_ADC_MAX_CHANNEL_NUM                 (5)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (1U)
#define SOC_ADC_PATT_LEN_MAX                    (8) /*!< One pattern table, each contains 8 items. Each item takes 1 byte */
#define SOC_ADC_DIGI_MIN_BITWIDTH               (12)
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_FILTER_NUM                 (2)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interva <= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_RTC_MIN_BITWIDTH                (12)
#define SOC_ADC_RTC_MAX_BITWIDTH                (12)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               (1U)
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FLEXIBLE_INTC       1

#define SOC_CPU_BREAKPOINTS_NUM         2
#define SOC_CPU_WATCHPOINTS_NUM         2
#define SOC_CPU_WATCHPOINT_SIZE         0x80000000 // bytes

#define SOC_CPU_IDRAM_SPLIT_USING_PMP   1

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PAGE_SIZE_CONFIGURABLE  (1)

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_GDMA_GROUPS                 (1U) // Number of GDMA groups
#define SOC_GDMA_PAIRS_PER_GROUP        (1U) // Number of GDMA pairs in each group
#define SOC_GDMA_TX_RX_SHARE_INTERRUPT  (1)  // TX and RX channel in the same pair will share the same interrupt source number

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-C2 has 1 GPIO peripheral
#define SOC_GPIO_PORT               (1U)
#define SOC_GPIO_PIN_COUNT          (21)

// Target has no full RTC IO subsystem, so GPIO is 100% "independent" of RTC
// On ESP32-C2, Digital IOs have their own registers to control pullup/down capability, independent of RTC registers.
#define SOC_GPIO_SUPPORTS_RTC_INDEPENDENT   (1)
// Force hold is a new function of ESP32-C2
#define SOC_GPIO_SUPPORT_FORCE_HOLD         (1)
// GPIO0~5 on ESP32-C2 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP   (1)

#define SOC_GPIO_VALID_GPIO_MASK        ((1U<<SOC_GPIO_PIN_COUNT) - 1)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK
#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5)

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_6~GPIO_NUM_20)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x00000000001FFFC0ULL

// Support to configure sleep status
#define SOC_GPIO_SUPPORT_SLP_SWITCH  (1)

/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_PERIPH_ALWAYS_ENABLE  (1) /*!< The dedicated GPIO (a.k.a. fast GPIO) is featured by some customized CPU instructions, which is always enabled */

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-C2 has 1 I2C
#define SOC_I2C_NUM                 (1U)

#define SOC_I2C_FIFO_LEN            (16) /*!< I2C hardware FIFO depth */

// FSM_RST only resets the FSM, not using it. So SOC_I2C_SUPPORT_HW_FSM_RST not defined.
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_XTAL        (1)
#define SOC_I2C_SUPPORT_RTC         (1)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK  (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK     (1)
#define SOC_LEDC_CHANNEL_NUM            (6)
#define SOC_LEDC_TIMER_BIT_WIDE_NUM     (14)
#define SOC_LEDC_SUPPORT_FADE_STOP      (1)

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

// Peripheral supports DIO, DOUT, QIO, or QOUT
// host_id = 0 -> SPI0/SPI1, host_id = 1 -> SPI2,
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})

// Peripheral supports output given level during its "dummy phase"
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUT 1

#define SOC_MEMSPI_IS_INDEPENDENT 1
#define SOC_SPI_MAX_PRE_DIVIDER 16

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)

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
#define SOC_TIMER_GROUP_SUPPORT_PLL_F40M  (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS      (1U)

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
#define SOC_UART_FIFO_LEN           (128)      /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)  /*!< Max bit rate supported by UART */
#define SOC_UART_SUPPORT_WAKEUP_INT (1)         /*!< Support UART wakeup interrupt */
#define SOC_UART_SUPPORT_PLL_F40M_CLK (1)   /*!< Support APB as the clock source */
#define SOC_UART_SUPPORT_RTC_CLK    (1)     /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK   (1)     /*!< Support XTAL clock as the clock source */

// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*-------------------------- HARDWARE ADVANCED EXTERNAL COEXISTENCE CAPS -------------------*/
#define SOC_EXTERNAL_COEX_ADVANCE       (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)
#define SOC_MAC_BB_PD_MEM_SIZE          (192*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BT_WAKEUP        (1)
#define SOC_PM_SUPPORT_CPU_PD           (0)
#define SOC_PM_SUPPORT_WIFI_PD          (0)
#define SOC_PM_SUPPORT_BT_PD            (0)

/*--------------------------- CLOCK SUBSYSTEM CAPS -------------------------- */
#define SOC_CLK_RC_FAST_D256_SUPPORTED            (1)
#define SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256     (1)

/*------------------------------------ WI-FI CAPS ------------------------------------*/
#define SOC_WIFI_HW_TSF                 (1)    /*!< Support hardware TSF */
#define SOC_WIFI_FTM_SUPPORT            (0)    /*!< FTM is not supported */
#define SOC_WIFI_GCMP_SUPPORT           (0)    /*!< GCMP is not supported(GCMP128 and GCMP256) */
#define SOC_WIFI_WAPI_SUPPORT           (0)    /*!< WAPI is not supported */
#define SOC_WIFI_CSI_SUPPORT            (0)    /*!< CSI is not supported */
#define SOC_WIFI_MESH_SUPPORT           (0)    /*!< WIFI MESH is not supported */

/*---------------------------------- Bluetooth CAPS ----------------------------------*/
#define SOC_BLE_SUPPORTED               (1)    /*!< Support Bluetooth Low Energy hardware */
#define SOC_BLE_MESH_SUPPORTED          (0)    /*!< Support BLE MESH */
#define SOC_ESP_NIMBLE_CONTROLLER       (1)    /*!< Support BLE EMBEDDED controller V1 */
