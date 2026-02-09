/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif


#if CONFIG_ESP_SLEEP_DEBUG
typedef struct {
    uint32_t lightsleep_cnt;
    uint64_t sleep_in_rtc_time_stamp;
    uint64_t sleep_out_rtc_time_stamp;
    uint32_t wakeup_triggers;
    uint32_t sleep_flags;
    esp_err_t sleep_request_result;
} esp_sleep_context_t;

/**
 * @brief Set the context pointer of last sleep request
 * @param sleep_ctx Structure where the context of the sleep information needs to be recorded in
 */
void esp_sleep_set_sleep_context(esp_sleep_context_t *sleep_ctx);
#endif

typedef enum {
    ESP_SLEEP_RTC_USE_RC_FAST_MODE,       //!< The mode requested by RTC peripherals to keep RC_FAST clock on during sleep (both HP_SLEEP and LP_SLEEP mode). (Will override the RC_FAST domain config by esp_sleep_pd_config)
    ESP_SLEEP_DIG_USE_RC_FAST_MODE,       //!< The mode requested by digital peripherals to keep RC_FAST clock on during sleep (both HP_SLEEP and LP_SLEEP mode). (!!! Only valid for lightsleep, will override the RC_FAST domain config by esp_sleep_pd_config)
    ESP_SLEEP_USE_ADC_TSEN_MONITOR_MODE,  //!< Will enables the use of ADC and temperature sensor in monitor (ULP) mode.
    ESP_SLEEP_ULTRA_LOW_MODE,             //!< In ultra low mode, 2uA is saved, but RTC memory can't use at high temperature, and RTCIO can't be used as INPUT.
    ESP_SLEEP_RTC_FAST_USE_XTAL_MODE,     //!< The mode in which the crystal is used as the RTC_FAST clock source, need keep XTAL on in HP_SLEEP mode when ULP is working.
    ESP_SLEEP_DIG_USE_XTAL_MODE,          //!< The mode requested by digital peripherals to keep XTAL clock on during sleep (both HP_SLEEP and LP_SLEEP mode). (!!! Only valid for lightsleep, will override the XTAL domain config by esp_sleep_pd_config)
    ESP_SLEEP_LP_USE_XTAL_MODE,           //!< The mode requested by lp peripherals to keep XTAL clock on during sleep. Only valid for lightsleep.
    ESP_SLEEP_LP_USE_RC_FAST_MODE,        //!< The mode requested by lp peripherals to keep FOSC clock on during sleep.
    ESP_SLEEP_VBAT_POWER_DEEPSLEEP_MODE,  //!< The mode to switch power supply to VBAT during deep sleep.
#if CONFIG_IDF_TARGET_ESP32
    ESP_SLEEP_ANALOG_LOW_POWER_MODE,      //!< If analog-related peripherals(ADC, TSENS, TOUCH) is not used in monitor mode, analog low power mode can be enabled to reduce power consumption (~300uA) in monitor state.
#endif
    ESP_SLEEP_MODE_MAX,
} esp_sleep_sub_mode_t;

/**
 * @brief Set sub-sleep power mode in sleep, mode enabled status is maintained by reference count.
 *        The caller should ensure that the enabling and disabling behavior is symmetric.
 *        This submode configuration will kept after deep sleep wakeup.
 *
 * @param  mode     sub-sleep mode type
 * @param  activate Activate or deactivate the sleep sub mode
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t esp_sleep_sub_mode_config(esp_sleep_sub_mode_t mode, bool activate);

/**
 * @brief Force disable sub-sleep power mode in sleep, usually used during initialization.
 *
 * @param  mode     sub-sleep mode type
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t esp_sleep_sub_mode_force_disable(esp_sleep_sub_mode_t mode);

/**
 * Dump the sub-sleep power mode enable status
 * @param  stream     The stream to dump to, if NULL then nothing will be dumped
 * @return            return the reference count array pointer
 */
int32_t* esp_sleep_sub_mode_dump_config(FILE *stream);

#if SOC_PM_SUPPORT_RTC_PERIPH_PD
/**
 * @brief Enable LP peripherals to use XTAL during sleep
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t esp_sleep_acquire_lp_use_xtal(void);

/**
 * @brief Disable LP peripherals to use XTAL during sleep
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t esp_sleep_release_lp_use_xtal(void);
#endif

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
/**
 * @brief Isolate all digital IOs except those that are held during deep sleep
 *
 * Reduce digital IOs current leakage during deep sleep.
 */
void esp_sleep_isolate_digital_gpio(void);
#endif

#if SOC_PM_SUPPORT_PMU_CLK_ICG
/**
 * @brief Clock ICG cells which can be gated in sleep mode
 */
typedef enum {
    ESP_SLEEP_CLOCK_IOMUX,  //!< The clock ICG cell mapping of IOMUX
    ESP_SLEEP_CLOCK_LEDC,   //!< The clock ICG cell mapping of LEDC
    ESP_SLEEP_CLOCK_UART0,   //!< The clock ICG cell mapping of UART0
    ESP_SLEEP_CLOCK_UART1,   //!< The clock ICG cell mapping of UART1
#if SOC_UART_HP_NUM > 2
    ESP_SLEEP_CLOCK_UART2,   //!< The clock ICG cell mapping of UART2
#endif
#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
    ESP_SLEEP_CLOCK_BT_USE_WIFI_PWR_CLK,  //!< The clock ICG cell remapping of RETENTION
#endif
    ESP_SLEEP_CLOCK_MAX     //!< Number of ICG cells
} esp_sleep_clock_t;

/**
 * @brief Clock ICG options
 */
typedef enum {
    ESP_SLEEP_CLOCK_OPTION_GATE,    //!< Gate the clock in sleep mode
    ESP_SLEEP_CLOCK_OPTION_UNGATE,  //!< Ungate the clock in sleep mode
    ESP_SLEEP_CLOCK_OPTION_MAX
} esp_sleep_clock_option_t;

/**
 * @brief Gate or Ungate the specified clock in sleep mode
 *
 * If not set set using this API, all clock default to ESP_SLEEP_CLOCK_OPTION_GATE.
 *
 * @param clock   the specified clock to configure
 * @param option  clock gate option (ESP_SLEEP_CLOCK_OPTION_GATE or ESP_SLEEP_CLOCK_OPTION_UNGATE)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t esp_sleep_clock_config(esp_sleep_clock_t clock, esp_sleep_clock_option_t option);
#endif

#if CONFIG_ESP_PHY_ENABLED
/**
  * Register a callback to be called from the deep sleep prepare for maintain the PHY state
  *          CPU is equal to min_freq_mhz (if DFS is enabled) when running this callback,
  *          and PLL clock is exists)
  *
  * @warning deepsleep PHY callbacks should without parameters, and MUST NOT,
  *          UNDER ANY CIRCUMSTANCES, CALL A FUNCTION THAT MIGHT BLOCK.
  *
  * @param new_dslp_cb     Callback to be called to close PHY related modules
  *
  * @return
  *     - ESP_OK:         PHY callback registered to the phy modules deepsleep prepare
  *     - ESP_ERR_NO_MEM: No more hook space for register the callback
  */
esp_err_t esp_deep_sleep_register_phy_hook(esp_deep_sleep_cb_t new_dslp_cb);

/**
  * @brief Unregister an PHY deepsleep callback
  *
  * @param old_dslp_cb     Callback to be unregistered
  */
void esp_deep_sleep_deregister_phy_hook(esp_deep_sleep_cb_t old_dslp_cb);
#endif

/**
 * @brief Notify the sleep process that `sleep_time_overhead_out` needs to be remeasured, which must be called
 *        in the following scenarios:
 *        1. When the CPU frequency changes to below the crystal oscillator frequency.
 *        2. When a new callback function is registered in the sleep process.
 *        3. Other events occur that affect the execution time of the CPU sleep process.
 */
void esp_sleep_overhead_out_time_refresh(void);

#ifdef __cplusplus
}
#endif
