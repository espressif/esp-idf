/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ************************* ESP32S31 Root Clock Source ****************************
 * 1) Internal 17.5MHz RC Oscillator: RC_FAST (may also referred as FOSC in TRM and reg. description)
 *
 *    This RC oscillator generates a ~17.5MHz clock signal output as the RC_FAST_CLK.
 *
 *    The exact frequency of RC_FAST_CLK can be computed in runtime through calibration.
 *
 * 2) External 40MHz Crystal Clock: XTAL
 *
 * 3) Internal 136kHz RC Oscillator: RC_SLOW (may also referred as SOSC in TRM or reg. description)
 *
 *    This RC oscillator generates from a ~600kHz clock signal with a divider of 4 output as the RC_SLOW_CLK.
 *    The exact frequency of this clock can be computed in runtime through calibration.
 *
 * 4) Internal 32kHz RC Oscillator: RC32K [NOT RECOMMENDED TO USE]
 *
 *    The exact frequency of this clock can **not** be computed in runtime through calibration.
 *
 * 5) External 32kHz Crystal Clock (optional): XTAL32K
 *
 *    The clock source for this XTAL32K_CLK should be a 32kHz crystal connecting to the XTAL_32K_P and XTAL_32K_N
 *    pins.
 *
 *    XTAL32K_CLK can also be calibrated to get its exact frequency.
 *
 *
 * PLL Clocks:
 *
 * from 40MHz XTAL oscillator frequency multipliers:
 * 1) CPLL (320MHz), CPU_PLL, used for CPU, MSPI-Flash, MSPI-PSRAM clock source
 * 2) MPLL (configurable), MSPI_PLL, used for MSPI-PSRAM clock source; and is further divided to PLL_F50M, PLL_F25M, to be used for peripheral's clock sources
 * 3) BBPLL (480MHz), SYS_PLL (AXI/AHB), directly used for MSPI-Flash, GPSPI clock sources; and is further divided to PLL_F240M, PLL_F160M, PLL_F120M, PLL_F80M, PLL_F20M, to be used for peripherals' clock sources
 * 4) APLL (configurable), AUDIO_PLL, can be the clock source for peripherals (I2S, GPSPI, LCD, CAM, etc.)
 */

/* With the default value of FOSC_DFREQ = 100, RC_FAST clock frequency is 17.5 MHz +/- 7% */
#define SOC_CLK_RC_FAST_FREQ_APPROX         17500000                            /*!< Approximate RC_FAST_CLK frequency in Hz */
#define SOC_CLK_RC_SLOW_FREQ_APPROX         136000                              /*!< Approximate RC_SLOW_CLK frequency in Hz */
#define SOC_CLK_XTAL32K_FREQ_APPROX         32768                               /*!< Approximate XTAL32K_CLK frequency in Hz */

// Naming convention: SOC_ROOT_CLK_{loc}_{type}_[attr]
// {loc}: EXT, INT
// {type}: XTAL, RC
// [attr] - optional: [frequency], FAST, SLOW
/**
 * @brief Root clock
 */
typedef enum {
    SOC_ROOT_CLK_INT_RC_FAST,          /*!< Internal 17.5MHz RC oscillator */
    SOC_ROOT_CLK_INT_RC_SLOW,          /*!< Internal 136kHz RC oscillator */
    SOC_ROOT_CLK_EXT_XTAL,             /*!< External 40MHz crystal */
    SOC_ROOT_CLK_EXT_XTAL32K,          /*!< External 32kHz crystal */
    // The following exist but will not be used
    // SOC_ROOT_CLK_INT_RC32K,            /*!< Internal 32kHz RC oscillator */
    // SOC_ROOT_CLK_EXT_OSC_SLOW,         /*!< External slow clock signal at pin1 */
} soc_root_clk_t;

/**
 * @brief ROOT clock circuit, which requires explicitly enabling the targeting circuit to use
 */
typedef enum {
    SOC_ROOT_CIRCUIT_CLK_BBPLL,         /*!< BBPLL_CLK is the output of the BBPLL generator circuit */
    SOC_ROOT_CIRCUIT_CLK_CPLL,          /*!< CPLL_CLK is the output of the CPLL generator circuit */
    SOC_ROOT_CIRCUIT_CLK_MPLL,          /*!< MPLL_CLK is the output of the MPLL generator circuit */
    SOC_ROOT_CIRCUIT_CLK_APLL,          /*!< APLL_CLK is the output of the APLL generator circuit */
} soc_root_clk_circuit_t;

/**
 * @brief CPU_CLK mux inputs, which are the supported clock sources for the CPU_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_CPU_CLK_SRC_XTAL = 0,              /*!< Select XTAL_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_CPLL = 1,              /*!< Select CPLL_CLK as CPU_CLK source (CPLL_CLK is the output of 40MHz crystal oscillator frequency multiplier, can be 320MHz) */
    SOC_CPU_CLK_SRC_RC_FAST = 2,           /*!< Select RC_FAST_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_PLL_F240M = 3,         /*!< Select PLL_F240M_CLK as CPU_CLK source (PLL_F240M_CLK is derived from BBPLL (480MHz), which is the output of the main crystal oscillator frequency multiplier) */
    SOC_CPU_CLK_SRC_INVALID,               /*!< Invalid CPU_CLK source */
} soc_cpu_clk_src_t;

/**
 * @brief RTC_SLOW_CLK mux inputs, which are the supported clock sources for the RTC_SLOW_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_RTC_SLOW_CLK_SRC_RC_SLOW = 0,                 /*!< Select RC_SLOW_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_XTAL32K = 1,                 /*!< Select XTAL32K_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_INVALID,                     /*!< Invalid RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_DEFAULT = SOC_RTC_SLOW_CLK_SRC_RC_SLOW, /*!< RC_SLOW_CLK is the default clock source for RTC_SLOW_CLK */
} soc_rtc_slow_clk_src_t;

/**
 * @brief RTC_FAST_CLK mux inputs, which are the supported clock sources for the RTC_FAST_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_RTC_FAST_CLK_SRC_RC_FAST = 0,      /*!< Select RC_FAST_CLK as RTC_FAST_CLK source */
    SOC_RTC_FAST_CLK_SRC_XTAL = 1,         /*!< Select XTAL_CLK as RTC_FAST_CLK source */
    SOC_RTC_FAST_CLK_SRC_INVALID,          /*!< Invalid RTC_FAST_CLK source */

    SOC_RTC_FAST_CLK_SRC_DEFAULT = SOC_RTC_FAST_CLK_SRC_XTAL, /*!< XTAL_CLK is the default clock source for RTC_FAST_CLK */
} soc_rtc_fast_clk_src_t;

/**
 * @brief Possible main XTAL frequency options on the target
 * @note Enum values equal to the frequency value in MHz
 * @note Not all frequency values listed here are supported in IDF. Please check SOC_XTAL_SUPPORT_XXX in soc_caps.h for
 *       the supported ones.
 */
typedef enum {
    SOC_XTAL_FREQ_40M = 40,                /*!< 40MHz XTAL */
} soc_xtal_freq_t;

// Naming convention: SOC_MOD_CLK_{[upstream]clock_name}_[attr]
// {[upstream]clock_name}: XTAL, (S/M/A)PLL, etc.
// [attr] - optional: FAST, SLOW, D<divider>, F<freq>
/**
 * @brief Supported clock sources for modules (CPU, peripherals, RTC, etc.)
 *
 * @note enum starts from 1, to save 0 for special purpose
 */
typedef enum {
    // For CPU domain
    SOC_MOD_CLK_CPU = 1,                       /*!< CPU_CLK can be sourced from XTAL, CPLL, or RC_FAST by configuring soc_cpu_clk_src_t */
    // For RTC domain
    SOC_MOD_CLK_RTC_FAST,                      /*!< RTC_FAST_CLK can be sourced from XTAL, or RC_FAST by configuring soc_rtc_fast_clk_src_t */
    SOC_MOD_CLK_RTC_SLOW,                      /*!< RTC_SLOW_CLK can be sourced from RC_SLOW, XTAL32K, or RC32K by configuring soc_rtc_slow_clk_src_t */
    // For digital domain: peripherals
    SOC_MOD_CLK_PLL_F20M,                      /*!< PLL_F20M_CLK is derived from BBPLL (clock gating + default divider 24), its default frequency is 20MHz */
    SOC_MOD_CLK_PLL_F25M,                      /*!< PLL_F25M_CLK is derived from MPLL (clock gating + configurable divider), it will have a frequency of 25MHz */
    SOC_MOD_CLK_PLL_F50M,                      /*!< PLL_F50M_CLK is derived from MPLL (clock gating + configurable divider 10), it will have a frequency of 50MHz */
    SOC_MOD_CLK_PLL_F80M,                      /*!< PLL_F80M_CLK is derived from BBPLL (clock gating + default divider 6), its default frequency is 80MHz */
    SOC_MOD_CLK_PLL_F160M,                     /*!< PLL_F160M_CLK is derived from BBPLL (clock gating + default divider 3), its default frequency is 160MHz */
    SOC_MOD_CLK_PLL_F240M,                     /*!< PLL_F240M_CLK is derived from BBPLL (clock gating + default divider 2), its default frequency is 240MHz */
    SOC_MOD_CLK_CPLL,                          /*!< CPLL is from 40MHz XTAL oscillator frequency multipliers */
    SOC_MOD_CLK_BBPLL,                         /*!< BBPLL is from 40MHz XTAL oscillator frequency multipliers, its default frequency is 480MHz */
    SOC_MOD_CLK_MPLL,                          /*!< MPLL is from 40MHz XTAL oscillator frequency multipliers */
    SOC_MOD_CLK_XTAL32K,                       /*!< XTAL32K_CLK comes from the external 32kHz crystal, passing a clock gating to the peripherals */
    SOC_MOD_CLK_RC_FAST,                       /*!< RC_FAST_CLK comes from the internal 20MHz rc oscillator, passing a clock gating to the peripherals */
    SOC_MOD_CLK_XTAL,                          /*!< XTAL_CLK comes from the external 40MHz crystal */
    SOC_MOD_CLK_APLL,                          /*!< Audio PLL is sourced from PLL, and its frequency is configurable through APLL configuration registers */
    SOC_MOD_CLK_APB,                           /*!< APB_CLK is highly dependent on the CPU_CLK source */
    // For LP peripherals
    SOC_MOD_CLK_XTAL_D2,                       /*!< XTAL_D2_CLK comes from the external 40MHz crystal, passing a div of 2 to the LP peripherals */
    SOC_MOD_CLK_LP_DYN_FAST,                   /*!< LP_DYN_FAST can be derived from RTC_SLOW_CLK or RTC_FAST_CLK depending on the chip’s power mode:
                                                    Only in LP_SLEEP mode, select RTC_SLOW_CLK as the clock source;
                                                    In other non-LP_SLEEP mode, select RTC_FAST_CLK as the clock source; */
    SOC_MOD_CLK_LP_PERI,                       /*!< LP_PERI_CLK is derived from LP_DYN_FAST (configurable divider) */
    SOC_MOD_CLK_INVALID,                       /*!< Indication of the end of the available module clock sources */
} soc_module_clk_t;

//////////////////////////////////////////////////LCD///////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of LCD
 */
#define SOC_LCD_CLKS {SOC_MOD_CLK_PLL_F160M, SOC_MOD_CLK_XTAL, SOC_MOD_CLK_APLL}

/**
 * @brief Type of LCD clock source
 */
typedef enum {
    LCD_CLK_SRC_PLL160M = SOC_MOD_CLK_PLL_F160M, /*!< Select PLL_F160M as the source clock */
    LCD_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
    LCD_CLK_SRC_APLL = SOC_MOD_CLK_APLL,         /*!< Select APLL as the source clock */
    LCD_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M, /*!< Select PLL_F160M as the default choice */
} soc_periph_lcd_clk_src_t;

//////////////////////////////////////////////////CAM///////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of CAM
 */
#define SOC_CAM_CLKS {SOC_MOD_CLK_PLL_F160M, SOC_MOD_CLK_XTAL, SOC_MOD_CLK_APLL}

/**
 * @brief Type of CAM clock source
 */
typedef enum {
    CAM_CLK_SRC_PLL160M = SOC_MOD_CLK_PLL_F160M, /*!< Select PLL_F160M as the source clock */
    CAM_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
    CAM_CLK_SRC_APLL = SOC_MOD_CLK_APLL,         /*!< Select APLL as the source clock */
    CAM_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M, /*!< Select PLL_F160M as the default choice */
} soc_periph_cam_clk_src_t;

//////////////////////////////////////////////////SYSTIMER//////////////////////////////////////////////////////////////

/**
 * @brief Type of SYSTIMER clock source
 */
typedef enum {
    SYSTIMER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,       /*!< SYSTIMER source clock is XTAL */
    SYSTIMER_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST, /*!< SYSTIMER source clock is RC_FAST */
    SYSTIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,    /*!< SYSTIMER source clock default choice is XTAL */
} soc_periph_systimer_clk_src_t;

//////////////////////////////////////////////////GPTimer///////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of GPTimer
 *
 * The following code can be used to iterate all possible clocks:
 * @code{c}
 * soc_periph_gptimer_clk_src_t gptimer_clks[] = (soc_periph_gptimer_clk_src_t)SOC_GPTIMER_CLKS;
 * for (size_t i = 0; i< sizeof(gptimer_clks) / sizeof(gptimer_clks[0]); i++) {
 *     soc_periph_gptimer_clk_src_t clk = gptimer_clks[i];
 *     // Test GPTimer with the clock `clk`
 * }
 * @endcode
 */
#define SOC_GPTIMER_CLKS {SOC_MOD_CLK_PLL_F80M, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_XTAL}

/**
 * @brief Type of GPTimer clock source
 */
typedef enum {
    GPTIMER_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M as the source clock */
    GPTIMER_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,   /*!< Select RC_FAST as the source clock */
    GPTIMER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
    GPTIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M as the default choice */
} soc_periph_gptimer_clk_src_t;

//////////////////////////////////////////////////ETM///////////////////////////////////////////////////////////////////

typedef enum {
    ETM_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M as the source clock */
    ETM_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,   /*!< Select RC_FAST as the source clock */
    ETM_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
    ETM_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M as the default choice */
} soc_periph_etm_clk_src_t;

//////////////////////////////////////////////////RMT///////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of RMT
 */
#define SOC_RMT_CLKS {SOC_MOD_CLK_PLL_F80M, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_XTAL}

/**
 * @brief Type of RMT clock source
 */
typedef enum {
    RMT_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M as the source clock */
    RMT_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,   /*!< Select RC_FAST as the source clock */
    RMT_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
    RMT_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M as the default choice */
} soc_periph_rmt_clk_src_t;

///////////////////////////////////////////////////UART/////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of UART
 */
#define SOC_UART_CLKS {SOC_MOD_CLK_PLL_F80M, SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST}

/**
 * @brief Type of UART clock source, reserved for the legacy UART driver
 */
typedef enum {
    UART_SCLK_PLL_F80M = SOC_MOD_CLK_PLL_F80M,           /*!< UART source clock is PLL_F80M */
    UART_SCLK_RTC = SOC_MOD_CLK_RC_FAST,                /*!< UART source clock is RC_FAST */
    UART_SCLK_XTAL = SOC_MOD_CLK_XTAL,                  /*!< UART source clock is XTAL */
    UART_SCLK_DEFAULT = SOC_MOD_CLK_PLL_F80M,           /*!< UART source clock default choice is PLL_F80M */
} soc_periph_uart_clk_src_legacy_t;

/**
 * @brief Array initializer for all supported clock sources of LP_UART
 */
#define SOC_LP_UART_CLKS {SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_XTAL}

/**
 * @brief Type of LP_UART clock source
 */
typedef enum {
    LP_UART_SCLK_RC_FAST = SOC_MOD_CLK_RC_FAST,         /*!< LP_UART source clock is FOSC */
    LP_UART_SCLK_XTAL = SOC_MOD_CLK_XTAL,               /*!< LP_UART source clock is XTAL */
    LP_UART_SCLK_DEFAULT = SOC_MOD_CLK_RC_FAST,         /*!< LP_UART source clock default choice is FOSC */
} soc_periph_lp_uart_clk_src_t;

//////////////////////////////////////////////////LEDC/////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of LEDC
 */
#define SOC_LEDC_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_PLL_F80M, SOC_MOD_CLK_RC_FAST}
#define SOC_LEDC_CLK_STRS {"LEDC_USE_XTAL_CLK", "LEDC_USE_PLL_DIV_CLK", "LEDC_USE_RC_FAST_CLK"}

/**
 * @brief Type of LEDC clock source, reserved for the legacy LEDC driver
 */
typedef enum {
    LEDC_AUTO_CLK = 0,                              /*!< LEDC source clock will be automatically selected based on the giving resolution and duty parameter when init the timer*/
    LEDC_USE_PLL_DIV_CLK = SOC_MOD_CLK_PLL_F80M,    /*!< Select PLL_F80M clock as the source clock */
    LEDC_USE_RC_FAST_CLK = SOC_MOD_CLK_RC_FAST,     /*!< Select RC_FAST as the source clock */
    LEDC_USE_XTAL_CLK = SOC_MOD_CLK_XTAL,           /*!< Select XTAL as the source clock */
} soc_periph_ledc_clk_src_legacy_t;

/////////////////////////////////////////////////I2C////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of I2C
 */
#define SOC_I2C_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST}

/**
 * @brief Type of I2C clock source.
 */
typedef enum {
    I2C_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,            /*!< Select XTAL as the source clock */
    I2C_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,      /*!< Select RC_FAST as the source clock */
    I2C_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the default clock choice */
} soc_periph_i2c_clk_src_t;

///////////////////////////////////////////////LP_I2C///////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of LP_I2C
 */
#define SOC_LP_I2C_CLKS {SOC_MOD_CLK_RTC_FAST, SOC_MOD_CLK_XTAL}

/**
 * @brief Type of LP_I2C clock source.
 */
typedef enum {
    LP_I2C_SCLK_LP_FAST = SOC_MOD_CLK_RTC_FAST,                 /*!< LP_I2C source clock is RTC_FAST */
    LP_I2C_SCLK_XTAL = SOC_MOD_CLK_XTAL,                  /*!< LP_I2C source clock is XTAL */
    LP_I2C_SCLK_DEFAULT = SOC_MOD_CLK_RTC_FAST,                 /*!< LP_I2C source clock default choice is RTC_FAST */
} soc_periph_lp_i2c_clk_src_t;

/////////////////////////////////////////////////SPI////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of SPI
 */
#define SOC_SPI_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_BBPLL}

/**
 * @brief Type of SPI clock source.
 */
typedef enum {
    SPI_CLK_SRC_XTAL     = SOC_MOD_CLK_XTAL,        /*!< Select XTAL as SPI source clock */
    SPI_CLK_SRC_RC_FAST  = SOC_MOD_CLK_RC_FAST,     /*!< Select RC_FAST_20M as SPI source clock */
    SPI_CLK_SRC_BBPLL    = SOC_MOD_CLK_BBPLL,       /*!< Select BBPLL as SPI source clock */
    SPI_CLK_SRC_DEFAULT  = SOC_MOD_CLK_BBPLL,       /*!< Select BBPLL as default source clock */
} soc_periph_spi_clk_src_t;

/////////////////////////////////////////////////PSRAM////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of PSRAM
 */
#define SOC_PSRAM_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_CPLL, SOC_MOD_CLK_MPLL}

/**
 * @brief Type of PSRAM clock source.
 */
typedef enum {
    PSRAM_CLK_SRC_DEFAULT = SOC_MOD_CLK_MPLL,         /*!< Select SOC_MOD_CLK_MPLL as PSRAM source clock */
    PSRAM_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,            /*!< Select SOC_MOD_CLK_XTAL as PSRAM source clock */
    PSRAM_CLK_SRC_CPLL = SOC_MOD_CLK_CPLL,            /*!< Select SOC_MOD_CLK_CPLL as PSRAM source clock */
    PSRAM_CLK_SRC_MPLL = SOC_MOD_CLK_MPLL,            /*!< Select SOC_MOD_CLK_MPLL as PSRAM source clock */
} soc_periph_psram_clk_src_t;

/////////////////////////////////////////////////FLASH////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of FLASH
 */
#define SOC_FLASH_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_CPLL, SOC_MOD_CLK_BBPLL}

/**
 * @brief Type of FLASH clock source.
 */
typedef enum {
    FLASH_CLK_SRC_DEFAULT = SOC_MOD_CLK_BBPLL,        /*!< Select SOC_MOD_CLK_BBPLL as FLASH source clock */
    FLASH_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,            /*!< Select SOC_MOD_CLK_XTAL as FLASH source clock */
    FLASH_CLK_SRC_CPLL = SOC_MOD_CLK_CPLL,            /*!< Select SOC_MOD_CLK_CPLL as FLASH source clock */
    FLASH_CLK_SRC_BBPLL = SOC_MOD_CLK_BBPLL,          /*!< Select SOC_MOD_CLK_BBPLL as FLASH source clock */
} soc_periph_flash_clk_src_t;

//////////////////////////////////////////////////MWDT/////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of MWDT
 */
#define SOC_MWDT_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_PLL_F80M, SOC_MOD_CLK_RC_FAST}

/**
 * @brief MWDT clock source
 */
typedef enum {
    MWDT_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,           /*!< Select XTAL as the source clock */
    MWDT_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M,   /*!< Select PLL fixed 80 MHz as the source clock */
    MWDT_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,     /*!< Select RTC fast as the source clock */
    MWDT_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,        /*!< Select XTAL 40 MHz as the default clock choice */
} soc_periph_mwdt_clk_src_t;

//////////////////////////////////////////////////SDM/////////////////////////////////////////////////////////////////

/**
 * @brief Sigma Delta Modulator clock source
 */
typedef enum {
    SDM_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL clock as the source clock */
    SDM_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M clock as the source clock */
    SDM_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M clock as the default clock choice */
} soc_periph_sdm_clk_src_t;

//////////////////////////////////////////////////GPIO Glitch Filter////////////////////////////////////////////////////

/**
 * @brief Glitch filter clock source
 */

typedef enum {
    GLITCH_FILTER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL clock as the source clock */
    GLITCH_FILTER_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M clock as the source clock */
    GLITCH_FILTER_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M clock as the default clock choice */
} soc_periph_glitch_filter_clk_src_t;

////////////////////////////////////////////////ANA_CMPR////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of Analog Comparator
 */
#define SOC_ANA_CMPR_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_PLL_F80M}

/**
 * @brief Type of Analog Comparator clock source
 */
typedef enum {
    ANA_CMPR_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL clock as the source clock */
    ANA_CMPR_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,   /*!< Select RC_FAST as the source clock */
    ANA_CMPR_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M as the source clock */
    ANA_CMPR_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M as the default clock choice */
} soc_periph_ana_cmpr_clk_src_t;

//////////////////////////////////////////////////TWAI//////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of TWAI
 */
#define SOC_TWAI_CLKS {(soc_periph_twai_clk_src_t)SOC_MOD_CLK_XTAL, (soc_periph_twai_clk_src_t)SOC_MOD_CLK_PLL_F80M}

/**
 * @brief TWAI clock source
 */
typedef enum {
    TWAI_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
    TWAI_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M as the source clock */
    TWAI_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M as the default clock choice */
} soc_periph_twai_clk_src_t;

//////////////////////////////////////////////////MCPWM/////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of MCPWM Timer
 */
#define SOC_MCPWM_TIMER_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_PLL_F160M}

/**
 * @brief Type of MCPWM timer clock source
 */
typedef enum {
    MCPWM_TIMER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,             /*!< Select XTAL as the source clock */
    MCPWM_TIMER_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,       /*!< Select RC_FAST as the source clock */
    MCPWM_TIMER_CLK_SRC_PLL160M = SOC_MOD_CLK_PLL_F160M,     /*!< Select PLL_F160M as the source clock */
    MCPWM_TIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M,     /*!< Select PLL_F160M as the default choice */
} soc_periph_mcpwm_timer_clk_src_t;

/**
 * @brief Array initializer for all supported clock sources of MCPWM Capture Timer
 */
#define SOC_MCPWM_CAPTURE_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_PLL_F160M}

/**
 * @brief Type of MCPWM capture clock source
 */
typedef enum {
    MCPWM_CAPTURE_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,           /*!< Select XTAL as the source clock */
    MCPWM_CAPTURE_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,     /*!< Select RC_FAST as the source clock */
    MCPWM_CAPTURE_CLK_SRC_PLL160M = SOC_MOD_CLK_PLL_F160M,   /*!< Select PLL_F160M as the source clock */
    MCPWM_CAPTURE_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M,   /*!< Select PLL_F160M as the default choice */
} soc_periph_mcpwm_capture_clk_src_t;

/**
 * @brief Array initializer for all supported clock sources of MCPWM Carrier
 */
#define SOC_MCPWM_CARRIER_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_PLL_F160M}

/**
 * @brief Type of MCPWM carrier clock source
 */
typedef enum {
    MCPWM_CARRIER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,           /*!< Select XTAL as the source clock */
    MCPWM_CARRIER_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,     /*!< Select RC_FAST as the source clock */
    MCPWM_CARRIER_CLK_SRC_PLL160M = SOC_MOD_CLK_PLL_F160M,   /*!< Select PLL_F160M as the source clock */
    MCPWM_CARRIER_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M,   /*!< Select PLL_F160M as the default choice */
} soc_periph_mcpwm_carrier_clk_src_t;

//////////////////////////////////////////////////I2S/////////////////////////////////////////////////////////////////

/**
 * @brief I2S clock source
 */
#define SOC_I2S_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_APLL, SOC_MOD_CLK_RC_FAST, I2S_CLK_SRC_EXTERNAL}

/**
 * @brief I2S clock source enum
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    I2S_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,                            /*!< Auto select maximum clock source as default source clock */
    I2S_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,                /*!< Select XTAL as the source clock */
    I2S_CLK_SRC_APLL = SOC_MOD_CLK_APLL,                /*!< Select APLL as the source clock */
    I2S_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,        /*!< Select RC_FAST as the source clock */
    I2S_CLK_SRC_EXTERNAL = -1,                          /*!< Select external clock as source clock */
} soc_periph_i2s_clk_src_t;

//////////////////////////////////////////////////PARLIO////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of PARLIO
 */
#define SOC_PARLIO_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_PLL_F160M}

/**
 * @brief PARLIO clock source
 */
typedef enum {
    PARLIO_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,           /*!< Select XTAL as the source clock */
    PARLIO_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,     /*!< Select RC_FAST as the source clock */
    PARLIO_CLK_SRC_PLL_F160M = SOC_MOD_CLK_PLL_F160M, /*!< Select PLL_F160M as the source clock */
    PARLIO_CLK_SRC_EXTERNAL = -1,                     /*!< Select EXTERNAL clock as the source clock */
    PARLIO_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M,   /*!< Select PLL_F160M as the default clock choice */
} soc_periph_parlio_clk_src_t;

//////////////////////////////////////////////CLOCK OUTPUT///////////////////////////////////////////////////////////
typedef enum {
    CLKOUT_SIG_MPLL_500M         = 0,   /*!< MPLL output at 500MHz */
    CLKOUT_SIG_BBPLL_480M        = 1,   /*!< BBPLL output at 480MHz */
    CLKOUT_SIG_CPLL_300M         = 2,   /*!< CPLL output at 300MHz */
    CLKOUT_SIG_XTAL_40M          = 3,   /*!< Main crystal clock at 40MHz */
    CLKOUT_SIG_FOSC_20M          = 4,   /*!< Fast RC oscillator ; nominally ~20MHz */
    CLKOUT_SIG_SOSC_600KD4       = 5,   /*!< Slow RC oscillator ; ~150kHz before RTC divider path, used for RTC slow timing */
    CLKOUT_SIG_RC_32K            = 6,   /*!< Internal ~32kHz RC oscillator output (low-precision; not recommended for applications) */
    CLKOUT_SIG_XTAL_32K          = 7,   /*!< External 32kHz crystal clock (XTAL32K on XTAL_32K_P/N) */
    CLKOUT_SIG_ANA_PLL_80M       = 8,   /*!< 80MHz analog PLL */
    CLKOUT_SIG_USB_OTGHS_PHYREF  = 9,   /*!< USB OTG HS PHY reference clock output for the high-speed PHY */
    CLKOUT_SIG_USB_OTGHS_ULPI    = 10,  /*!< USB OTG HS ULPI interface clock output */
    CLKOUT_SIG_MCPWM2            = 11,  /*!< MCPWM 2 module clock */
    CLKOUT_SIG_MCPWM3            = 12,  /*!< MCPWM 3 module clock */
    CLKOUT_SIG_CORE0_CLIC        = 13,  /*!< Core 0 clock */
    CLKOUT_SIG_CORE1_CLIC        = 14,  /*!< Core 1 clock */
    CLKOUT_SIG_AUDIO_PLL         = 15,  /*!< Audio PLL (APLL) output; frequency set via APLL configuration registers */
    CLKOUT_SIG_PAD_I2S0_MCLK     = 16,  /*!< I2S0 master clock */
    CLKOUT_SIG_PAD_I2S1_MCLK     = 17,  /*!< I2S1 master clock */
    CLKOUT_SIG_USB_DEVICE_INTPHY = 18,  /*!< Full-speed USB device internal PHY clock */
    CLKOUT_SIG_PAD_UART0_SLP     = 19,  /*!< UART0 sleep pad clock */
    CLKOUT_SIG_PAD_UART1_SLP     = 20,  /*!< UART1 sleep pad clock */
    CLKOUT_SIG_PAD_UART2_SLP     = 21,  /*!< UART2 sleep pad clock */
    CLKOUT_SIG_PAD_UART3_SLP     = 22,  /*!< UART3 sleep pad clock */
    CLKOUT_SIG_USB_DEVICE_48M    = 23,  /*!< USB device 48MHz functional clock output */
    CLKOUT_SIG_PAD_PARLIO_RX     = 24,  /*!< Parallel IO (PARLIO) RX clock */
    CLKOUT_SIG_PAD_PARLIO_TX     = 25,  /*!< Parallel IO (PARLIO) TX clock */
    CLKOUT_SIG_TCELL_CPU         = 26,  /*!< CPU domain clock */
    CLKOUT_SIG_TCELL_MEM         = 27,  /*!< memory subsystem clock */
    CLKOUT_SIG_TCELL_SYS         = 28,  /*!< system bus clock */
    CLKOUT_SIG_TCELL_APB         = 29,  /*!< APB peripheral bus clock */
    CLKOUT_SIG_FLASH_PLL         = 30,  /*!< MSPI flash PLL */
    CLKOUT_SIG_FLASH_CORE        = 31,  /*!< MSPI flash controller core clock */
    CLKOUT_SIG_PSRAM_PLL         = 32,  /*!< MSPI PSRAM PLL clock */
    CLKOUT_SIG_PSRAM_CORE        = 33,  /*!< MSPI PSRAM controller core clock */
    CLKOUT_SIG_ULPI_CLK_PAD_OUT  = 34,  /*!< ULPI clock forwarded to pad output */
    CLKOUT_SIG_SDIO_SLF          = 38,  /*!< SDIO slave  */
    CLKOUT_SIG_SDIO_DRV          = 39,  /*!< sdio_drv_clk_o */
    CLKOUT_SIG_SDIO_SAM          = 40,  /*!< sdio_sam_clk_o */
    CLKOUT_SIG_I2C0              = 44,  /*!< I2C0 peripheral module clock */
    CLKOUT_SIG_I2C1              = 45,  /*!< I2C1 peripheral module clock */
    CLKOUT_SIG_I2S0_RX           = 46,  /*!< I2S0 rx clock */
    CLKOUT_SIG_I2S0_TX           = 47,  /*!< I2S0 tx clock */
    CLKOUT_SIG_I2S1_RX           = 48,  /*!< I2S1 rx clock */
    CLKOUT_SIG_I2S1_TX           = 49,  /*!< I2S1 tx clock */
    CLKOUT_SIG_LCD               = 52,  /*!< LCD controller module clock */
    CLKOUT_SIG_UART0_SCLK        = 53,  /*!< UART0 source clock */
    CLKOUT_SIG_UART0_FCLK        = 54,  /*!< UART0 functional clock */
    CLKOUT_SIG_UART0_SLP         = 55,  /*!< UART0 sleep clock */
    CLKOUT_SIG_UART1_SCLK        = 56,  /*!< UART1 source clock */
    CLKOUT_SIG_UART1_FCLK        = 57,  /*!< UART1 functional clock */
    CLKOUT_SIG_UART1_SLP         = 58,  /*!< UART1 sleep clock */
    CLKOUT_SIG_UART2_SCLK        = 59,  /*!< UART2 source clock */
    CLKOUT_SIG_UART2_FCLK        = 60,  /*!< UART2 functional clock */
    CLKOUT_SIG_UART2_SLP         = 61,  /*!< UART2 sleep clock */
    CLKOUT_SIG_UART3_SCLK        = 62,  /*!< UART3 source clock */
    CLKOUT_SIG_UART3_FCLK        = 63,  /*!< UART3 functional clock */
    CLKOUT_SIG_UART3_SLP         = 64,  /*!< UART3 sleep clock */
    CLKOUT_SIG_CORDIC_CORE       = 65,  /*!< CORDIC accelerator core clock */
    CLKOUT_SIG_ZERO_DET_CORE     = 66,  /*!< Zero-cross / zero-detection peripheral core clock */
    CLKOUT_SIG_TWAI0             = 68,  /*!< TWAI0 module clock */
    CLKOUT_SIG_TWAI1             = 69,  /*!< TWAI1 module clock */
    CLKOUT_SIG_GPSPI2_HS         = 71,  /*!< SPI2 high-speed clock */
    CLKOUT_SIG_GPSPI2_MST        = 72,  /*!< SPI2 master functional clock */
    CLKOUT_SIG_GPSPI3_HS         = 73,  /*!< SPI3 high-speed clock */
    CLKOUT_SIG_GPSPI3_MST        = 74,  /*!< SPI3 master functional clock */
    CLKOUT_SIG_PARLIO_RX         = 75,  /*!< PARLIO RX clock */
    CLKOUT_SIG_PARLIO_TX         = 76,  /*!< PARLIO TX clock */
    CLKOUT_SIG_MCPWM0            = 78,  /*!< MCPWM0 module clock */
    CLKOUT_SIG_MCPWM1            = 79,  /*!< MCPWM1 module clock */
    CLKOUT_SIG_TIMERGRP0_T0      = 80,  /*!< Timer group 0, timer 0 module clock */
    CLKOUT_SIG_TIMERGRP0_T1      = 81,  /*!< Timer group 0, timer 1 module clock */
    CLKOUT_SIG_TIMERGRP0_WDT     = 82,  /*!< Timer group 0 watchdog module clock */
    CLKOUT_SIG_TIMERGRP1_T0      = 83,  /*!< Timer group 1, timer 0 module clock */
    CLKOUT_SIG_TIMERGRP1_T1      = 84,  /*!< Timer group 1, timer 1 module clock */
    CLKOUT_SIG_TIMERGRP1_WDT     = 85,  /*!< Timer group 1 watchdog module clock */
    CLKOUT_SIG_SYSTIMER          = 86,  /*!< System timer module clock */
    CLKOUT_SIG_LEDC0             = 87,  /*!< LED PWM controller instance 0 module clock */
    CLKOUT_SIG_RMT               = 88,  /*!< RMT peripheral module clock */
    CLKOUT_SIG_ADC               = 89,  /*!< ADC module clock */
    CLKOUT_SIG_ADC_SAR1          = 90,  /*!< SAR_ADC1 clock */
    CLKOUT_SIG_ADC_SAR2          = 91,  /*!< SAR_ADC2 clock */
    CLKOUT_SIG_ADC_SAR1_GCLK     = 92,  /*!< SAR_ADC1 GCLK */
    CLKOUT_SIG_ADC_SAR2_GCLK     = 93,  /*!< SAR_ADC2 GCLK */
    CLKOUT_SIG_PVT               = 94,  /*!< PVT module clock */
    CLKOUT_SIG_PVT_PERI_GROUP1   = 95,  /*!< PVT peripheral group 1 clock */
    CLKOUT_SIG_CRYPTO_SEC        = 96,  /*!< Security / crypto accelerator module clock */
    CLKOUT_SIG_LEDC1             = 97,  /*!< LED PWM controller instance 1 module clock */
    CLKOUT_SIG_IOMUX             = 98,  /*!< IO MUX clock */
    CLKOUT_SIG_REF_50M           = 99,  /*!< 50MHz reference clock */
    CLKOUT_SIG_REF_120M          = 100, /*!< 120MHz reference clock */
    CLKOUT_SIG_REF_160M          = 101, /*!< 160MHz reference clock */
    CLKOUT_SIG_REF_20M           = 102, /*!< 20MHz reference clock */
    CLKOUT_SIG_REF_240M          = 103, /*!< 240MHz reference clock */
    CLKOUT_SIG_REF_25M           = 104, /*!< 25MHz reference clock */
    CLKOUT_SIG_REF_80M           = 105, /*!< 80MHz reference clock */
    CLKOUT_SIG_REF_60M           = 106, /*!< 60MHz reference clock */
    /* Short / legacy aliases (identical mux index as the primary names above) */
    CLKOUT_SIG_MPLL              = CLKOUT_SIG_MPLL_500M,       /*!< Legacy short name; same mux as CLKOUT_SIG_MPLL_500M */
    CLKOUT_SIG_BBPLL             = CLKOUT_SIG_BBPLL_480M,      /*!< Legacy short name; same mux as CLKOUT_SIG_BBPLL_480M */
    CLKOUT_SIG_CPLL              = CLKOUT_SIG_CPLL_300M,       /*!< Legacy short name; same mux as CLKOUT_SIG_CPLL_300M */
    CLKOUT_SIG_XTAL              = CLKOUT_SIG_XTAL_40M,        /*!< Legacy short name; same mux as CLKOUT_SIG_XTAL_40M */
    CLKOUT_SIG_RC_FAST           = CLKOUT_SIG_FOSC_20M,        /*!< Legacy name for RC_FAST / FOSC domain; same mux as CLKOUT_SIG_FOSC_20M */
    CLKOUT_SIG_RC_SLOW           = CLKOUT_SIG_SOSC_600KD4,     /*!< Legacy name for RC_SLOW / SOSC domain; same mux as CLKOUT_SIG_SOSC_600KD4 */
    CLKOUT_SIG_XTAL32K           = CLKOUT_SIG_XTAL_32K,        /*!< Legacy short name; same mux as CLKOUT_SIG_XTAL_32K */
    CLKOUT_SIG_I2S0              = CLKOUT_SIG_PAD_I2S0_MCLK,   /*!< Legacy short name; same mux as CLKOUT_SIG_PAD_I2S0_MCLK */
    CLKOUT_SIG_I2S1              = CLKOUT_SIG_PAD_I2S1_MCLK,    /*!< Legacy short name; same mux as CLKOUT_SIG_PAD_I2S1_MCLK */
    CLKOUT_SIG_CPU               = CLKOUT_SIG_TCELL_CPU,       /*!< Legacy short name; same mux as CLKOUT_SIG_TCELL_CPU */
    CLKOUT_SIG_MEM               = CLKOUT_SIG_TCELL_MEM,       /*!< Legacy short name; same mux as CLKOUT_SIG_TCELL_MEM */
    CLKOUT_SIG_SYS               = CLKOUT_SIG_TCELL_SYS,       /*!< Legacy short name; same mux as CLKOUT_SIG_TCELL_SYS */
    CLKOUT_SIG_APB               = CLKOUT_SIG_TCELL_APB,       /*!< Legacy short name; same mux as CLKOUT_SIG_TCELL_APB */
    CLKOUT_SIG_PLL_F80M          = CLKOUT_SIG_REF_80M,         /*!< Legacy name; same mux as CLKOUT_SIG_REF_80M (80MHz reference tap) */
    CLKOUT_SIG_INVALID           = 0xFF,                       /*!< Invalid mux value; channel disabled or not connected */
} soc_clkout_sig_id_t;

//////////////////////////////////////CLOCK FREQUENCY CALCULATION////////////////////////////////////////////////////
/**
 * @brief Clock frequency calculation source selection
 */
typedef enum {
    CLK_CAL_RTC_SLOW = -1,                           /*!< Select to calculate frequency of RTC_SLOW_CLK */
    CLK_CAL_MPLL,                                    /*!< Select to calculate frequency of MPLL_CLK */
    CLK_CAL_BBPLL,                                   /*!< Select to calculate frequency of BBPLL_CLK */
    CLK_CAL_CPLL,                                    /*!< Select to calculate frequency of CPLL_CLK */
    CLK_CAL_APLL,                                    /*!< Select to calculate frequency of APLL_CLK */
    CLK_CAL_AHB,                                     /*!< Select to calculate frequency of AHB_CLK */
    CLK_CAL_APB,                                     /*!< Select to calculate frequency of APB_CLK */
    CLK_CAL_ROM,                                     /*!< Select to calculate frequency of ROM_CLK */
    CLK_CAL_RC_FAST,                                 /*!< Select to calculate frequency of RC_FAST_CLK */
    CLK_CAL_RC_SLOW,                                 /*!< Select to calculate frequency of RC_SLOW_CLK */
    CLK_CAL_32K_XTAL,                                /*!< Select to calculate frequency of XTAL32K_CLK */
    CLK_CAL_XTAL,                                    /*!< Select to calculate frequency of XTAL_CLK */
    CLK_CAL_UART0_PAD,                               /*!< Select to calculate frequency of UART0_PAD_CLK */
    CLK_CAL_CORE0,                                   /*!< Select to calculate frequency of CORE0_CLK */
    CLK_CAL_CORE1,                                   /*!< Select to calculate frequency of CORE1_CLK */
} soc_clk_freq_calculation_src_t;

//////////////////////////////////////////////////CORDIC///////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of CORDIC
 */
#define SOC_CORDIC_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_PLL_F160M}

typedef enum {
    CORDIC_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,
    CORDIC_CLK_SRC_PLL_F160M = SOC_MOD_CLK_PLL_F160M,
    CORDIC_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,
    CORDIC_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M,
} soc_periph_cordic_clk_src_t;

//////////////////////////////////////////////////PCNT//////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of PCNT
 */
#define SOC_PCNT_CLKS {SOC_MOD_CLK_APB}

/**
 * @brief Type of PCNT clock source
 */
typedef enum {
    PCNT_CLK_SRC_APB = SOC_MOD_CLK_APB,           /*!< Select APB as the source clock */
    PCNT_CLK_SRC_DEFAULT = SOC_MOD_CLK_APB,       /*!< Select APB as the default choice */
} soc_periph_pcnt_clk_src_t;

//////////////////////////////////////////////////ADC///////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of ADC
 */
#define SOC_ADC_DIGI_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST}

typedef enum {
    ADC_DIGI_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,   /*!< Select FOSC (internal RC oscillator) as ADC source clock */
    ADC_DIGI_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as ADC source clock */
    ADC_DIGI_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,       /*!< Select XTAL as the default clock choice */
} soc_periph_adc_digi_clk_src_t;

//////////////////////////////////////////////////Temp Sensor///////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of Temperature Sensor
 */
#define SOC_TEMP_SENSOR_CLKS {SOC_MOD_CLK_LP_PERI}

/**
* @brief Type of Temp Sensor clock source
*/
typedef enum {
    TEMPERATURE_SENSOR_CLK_SRC_LP_PERI = SOC_MOD_CLK_LP_PERI,     /*!< Select LP_PERI as the source clock */
    TEMPERATURE_SENSOR_CLK_SRC_DEFAULT = SOC_MOD_CLK_LP_PERI,    /*!< Select LP_PERI as the default choice */
} soc_periph_temperature_sensor_clk_src_t;

#ifdef __cplusplus
}
#endif
