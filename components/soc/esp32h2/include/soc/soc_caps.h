/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// separate these information into periph_caps.h for each peripheral and
// include them here.

/*
 * These defines are parsed and imported as kconfig variables via the script
 * `tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py`
 *
 * If this file is changed the script will automatically run the script
 * and generate the kconfig variables as part of the pre-commit hooks.
 *
 * It can also be ran manually with `./tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py 'components/soc/esp32h2/include/soc/'`
 *
 * For more information see `tools/gen_soc_caps_kconfig/README.md`
 *
*/

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
// #define SOC_ADC_SUPPORTED               1 // TODO: IDF-6214
// #define SOC_DEDICATED_GPIO_SUPPORTED    1 // TODO: IDF-6241
#define SOC_GDMA_SUPPORTED              1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_PCNT_SUPPORTED              1
// #define SOC_MCPWM_SUPPORTED             1 // TODO: IDF-6237
// #define SOC_TWAI_SUPPORTED              1 // TODO: IDF-6217
// #define SOC_BT_SUPPORTED                1 // TODO: IDF-6416
// #define SOC_IEEE802154_SUPPORTED        1 // TODO: IDF-6577
#define SOC_GPTIMER_SUPPORTED              1
#define SOC_IEEE802154_BLE_ONLY            1
// #define SOC_USB_SERIAL_JTAG_SUPPORTED   1 // TODO: IDF-6239
// #define SOC_TEMP_SENSOR_SUPPORTED       1 // TODO: IDF-6229
// #define SOC_SUPPORTS_SECURE_DL_MODE     1 // TODO: IDF-6281
//#define SOC_RISCV_COPROC_SUPPORTED       1 // TODO: IDF-6272
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1
#define SOC_EFUSE_HAS_EFUSE_RST_BUG     1
#define SOC_RTC_FAST_MEM_SUPPORTED      1
#define SOC_RTC_MEM_SUPPORTED           1
#define SOC_I2S_SUPPORTED               1
#define SOC_SDM_SUPPORTED               1
#define SOC_RMT_SUPPORTED               1
// #define SOC_GPSPI_SUPPORTED             1 // TODO: IDF-6264
#define SOC_SYSTIMER_SUPPORTED          1
// #define SOC_SUPPORT_COEXISTENCE         1 // TODO: IDF-6416
// #define SOC_AES_SUPPORTED               1 // TODO: IDF-6280
// #define SOC_MPI_SUPPORTED               1 // TODO: IDF-6415
// #define SOC_SHA_SUPPORTED               1 // TODO: IDF-6275
// #define SOC_HMAC_SUPPORTED              1 // TODO: IDF-6279
// #define SOC_DIG_SIGN_SUPPORTED          1 // TODO: IDF-6285
// #define SOC_FLASH_ENC_SUPPORTED         1 // TODO: IDF-6282
// #define SOC_SECURE_BOOT_SUPPORTED       1 // TODO: IDF-6281
#define SOC_BOD_SUPPORTED               1
#define SOC_APM_SUPPORTED               1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_32M            1

// TODO: IDF-6280 (Copy from esp32c6, need check)
/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GDMA            (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

// TODO: IDF-6215 (Copy from esp32c6, need check)
/*-------------------------- ADC CAPS -------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_ARBITER_SUPPORTED               1
#define SOC_ADC_FILTER_SUPPORTED                1
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_PERIPH_NUM                      (1U)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         ((PERIPH_NUM==0)? 5 : 1)
#define SOC_ADC_MAX_CHANNEL_NUM                 (5)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (1U)
#define SOC_ADC_PATT_LEN_MAX                    (8) /*!< One pattern table, each contains 8 items. Each item takes 1 byte */
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_FILTER_NUM                 (2)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interval<= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_RTC_MIN_BITWIDTH                (12)
#define SOC_ADC_RTC_MAX_BITWIDTH                (12)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (0) /*!< support HW offset calibration version 1*/

// ESP32H2-TODO: Copy from esp32c6, need check
/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (0)

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_SHARED_IDCACHE_SUPPORTED            1   //Shared Cache for both instructions and data

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               (1U)
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FLEXIBLE_INTC       1
#define SOC_INT_PLIC_SUPPORTED          1       //riscv platform-level interrupt controller

#define SOC_CPU_BREAKPOINTS_NUM         4
#define SOC_CPU_WATCHPOINTS_NUM         4
#define SOC_CPU_WATCHPOINT_SIZE         0x80000000 // bytes

// TODO: IDF-6370 (Copy from esp32c6, need check)
/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PAGE_SIZE_CONFIGURABLE  (1)

// TODO: IDF-6285 (Copy from esp32c6, need check)
/*-------------------------- DIGITAL SIGNATURE CAPS ----------------------------------------*/
/** The maximum length of a Digital Signature in bits. */
#define SOC_DS_SIGNATURE_MAX_BIT_LEN (3072)

/** Initialization vector (IV) length for the RSA key parameter message digest (MD) in bytes. */
#define SOC_DS_KEY_PARAM_MD_IV_LENGTH (16)

/** Maximum wait time for DS parameter decryption key. If overdue, then key error.
    See TRM DS chapter for more details */
#define SOC_DS_KEY_CHECK_MAX_WAIT_US (1100)

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_GDMA_GROUPS                 (1U) // Number of GDMA groups
#define SOC_GDMA_PAIRS_PER_GROUP        (3)  // Number of GDMA pairs in each group
#define SOC_GDMA_SUPPORT_ETM            (1)  // Support ETM submodule

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-H2 has 1 GPIO peripheral
#define SOC_GPIO_PORT               (1U)
#define SOC_GPIO_PIN_COUNT          (28)

// Target has no full LP IO subsystem, GPIO7~14 remain LP function (powered by VDD3V3_LP, and can be used as deep-sleep wakeup pins)

// GPIO7~14 on ESP32H2 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP   (1)

#define SOC_GPIO_VALID_GPIO_MASK        ((1U<<SOC_GPIO_PIN_COUNT) - 1)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK
#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | BIT7 | BIT8 | BIT9 | BIT10 | BIT11 | BIT12 | BIT13 | BIT14)

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_0~6. GPIO_NUM_15~27)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x000000000FFF807FULL

// Support to configure sleep status
#define SOC_GPIO_SUPPORT_SLP_SWITCH  (1)

// TODO: IDF-6241
/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_PERIPH_ALWAYS_ENABLE  (1) /*!< The dedicated GPIO (a.k.a. fast GPIO) is featured by some customized CPU instructions, which is always enabled */

// TODO: IDF-6233
/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-C6 has 1 I2C
#define SOC_I2C_NUM                 (1U)

#define SOC_I2C_FIFO_LEN            (32) /*!< I2C hardware FIFO depth */
// #define SOC_I2C_SUPPORT_SLAVE       (1)

// FSM_RST only resets the FSM, not using it. So SOC_I2C_SUPPORT_HW_FSM_RST not defined.
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_XTAL        (1)
#define SOC_I2C_SUPPORT_RTC         (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_NUM                 (1U)
#define SOC_I2S_HW_VERSION_2        (1)
#define SOC_I2S_SUPPORTS_XTAL       (1)
#define SOC_I2S_SUPPORTS_PLL_F96M   (1)
#define SOC_I2S_SUPPORTS_PLL_F64M   (1)
#define SOC_I2S_SUPPORTS_PCM        (1)
#define SOC_I2S_SUPPORTS_PDM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)
#define SOC_I2S_PDM_MAX_TX_LINES    (2)
#define SOC_I2S_SUPPORTS_TDM        (1)
#define SOC_I2S_TDM_FULL_DATA_WIDTH (1)  /*!< No limitation to data bit width when using multiple slots */

// TODO: IDF-6235 (Copy from esp32c6, need check)
/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK      (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK         (1)
#define SOC_LEDC_CHANNEL_NUM                (6)
#define SOC_LEDC_TIMER_BIT_WIDTH            (20)
#define SOC_LEDC_SUPPORT_FADE_STOP          (1)
#define SOC_LEDC_GAMMA_FADE_RANGE_MAX       (16)

// TODO: IDF-6332 (Copy from esp32c6, need check)
/*-------------------------- MPU CAPS ----------------------------------------*/
#define SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED    0
#define SOC_MPU_MIN_REGION_SIZE                   0x20000000U
#define SOC_MPU_REGIONS_MAX_NUM                   8
#define SOC_MPU_REGION_RO_SUPPORTED               0
#define SOC_MPU_REGION_WO_SUPPORTED               0

/*-------------------------- PCNT CAPS ---------------------------------------*/
#define SOC_PCNT_GROUPS                       1U
#define SOC_PCNT_UNITS_PER_GROUP              4
#define SOC_PCNT_CHANNELS_PER_UNIT            2
#define SOC_PCNT_THRES_POINT_PER_UNIT         2
#define SOC_PCNT_SUPPORT_RUNTIME_THRES_UPDATE 1

/*--------------------------- RMT CAPS ---------------------------------------*/
#define SOC_RMT_GROUPS                        1U /*!< One RMT group */
#define SOC_RMT_TX_CANDIDATES_PER_GROUP       2  /*!< Number of channels that capable of Transmit */
#define SOC_RMT_RX_CANDIDATES_PER_GROUP       2  /*!< Number of channels that capable of Receive */
#define SOC_RMT_CHANNELS_PER_GROUP            4  /*!< Total 4 channels */
#define SOC_RMT_MEM_WORDS_PER_CHANNEL         48 /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_PINGPONG           1  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_RX_DEMODULATION       1  /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_ASYNC_STOP         1  /*!< Support stop transmission asynchronously */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT         1  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP     1  /*!< Hardware support of auto-stop in loop mode */
#define SOC_RMT_SUPPORT_TX_SYNCHRO            1  /*!< Support coordinate a group of TX channels to start simultaneously */
#define SOC_RMT_SUPPORT_TX_CARRIER_DATA_ONLY  1  /*!< TX carrier can be modulated to data phase only */
#define SOC_RMT_SUPPORT_XTAL                  1  /*!< Support set XTAL clock as the RMT clock source */
// #define SOC_RMT_SUPPORT_RC_FAST               1  /*!< Support set RC_FAST as the RMT clock source */

// TODO: IDF-6237
/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_GROUPS                     (1U)   ///< 1 MCPWM groups on the chip (i.e., the number of independent MCPWM peripherals)
#define SOC_MCPWM_TIMERS_PER_GROUP           (3)    ///< The number of timers that each group has
#define SOC_MCPWM_OPERATORS_PER_GROUP        (3)    ///< The number of operators that each group has
#define SOC_MCPWM_COMPARATORS_PER_OPERATOR   (2)    ///< The number of comparators that each operator has
#define SOC_MCPWM_GENERATORS_PER_OPERATOR    (2)    ///< The number of generators that each operator has
#define SOC_MCPWM_TRIGGERS_PER_OPERATOR      (2)    ///< The number of triggers that each operator has
#define SOC_MCPWM_GPIO_FAULTS_PER_GROUP      (3)    ///< The number of fault signal detectors that each group has
#define SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP   (1)    ///< The number of capture timers that each group has
#define SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER (3)    ///< The number of capture channels that each capture timer has
#define SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP    (3)    ///< The number of GPIO synchros that each group has
#define SOC_MCPWM_SWSYNC_CAN_PROPAGATE       (1)    ///< Software sync event can be routed to its output
#define SOC_MCPWM_SUPPORT_ETM                (1)    ///< Support ETM (Event Task Matrix)
#define SOC_MCPWM_CAPTURE_CLK_FROM_GROUP  (1)    ///< Capture timer shares clock with other PWM timers
#define SOC_MCPWM_CLK_SUPPORT_PLL160M        (1)    ///< Support PLL160M as clock source
#define SOC_MCPWM_CLK_SUPPORT_XTAL           (1)    ///< Support XTAL as clock source

// TODO: IDF-6267 (Copy from esp32c6, need check)
/*-------------------------- RTC CAPS --------------------------------------*/
#define SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH       (128)
#define SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM        (108)
#define SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN      (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)
#define SOC_RTC_CNTL_CPU_PD_DMA_BLOCK_SIZE      (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)

#define SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE  (SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM * (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3))

/*-------------------------- RTCIO CAPS --------------------------------------*/
/* No dedicated RTCIO subsystem on ESP32-C6. RTC functions are still supported
 * for hold, wake & 32kHz crystal functions - via rtc_cntl_reg */
#define SOC_RTCIO_PIN_COUNT    (0U)

// TODO: IDF-6284 (Copy from esp32c6, need check)
/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (3072)

// TODO: IDF-6275 (Copy from esp32c6, need check)
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

/*-------------------------- Sigma Delta Modulator CAPS -----------------*/
#define SOC_SDM_GROUPS               1U
#define SOC_SDM_CHANNELS_PER_GROUP   4
#define SOC_SDM_CLK_SUPPORT_PLL_F48M 1
#define SOC_SDM_CLK_SUPPORT_XTAL     1

// TODO: IDF-6245 (Copy from esp32c6, need check)
/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM          2
#define SOC_SPI_PERIPH_CS_NUM(i)    6
#define SOC_SPI_MAX_CS_NUM          6

#define SOC_SPI_MAXIMUM_BUFFER_SIZE     64

#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_CONTINUOUS_TRANS    1
//#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1 //TODO: IDF-6247
#define SOC_SPI_SUPPORT_CLK_PLL_F48M        1

// Peripheral supports DIO, DOUT, QIO, or QOUT
// host_id = 0 -> SPI0/SPI1, host_id = 1 -> SPI2,
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})

#define SOC_MEMSPI_IS_INDEPENDENT 1
#define SOC_SPI_MAX_PRE_DIVIDER 16

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)

#define SOC_MEMSPI_SRC_FREQ_48M_SUPPORTED         1

/*-------------------------- SYSTIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM            2  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM              3  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO           32 // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI           20 // Bit width of systimer high part
#define SOC_SYSTIMER_FIXED_DIVIDER          1  // Clock source divider is fixed to 2 when clock source is XTAL
#define SOC_SYSTIMER_SUPPORT_RC_FAST        1  // Systimer can use RC_FAST clock source
#define SOC_SYSTIMER_INT_LEVEL              1  // Systimer peripheral uses level interrupt
#define SOC_SYSTIMER_ALARM_MISS_COMPENSATE  1  // Systimer peripheral can generate interrupt immediately if t(target) > t(current)
#define SOC_SYSTIMER_SUPPORT_ETM            1  // Systimer comparator can generate ETM event

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (1U)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (54)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_SUPPORT_PLL_F48M  (1)
// #define SOC_TIMER_GROUP_SUPPORT_RC_FAST   (1) // TODO: IDF-6265
#define SOC_TIMER_GROUP_TOTAL_TIMERS      (2)
// #define SOC_TIMER_SUPPORT_ETM             (1)

// TODO: IDF-6217 (Copy from esp32c6, need check)
/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM         1
#define SOC_TWAI_CLK_SUPPORT_XTAL       1
#define SOC_TWAI_BRP_MIN                2
#define SOC_TWAI_BRP_MAX                32768
#define SOC_TWAI_SUPPORTS_RX_STATUS     1

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1

// TODO: IDF-6281 (Copy from esp32c6, need check)
/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              1
#define SOC_SECURE_BOOT_V2_ECC              1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   1
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  1

// TODO: IDF-6282 (Copy from esp32c6, need check)
/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (32)
#define SOC_FLASH_ENCRYPTION_XTS_AES        1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128    1

// TODO:    IDF-6332 (Copy from esp32c6, need check)
/*-------------------------- MEMPROT CAPS ------------------------------------*/
#define SOC_MEMPROT_CPU_PREFETCH_PAD_SIZE   16
#define SOC_MEMPROT_MEM_ALIGN_SIZE          512

// TODO: IDF-6249 (Copy from esp32c6, need check)
/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-H2 has 2 UARTs
#define SOC_UART_NUM                (2)
#define SOC_UART_FIFO_LEN           (128)      /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)  /*!< Max bit rate supported by UART */

// #define SOC_UART_SUPPORT_APB_CLK    (1)     /*!< Support APB as the clock source */
#define SOC_UART_SUPPORT_RTC_CLK    (0)     /*!< Support RTC clock as the clock source */ // TODO: IDF-6249
#define SOC_UART_SUPPORT_XTAL_CLK   (1)     /*!< Support XTAL clock as the clock source */
// #define SOC_UART_SUPPORT_WAKEUP_INT (1)         /*!< Support UART wakeup interrupt */ // TODO: IDF-6249
#define SOC_UART_REQUIRE_CORE_RESET (1)

// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)

// TODO: IDF-5679 (Copy from esp32c6, need check)
/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

// TODO: IDF-6337
/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

// TODO: IDF-5679 (Copy from esp32c6, need check)
/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

// TODO: IDF-6270 (Copy from esp32c6, need check)
/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BT_WAKEUP        (1)
#define SOC_PM_SUPPORT_CPU_PD           (1)
#define SOC_PM_SUPPORT_WIFI_PD          (1)
#define SOC_PM_SUPPORT_BT_PD            (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)

#define SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY   (1) /*!<Supports CRC only the stub code in RTC memory */

/*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
// #define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */
#define SOC_CLK_OSC_SLOW_SUPPORTED                (1)     /*!< Support to connect an external oscillator, not a crystal */
#define SOC_CLK_RC32K_SUPPORTED                   (1)     /*!< Support an internal 32kHz RC oscillator */

// TODO: IDF-6229 (Copy from esp32c6, need check)
/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC                (1)
#define SOC_TEMPERATURE_SENSOR_SUPPORT_XTAL                (1)

/*---------------------------------- Bluetooth CAPS ----------------------------------*/
#define SOC_BLE_SUPPORTED               (1)    /*!< Support Bluetooth Low Energy hardware */
