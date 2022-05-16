/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The long term plan is to have a single soc_caps.h for all peripherals.
// During the refactoring and multichip support development process, we
// separate these information into periph_caps.h for each peripheral and
// include them here to avoid developing conflicts.

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_ADC_SUPPORTED               1
#define SOC_PCNT_SUPPORTED              1
#define SOC_TWAI_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_LCDCAM_SUPPORTED            1
#define SOC_MCPWM_SUPPORTED             1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_CPU_CORES_NUM               2
#define SOC_CACHE_SUPPORT_WRAP          1
#define SOC_ULP_SUPPORTED               1
#define SOC_BT_SUPPORTED                1
#define SOC_USB_OTG_SUPPORTED           1
#define SOC_USB_SERIAL_JTAG_SUPPORTED   1
#define SOC_RTC_SLOW_MEM_SUPPORTED      1
#define SOC_CCOMP_TIMER_SUPPORTED       1
#define SOC_DIG_SIGN_SUPPORTED          1
#define SOC_HMAC_SUPPORTED              1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS 3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS 1
#define SOC_SDMMC_HOST_SUPPORTED        1
#define SOC_FLASH_ENCRYPTION_XTS_AES      1
#define SOC_FLASH_ENCRYPTION_XTS_AES_256 1
#define SOC_PSRAM_DMA_CAPABLE           1
#define SOC_XT_WDT_SUPPORTED            1

/*-------------------------- SOC CAPS ----------------------------------------*/
#define SOC_APPCPU_HAS_CLOCK_GATING_BUG (1)

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
#define SOC_ADC_PATT_LEN_MAX                    (24)    //Two pattern table, each contains 12 items. Each item takes 1 byte
#define SOC_ADC_DIGI_MAX_BITWIDTH               (13)
/*!< F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interva <= 4095 */
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*!< RTC */
#define SOC_ADC_MAX_BITWIDTH                    (12)

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (1) /*!< support HW offset calibration version 1*/


/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (1)

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#include "brownout_caps.h"

/*-------------------------- CPU CAPS ----------------------------------------*/
#include "cpu_caps.h"

/*-------------------------- DIGITAL SIGNATURE CAPS ----------------------------------------*/
/** The maximum length of a Digital Signature in bits. */
#define SOC_DS_SIGNATURE_MAX_BIT_LEN (4096)

/** Initialization vector (IV) length for the RSA key parameter message digest (MD) in bytes. */
#define SOC_DS_KEY_PARAM_MD_IV_LENGTH (16)

/** Maximum wait time for DS parameter decryption key. If overdue, then key error.
    See TRM DS chapter for more details */
#define SOC_DS_KEY_CHECK_MAX_WAIT_US (1100)

/*-------------------------- GDMA CAPS ---------------------------------------*/
#define SOC_GDMA_GROUPS            (1)  // Number of GDMA groups
#define SOC_GDMA_PAIRS_PER_GROUP   (5)  // Number of GDMA pairs in each group
#define SOC_GDMA_SUPPORT_PSRAM     (1)  // GDMA can access external PSRAM
#define SOC_GDMA_PSRAM_MIN_ALIGN   (16) // Minimal alignment for PSRAM transaction

/*-------------------------- GPIO CAPS ---------------------------------------*/
#include "gpio_caps.h"

/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_GPIO_OUT_AUTO_ENABLE  (1) /*!< Dedicated GPIO output attribution is enabled automatically */

/*-------------------------- I2C CAPS ----------------------------------------*/
#include "i2c_caps.h"

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_NUM                 (2)
#define SOC_I2S_SUPPORTS_PCM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)
#define SOC_I2S_SUPPORTS_PDM_RX     (1)
#define SOC_I2S_SUPPORTS_PDM_CODEC  (1)
#define SOC_I2S_SUPPORTS_TDM        (1)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#include "ledc_caps.h"

/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_GROUPS                     (2)    ///< 2 MCPWM groups on the chip (i.e., the number of independent MCPWM peripherals)
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
#define SOC_MCPWM_BASE_CLK_HZ       (160000000ULL)  ///< Base Clock frequency of 160MHz

/*-------------------------- MPU CAPS ----------------------------------------*/
#include "mpu_caps.h"

/*-------------------------- PCNT CAPS ---------------------------------------*/
#define SOC_PCNT_GROUPS               (1)
#define SOC_PCNT_UNITS_PER_GROUP      (4)
#define SOC_PCNT_CHANNELS_PER_UNIT    (2)
#define SOC_PCNT_THRES_POINT_PER_UNIT (2)

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_GROUPS                    (1)  /*!< One RMT group */
#define SOC_RMT_TX_CANDIDATES_PER_GROUP   (4)  /*!< Number of channels that capable of Transmit in each group */
#define SOC_RMT_RX_CANDIDATES_PER_GROUP   (4)  /*!< Number of channels that capable of Receive in each group */
#define SOC_RMT_CHANNELS_PER_GROUP        (8)  /*!< Total 8 channels */
#define SOC_RMT_MEM_WORDS_PER_CHANNEL     (48) /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_PINGPONG       (1)  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_RX_DEMODULATION   (1)  /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT     (1)  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_LOOP_AUTOSTOP  (1)  /*!< Hardware support of auto-stop in loop mode */
#define SOC_RMT_SUPPORT_TX_SYNCHRO        (1)  /*!< Support coordinate a group of TX channels to start simultaneously */
#define SOC_RMT_SUPPORT_XTAL              (1)  /*!< Support set XTAL clock as the RMT clock source */


/*-------------------------- LCD CAPS ----------------------------------------*/
/* Notes: On esp32-s3, I80 bus and RGB timing generator can't work at the same time */
#define SOC_LCD_I80_SUPPORTED           (1)  /*!< Intel 8080 LCD is supported */
#define SOC_LCD_RGB_SUPPORTED           (1)  /*!< RGB LCD is supported */
#define SOC_LCD_I80_BUSES               (1)  /*!< Has one LCD Intel 8080 bus */
#define SOC_LCD_RGB_PANELS              (1)  /*!< Support one RGB LCD panel */
#define SOC_LCD_I80_BUS_WIDTH           (16) /*!< Intel 8080 bus width */
#define SOC_LCD_RGB_DATA_WIDTH          (16) /*!< Number of LCD data lines */

/*-------------------------- RTC CAPS --------------------------------------*/
#define SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH       (128)
#define SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM        (549)
#define SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN      (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)
#define SOC_RTC_CNTL_CPU_PD_DMA_BLOCK_SIZE      (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3)

#define SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE  (SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM * (SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH >> 3))

/* I/D Cache tag memory retention hardware parameters */
#define SOC_RTC_CNTL_TAGMEM_PD_DMA_BUS_WIDTH    (128)
#define SOC_RTC_CNTL_TAGMEM_PD_DMA_ADDR_ALIGN   (SOC_RTC_CNTL_TAGMEM_PD_DMA_BUS_WIDTH >> 3)

/*-------------------------- RTCIO CAPS --------------------------------------*/
#include "rtc_io_caps.h"

/*-------------------------- SIGMA DELTA CAPS --------------------------------*/
#define SOC_SIGMADELTA_NUM         (1) // 1 sigma-delta peripheral
#define SOC_SIGMADELTA_CHANNEL_NUM (8) // 8 channels

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM                  3
#define SOC_SPI_DMA_CHAN_NUM                3
#define SOC_SPI_PERIPH_CS_NUM(i)            3
#define SOC_SPI_MAXIMUM_BUFFER_SIZE         64
#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_CONTINUOUS_TRANS    1
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1

// Peripheral supports DIO, DOUT, QIO, or QOUT
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})

// Peripheral supports output given level during its "dummy phase"
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUTPUT 1
#define SOC_MEMSPI_IS_INDEPENDENT                   1
#define SOC_SPI_MAX_PRE_DIVIDER                     16
#define SOC_SPI_SUPPORT_OCT                         1

/*-------------------------- SPIRAM CAPS ----------------------------------------*/
#define SOC_SPIRAM_SUPPORTED            1

/*-------------------------- SYS TIMER CAPS ----------------------------------*/
#define SOC_TOUCH_VERSION_2                (1)  // Hardware version of touch sensor
#define SOC_SYSTIMER_COUNTER_NUM           (2)  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM             (3)  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO          (32) // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI          (20) // Bit width of systimer high part
#define SOC_SYSTIMER_FIXED_TICKS_US        (16) // Number of ticks per microsecond is fixed
#define SOC_SYSTIMER_INT_LEVEL             (1)  // Systimer peripheral uses level
#define SOC_SYSTIMER_ALARM_MISS_COMPENSATE (1)  // Systimer peripheral can generate interrupt immediately if t(target) > t(current)

/*-------------------------- TIMER GROUP CAPS --------------------------------*/
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (2)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (54)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS (SOC_TIMER_GROUPS * SOC_TIMER_GROUP_TIMERS_PER_GROUP)

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_NUM                (15) /*! 15 Touch channels */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM     (3)  /* Sopport touch proximity channel number. */
#define SOC_TOUCH_PROXIMITY_MEAS_DONE_SUPPORTED (1) /*Sopport touch proximity channel measure done interrupt type. */

#define SOC_TOUCH_PAD_THRESHOLD_MAX         (0x1FFFFF)  /*!<If set touch threshold max value, The touch sensor can't be in touched status */
#define SOC_TOUCH_PAD_MEASURE_WAIT_MAX      (0xFF)  /*!<The timer frequency is 8Mhz, the max value is 0xff */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#include "twai_caps.h"

/*-------------------------- UART CAPS ---------------------------------------*/
#include "uart_caps.h"

#define SOC_UART_SUPPORT_RTC_CLK    (1)     /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK   (1)     /*!< Support XTAL clock as the clock source */
#define SOC_UART_REQUIRE_CORE_RESET (1)

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

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_SHA_GDMA             (1)

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


/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)


/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GDMA            (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)


/*-------------------------- Power Management CAPS ---------------------------*/
#define SOC_PM_SUPPORT_EXT_WAKEUP       (1)

#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)

#define SOC_PM_SUPPORT_BT_WAKEUP        (1)

#define SOC_PM_SUPPORT_CPU_PD           (1)

#define SOC_PM_SUPPORT_TAGMEM_PD        (1)

#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP    (1)     /*!<Supports waking up from touch pad trigger */

#define SOC_PM_SUPPORT_DEEPSLEEP_VERIFY_STUB_ONLY   (1)


/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (64)


/*-------------------------- WI-FI HARDWARE TSF CAPS -------------------------------*/
#define SOC_WIFI_HW_TSF                 (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)
#define SOC_MAC_BB_PD_MEM_SIZE          (192*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_OPI_MODE                      (1)
#define SOC_SPI_MEM_SUPPORT_TIME_TUNING                   (1)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*-------------------------- SDMMC CAPS -----------------------------------------*/

/* Card detect, write protect, interrupt use GPIO Matrix on all chips.
 * On ESP32-S3, clock/cmd/data pins use GPIO Matrix as well.
 */
#define SOC_SDMMC_USE_GPIO_MATRIX  1
#define SOC_SDMMC_NUM_SLOTS        2
/* Indicates that there is an option to use XTAL clock instead of PLL for SDMMC */
#define SOC_SDMMC_SUPPORT_XTAL_CLOCK    1
