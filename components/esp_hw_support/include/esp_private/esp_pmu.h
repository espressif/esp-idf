/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "soc/soc_caps.h"

#if SOC_PMU_SUPPORTED
#include "hal/pmu_hal.h"
#include "pmu_param.h"
#include "pmu_bit_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PMU ICG modem code of HP system
 * @note  This type is required in rtc_clk_init.c when PMU not fully supported
 */
typedef enum {
    PMU_HP_ICG_MODEM_CODE_SLEEP = 0,
    PMU_HP_ICG_MODEM_CODE_MODEM = 1,
    PMU_HP_ICG_MODEM_CODE_ACTIVE = 2,
} pmu_hp_icg_modem_mode_t;

#if SOC_PMU_SUPPORTED

#define RTC_SLEEP_PD_DIG                PMU_SLEEP_PD_TOP        //!< Deep sleep (power down digital domain, includes all power domains
                                                                //   except CPU, Modem, LP peripheral, AON，VDDSDIO, MEM and clock power domains)
#define RTC_SLEEP_PD_RTC_PERIPH         PMU_SLEEP_PD_LP_PERIPH  //!< Power down RTC peripherals
#define RTC_SLEEP_PD_VDDSDIO            PMU_SLEEP_PD_VDDSDIO    //!< Power down VDDSDIO regulator
#define RTC_SLEEP_PD_CPU                PMU_SLEEP_PD_CPU        //!< Power down CPU when in lightsleep, but not restart
#define RTC_SLEEP_PD_DIG_PERIPH         PMU_SLEEP_PD_HP_PERIPH  //!< Power down DIG peripherals
#define RTC_SLEEP_PD_INT_8M             PMU_SLEEP_PD_RC_FAST    //!< Power down Internal 20M oscillator
#define RTC_SLEEP_PD_XTAL               PMU_SLEEP_PD_XTAL       //!< Power down main XTAL
#define RTC_SLEEP_PD_MODEM              PMU_SLEEP_PD_MODEM      //!< Power down modem(include wifi, ble and 15.4)

//These flags are not power domains, but will affect some sleep parameters
#define RTC_SLEEP_DIG_USE_8M            BIT(16)
#define RTC_SLEEP_USE_ADC_TESEN_MONITOR BIT(17)
#define RTC_SLEEP_NO_ULTRA_LOW          BIT(18) //!< Avoid using ultra low power in deep sleep, in which RTCIO cannot be used as input, and RTCMEM can't work under high temperature

#if SOC_PM_SUPPORT_EXT0_WAKEUP
#define RTC_EXT0_TRIG_EN            PMU_EXT0_WAKEUP_EN      //!< EXT0 wakeup
#else
#define RTC_EXT0_TRIG_EN            0
#endif

#if SOC_PM_SUPPORT_EXT1_WAKEUP
#define RTC_EXT1_TRIG_EN            PMU_EXT1_WAKEUP_EN      //!< EXT1 wakeup
#else
#define RTC_EXT1_TRIG_EN            0
#endif

#define RTC_GPIO_TRIG_EN            PMU_GPIO_WAKEUP_EN      //!< GPIO wakeup

#if SOC_LP_TIMER_SUPPORTED
#define RTC_TIMER_TRIG_EN           PMU_LP_TIMER_WAKEUP_EN  //!< Timer wakeup
#else
#define RTC_TIMER_TRIG_EN           0
#endif

#if SOC_WIFI_SUPPORTED
#define RTC_WIFI_TRIG_EN            PMU_WIFI_SOC_WAKEUP_EN  //!< WIFI wakeup (light sleep only)
#else
#define RTC_WIFI_TRIG_EN            0
#endif

#if SOC_UART_SUPPORT_WAKEUP_INT
#define RTC_UART0_TRIG_EN           PMU_UART0_WAKEUP_EN     //!< UART0 wakeup (light sleep only)
#define RTC_UART1_TRIG_EN           PMU_UART1_WAKEUP_EN     //!< UART1 wakeup (light sleep only)
#else
#define RTC_UART0_TRIG_EN           0
#define RTC_UART1_TRIG_EN           0
#endif

#if SOC_BT_SUPPORTED
#define RTC_BT_TRIG_EN              PMU_BLE_SOC_WAKEUP_EN   //!< BT wakeup (light sleep only)
#else
#define RTC_BT_TRIG_EN              0
#endif

#define RTC_USB_TRIG_EN             PMU_USB_WAKEUP_EN

#if SOC_LP_CORE_SUPPORTED
#define RTC_LP_CORE_TRIG_EN         PMU_LP_CORE_WAKEUP_EN   //!< LP core wakeup
#else
#define RTC_LP_CORE_TRIG_EN         0
#endif //SOC_LP_CORE_SUPPORTED

#define RTC_XTAL32K_DEAD_TRIG_EN    0 // TODO
#define RTC_BROWNOUT_DET_TRIG_EN    0 // TODO

/**
 * RTC_SLEEP_REJECT_MASK records sleep reject sources supported by chip
 */
#define RTC_SLEEP_REJECT_MASK (RTC_EXT0_TRIG_EN         | \
                               RTC_EXT1_TRIG_EN         | \
                               RTC_GPIO_TRIG_EN         | \
                               RTC_TIMER_TRIG_EN        | \
                               RTC_WIFI_TRIG_EN         | \
                               RTC_UART0_TRIG_EN        | \
                               RTC_UART1_TRIG_EN        | \
                               RTC_BT_TRIG_EN           | \
                               RTC_LP_CORE_TRIG_EN      | \
                               RTC_XTAL32K_DEAD_TRIG_EN | \
                               RTC_USB_TRIG_EN          | \
                               RTC_BROWNOUT_DET_TRIG_EN)


#define PMU_SLEEP_PD_TOP            BIT(0)
#define PMU_SLEEP_PD_VDDSDIO        BIT(1)
#define PMU_SLEEP_PD_MODEM          BIT(2)
#define PMU_SLEEP_PD_HP_PERIPH      BIT(3)
#define PMU_SLEEP_PD_CPU            BIT(4)

#if SOC_PM_SUPPORT_HP_AON_PD
#define PMU_SLEEP_PD_HP_AON         BIT(5)
#endif

#define PMU_SLEEP_PD_MEM_G0         BIT(6)
#define PMU_SLEEP_PD_MEM_G1         BIT(7)
#define PMU_SLEEP_PD_MEM_G2         BIT(8)
#define PMU_SLEEP_PD_MEM_G3         BIT(9)
#define PMU_SLEEP_PD_MEM            (PMU_SLEEP_PD_MEM_G0|PMU_SLEEP_PD_MEM_G1|PMU_SLEEP_PD_MEM_G2|PMU_SLEEP_PD_MEM_G3)
#define PMU_SLEEP_PD_XTAL           BIT(10)
#define PMU_SLEEP_PD_RC_FAST        BIT(11)
#define PMU_SLEEP_PD_XTAL32K        BIT(12)
#define PMU_SLEEP_PD_RC32K          BIT(13)
#define PMU_SLEEP_PD_LP_PERIPH      BIT(14)

#if SOC_PM_SUPPORT_CNNT_PD
#define PMU_SLEEP_PD_CNNT           BIT(15)
#endif

typedef struct {
    pmu_hal_context_t *hal;
    void *mc;
} pmu_context_t;

pmu_context_t * PMU_instance(void);

typedef enum pmu_hp_sysclk_src {
    PMU_HP_SYSCLK_XTAL = 0,
    PMU_HP_SYSCLK_PLL,
    PMU_HP_SYSCLK_FOSC
} pmu_hp_sysclk_src_t;

typedef enum pmu_sleep_protect_mode {
    PMU_SLEEP_PROTECT_HP_SLEEP = 0,
    PMU_SLEEP_PROTECT_XTAL,
    PMU_SLEEP_PROTECT_HP_LP_SLEEP,
    PMU_SLEEP_PROTECT_DISABLE
} pmu_sleep_protect_mode_t;

typedef enum pmu_sleep_regdma_entry {
    PMU_SLEEP_REGDMA_ENTRY_0 = 0,
    PMU_SLEEP_REGDMA_ENTRY_1,
    PMU_SLEEP_REGDMA_ENTRY_2,
    PMU_SLEEP_REGDMA_ENTRY_3,
    PMU_SLEEP_REGDMA_ENTRY_MAX
} pmu_sleep_regdma_entry_t;

/**
  * @brief  Enable_regdma_backup.
  */
void pmu_sleep_enable_regdma_backup(void);

/**
  * @brief  Disable_regdma_backup.
  */
void pmu_sleep_disable_regdma_backup(void);

/**
 * @brief Get sleep PLL enable status
 *
 * @return true if PLL is enabled by PMU in modem state
 */
bool pmu_sleep_pll_already_enabled(void);

/**
 * @brief Calculate the LP system hardware time overhead during sleep
 *
 * @param pd_flags flags indicates the power domain that will be powered down
 * @param slowclk_period re-calibrated slow clock period
 * @param fastclk_period re-calibrated fast clock period
 *
 * @return hardware time overhead in us
 */
uint32_t pmu_sleep_calculate_lp_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period);

/**
 * @brief Calculate the HP system hardware time overhead during sleep
 *
 * @param pd_flags flags indicates the power domain that will be powered down
 * @param slowclk_period re-calibrated slow clock period
 * @param fastclk_period re-calibrated fast clock period
 *
 * @return hardware time overhead in us
 */
uint32_t pmu_sleep_calculate_hp_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period);

/**
 * @brief Calculate the hardware time overhead during sleep to compensate for sleep time
 *
 * @param pd_flags flags indicates the power domain that will be powered down
 * @param slowclk_period re-calibrated slow clock period
 * @param fastclk_period re-calibrated fast clock period
 *
 * @return hardware time overhead in us
 */
uint32_t pmu_sleep_calculate_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period);

/**
 * @brief Get default sleep configuration
 * @param config pmu_sleep_config instance
 * @param pd_flags flags indicates the power domain that will be powered down
 * @param adjustment total software and hardware time overhead
 * @param slowclk_period re-calibrated slow clock period in microseconds,
 *                       Q13.19 fixed point format
 * @param fastclk_period re-calibrated fast clock period in microseconds,
 *                       Q13.19 fixed point format
 * @param dslp configuration for deep sleep mode

 * @return hardware time overhead in us
 */
const pmu_sleep_config_t* pmu_sleep_config_default(pmu_sleep_config_t *config, uint32_t pd_flags, uint32_t adjustment, uint32_t slowclk_period, uint32_t fastclk_period, bool dslp);

/**
 * @brief Prepare the chip to enter sleep mode
 *
 * This function configures various power/analog parameters and lp/lp system configuration
 * used in sleep state machines
 *
 * This function does not actually enter sleep mode; this is done using
 * pmu_sleep_start function. Software may do some other actions between
 * pmu_sleep_init and pmu_sleep_start, such as set wakeup timer and configure
 * wakeup sources.
 *
 * @param config sleep mode configuration
 *
 * @param dslp is initialize for deep sleep mode
 */
void pmu_sleep_init(const pmu_sleep_config_t *config, bool dslp);

#if SOC_DCDC_SUPPORTED
/**
 * @brief Increase hp_ldo voltage, in preparation for taking over the power supply from DCDC
 */
void pmu_sleep_increase_ldo_volt(void);

/**
 * @brief LDO has taken over power supply, shut down DCDC to save power consumption and goto sleep
 *        and after shutdown the DCDC, it is also necessary to decrease the LDO voltage to save
 *        power in the sleep and wake-up processes.
 */
void pmu_sleep_shutdown_dcdc(void);

/**
 * @brief DCDC has taken over power supply, shut down LDO to save power consumption
 */
void pmu_sleep_shutdown_ldo(void);
#endif

/**
 * @brief Enter deep or light sleep mode
 *
 * This function enters the sleep mode previously configured using pmu_sleep_init
 * function. Before entering sleep, software should configure wake up sources
 * appropriately (set up GPIO wakeup registers, timer wakeup registers,
 * and so on).
 *
 * If deep sleep mode was configured using pmu_sleep_init, and sleep is not
 * rejected by hardware (based on reject_opt flags), this function never returns.
 * When the chip wakes up from deep sleep, CPU is reset and execution starts
 * from ROM bootloader.
 *
 * If light sleep mode was configured using pmu_sleep_init, this function
 * returns on wakeup, or if sleep is rejected by hardware.
 *
 * @param wakeup_opt  bit mask wake up reasons to enable (RTC_xxx_TRIG_EN flags
 *                    combined with OR)
 * @param reject_opt  bit mask of sleep reject reasons, used to
 *                    prevent wakeup source set before the sleep request)
 * @param lslp_mem_inf_fpu If non-zero then the low power config is restored
 *                         immediately on wake. Recommended for light sleep,
 *                         has no effect if the system goes into deep sleep.
 *
 * @return non-zero if sleep was rejected by hardware
 */
uint32_t pmu_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp);

/**
 * @brief   Finish sleep process settings and get sleep reject status
 * @return  return sleep reject status
 */
bool pmu_sleep_finish(void);

/**
 * @brief Initialize PMU related power/clock/digital parameters and functions
 */
void pmu_init(void);

/**
 * @brief Enable or disable system clock in PMU HP sleep state
 *
 * This API only used for fix BLE 40 MHz low power clock source issue
 *
 * @param enable  true to enable, false to disable
 */
void pmu_sleep_enable_hp_sleep_sysclk(bool enable);

/**
 * Get the time overhead used by regdma to work on the retention link during the hardware wake-up process
 * @return  regdma time cost during hardware wake-up stage in microseconds
 */
uint32_t pmu_sleep_get_wakup_retention_cost(void);

#endif //#if SOC_PMU_SUPPORTED

#ifdef __cplusplus
}
#endif
