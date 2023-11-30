/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
 * It can also be ran manually with `./tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py 'components/soc/esp32p4/include/soc/'`
 *
 * For more information see `tools/gen_soc_caps_kconfig/README.md`
 *
*/

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
// #define SOC_ADC_SUPPORTED               1  //TODO: IDF-6496
#define SOC_ANA_CMPR_SUPPORTED          1
// #define SOC_DEDICATED_GPIO_SUPPORTED    1  //TODO: IDF-7552
#define SOC_UART_SUPPORTED              1
#define SOC_GDMA_SUPPORTED              1
#define SOC_AHB_GDMA_SUPPORTED          1
#define SOC_AXI_GDMA_SUPPORTED          1
#define SOC_GPTIMER_SUPPORTED           1
#define SOC_PCNT_SUPPORTED              1
#define SOC_MCPWM_SUPPORTED             1
// #define SOC_TWAI_SUPPORTED              1  //TODO: IDF-7470
#define SOC_ETM_SUPPORTED               1
#define SOC_PARLIO_SUPPORTED            1  //TODO: IDF-7471
#define SOC_ASYNC_MEMCPY_SUPPORTED      1
// disable usb serial jtag for esp32p4, current image does not support
// #define SOC_USB_SERIAL_JTAG_SUPPORTED   1  //TODO: IDF-7496
// #define SOC_TEMP_SENSOR_SUPPORTED       1  //TODO: IDF-7482
#define SOC_SUPPORTS_SECURE_DL_MODE     1
// #define SOC_RISCV_COPROC_SUPPORTED      1
#define SOC_EFUSE_KEY_PURPOSE_FIELD     1
#define SOC_EFUSE_SUPPORTED             1
#define SOC_RTC_FAST_MEM_SUPPORTED      1
#define SOC_RTC_MEM_SUPPORTED           1
#define SOC_RMT_SUPPORTED               1
#define SOC_I2S_SUPPORTED               1
// #define SOC_RMT_SUPPORTED               1  //TODO: IDF-7476
// #define SOC_SDM_SUPPORTED               1  //TODO: IDF-7551
#define SOC_GPSPI_SUPPORTED             1
#define SOC_LEDC_SUPPORTED              1
#define SOC_I2C_SUPPORTED               1  //TODO: IDF-6507, TODO: IDF-7491
#define SOC_SYSTIMER_SUPPORTED          1
// #define SOC_AES_SUPPORTED               1  //TODO: IDF-6519
#define SOC_MPI_SUPPORTED               1
// #define SOC_SHA_SUPPORTED               1  //TODO: IDF-7541
#define SOC_HMAC_SUPPORTED              1
#define SOC_DIG_SIGN_SUPPORTED          1
#define SOC_ECC_SUPPORTED               1
#define SOC_ECC_EXTENDED_MODES_SUPPORTED   1
#define SOC_ECDSA_SUPPORTED             1
// #define SOC_KEY_MANAGER_SUPPORTED       1  //TODO: IDF-7925
#define SOC_FLASH_ENC_SUPPORTED         1
#define SOC_SECURE_BOOT_SUPPORTED       1
// #define SOC_BOD_SUPPORTED               1  //TODO: IDF-7519
// #define SOC_APM_SUPPORTED               1  //TODO: IDF-7542
// #define SOC_PMU_SUPPORTED               1  //TODO: IDF-7531
// #define SOC_PAU_SUPPORTED               1  //TODO: IDF-7531
// #define SOC_LP_TIMER_SUPPORTED          1  //TODO: IDF-7532
// #define SOC_ULP_LP_UART_SUPPORTED       1  //TODO: IDF-7533
#define SOC_LP_GPIO_MATRIX_SUPPORTED    1
#define SOC_LP_PERIPHERALS_SUPPORTED    1
#define SOC_SPIRAM_SUPPORTED            1
#define SOC_PSRAM_DMA_CAPABLE           1
// #define SOC_ULP_SUPPORTED               1  //TODO: IDF-7534
#define SOC_SDMMC_HOST_SUPPORTED        1
// #define SOC_CLK_TREE_SUPPORTED          1  //TODO: IDF-7526
// #define SOC_ASSIST_DEBUG_SUPPORTED      1  //TODO: IDF-7565
#define SOC_WDT_SUPPORTED               1
#define SOC_SPI_FLASH_SUPPORTED         1

/*-------------------------- XTAL CAPS ---------------------------------------*/
#define SOC_XTAL_SUPPORT_40M            1

/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GDMA            (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- ADC CAPS -------------------------------*/
/*!< SAR ADC Module*/
// #define SOC_ADC_DIG_CTRL_SUPPORTED              1  //TODO: IDF-6496, TODO: IDF-6497
// #define SOC_ADC_DIG_IIR_FILTER_SUPPORTED        1
// #define SOC_ADC_MONITOR_SUPPORTED               1
#define SOC_ADC_DIG_SUPPORTED_UNIT(UNIT)        1    //Digital controller supported ADC unit
// #define SOC_ADC_DMA_SUPPORTED                   1
#define SOC_ADC_PERIPH_NUM                      (1U)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         (7)
#define SOC_ADC_MAX_CHANNEL_NUM                 (7)
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

/*!< Calibration */
#define SOC_ADC_CALIBRATION_V1_SUPPORTED        (0) /*!< support HW offset calibration version 1*/

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
#define SOC_INT_PLIC_SUPPORTED          0       //riscv platform-level interrupt controller
#define SOC_INT_CLIC_SUPPORTED          1
#define SOC_INT_HW_NESTED_SUPPORTED     1       // Support for hardware interrupts nesting
#define SOC_BRANCH_PREDICTOR_SUPPORTED  1
#define SOC_CPU_HAS_FPU                 1
#define SOC_CPU_HAS_FPU_EXT_ILL_BUG     1       // EXT_ILL CSR doesn't support FLW/FSW
#define SOC_CPU_COPROC_NUM              2
#define SOC_HP_CPU_HAS_MULTIPLE_CORES   1   // Convenience boolean macro used to determine if a target has multiple cores.

#define SOC_CPU_BREAKPOINTS_NUM             3
#define SOC_CPU_WATCHPOINTS_NUM             3
#define SOC_CPU_WATCHPOINT_MAX_REGION_SIZE  0x100   // bytes

#define SOC_CPU_HAS_PMA                 1
#define SOC_CPU_IDRAM_SPLIT_USING_PMP   1

/*-------------------------- DIGITAL SIGNATURE CAPS ----------------------------------------*/
/** The maximum length of a Digital Signature in bits. */
#define SOC_DS_SIGNATURE_MAX_BIT_LEN (4096)

/** Initialization vector (IV) length for the RSA key parameter message digest (MD) in bytes. */
#define SOC_DS_KEY_PARAM_MD_IV_LENGTH (16)

/** Maximum wait time for DS parameter decryption key. If overdue, then key error.
    See TRM DS chapter for more details */
#define SOC_DS_KEY_CHECK_MAX_WAIT_US (1100)

/*-------------------------- GDMA CAPS -------------------------------------*/
#define SOC_AHB_GDMA_VERSION            2
#define SOC_GDMA_SUPPORT_CRC            1
#define SOC_GDMA_NUM_GROUPS_MAX         2
#define SOC_GDMA_PAIRS_PER_GROUP_MAX    3
#define SOC_AXI_GDMA_SUPPORT_PSRAM      1
// #define SOC_GDMA_SUPPORT_ETM            1

/*-------------------------- ETM CAPS --------------------------------------*/
#define SOC_ETM_GROUPS                  1U  // Number of ETM groups
#define SOC_ETM_CHANNELS_PER_GROUP      50  // Number of ETM channels in the group

/*-------------------------- GPIO CAPS ---------------------------------------*/
// ESP32-P4 has 1 GPIO peripheral
#define SOC_GPIO_PORT                      1U
#define SOC_GPIO_PIN_COUNT                 57
// #define SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER 1  //TODO: IDF-7481
// #define SOC_GPIO_FLEX_GLITCH_FILTER_NUM    8  //TODO: IDF-7481
#define SOC_GPIO_SUPPORT_PIN_HYS_FILTER    1

// GPIO peripheral has the ETM extension
#define SOC_GPIO_SUPPORT_ETM          1
#define SOC_GPIO_ETM_EVENTS_PER_GROUP 8
#define SOC_GPIO_ETM_TASKS_PER_GROUP  8

// Target has the full LP IO subsystem
// On ESP32-P4, Digital IOs have their own registers to control pullup/down capability, independent of LP registers.
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT    (1)
// GPIO0~15 on ESP32P4 can support chip deep sleep wakeup
#define SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP   (1)

#define SOC_GPIO_VALID_GPIO_MASK        (0x01FFFFFFFFFFFFFF)
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK SOC_GPIO_VALID_GPIO_MASK

#define SOC_GPIO_IN_RANGE_MAX           56
#define SOC_GPIO_OUT_RANGE_MAX          56

#define SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK        (0ULL | 0xFFFF)

// digital I/O pad powered by VDD3P3_CPU or VDD_SPI(GPIO_NUM_16~GPIO_NUM_56)
#define SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK 0x01FFFFFFFFFF0000ULL

// Support to force hold all IOs
#define SOC_GPIO_SUPPORT_FORCE_HOLD              (1)
// Support to hold a single digital I/O when the digital domain is powered off
#define SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP  (1)

/*-------------------------- RTCIO CAPS --------------------------------------*/
#define SOC_RTCIO_PIN_COUNT                 16
#define SOC_RTCIO_INPUT_OUTPUT_SUPPORTED    1   /* This macro indicates that the target has separate RTC IOMUX hardware feature,
                                                 * so it supports unique IOMUX configuration (including IE, OE, PU, PD, DRV etc.)
                                                 * when the pins are switched to RTC function.
                                                 */
#define SOC_RTCIO_HOLD_SUPPORTED            1
#define SOC_RTCIO_WAKE_SUPPORTED            1

/*-------------------------- Dedicated GPIO CAPS -----------------------------*/
#define SOC_DEDIC_GPIO_OUT_CHANNELS_NUM (8) /*!< 8 outward channels on each CPU core */
#define SOC_DEDIC_GPIO_IN_CHANNELS_NUM  (8) /*!< 8 inward channels on each CPU core */
#define SOC_DEDIC_PERIPH_ALWAYS_ENABLE  (1) /*!< The dedicated GPIO (a.k.a. fast GPIO) is featured by some customized CPU instructions, which is always enabled */

/*------------------------- Analog Comparator CAPS ---------------------------*/
#define SOC_ANA_CMPR_NUM                       (2)
#define SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE      (1)  // Support positive/negative/any cross interrupt
#define SOC_ANA_CMPR_SUPPORT_ETM               (1)

/*-------------------------- I2C CAPS ----------------------------------------*/
// ESP32-P4 has 2 I2Cs
#define SOC_I2C_NUM                 (2U)

#define SOC_I2C_FIFO_LEN            (32) /*!< I2C hardware FIFO depth */
#define SOC_I2C_CMD_REG_NUM         (8)  /*!< Number of I2C command registers */
#define SOC_I2C_SUPPORT_SLAVE       (1)

// FSM_RST only resets the FSM, not using it. So SOC_I2C_SUPPORT_HW_FSM_RST not defined.
#define SOC_I2C_SUPPORT_HW_CLR_BUS  (1)

#define SOC_I2C_SUPPORT_XTAL        (1)
#define SOC_I2C_SUPPORT_RTC         (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR  (1)
#define SOC_I2C_SLAVE_SUPPORT_BROADCAST    (1)
#define SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS   (1)
#define SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH    (1)

/*-------------------------- I2S CAPS ----------------------------------------*/
#define SOC_I2S_NUM                 (3U)
#define SOC_I2S_HW_VERSION_2        (1)
#define SOC_I2S_SUPPORTS_XTAL       (1)
#define SOC_I2S_SUPPORTS_APLL       (1)
#define SOC_I2S_SUPPORTS_PCM        (1)
#define SOC_I2S_SUPPORTS_PDM        (1)
#define SOC_I2S_SUPPORTS_PDM_TX     (1)
#define SOC_I2S_SUPPORTS_PDM_RX     (1)
#define SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER (1)
#define SOC_I2S_SUPPORTS_TX_SYNC_CNT      (1)
#define SOC_I2S_SUPPORTS_TDM        (1)
#define SOC_I2S_PDM_MAX_TX_LINES    (2)     // On I2S0
#define SOC_I2S_PDM_MAX_RX_LINES    (4)     // On I2S0
#define SOC_I2S_TDM_FULL_DATA_WIDTH (1)  /*!< No limitation to data bit width when using multiple slots */

/*-------------------------- LEDC CAPS ---------------------------------------*/
#define SOC_LEDC_SUPPORT_PLL_DIV_CLOCK      (1)
#define SOC_LEDC_SUPPORT_XTAL_CLOCK         (1)
#define SOC_LEDC_CHANNEL_NUM                (8)
#define SOC_LEDC_TIMER_BIT_WIDTH            (20)
#define SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED (1)
#define SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX (16)
#define SOC_LEDC_SUPPORT_FADE_STOP          (1)
#define SOC_LEDC_FADE_PARAMS_BIT_WIDTH      (10)

/*-------------------------- MMU CAPS ----------------------------------------*/
#define SOC_MMU_PAGE_SIZE_CONFIGURABLE        (0)
#define SOC_MMU_PERIPH_NUM                    (2U)
#define SOC_MMU_LINEAR_ADDRESS_REGION_NUM     (2U)
#define SOC_MMU_DI_VADDR_SHARED               (1) /*!< D/I vaddr are shared */

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
#define SOC_PCNT_SUPPORT_CLEAR_SIGNAL         1  /*!< Support clear signal input */

/*--------------------------- RMT CAPS ---------------------------------------*/
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
// #define SOC_RMT_SUPPORT_RC_FAST               1  /*!< Support set RC_FAST clock as the RMT clock source */
#define SOC_RMT_SUPPORT_DMA                   1  /*!< RMT peripheral can connect to DMA channel */

/*-------------------------- MCPWM CAPS --------------------------------------*/
#define SOC_MCPWM_GROUPS                     (2U)   ///< 2 MCPWM groups on the chip (i.e., the number of independent MCPWM peripherals)
#define SOC_MCPWM_TIMERS_PER_GROUP           (3)    ///< The number of timers that each group has
#define SOC_MCPWM_OPERATORS_PER_GROUP        (3)    ///< The number of operators that each group has
#define SOC_MCPWM_COMPARATORS_PER_OPERATOR   (2)    ///< The number of comparators that each operator has
#define SOC_MCPWM_EVENT_COMPARATORS_PER_OPERATOR   (2)    ///< The number of event comparators that each operator has
#define SOC_MCPWM_GENERATORS_PER_OPERATOR    (2)    ///< The number of generators that each operator has
#define SOC_MCPWM_TRIGGERS_PER_OPERATOR      (2)    ///< The number of triggers that each operator has
#define SOC_MCPWM_GPIO_FAULTS_PER_GROUP      (3)    ///< The number of fault signal detectors that each group has
#define SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP   (1)    ///< The number of capture timers that each group has
#define SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER (3)    ///< The number of capture channels that each capture timer has
#define SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP    (3)    ///< The number of GPIO synchros that each group has
#define SOC_MCPWM_SWSYNC_CAN_PROPAGATE       (1)    ///< Software sync event can be routed to its output
#define SOC_MCPWM_SUPPORT_ETM                (1)    ///< Support ETM (Event Task Matrix)
#define SOC_MCPWM_SUPPORT_EVENT_COMPARATOR   (1)    ///< Support event comparator (based on ETM)
#define SOC_MCPWM_CAPTURE_CLK_FROM_GROUP     (1)    ///< Capture timer shares clock with other PWM timers

/*------------------------ USB SERIAL JTAG CAPS ------------------------------*/
// #define SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP     (1)     /*!< Support to maintain minimum usb communication during light sleep */ // TODO: IDF-6395

/*-------------------------- PARLIO CAPS --------------------------------------*/
#define SOC_PARLIO_GROUPS                    1U  /*!< Number of parallel IO peripherals */
#define SOC_PARLIO_TX_UNITS_PER_GROUP        1U  /*!< number of TX units in each group */
#define SOC_PARLIO_RX_UNITS_PER_GROUP        1U  /*!< number of RX units in each group */
#define SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH    16  /*!< Number of data lines of the TX unit */
#define SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH    16  /*!< Number of data lines of the RX unit */
#define SOC_PARLIO_TX_SIZE_BY_DMA            1   /*!< Transaction length is controlled by DMA instead of indicated by register */

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
/* Supported host clock delay phase number */
#define SOC_SDMMC_DELAY_PHASE_NUM    4

// TODO: IDF-5353 (Copy from esp32c3, need check)
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

/*--------------------------- ECDSA CAPS ---------------------------------------*/
#define SOC_ECDSA_SUPPORT_EXPORT_PUBKEY     (1)

/*-------------------------- Sigma Delta Modulator CAPS -----------------*/
#define SOC_SDM_GROUPS               1U
#define SOC_SDM_CHANNELS_PER_GROUP   4
#define SOC_SDM_CLK_SUPPORT_PLL_F80M 1
#define SOC_SDM_CLK_SUPPORT_XTAL     1

/*-------------------------- SPI CAPS ----------------------------------------*/
#define SOC_SPI_PERIPH_NUM              3
#define SOC_SPI_PERIPH_CS_NUM(i)        (((i)==0)? 2: (((i)==1)? 6: 3))
#define SOC_SPI_MAX_CS_NUM              6
#define SOC_SPI_MAXIMUM_BUFFER_SIZE     64

// #define SOC_SPI_SUPPORT_SLAVE_HD_VER2       1    //TODO: IDF-7505
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_OCT                 1
#define SOC_SPI_SUPPORT_CLK_XTAL            1
// #define SOC_SPI_SUPPORT_CLK_RC_FAST         1    //bellow clks are waiting for clock tree
// #define SOC_SPI_SUPPORT_CLK_SPLL_F480M      1    //super pll
// #define SOC_SPI_SUPPORT_CLK_SDIO            1    //sdio pll
// #define SOC_SPI_SUPPORT_CLK_APLL            1    //audio pll

// Peripheral supports DIO, DOUT, QIO, or QOUT
// host_id = 0 -> SPI0/SPI1, host_id = 1 -> SPI2,
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)  ({(void)host_id; 1;})

#define SOC_MEMSPI_IS_INDEPENDENT   1
#define SOC_SPI_MAX_PRE_DIVIDER     16

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
//#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                (1) //TODO: IDF-7518
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
// #define SOC_SYSTIMER_SUPPORT_ETM            1  // Systimer comparator can generate ETM event  //TODO: IDF-7486

/*-------------------------- LP_TIMER CAPS ----------------------------------*/
#define SOC_LP_TIMER_BIT_WIDTH_LO           32 // Bit width of lp_timer low part
#define SOC_LP_TIMER_BIT_WIDTH_HI           16 // Bit width of lp_timer high part

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_GROUPS                  2
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  2
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH 54
#define SOC_TIMER_GROUP_SUPPORT_XTAL      1
#define SOC_TIMER_GROUP_SUPPORT_RC_FAST   1
#define SOC_TIMER_GROUP_TOTAL_TIMERS      4
#define SOC_TIMER_SUPPORT_ETM             1

/*--------------------------- WATCHDOG CAPS ---------------------------------------*/
#define SOC_MWDT_SUPPORT_XTAL              (1)

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_CONTROLLER_NUM         2
#define SOC_TWAI_CLK_SUPPORT_XTAL       1
#define SOC_TWAI_BRP_MIN                2
#define SOC_TWAI_BRP_MAX                32768
#define SOC_TWAI_SUPPORTS_RX_STATUS     1

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_DOWNLOAD_ICACHE 1
#define SOC_EFUSE_DIS_PAD_JTAG 1
#define SOC_EFUSE_DIS_USB_JTAG 1
#define SOC_EFUSE_DIS_DIRECT_BOOT 1
#define SOC_EFUSE_SOFT_DIS_JTAG 1

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              1
#define SOC_SECURE_BOOT_V2_ECC              1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   3
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   1
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  1

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX   (32)
#define SOC_FLASH_ENCRYPTION_XTS_AES        1
#define SOC_FLASH_ENCRYPTION_XTS_AES_128    1

/*-------------------------- MEMPROT CAPS ------------------------------------*/

/*-------------------------- UART CAPS ---------------------------------------*/
// ESP32-P4 has 6 UARTs (5 HP UART, and 1 LP UART)
#define SOC_UART_NUM                    (6)
#define SOC_UART_HP_NUM                 (5)
#define SOC_UART_LP_NUM                 (1U)
#define SOC_UART_FIFO_LEN               (128)       /*!< The UART hardware FIFO length */
#define SOC_LP_UART_FIFO_LEN            (16)        /*!< The LP UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX            (5000000)   /*!< Max bit rate supported by UART */
#define SOC_UART_SUPPORT_PLL_F80M_CLK   (1)         /*!< Support PLL_F80M as the clock source */
#define SOC_UART_SUPPORT_RTC_CLK        (1)         /*!< Support RTC clock as the clock source */
#define SOC_UART_SUPPORT_XTAL_CLK       (1)         /*!< Support XTAL clock as the clock source */
#define SOC_UART_SUPPORT_WAKEUP_INT     (1)         /*!< Support UART wakeup interrupt */

// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)

// TODO: IDF-5679 (Copy from esp32c3, need check)
/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)

// TODO: IDF-5679 (Copy from esp32c3, need check)
/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

// TODO: IDF-5351 (Copy from esp32c3, need check)
/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)
#define SOC_PM_SUPPORT_CPU_PD           (1)
#define SOC_PM_SUPPORT_MODEM_PD         (1)
#define SOC_PM_SUPPORT_XTAL32K_PD       (1)
#define SOC_PM_SUPPORT_RC32K_PD         (1)
#define SOC_PM_SUPPORT_RC_FAST_PD       (1)
#define SOC_PM_SUPPORT_VDDSDIO_PD       (1)
#define SOC_PM_SUPPORT_TOP_PD           (1)

#define SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY   (1) /*!<Supports CRC only the stub code in RTC memory */

#define SOC_PM_CPU_RETENTION_BY_SW      (0)

#define SOC_PM_PAU_LINK_NUM             (4)

/*-------------------------- CLOCK SUBSYSTEM CAPS ----------------------------------------*/
#define SOC_CLK_RC_FAST_SUPPORT_CALIBRATION       (0)
#define SOC_MODEM_CLOCK_IS_INDEPENDENT            (0)

#define SOC_CLK_APLL_SUPPORTED                    (1)     /*!< Support Audio PLL */
#define SOC_CLK_XTAL32K_SUPPORTED                 (1)     /*!< Support to connect an external low frequency crystal */
#define SOC_CLK_OSC_SLOW_SUPPORTED                (1)     /*!< Support to connect an external oscillator, not a crystal */
#define SOC_CLK_RC32K_SUPPORTED                   (1)     /*!< Support an internal 32kHz RC oscillator */

#define SOC_PERIPH_CLK_CTRL_SHARED                (1)     /*!< Peripheral clock control (e.g. set clock source) is shared between various peripherals */

/*-------------------------- Temperature Sensor CAPS -------------------------------------*/
#define SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC                (1)
#define SOC_TEMPERATURE_SENSOR_SUPPORT_XTAL                (1)

/*-------------------------- Memory CAPS --------------------------*/
#define SOC_MEM_TCM_SUPPORTED    (1)
