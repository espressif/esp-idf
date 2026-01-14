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

// TODO: to be checked IDF-14696

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
 *    This RC oscillator generates a ~136kHz clock signal output as the RC_SLOW_CLK. The exact frequency of this clock
 *    can be computed in runtime through calibration.
 *
 * 4) Internal 32kHz RC Oscillator: RC32K [NOT RECOMMENDED TO USE]
 *
 *    The exact frequency of this clock can be computed in runtime through calibration.
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
 * 1) CPLL (320/360/400MHz), CPU_PLL, used for CPU, MSPI-Flash, MSPI-PSRAM clock source
 * 2) MPLL (configurable, 400MHz at power-on reset), MSPI_PLL, used for MSPI-PSRAM clock source; and is further divided to PLL_F50M, PLL_F25M, to be used for peripheral's clock sources
 * 3) SPLL (480MHz), SYS_PLL (AXI/AHB), directly used for MSPI-Flash, MSPI-PSRAM, GPSPI clock sources; and is further divided to PLL_F240M, PLL_F160M, PLL_F120M, PLL_F80M, PLL_F20M, to be used for peripherals' clock sources
 * 4) APLL (configurable), AUDIO_PLL, can be the clock source for peripherals (I2S, GPSPI, LCD, CAM, etc.)
 * 5) SDIO_PLL0/1/2
 *
 * from 32kHz slow clock oscillator frequency multiplier:
 * 6) LP_PLL (8MHz), used for RTC_FAST_CLK clock source and LP peripherals' clock sources
 */

/* With the default value of FOSC_DFREQ = 100, RC_FAST clock frequency is 17.5 MHz +/- 7% */
#define SOC_CLK_RC_FAST_FREQ_APPROX         17500000                            /*!< Approximate RC_FAST_CLK frequency in Hz */
#define SOC_CLK_RC_SLOW_FREQ_APPROX         136000                              /*!< Approximate RC_SLOW_CLK frequency in Hz */
#define SOC_CLK_RC32K_FREQ_APPROX           32768                               /*!< Approximate RC32K_CLK frequency in Hz */
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
    SOC_ROOT_CLK_INT_RC32K,            /*!< Internal 32kHz RC oscillator */
    SOC_ROOT_CLK_EXT_OSC_SLOW,         /*!< External slow clock signal at pin1 */
} soc_root_clk_t;

/**
 * @brief ROOT clock circuit, which requires explicitly enabling the targeting circuit to use
 */
typedef enum {
    SOC_ROOT_CIRCUIT_CLK_BBPLL,         /*!< BBPLL_CLK is the output of the BBPLL generator circuit */
} soc_root_clk_circuit_t;

/**
 * @brief CPU_CLK mux inputs, which are the supported clock sources for the CPU_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_CPU_CLK_SRC_XTAL = 0,              /*!< Select XTAL_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_CPLL = 1,              /*!< Select CPLL_CLK as CPU_CLK source (CPLL_CLK is the output of 40MHz crystal oscillator frequency multiplier, can be 320/360/400MHz) */
    SOC_CPU_CLK_SRC_PLL = SOC_CPU_CLK_SRC_CPLL, /*!< Alias name for `SOC_CPU_CLK_SRC_CPLL` */
    SOC_CPU_CLK_SRC_RC_FAST = 2,           /*!< Select RC_FAST_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_INVALID,               /*!< Invalid CPU_CLK source */
} soc_cpu_clk_src_t;

/**
 * @brief RTC_SLOW_CLK mux inputs, which are the supported clock sources for the RTC_SLOW_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_RTC_SLOW_CLK_SRC_RC_SLOW = 0,                 /*!< Select RC_SLOW_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_XTAL32K = 1,                 /*!< Select XTAL32K_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_RC32K = 2,                   /*!< Select RC32K_CLK as RTC_SLOW_CLK source */
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
    SOC_RTC_FAST_CLK_SRC_LP_PLL = 2,       /*!< Select LP_PLL_CLK as RTC_FAST_CLK source (LP_PLL_CLK is a 8MHz clock sourced from RC32K or XTAL32K) */
    SOC_RTC_FAST_CLK_SRC_INVALID,          /*!< Invalid RTC_FAST_CLK source */

    SOC_RTC_FAST_CLK_SRC_DEFAULT = SOC_RTC_FAST_CLK_SRC_XTAL, /*!< XTAL_CLK is the default clock source for RTC_FAST_CLK */

    SOC_RTC_FAST_CLK_SRC_XTAL_DIV __attribute__((deprecated)) = SOC_RTC_FAST_CLK_SRC_XTAL, /*!< Alias name for `SOC_RTC_FAST_CLK_SRC_XTAL` */
} soc_rtc_fast_clk_src_t;

/**
 * @brief LP_PLL_CLK mux inputs, which are the supported clock sources for the LP_PLL_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_LP_PLL_CLK_SRC_RC32K = 0,          /*!< Select RC32K_CLK as LP_PLL_CLK source */
    SOC_LP_PLL_CLK_SRC_XTAL32K = 1,        /*!< Select XTAL32K_CLK as LP_PLL_CLK source */
    SOC_LP_PLL_CLK_SRC_INVALID,            /*!< Invalid LP_PLL_CLK source */
} soc_lp_pll_clk_src_t;

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
    SOC_MOD_CLK_RTC_FAST,                      /*!< RTC_FAST_CLK can be sourced from XTAL, RC_FAST, or LP_PLL by configuring soc_rtc_fast_clk_src_t */
    SOC_MOD_CLK_RTC_SLOW,                      /*!< RTC_SLOW_CLK can be sourced from RC_SLOW, XTAL32K, or RC32K by configuring soc_rtc_slow_clk_src_t */
    // For digital domain: peripherals
    SOC_MOD_CLK_PLL_F20M,                      /*!< PLL_F20M_CLK is derived from SPLL (clock gating + default divider 24), its default frequency is 20MHz */
    SOC_MOD_CLK_PLL_F25M,                      /*!< PLL_F25M_CLK is derived from MPLL (clock gating + configurable divider), it will have a frequency of 25MHz */
    SOC_MOD_CLK_PLL_F50M,                      /*!< PLL_F50M_CLK is derived from MPLL (clock gating + configurable divider 10), it will have a frequency of 50MHz */
    SOC_MOD_CLK_PLL_F80M,                      /*!< PLL_F80M_CLK is derived from SPLL (clock gating + default divider 6), its default frequency is 80MHz */
    SOC_MOD_CLK_PLL_F160M,                     /*!< PLL_F160M_CLK is derived from SPLL (clock gating + default divider 3), its default frequency is 160MHz */
    SOC_MOD_CLK_PLL_F240M,                     /*!< PLL_F240M_CLK is derived from SPLL (clock gating + default divider 2), its default frequency is 240MHz */
    SOC_MOD_CLK_CPLL,                          /*!< CPLL is from 40MHz XTAL oscillator frequency multipliers */
    SOC_MOD_CLK_SPLL,                          /*!< SPLL is from 40MHz XTAL oscillator frequency multipliers, its default frequency is 480MHz */
    SOC_MOD_CLK_MPLL,                          /*!< MPLL is from 40MHz XTAL oscillator frequency multipliers */
    SOC_MOD_CLK_SDIO_PLL,                      /*!< SDIO PLL is from 40MHz XTAL oscillator frequency multipliers, its default frequency is 200MHz */
    SOC_MOD_CLK_XTAL32K,                       /*!< XTAL32K_CLK comes from the external 32kHz crystal, passing a clock gating to the peripherals */
    SOC_MOD_CLK_RC_FAST,                       /*!< RC_FAST_CLK comes from the internal 20MHz rc oscillator, passing a clock gating to the peripherals */
    SOC_MOD_CLK_XTAL,                          /*!< XTAL_CLK comes from the external 40MHz crystal */
    SOC_MOD_CLK_APLL,                          /*!< Audio PLL is sourced from PLL, and its frequency is configurable through APLL configuration registers */
    // For LP peripherals
    SOC_MOD_CLK_XTAL_D2,                       /*!< XTAL_D2_CLK comes from the external 40MHz crystal, passing a div of 2 to the LP peripherals */
    SOC_MOD_CLK_LP_PLL,                        /*!< LP_PLL is from 32kHz XTAL oscillator frequency multipliers, it has a fixed frequency of 8MHz */
    SOC_MOD_CLK_LP_DYN_FAST,                   /*!< LP_DYN_FAST can be derived from RTC_SLOW_CLK or RTC_FAST_CLK depending on the chip’s power mode:
                                                    Only in LP_SLEEP mode, select RTC_SLOW_CLK as the clock source;
                                                    In other non-LP_SLEEP mode, select RTC_FAST_CLK as the clock source; */
    SOC_MOD_CLK_LP_PERI,                       /*!< LP_PERI_CLK is derived from LP_DYN_FAST (configurable divider) */
    SOC_MOD_CLK_INVALID,                       /*!< Indication of the end of the available module clock sources */
} soc_module_clk_t;

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
#if SOC_CLK_TREE_SUPPORTED
#define SOC_GPTIMER_CLKS {SOC_MOD_CLK_PLL_F80M, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_XTAL}
#else
#define SOC_GPTIMER_CLKS {SOC_MOD_CLK_XTAL}
#endif

/**
 * @brief Type of GPTimer clock source
 */
typedef enum {
    GPTIMER_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M as the source clock */
    GPTIMER_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,   /*!< Select RC_FAST as the source clock */
    GPTIMER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL as the source clock */
#if SOC_CLK_TREE_SUPPORTED
    GPTIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M as the default choice */
#else
    GPTIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,      /*!< Select XTAL as the default choice */
#endif // SOC_CLK_TREE_SUPPORTED
} soc_periph_gptimer_clk_src_t;

/**
 * @brief Type of Timer Group clock source, reserved for the legacy timer group driver
 */
typedef enum {
    TIMER_SRC_CLK_PLL_F80M = SOC_MOD_CLK_PLL_F80M,     /*!< Timer group clock source is PLL_F80M */
    TIMER_SRC_CLK_XTAL = SOC_MOD_CLK_XTAL,             /*!< Timer group clock source is XTAL */
#if SOC_CLK_TREE_SUPPORTED
    TIMER_SRC_CLK_DEFAULT = SOC_MOD_CLK_PLL_F80M,      /*!< Timer group clock source default choice is PLL_F80M */
#else
    TIMER_SRC_CLK_DEFAULT = SOC_MOD_CLK_XTAL,          /*!< Timer group clock source default choice is XTAL */
#endif // SOC_CLK_TREE_SUPPORTED
} soc_periph_tg_clk_src_legacy_t;

//////////////////////////////////////////////////RMT///////////////////////////////////////////////////////////////////

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
    UART_SCLK_DEFAULT = SOC_MOD_CLK_XTAL,           /*!< UART source clock default choice is XTAL in fpga env */
} soc_periph_uart_clk_src_legacy_t;

/////////////////////////////////////////////////SPI////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of SPI
 */
#define SOC_SPI_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_SPLL}

/**
 * @brief Type of SPI clock source.
 */
typedef enum {
    SPI_CLK_SRC_XTAL     = SOC_MOD_CLK_XTAL,        /*!< Select XTAL as SPI source clock */
    SPI_CLK_SRC_RC_FAST  = SOC_MOD_CLK_RC_FAST,     /*!< Select RC_FAST_20M as SPI source clock */
    SPI_CLK_SRC_SPLL     = SOC_MOD_CLK_SPLL,        /*!< Select SPLL as SPI source clock */
    SPI_CLK_SRC_DEFAULT  = SOC_MOD_CLK_SPLL,        /*!< Select XTAL as default source clock */
} soc_periph_spi_clk_src_t;

/////////////////////////////////////////////////FLASH////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of FLASH
 */
#define SOC_FLASH_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_CPLL, SOC_MOD_CLK_SPLL}

/**
 * @brief Type of FLASH clock source.
 */
typedef enum {
    FLASH_CLK_SRC_DEFAULT = SOC_MOD_CLK_SPLL,         /*!< Select SOC_MOD_CLK_SPLL as FLASH source clock */
    FLASH_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,            /*!< Select SOC_MOD_CLK_XTAL as FLASH source clock */
    FLASH_CLK_SRC_CPLL = SOC_MOD_CLK_CPLL,            /*!< Select SOC_MOD_CLK_CPLL as FLASH source clock */
    FLASH_CLK_SRC_SPLL = SOC_MOD_CLK_SPLL,            /*!< Select SOC_MOD_CLK_SPLL as FLASH source clock */
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

/**
 * @brief Sigma Delta Modulator clock source
 */
typedef enum {
    SDM_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,         /*!< Select XTAL clock as the source clock */
    SDM_CLK_SRC_PLL_F80M = SOC_MOD_CLK_PLL_F80M, /*!< Select PLL_F80M clock as the source clock */
    SDM_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F80M,  /*!< Select PLL_F80M clock as the default clock choice */
} soc_periph_sdm_clk_src_t;

//////////////////////////////////////////////CLOCK OUTPUT///////////////////////////////////////////////////////////
typedef enum {
    CLKOUT_SIG_MPLL     = 0,    /*!< MPLL is from 40MHz XTAL oscillator frequency multipliers */
    CLKOUT_SIG_SPLL     = 1,    /*!< SPLL is from 40MHz XTAL oscillator frequency multipliers, its default frequency is 480MHz */
    CLKOUT_SIG_CPLL     = 2,    /*!< CPLL_CLK is the output of 40MHz crystal oscillator frequency multiplier, can be 320/360/400MHz */
    CLKOUT_SIG_XTAL     = 3,    /*!< External 40MHz crystal */
    CLKOUT_SIG_RC_FAST  = 4,    /*!< Internal 17.5MHz RC oscillator */
    CLKOUT_SIG_RC_SLOW  = 5,    /*!< Internal 136kHz RC oscillator */
    CLKOUT_SIG_RC_32K   = 6,    /*!< Internal 32kHz RC oscillator */
    CLKOUT_SIG_XTAL32K  = 7,    /*!< External 32kHz crystal clock */
    CLKOUT_SIG_I2S0     = 16,   /*!< I2S0 clock, depends on the i2s driver configuration */
    CLKOUT_SIG_I2S1     = 17,   /*!< I2S1 clock, depends on the i2s driver configuration */
    CLKOUT_SIG_I2S2     = 18,   /*!< I2S2 clock, depends on the i2s driver configuration */
    CLKOUT_SIG_CPU      = 26,   /*!< CPU clock */
    CLKOUT_SIG_MEM      = 27,   /*!< MEM clock */
    CLKOUT_SIG_SYS      = 28,   /*!< SYS clock */
    CLKOUT_SIG_APB      = 29,   /*!< APB clock */
    CLKOUT_SIG_PLL_F80M = 105,  /*!< From PLL, usually be 80MHz */
    CLKOUT_SIG_INVALID  = 0xFF,
} soc_clkout_sig_id_t;

//////////////////////////////////////CLOCK FREQUENCY CALCULATION////////////////////////////////////////////////////
/**
 * @brief Clock frequency calculation source selection
 */
typedef enum {
    CLK_CAL_RTC_SLOW = -1,                           /*!< Select to calculate frequency of RTC_SLOW_CLK */
    CLK_CAL_MPLL,                                    /*!< Select to calculate frequency of MPLL_CLK */
    CLK_CAL_SPLL,                                    /*!< Select to calculate frequency of SPLL_CLK */
    CLK_CAL_CPLL,                                    /*!< Select to calculate frequency of CPLL_CLK */
    CLK_CAL_APLL,                                    /*!< Select to calculate frequency of APLL_CLK */
    CLK_CAL_SDIO_PLL0,                               /*!< Select to calculate frequency of SDIO_PLL0_CLK */
    CLK_CAL_SDIO_PLL1,                               /*!< Select to calculate frequency of SDIO_PLL1_CLK */
    CLK_CAL_SDIO_PLL2,                               /*!< Select to calculate frequency of SDIO_PLL2_CLK */
    CLK_CAL_RC_FAST,                                 /*!< Select to calculate frequency of RC_FAST_CLK */
    CLK_CAL_RC_SLOW,                                 /*!< Select to calculate frequency of RC_SLOW_CLK */
    CLK_CAL_RC32K,                                   /*!< Select to calculate frequency of RC32K_CLK */
    CLK_CAL_32K_XTAL,                                /*!< Select to calculate frequency of XTAL32K_CLK */
    CLK_CAL_LP_PLL,                                  /*!< Select to calculate frequency of LP_PLL_CLK */
} soc_clk_freq_calculation_src_t;

#ifdef __cplusplus
}
#endif
