/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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

#if SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP && !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
/**
 * @brief Isolate all digital IOs except those that are held during deep sleep
 *
 * Reduce digital IOs current leakage during deep sleep.
 */
void esp_sleep_isolate_digital_gpio(void);
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

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_PM_MMU_TABLE_RETENTION_WHEN_TOP_PD
/**
 * @brief Backup or restore the MMU when the top domain is powered down.
 * @param backup_or_restore decide to backup mmu or restore mmu
 */
void esp_sleep_mmu_retention(bool backup_or_restore);

/**
 * @brief Whether to allow the top domain to be powered off due to mmu domain requiring retention.
 *
 * In light sleep mode, only when the system can provide enough memory
 * for mmu retention, the top power domain can be powered off.
 */
bool mmu_domain_pd_allowed(void);
#endif

#ifdef __cplusplus
}
#endif
