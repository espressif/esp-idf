/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The long term plan is to have a single soc_caps.h for all peripherals.
// During the refactoring and multichip support development process, we
// separate these information into periph_caps.h for each peripheral and
// include them here to avoid developing conflicts.

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
#define SOC_UART_SUPPORTED              1
#define SOC_PCNT_SUPPORTED              1
#define SOC_PHY_SUPPORTED               1
#define SOC_WIFI_SUPPORTED              1
#define SOC_TWAI_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_GPTIMER_SUPPORTED           1
#define SOC_LCDCAM_SUPPORTED            1
#define SOC_LCDCAM_I80_LCD_SUPPORTED    1
#define SOC_LCDCAM_RGB_LCD_SUPPORTED    1
#define SOC_MCPWM_SUPPORTED             1
#define SOC_DEDICATED_GPIO_SUPPORTED    1
#define SOC_CACHE_SUPPORT_WRAP          1
#define SOC_ULP_SUPPORTED               1
#define SOC_ULP_FSM_SUPPORTED           1
#define SOC_RISCV_COPROC_SUPPORTED      1
#define SOC_BT_SUPPORTED                1
#define SOC_USB_OTG_SUPPORTED           1
#define SOC_USB_SERIAL_JTAG_SUPPORTED   1
#define SOC_CCOMP_TIMER_SUPPORTED       1
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1
#define SOC_EFUSE_SUPPORTED             1
#define SOC_SDMMC_HOST_SUPPORTED        1
#define SOC_RTC_FAST_MEM_SUPPORTED      1
#define SOC_RTC_SLOW_MEM_SUPPORTED      1
#define SOC_RTC_MEM_SUPPORTED           1
#define SOC_PSRAM_DMA_CAPABLE           1
#define SOC_XT_WDT_SUPPORTED            1
#define SOC_I2S_SUPPORTED               1
#define SOC_RMT_SUPPORTED               1
#define SOC_SDM_SUPPORTED               1
#define SOC_GPSPI_SUPPORTED             1
#define SOC_LEDC_SUPPORTED              1
#define SOC_I2C_SUPPORTED               1
#define SOC_SYSTIMER_SUPPORTED          1
#define SOC_SUPPORT_COEXISTENCE         1
#define SOC_TEMP_SENSOR_SUPPORTED       1
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
#define SOC_SIMD_INSTRUCTION_SUPPORTED  1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- SOC CAPS ----------------------------------------*/
#define SOC_APPCPU_HAS_CLOCK_GATING_BUG (1)

/*-------------------------- ADC CAPS ----------------------------------------*/
/*!< SAR ADC Module*/
#define SOC_ADC_RTC_CTRL_SUPPORTED              1
#define SOC_ADC_DIG_CTRL_SUPPORTED              1
#define SOC_ADC_ARBITER_SUPPORTED               1
#define SOC_ADC_DIG_IIR_FILTER_SUPPORTED        1
#define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        ((UNIT == 0) ? 1 : 0)    //Digital controller supported ADC unit
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (10)
#define SOC_ADC_MAX_CHANNEL_NUM                 (10)
#define SOC_ADC_ATTEN_NUM                       (4)

/*!< Digital */
#define SOC_ADC_DIGI_CONTROLLER_NUM             (2)
#define SOC_ADC_PATT_LEN_MAX                    (24)    //Two pattern table, each contains 12 items. Each item takes 1 byte
#define SOC_ADC_DIGI_MIN_BITWIDTH               (12)
#define SOC_ADC_DIGI_MAX_BITWIDTH               (12)
#define SOC_ADC_DIGI_RESULT_BYTES               (4)
#define SOC_ADC_DIGI_DATA_BYTES_PER_CONV        (4)
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

/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (1)

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#define SOC_BROWNOUT_RESET_SUPPORTED 1

/*-------------------------- CACHE CAPS --------------------------------------*/
#define SOC_CACHE_WRITEBACK_SUPPORTED           1
#define SOC_CACHE_FREEZE_SUPPORTED              1

/*-------------------------- CPU CAPS ----------------------------------------*/
#define SOC_CPU_CORES_NUM               2
#define SOC_CPU_INTR_NUM                32
#define SOC_CPU_HAS_FPU                 1
#define SOC_HP_CPU_HAS_MULTIPLE_CORES   1   // Convenience boolean macro used to determine if a target has multiple cores.

#define SOC_CPU_BREAKPOINTS_NUM             2
#define SOC_CPU_WATCHPOINTS_NUM             2
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  64 // bytes

#define SOC_SIMD_PREFERRED_DATA_ALIGNMENT 16 // The preferred data alignment accepted by the SIMD instructions, in bytes

/*-------------------------- DIGITAL SIGNATURE CAPS ----------------------------------------*/
/** The maximum length of a Digital Signature in bits. */
#define SOC_DS_SIGNATURE_MAX_BIT_LEN (4096)

/** Initialization vector (IV) length for the RSA key parameter message digest (MD) in bytes. */
#define SOC_DS_KEY_PARAM_MD_IV_LENGTH (16)

/** Maximum wait time for DS parameter decryption key. If overdue, then key error.
    See TRM DS chapter for more details */
#define SOC_DS_KEY_CHECK_MAX_WAIT_US (1100)

/*-------------------------- GDMA CAPS ---------------------------------------*/
#define SOC_AHB_GDMA_VERSION           1U
#define SOC_GDMA_NUM_GROUPS_MAX        1U
#define SOC_GDMA_PAIRS_PER_GROUP       5 // esp32s3 has only one kind of GDMA, which is AHB GDMA, and it has 5 pairs in total.
#define SOC_GDMA_PAIRS_PER_GROUP_MAX   5 // when there're multiple GDMA instances, this macro represents the maximum number of GDMA pairs in the same group.
#define SOC_AHB_GDMA_SUPPORT_PSRAM     1

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-S3 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 49
#define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1
#define SOC_GPIO_FILTER_CLK_SUPPORT_APB 1

// On ESP32-S3, Digital IOs have their own registers to control pullup/down/capability, independent with RTC registers.
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT (1)
// Force hold is a new function of ESP32-S3
#define SOC_GPIO_SUPPORT_FORCE_HOLD      (1)

// 0~48 valid except 22~25
#define SOC_GPIO_VALID_GPIO_MASK         (0x1FFFFFFFFFFFFULL & ~(0ULL | BIT22 | BIT23 | BIT24 | BIT25))
// No GPIO is input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK  (SOC_GPIO_VALID_GPIO_MASK)

#define SOC_GPIO_IN_RANGE_MAX            48
#define SOC_GPIO_OUT_RANGE_MAX           48

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_26~GPIO_NUM_48)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x0001FFFFFC000000ULL

// The Clock Out signal is binding to the pin's IO_MUX function
#define SOC_GPIO_CLOCKOUT_BY_IO_MUX    (1)
#define SOC_GPIO_CLOCKOUT_CHANNEL_NUM  (3)

// RTC_IOs and DIG_IOs can be hold during deep sleep and after waking up
#define SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP (1)

/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_GPIO_OUT_AUTO_ENABLE  (1) /*!< Dedicated GPIO output attribution is enabled automatically */

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-S3 has 2 I2C
#define SOC_I2C_NUM                 (2U)
#define SOC_HP_I2C_NUM              (2U)

#define SOC_I2C_FIFO_LEN       (32) /*!< I2C hardware FIFO depth */
#define SOC_I2C_CMD_REG_NUM         (8)  /*!< Number of I2C command registers */
#define SOC_I2C_SUPPORT_SLAVE       (1)

// FSM_RST only resets the FSM, not using it. So SOC_I2C_SUPPORT_HW_FSM_RST not defined.
//ESP32-S3 support hardware clear bus
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_XTAL       (1)
#define SOC_I2C_SUPPORT_RTC        (1)

#define SOC_I2C_SUPPORT_10BIT_ADDR  (1)
#define SOC_I2C_SLAVE_SUPPORT_BROADCAST    (1)
#define SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS   (1)
#define SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_NUM                 (2U)
#define SOC_I2S_HW_VERSION_2        (1)
#define SOC_I2S_SUPPORTS_XTAL       (1)
#define SOC_I2S_SUPPORTS_PLL_F160M  (1)
#define SOC_I2S_SUPPORTS_PCM        (1)
#define SOC_I2S_SUPPORTS_PDM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)
#define SOC_I2S_PDM_MAX_TX_LINES    (2)
#define SOC_I2S_SUPPORTS_PDM_RX     (1)
#define SOC_I2S_PDM_MAX_RX_LINES    (4)
#define SOC_I2S_SUPPORTS_TDM        (1)

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_APB_CLOCK       (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK      (1)
#define SOC_LEDC_TIMER_NUM               (4)
#define SOC_LEDC_CHANNEL_NUM             (8)
#define SOC_LEDC_TIMER_BIT_WIDTH         (14)
#define SOC_LEDC_SUPPORT_FADE_STOP       (1)

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

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM       (1U)
#define SOC_MMU_PERIPH_NUM                      (1U)

/*-------------------------- MPU CAPS ----------------------------------------*/
#include "mpu_caps.h"

/*-------------------------- PCNT CAPS ---------------------------------------*/
#define SOC_PCNT_GROUPS               (1U)
#define SOC_PCNT_UNITS_PER_GROUP      (4)
#define SOC_PCNT_CHANNELS_PER_UNIT    (2)
#define SOC_PCNT_THRES_POINT_PER_UNIT (2)

/*-------------------------- RMT CAPS ----------------------------------------*/
#define SOC_RMT_GROUPS                        1U /*!< One RMT group */
#define SOC_RMT_TX_CANDIDATES_PER_GROUP       4  /*!< Number of channels that capable of Transmit in each group */
#define SOC_RMT_RX_CANDIDATES_PER_GROUP       4  /*!< Number of channels that capable of Receive in each group */
#define SOC_RMT_CHANNELS_PER_GROUP            8  /*!< Total 8 channels */
#define SOC_RMT_MEM_WORDS_PER_CHANNEL         48 /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_PINGPONG           1  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_RX_DEMODULATION       1  /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_ASYNC_STOP         1  /*!< Support stop transmission asynchronously */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT         1  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP     1  /*!< Hardware support of auto-stop in loop mode */
#define SOC_RMT_SUPPORT_TX_SYNCHRO            1  /*!< Support coordinate a group of TX channels to start simultaneously */
#define SOC_RMT_SUPPORT_TX_CARRIER_DATA_ONLY  1  /*!< TX carrier can be modulated to data phase only */
#define SOC_RMT_SUPPORT_XTAL                  1  /*!< Support set XTAL clock as the RMT clock source */
#define SOC_RMT_SUPPORT_RC_FAST               1  /*!< Support set RC_FAST clock as the RMT clock source */
#define SOC_RMT_SUPPORT_APB                   1  /*!< Support set APB as the RMT clock source */
#define SOC_RMT_SUPPORT_DMA                   1  /*!< RMT peripheral can connect to DMA channel */

/*-------------------------- LCD CAPS ----------------------------------------*/
/* Notes: On esp32-s3, I80 bus and RGB timing generator can't work at the same time */
#define SOC_LCD_I80_SUPPORTED           (1)  /*!< Intel 8080 LCD is supported */
#define SOC_LCD_RGB_SUPPORTED           (1)  /*!< RGB LCD is supported */
#define SOC_LCD_I80_BUSES               (1U) /*!< Has one LCD Intel 8080 bus */
#define SOC_LCD_RGB_PANELS              (1U) /*!< Support one RGB LCD panel */
#define SOC_LCD_I80_BUS_WIDTH           (16) /*!< Intel 8080 bus width */
#define SOC_LCD_RGB_DATA_WIDTH          (16) /*!< Number of LCD data lines */
#define SOC_LCD_SUPPORT_RGB_YUV_CONV    (1)  /*!< Support color format conversion between RGB and YUV */
#define SOC_LCDCAM_I80_NUM_BUSES        (1U) /*!< LCD_CAM peripheral provides one LCD Intel 8080 bus */
#define SOC_LCDCAM_I80_BUS_WIDTH        (16) /*!< Intel 8080 bus max data width */
#define SOC_LCDCAM_RGB_NUM_PANELS       (1U) /*!< LCD_CAM peripheral provides one RGB panel */
#define SOC_LCDCAM_RGB_DATA_WIDTH       (16) /*!< RGB panel max data width */

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
#define SOC_RTCIO_PIN_COUNT   22
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED 1  /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                             * so it supports unique IOMUX configuration (including IE, OE, PU, PD, DRV etc.)
                                             * when the pins are switched to RTC function.
                                             */
#define SOC_RTCIO_HOLD_SUPPORTED 1
#define SOC_RTCIO_WAKE_SUPPORTED 1
// LP IO peripherals have independent clock gating to manage
#define SOC_LP_IO_CLOCK_IS_INDEPENDENT 1

/*-------------------------- Sigma Delta Modulator CAPS -----------------*/
#define SOC_SDM_GROUPS             1
#define SOC_SDM_CHANNELS_PER_GROUP 8
#define SOC_SDM_CLK_SUPPORT_APB    1

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM                  3
#define SOC_SPI_PERIPH_CS_NUM(i)            (((i)==0)? 2: (((i)==1)? 6: 3))
#define SOC_SPI_MAX_CS_NUM                  6
#define SOC_SPI_MAXIMUM_BUFFER_SIZE         64
#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_CONTINUOUS_TRANS    1
#define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1
#define SOC_SPI_SUPPORT_CLK_APB             1
#define SOC_SPI_SUPPORT_CLK_XTAL            1

// Peripheral supports DIO, DOUT, QIO, or QOUT
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})

// Peripheral supports output given level during its "dummy phase"
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUT 1
#define SOC_MEMSPI_IS_INDEPENDENT                   1
#define SOC_SPI_MAX_PRE_DIVIDER                     16
#define SOC_SPI_SUPPORT_OCT                         1

#define SOC_SPI_SCT_SUPPORTED                     1
#define SOC_SPI_SCT_SUPPORTED_PERIPH(PERIPH_NUM)  ((PERIPH_NUM==1) ? 1 : 0)    //Support Segmented-Configure-Transfer
#define SOC_SPI_SCT_REG_NUM                       14
#define SOC_SPI_SCT_BUFFER_NUM_MAX                (1 + SOC_SPI_SCT_REG_NUM)  //1-word-bitmap + 14-word-regs
#define SOC_SPI_SCT_CONF_BITLEN_MAX               0x3FFFA       //18 bits wide reg

#define SOC_MEMSPI_SRC_FREQ_120M         1
#define SOC_MEMSPI_SRC_FREQ_80M_SUPPORTED          1
#define SOC_MEMSPI_SRC_FREQ_40M_SUPPORTED          1
#define SOC_MEMSPI_SRC_FREQ_20M_SUPPORTED          1

/*-------------------------- SPIRAM CAPS ----------------------------------------*/
#define SOC_SPIRAM_SUPPORTED            1
#define SOC_SPIRAM_XIP_SUPPORTED        1

/*-------------------------- SYS TIMER CAPS ----------------------------------*/
#define SOC_SYSTIMER_COUNTER_NUM            2  // Number of counter units
#define SOC_SYSTIMER_ALARM_NUM              3  // Number of alarm units
#define SOC_SYSTIMER_BIT_WIDTH_LO           32 // Bit width of systimer low part
#define SOC_SYSTIMER_BIT_WIDTH_HI           20 // Bit width of systimer high part
#define SOC_SYSTIMER_FIXED_DIVIDER          1  // Clock source divider is fixed: 2.5
#define SOC_SYSTIMER_INT_LEVEL              1  // Systimer peripheral uses level
#define SOC_SYSTIMER_ALARM_MISS_COMPENSATE  1  // Systimer peripheral can generate interrupt immediately if t(target) > t(current)

/*-------------------------- TIMER GROUP CAPS --------------------------------*/
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (2)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (54)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_SUPPORT_APB       (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS      (4)

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_VERSION                    (2)  /*!< Hardware version of touch sensor */
#define SOC_TOUCH_SENSOR_NUM                        (15) /*!< 15 Touch channels */
#define SOC_TOUCH_SUPPORT_SLEEP_WAKEUP               (1)  /*!< Touch sensor supports sleep awake */
#define SOC_TOUCH_SUPPORT_WATERPROOF                (1)  /*!< Touch sensor supports waterproof */
#define SOC_TOUCH_SUPPORT_PROX_SENSING              (1)  /*!< Touch sensor supports proximity sensing */
#define SOC_TOUCH_PROXIMITY_CHANNEL_NUM             (3)  /*!< Support touch proximity sensing channel number. */
#define SOC_TOUCH_PROXIMITY_MEAS_DONE_SUPPORTED     (1)  /*!< Support touch proximity sensing measure done interrupt type. */

#define SOC_TOUCH_SAMPLE_CFG_NUM                    (1U) /*!< The sample configuration number in total, each sampler can be used to sample on one frequency */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM         1UL
#define SOC_TWAI_CLK_SUPPORT_APB        1
#define SOC_TWAI_BRP_MIN                2
#define SOC_TWAI_BRP_MAX                16384
#define SOC_TWAI_SUPPORTS_RX_STATUS     1

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-S3 has 3 UARTs
#define SOC_UART_NUM                (3)
#define SOC_UART_HP_NUM             (3)
#define SOC_UART_FIFO_LEN           (128)      /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX        (5000000)  /*!< Max bit rate supported by UART */
// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)
#define SOC_UART_SUPPORT_WAKEUP_INT (1)        /*!< Support UART wakeup interrupt */
#define SOC_UART_SUPPORT_APB_CLK    (1)     /*!< Support APB as the clock source */
#define SOC_UART_SUPPORT_RTC_CLK    (1)     /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK   (1)     /*!< Support XTAL clock as the clock source */

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

/*--------------------------- MPI CAPS ---------------------------------------*/
#define SOC_MPI_MEM_BLOCKS_NUM (4)
#define SOC_MPI_OPERATIONS_NUM (3)

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (4096)

/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GDMA            (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- Power Management CAPS ---------------------------*/
#define SOC_PM_SUPPORT_EXT0_WAKEUP      (1)
#define SOC_PM_SUPPORT_EXT1_WAKEUP      (1)
#define SOC_PM_SUPPORT_EXT_WAKEUP       (1)     /*!<Compatible to the old version of IDF */
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_BT_WAKEUP        (1)
#define SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP    (1)     /*!<Supports waking up from touch pad trigger */

#define SOC_PM_SUPPORT_CPU_PD           (1)
#define SOC_PM_SUPPORT_TAGMEM_PD        (1)
#define SOC_PM_SUPPORT_RTC_PERIPH_PD    (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)
#define SOC_PM_SUPPORT_MAC_BB_PD        (1)
#define SOC_PM_SUPPORT_MODEM_PD         (1)     /*!<Modem here includes wifi and ble */

#define SOC_CONFIGURABLE_VDDSDIO_SUPPORTED        (1)
#define SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY   (1) /*!<Supports CRC only the stub code in RTC memory */

#define SOC_PM_CPU_RETENTION_BY_RTCCNTL         (1)
#define SOC_PM_MODEM_RETENTION_BY_BACKUPDMA     (1)
#define SOC_PM_MODEM_PD_BY_SW                   (1)

/*--------------------------- CLOCK SUBSYSTEM CAPS -------------------------- */
#define SOC_CLK_RC_FAST_D256_SUPPORTED            (1)
#define SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256     (1)
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (1)

#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_DOWNLOAD_ICACHE 1
#define SOC_EFUSE_DIS_DOWNLOAD_DCACHE 1
#define SOC_EFUSE_HARD_DIS_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_DIS_ICACHE 1
#define SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK 1  // XTS-AES key purpose not supported for this block

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
#define SOC_MEMPROT_MEM_ALIGN_SIZE          256

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
#define SOC_SPI_MEM_SUPPORT_TIMING_TUNING                 (1)
#define SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE          (1)
#define SOC_SPI_MEM_SUPPORT_WRAP                          (1)
#define SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY            (1)
#define SOC_MEMSPI_CORE_CLK_SHARED_WITH_PSRAM             (1)
#define SOC_SPI_MEM_SUPPORT_CACHE_32BIT_ADDR_MAP          (1)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*-------------------------- EXTERNAL COEXISTENCE CAPS -------------------------------------*/
#define SOC_EXTERNAL_COEX_ADVANCE              (0) /*!< HARDWARE ADVANCED EXTERNAL COEXISTENCE CAPS */
#define SOC_EXTERNAL_COEX_LEADER_TX_LINE       (1) /*!< EXTERNAL COEXISTENCE TX LINE CAPS */

/*-------------------------- SDMMC CAPS -----------------------------------------*/

/* Card detect, write protect, interrupt use GPIO Matrix on all chips.
 * On ESP32-S3, clock/cmd/data pins use GPIO Matrix as well.
 */
#define SOC_SDMMC_USE_GPIO_MATRIX  1
#define SOC_SDMMC_NUM_SLOTS        2
/* Indicates that there is an option to use XTAL clock instead of PLL for SDMMC */
#define SOC_SDMMC_SUPPORT_XTAL_CLOCK    1
/* Supported host clock delay phase number */
#define SOC_SDMMC_DELAY_PHASE_NUM    4

/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC                (1)

/*------------------------------------ WI-FI CAPS ------------------------------------*/
#define SOC_WIFI_HW_TSF                     (1)    /*!< Support hardware TSF */
#define SOC_WIFI_FTM_SUPPORT                (1)    /*!< Support FTM */
#define SOC_WIFI_GCMP_SUPPORT               (1)    /*!< Support GCMP(GCMP128 and GCMP256) */
#define SOC_WIFI_WAPI_SUPPORT               (1)    /*!< Support WAPI */
#define SOC_WIFI_CSI_SUPPORT                (1)    /*!< Support CSI */
#define SOC_WIFI_MESH_SUPPORT               (1)    /*!< Support WIFI MESH */
#define SOC_WIFI_SUPPORT_VARIABLE_BEACON_WINDOW   (1)    /*!< Support delta early time for rf phy on/off */
#define SOC_WIFI_PHY_NEEDS_USB_WORKAROUND   (1)    /*!< SoC has WiFi and USB PHYs interference, needs a workaround */

/*---------------------------------- Bluetooth CAPS ----------------------------------*/
#define SOC_BLE_SUPPORTED               (1)    /*!< Support Bluetooth Low Energy hardware */
#define SOC_BLE_MESH_SUPPORTED          (1)    /*!< Support BLE MESH */
#define SOC_BLE_50_SUPPORTED            (1)    /*!< Support Bluetooth 5.0 */
#define SOC_BLE_DEVICE_PRIVACY_SUPPORTED (1)   /*!< Support BLE device privacy mode */
#define SOC_BLUFI_SUPPORTED             (1)    /*!< Support BLUFI */

/*-------------------------- ULP CAPS ----------------------------------------*/
#define SOC_ULP_HAS_ADC                     (1)    /* ADC can be accessed from ULP */

/*------------------------------------- PHY CAPS -------------------------------------*/
#define SOC_PHY_COMBO_MODULE                  (1) /*!< Support Wi-Fi and BLE*/
