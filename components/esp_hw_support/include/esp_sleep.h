// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include "esp_err.h"

#include "hal/touch_sensor_types.h"
#include "hal/gpio_types.h"

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logic function used for EXT1 wakeup mode.
 */
typedef enum {
    ESP_EXT1_WAKEUP_ALL_LOW = 0,    //!< Wake the chip when all selected GPIOs go low
    ESP_EXT1_WAKEUP_ANY_HIGH = 1    //!< Wake the chip when any of the selected GPIOs go high
} esp_sleep_ext1_wakeup_mode_t;

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
typedef enum {
    ESP_GPIO_WAKEUP_GPIO_LOW = 0,
    ESP_GPIO_WAKEUP_GPIO_HIGH = 1
} esp_deepsleep_gpio_wake_up_mode_t;
#endif

/**
 * @brief Power domains which can be powered down in sleep mode
 */
typedef enum {
    ESP_PD_DOMAIN_RTC_PERIPH,      //!< RTC IO, sensors and ULP co-processor
    ESP_PD_DOMAIN_RTC_SLOW_MEM,    //!< RTC slow memory
    ESP_PD_DOMAIN_RTC_FAST_MEM,    //!< RTC fast memory
    ESP_PD_DOMAIN_XTAL,            //!< XTAL oscillator
    ESP_PD_DOMAIN_CPU,             //!< CPU core
    ESP_PD_DOMAIN_VDDSDIO,         //!< VDD_SDIO
    ESP_PD_DOMAIN_MAX              //!< Number of domains
} esp_sleep_pd_domain_t;

/**
 * @brief Power down options
 */
typedef enum {
    ESP_PD_OPTION_OFF,      //!< Power down the power domain in sleep mode
    ESP_PD_OPTION_ON,       //!< Keep power domain enabled during sleep mode
    ESP_PD_OPTION_AUTO      //!< Keep power domain enabled in sleep mode, if it is needed by one of the wakeup options. Otherwise power it down.
} esp_sleep_pd_option_t;

/**
 * @brief Sleep wakeup cause
 */
typedef enum {
    ESP_SLEEP_WAKEUP_UNDEFINED,    //!< In case of deep sleep, reset was not caused by exit from deep sleep
    ESP_SLEEP_WAKEUP_ALL,          //!< Not a wakeup cause, used to disable all wakeup sources with esp_sleep_disable_wakeup_source
    ESP_SLEEP_WAKEUP_EXT0,         //!< Wakeup caused by external signal using RTC_IO
    ESP_SLEEP_WAKEUP_EXT1,         //!< Wakeup caused by external signal using RTC_CNTL
    ESP_SLEEP_WAKEUP_TIMER,        //!< Wakeup caused by timer
    ESP_SLEEP_WAKEUP_TOUCHPAD,     //!< Wakeup caused by touchpad
    ESP_SLEEP_WAKEUP_ULP,          //!< Wakeup caused by ULP program
    ESP_SLEEP_WAKEUP_GPIO,         //!< Wakeup caused by GPIO (light sleep only)
    ESP_SLEEP_WAKEUP_UART,         //!< Wakeup caused by UART (light sleep only)
    ESP_SLEEP_WAKEUP_WIFI,              //!< Wakeup caused by WIFI (light sleep only)
    ESP_SLEEP_WAKEUP_COCPU,             //!< Wakeup caused by COCPU int
    ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG,   //!< Wakeup caused by COCPU crash
    ESP_SLEEP_WAKEUP_BT,           //!< Wakeup caused by BT (light sleep only)
} esp_sleep_source_t;

/* Leave this type define for compatibility */
typedef esp_sleep_source_t esp_sleep_wakeup_cause_t;

/**
 * @brief Disable wakeup source
 *
 * This function is used to deactivate wake up trigger for source
 * defined as parameter of the function.
 *
 * @note This function does not modify wake up configuration in RTC.
 *       It will be performed in esp_sleep_start function.
 *
 * See docs/sleep-modes.rst for details.
 *
 * @param source - number of source to disable of type esp_sleep_source_t
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if trigger was not active
 */
esp_err_t esp_sleep_disable_wakeup_source(esp_sleep_source_t source);

#if SOC_ULP_SUPPORTED
/**
 * @brief Enable wakeup by ULP coprocessor
 * @note In revisions 0 and 1 of the ESP32, ULP wakeup source
 *       cannot be used when RTC_PERIPH power domain is forced
 *       to be powered on (ESP_PD_OPTION_ON) or when
 *       ext0 wakeup source is used.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if additional current by touch (CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT) is enabled.
 *      - ESP_ERR_INVALID_STATE if ULP co-processor is not enabled or if wakeup triggers conflict
 */
esp_err_t esp_sleep_enable_ulp_wakeup(void);

#endif // SOC_ULP_SUPPORTED

/**
 * @brief Enable wakeup by timer
 * @param time_in_us  time before wakeup, in microseconds
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if value is out of range (TBD)
 */
esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us);

#if SOC_TOUCH_SENSOR_NUM > 0

/**
 * @brief Enable wakeup by touch sensor
 *
 * @note In revisions 0 and 1 of the ESP32, touch wakeup source
 *       can not be used when RTC_PERIPH power domain is forced
 *       to be powered on (ESP_PD_OPTION_ON) or when ext0 wakeup
 *       source is used.
 *
 * @note The FSM mode of the touch button should be configured
 *       as the timer trigger mode.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if additional current by touch (CONFIG_ESP32_RTC_EXT_CRYST_ADDIT_CURRENT) is enabled.
 *      - ESP_ERR_INVALID_STATE if wakeup triggers conflict
 */
esp_err_t esp_sleep_enable_touchpad_wakeup(void);

/**
 * @brief Get the touch pad which caused wakeup
 *
 * If wakeup was caused by another source, this function will return TOUCH_PAD_MAX;
 *
 * @return touch pad which caused wakeup
 */
touch_pad_t esp_sleep_get_touchpad_wakeup_status(void);

#endif // SOC_TOUCH_SENSOR_NUM > 0

/**
 * @brief Returns true if a GPIO number is valid for use as wakeup source.
 *
 * @note For SoCs with RTC IO capability, this can be any valid RTC IO input pin.
 *
 * @param gpio_num Number of the GPIO to test for wakeup source capability
 *
 * @return True if this GPIO number will be accepted as a sleep wakeup source.
 */
bool esp_sleep_is_valid_wakeup_gpio(gpio_num_t gpio_num);

#if SOC_PM_SUPPORT_EXT_WAKEUP

/**
 * @brief Enable wakeup using a pin
 *
 * This function uses external wakeup feature of RTC_IO peripheral.
 * It will work only if RTC peripherals are kept on during sleep.
 *
 * This feature can monitor any pin which is an RTC IO. Once the pin transitions
 * into the state given by level argument, the chip will be woken up.
 *
 * @note This function does not modify pin configuration. The pin is
 *       configured in esp_sleep_start, immediately before entering sleep mode.
 *
 * @note In revisions 0 and 1 of the ESP32, ext0 wakeup source
 *       can not be used together with touch or ULP wakeup sources.
 *
 * @param gpio_num  GPIO number used as wakeup source. Only GPIOs which are have RTC
 *             functionality can be used: 0,2,4,12-15,25-27,32-39.
 * @param level  input level which will trigger wakeup (0=low, 1=high)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the selected GPIO is not an RTC GPIO,
 *        or the mode is invalid
 *      - ESP_ERR_INVALID_STATE if wakeup triggers conflict
 */
esp_err_t esp_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level);

/**
 * @brief Enable wakeup using multiple pins
 *
 * This function uses external wakeup feature of RTC controller.
 * It will work even if RTC peripherals are shut down during sleep.
 *
 * This feature can monitor any number of pins which are in RTC IOs.
 * Once any of the selected pins goes into the state given by mode argument,
 * the chip will be woken up.
 *
 * @note This function does not modify pin configuration. The pins are
 *       configured in esp_sleep_start, immediately before
 *       entering sleep mode.
 *
 * @note internal pullups and pulldowns don't work when RTC peripherals are
 *       shut down. In this case, external resistors need to be added.
 *       Alternatively, RTC peripherals (and pullups/pulldowns) may be
 *       kept enabled using esp_sleep_pd_config function.
 *
 * @param mask  bit mask of GPIO numbers which will cause wakeup. Only GPIOs
 *              which are have RTC functionality can be used in this bit map:
 *              0,2,4,12-15,25-27,32-39.
 * @param mode select logic function used to determine wakeup condition:
 *            - ESP_EXT1_WAKEUP_ALL_LOW: wake up when all selected GPIOs are low
 *            - ESP_EXT1_WAKEUP_ANY_HIGH: wake up when any of the selected GPIOs is high
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the selected GPIOs is not an RTC GPIO,
 *        or mode is invalid
 */
esp_err_t esp_sleep_enable_ext1_wakeup(uint64_t mask, esp_sleep_ext1_wakeup_mode_t mode);

#endif // SOC_PM_SUPPORT_EXT_WAKEUP

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
/**
 * @brief Enable wakeup using specific gpio pins
 *
 * This function enables an IO pin to wake the chip from deep sleep
 *
 * @note This function does not modify pin configuration. The pins are
 *       configured in esp_sleep_start, immediately before
 *       entering sleep mode.
 *
 * @note You don't need to care to pull-up or pull-down before using this
 *       function, because this will be done in esp_sleep_start based on
 *       param mask you give. BTW, when you use low level to wake up the
 *       chip, we strongly recommand you to add external registors(pull-up).
 *
 * @param gpio_pin_mask  Bit mask of GPIO numbers which will cause wakeup. Only GPIOs
 *              which are have RTC functionality can be used in this bit map.
 * @param mode Select logic function used to determine wakeup condition:
 *            - ESP_GPIO_WAKEUP_GPIO_LOW: wake up when the gpio turn to low.
 *            - ESP_GPIO_WAKEUP_GPIO_HIGH: wake up when the gpio turn to high.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if gpio num is more than 5 or mode is invalid,
 */
esp_err_t esp_deep_sleep_enable_gpio_wakeup(uint64_t gpio_pin_mask, esp_deepsleep_gpio_wake_up_mode_t mode);
#endif
/**
 * @brief Enable wakeup from light sleep using GPIOs
 *
 * Each GPIO supports wakeup function, which can be triggered on either low level
 * or high level. Unlike EXT0 and EXT1 wakeup sources, this method can be used
 * both for all IOs: RTC IOs and digital IOs. It can only be used to wakeup from
 * light sleep though.
 *
 * To enable wakeup, first call gpio_wakeup_enable, specifying gpio number and
 * wakeup level, for each GPIO which is used for wakeup.
 * Then call this function to enable wakeup feature.
 *
 * @note In revisions 0 and 1 of the ESP32, GPIO wakeup source
 *       can not be used together with touch or ULP wakeup sources.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if wakeup triggers conflict
 */
esp_err_t esp_sleep_enable_gpio_wakeup(void);

/**
 * @brief Enable wakeup from light sleep using UART
 *
 * Use uart_set_wakeup_threshold function to configure UART wakeup threshold.
 *
 * Wakeup from light sleep takes some time, so not every character sent
 * to the UART can be received by the application.
 *
 * @note ESP32 does not support wakeup from UART2.
 *
 * @param uart_num  UART port to wake up from
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if wakeup from given UART is not supported
 */
esp_err_t esp_sleep_enable_uart_wakeup(int uart_num);

/**
 * @brief Enable wakeup by WiFi MAC
 * @return
 *      - ESP_OK on success
 */
esp_err_t esp_sleep_enable_wifi_wakeup(void);


/**
 * @brief Get the bit mask of GPIOs which caused wakeup (ext1)
 *
 * If wakeup was caused by another source, this function will return 0.
 *
 * @return bit mask, if GPIOn caused wakeup, BIT(n) will be set
 */
uint64_t esp_sleep_get_ext1_wakeup_status(void);

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
/**
 * @brief Get the bit mask of GPIOs which caused wakeup (gpio)
 *
 * If wakeup was caused by another source, this function will return 0.
 *
 * @return bit mask, if GPIOn caused wakeup, BIT(n) will be set
 */
uint64_t esp_sleep_get_gpio_wakeup_status(void);
#endif //SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

/**
 * @brief Set power down mode for an RTC power domain in sleep mode
 *
 * If not set set using this API, all power domains default to ESP_PD_OPTION_AUTO.
 *
 * @param domain  power domain to configure
 * @param option  power down option (ESP_PD_OPTION_OFF, ESP_PD_OPTION_ON, or ESP_PD_OPTION_AUTO)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t esp_sleep_pd_config(esp_sleep_pd_domain_t domain,
                                   esp_sleep_pd_option_t option);

/**
 * @brief Enter deep sleep with the configured wakeup options
 *
 * This function does not return.
 */
void esp_deep_sleep_start(void) __attribute__((noreturn));

/**
 * @brief Enter light sleep with the configured wakeup options
 *
 * @return
 *  - ESP_OK on success (returned after wakeup)
 *  - ESP_ERR_INVALID_STATE if WiFi or BT is not stopped
 */
esp_err_t esp_light_sleep_start(void);

/**
 * @brief Enter deep-sleep mode
 *
 * The device will automatically wake up after the deep-sleep time
 * Upon waking up, the device calls deep sleep wake stub, and then proceeds
 * to load application.
 *
 * Call to this function is equivalent to a call to esp_deep_sleep_enable_timer_wakeup
 * followed by a call to esp_deep_sleep_start.
 *
 * esp_deep_sleep does not shut down WiFi, BT, and higher level protocol
 * connections gracefully.
 * Make sure relevant WiFi and BT stack functions are called to close any
 * connections and deinitialize the peripherals. These include:
 *     - esp_bluedroid_disable
 *     - esp_bt_controller_disable
 *     - esp_wifi_stop
 *
 * This function does not return.
 *
 * @param time_in_us  deep-sleep time, unit: microsecond
 */
void esp_deep_sleep(uint64_t time_in_us) __attribute__((noreturn));


/**
 * @brief Get the wakeup source which caused wakeup from sleep
 *
 * @return cause of wake up from last sleep (deep sleep or light sleep)
 */
esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause(void);


/**
 * @brief Default stub to run on wake from deep sleep.
 *
 * Allows for executing code immediately on wake from sleep, before
 * the software bootloader or ESP-IDF app has started up.
 *
 * This function is weak-linked, so you can implement your own version
 * to run code immediately when the chip wakes from
 * sleep.
 *
 * See docs/deep-sleep-stub.rst for details.
 */
void esp_wake_deep_sleep(void);

/**
 * @brief Function type for stub to run on wake from sleep.
 *
 */
typedef void (*esp_deep_sleep_wake_stub_fn_t)(void);

/**
 * @brief Install a new stub at runtime to run on wake from deep sleep
 *
 * If implementing esp_wake_deep_sleep() then it is not necessary to
 * call this function.
 *
 * However, it is possible to call this function to substitute a
 * different deep sleep stub. Any function used as a deep sleep stub
 * must be marked RTC_IRAM_ATTR, and must obey the same rules given
 * for esp_wake_deep_sleep().
 */
void esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub);

/**
 * @brief Get current wake from deep sleep stub
 * @return Return current wake from deep sleep stub, or NULL if
 *         no stub is installed.
 */
esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void);

/**
 *  @brief The default esp-idf-provided esp_wake_deep_sleep() stub.
 *
 *  See docs/deep-sleep-stub.rst for details.
 */
void esp_default_wake_deep_sleep(void);

/**
 * @brief Disable logging from the ROM code after deep sleep.
 *
 * Using LSB of RTC_STORE4.
 */
void esp_deep_sleep_disable_rom_logging(void);

#ifdef SOC_PM_SUPPORT_CPU_PD
/**
 * @brief CPU Power down low-level initialize
 *
 * @param enable  enable or disable CPU power down during light sleep
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough retention memory
 */
esp_err_t esp_sleep_cpu_pd_low_init(bool enable);
#endif

#if SOC_GPIO_SUPPORT_SLP_SWITCH
/**
 * @brief Configure to isolate all GPIO pins in sleep state
 */
void esp_sleep_config_gpio_isolate(void);

/**
 * @brief Enable or disable GPIO pins status switching between slept status and waked status.
 * @param enable decide whether to switch status or not
 */
void esp_sleep_enable_gpio_switch(bool enable);
#endif

#if CONFIG_MAC_BB_PD
/**
 * @brief Function type for stub to run mac bb power down.
 */
typedef void (* mac_bb_power_down_cb_t)(void);

/**
 * @brief Function type for stub to run mac bb power up.
 */
typedef void (* mac_bb_power_up_cb_t)(void);

/**
 * @brief  Registet mac bb power down callback.
 * @param  cb mac bb power down callback.
 * @return
 *  - ESP_OK on success
 */
esp_err_t esp_register_mac_bb_pd_callback(mac_bb_power_down_cb_t cb);

/**
 * @brief  Unregistet mac bb power down callback.
 * @param  cb mac bb power down callback.
 * @return
 *  - ESP_OK on success
 */
esp_err_t esp_unregister_mac_bb_pd_callback(mac_bb_power_down_cb_t cb);

/**
 * @brief  Registet mac bb power up callback.
 * @param  cb mac bb power up callback.
 * @return
 *  - ESP_OK on success
 */
esp_err_t esp_register_mac_bb_pu_callback(mac_bb_power_up_cb_t cb);

/**
 * @brief  Unregistet mac bb power up callback.
 * @param  cb mac bb power up callback.
 * @return
 *  - ESP_OK on success
 */
esp_err_t esp_unregister_mac_bb_pu_callback(mac_bb_power_up_cb_t cb);
#endif

#ifdef __cplusplus
}
#endif
