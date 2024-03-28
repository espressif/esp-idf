/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "hal/gpio_types.h"
#include "esp_rom_gpio.h"
#include "driver/gpio_etm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_PIN_COUNT                      (SOC_GPIO_PIN_COUNT)
/// Check whether it is a valid GPIO number
#define GPIO_IS_VALID_GPIO(gpio_num)        ((gpio_num >= 0) && \
                                              (((1ULL << (gpio_num)) & SOC_GPIO_VALID_GPIO_MASK) != 0))
/// Check whether it can be a valid GPIO number of output mode
#define GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) ((gpio_num >= 0) && \
                                              (((1ULL << (gpio_num)) & SOC_GPIO_VALID_OUTPUT_GPIO_MASK) != 0))
/// Check whether it can be a valid digital I/O pad
#define GPIO_IS_VALID_DIGITAL_IO_PAD(gpio_num) ((gpio_num >= 0) && \
                                                 (((1ULL << (gpio_num)) & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK) != 0))

typedef intr_handle_t gpio_isr_handle_t;

/**
 * @brief GPIO interrupt handler
 *
 * @param arg User registered data
 */
typedef void (*gpio_isr_t)(void *arg);

/**
 * @brief Configuration parameters of GPIO pad for gpio_config function
 */
typedef struct {
    uint64_t pin_bit_mask;          /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    gpio_mode_t mode;               /*!< GPIO mode: set input/output mode                     */
    gpio_pullup_t pull_up_en;       /*!< GPIO pull-up                                         */
    gpio_pulldown_t pull_down_en;   /*!< GPIO pull-down                                       */
    gpio_int_type_t intr_type;      /*!< GPIO interrupt type                                  */
#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
    gpio_hys_ctrl_mode_t hys_ctrl_mode;       /*!< GPIO hysteresis: hysteresis filter on slope input    */
#endif
} gpio_config_t;

/**
 * @brief GPIO common configuration
 *
 *        Configure GPIO's Mode,pull-up,PullDown,IntrType
 *
 * @param  pGPIOConfig Pointer to GPIO configure struct
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig);

/**
 * @brief Reset an gpio to default state (select gpio function, enable pullup and disable input and output).
 *
 * @param gpio_num GPIO number.
 *
 * @note This function also configures the IOMUX for this pin to the GPIO
 *       function, and disconnects any other peripheral output configured via GPIO
 *       Matrix.
 *
 * @return Always return ESP_OK.
 */
esp_err_t gpio_reset_pin(gpio_num_t gpio_num);

/**
 * @brief  GPIO set interrupt trigger type
 *
 * @param  gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  intr_type Interrupt type, select from gpio_int_type_t
 *
 * @return
 *     - ESP_OK  Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief  Enable GPIO module interrupt signal
 *
 * @note ESP32: Please do not use the interrupt of GPIO36 and GPIO39 when using ADC or Wi-Fi and Bluetooth with sleep mode enabled.
 *       Please refer to the comments of `adc1_get_raw`.
 *       Please refer to Section 3.11 of <a href="https://espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf">ESP32 ECO and Workarounds for Bugs</a> for the description of this issue.

 *
 * @param  gpio_num GPIO number. If you want to enable an interrupt on e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t gpio_intr_enable(gpio_num_t gpio_num);

/**
 * @brief  Disable GPIO module interrupt signal
 *
 * @note This function is allowed to be executed when Cache is disabled within ISR context, by enabling `CONFIG_GPIO_CTRL_FUNC_IN_IRAM`
 *
 * @param  gpio_num GPIO number. If you want to disable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t gpio_intr_disable(gpio_num_t gpio_num);

/**
 * @brief  GPIO set output level
 *
 * @note This function is allowed to be executed when Cache is disabled within ISR context, by enabling `CONFIG_GPIO_CTRL_FUNC_IN_IRAM`
 *
 * @param  gpio_num GPIO number. If you want to set the output level of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  level Output level. 0: low ; 1: high
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 *
 */
esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level);

/**
 * @brief  GPIO get input level
 *
 * @warning If the pad is not configured for input (or input and output) the returned value is always 0.
 *
 * @param  gpio_num GPIO number. If you want to get the logic level of e.g. pin GPIO16, gpio_num should be GPIO_NUM_16 (16);
 *
 * @return
 *     - 0 the GPIO input level is 0
 *     - 1 the GPIO input level is 1
 *
 */
int gpio_get_level(gpio_num_t gpio_num);

/**
 * @brief    GPIO set direction
 *
 * Configure GPIO direction,such as output_only,input_only,output_and_input
 *
 * @param  gpio_num  Configure GPIO pins number, it should be GPIO number. If you want to set direction of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  mode GPIO direction
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO error
 *
 */
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);

/**
 * @brief  Configure GPIO pull-up/pull-down resistors
 *
 * @note ESP32: Only pins that support both input & output have integrated pull-up and pull-down resistors. Input-only GPIOs 34-39 do not.
 *
 * @param  gpio_num GPIO number. If you want to set pull up or down mode for e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  pull GPIO pull up/down mode.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG : Parameter error
 *
 */
esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull);

/**
 * @brief Enable GPIO wake-up function.
 *
 * @param gpio_num GPIO number.
 *
 * @param intr_type GPIO wake-up type. Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief Disable GPIO wake-up function.
 *
 * @param gpio_num GPIO number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num);

/**
 * @brief   Register GPIO interrupt handler, the handler is an ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 *
 * This ISR function is called whenever any GPIO interrupt occurs. See
 * the alternative gpio_install_isr_service() and
 * gpio_isr_handler_add() API in order to have the driver support
 * per-GPIO ISRs.
 *
 * @param  fn  Interrupt handler function.
 * @param  arg  Parameter for handler function
 * @param  intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param  handle Pointer to return handle. If non-NULL, a handle for the interrupt will be returned here.
 *
 * \verbatim embed:rst:leading-asterisk
 * To disable or remove the ISR, pass the returned handle to the :doc:`interrupt allocation functions </api-reference/system/intr_alloc>`.
 * \endverbatim
 *
 * @return
 *     - ESP_OK Success ;
 *     - ESP_ERR_INVALID_ARG GPIO error
 *     - ESP_ERR_NOT_FOUND No free interrupt found with the specified flags
 */
esp_err_t gpio_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle);

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pullup_en(gpio_num_t gpio_num);

/**
  * @brief Disable pull-up on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pullup_dis(gpio_num_t gpio_num);

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pulldown_en(gpio_num_t gpio_num);

/**
  * @brief Disable pull-down on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num);

/**
  * @brief Install the GPIO driver's ETS_GPIO_INTR_SOURCE ISR handler service, which allows per-pin GPIO interrupt handlers.
  *
  * This function is incompatible with gpio_isr_register() - if that function is used, a single global ISR is registered for all GPIO interrupts. If this function is used, the ISR service provides a global GPIO ISR and individual pin handlers are registered via the gpio_isr_handler_add() function.
  *
  * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
  *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_NO_MEM No memory to install this service
  *     - ESP_ERR_INVALID_STATE ISR service already installed.
  *     - ESP_ERR_NOT_FOUND No free interrupt found with the specified flags
  *     - ESP_ERR_INVALID_ARG GPIO error
  */
esp_err_t gpio_install_isr_service(int intr_alloc_flags);

/**
  * @brief Uninstall the driver's GPIO ISR service, freeing related resources.
  */
void gpio_uninstall_isr_service(void);

/**
  * @brief Add ISR handler for the corresponding GPIO pin.
  *
  * Call this function after using gpio_install_isr_service() to
  * install the driver's GPIO ISR handler service.
  *
  * The pin ISR handlers no longer need to be declared with IRAM_ATTR,
  * unless you pass the ESP_INTR_FLAG_IRAM flag when allocating the
  * ISR in gpio_install_isr_service().
  *
  * This ISR handler will be called from an ISR. So there is a stack
  * size limit (configurable as "ISR stack size" in menuconfig). This
  * limit is smaller compared to a global GPIO interrupt handler due
  * to the additional level of indirection.
  *
  * @param gpio_num GPIO number
  * @param isr_handler ISR handler function for the corresponding GPIO number.
  * @param args parameter for ISR handler.
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_STATE Wrong state, the ISR service has not been initialized.
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args);

/**
  * @brief Remove ISR handler for the corresponding GPIO pin.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_STATE Wrong state, the ISR service has not been initialized.
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num);

/**
  * @brief Set GPIO pad drive capability
  *
  * @param gpio_num GPIO number, only support output GPIOs
  * @param strength Drive capability of the pad
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength);

/**
  * @brief Get GPIO pad drive capability
  *
  * @param gpio_num GPIO number, only support output GPIOs
  * @param strength Pointer to accept drive capability of the pad
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t *strength);

/**
  * @brief Enable gpio pad hold function.
  *
  * When a GPIO is set to hold, its state is latched at that moment and will not change when the internal
  * signal or the IO MUX/GPIO configuration is modified (including input enable, output enable, output value,
  * function, and drive strength values). This function can be used to retain the state of GPIOs when the power
  * domain of where GPIO/IOMUX belongs to becomes off. For example, chip or system is reset (e.g. watchdog
  * time-out, deep-sleep events are triggered), or peripheral power-down in light-sleep.
  *
  * This function works in both input and output modes, and only applicable to output-capable GPIOs.
  * If this function is enabled:
  *   in output mode: the output level of the GPIO will be locked and can not be changed.
  *   in input mode: the input read value can still reflect the changes of the input signal.
  *
  * However, on ESP32/S2/C3/S3/C2, this function cannot be used to hold the state of a digital GPIO during Deep-sleep.
  * Even if this function is enabled, the digital GPIO will be reset to its default state when the chip wakes up from
  * Deep-sleep. If you want to hold the state of a digital GPIO during Deep-sleep, please call `gpio_deep_sleep_hold_en`.
  *
  * Power down or call `gpio_hold_dis` will disable this function.
  *
  * @param gpio_num GPIO number, only support output-capable GPIOs
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_NOT_SUPPORTED Not support pad hold function
  */
esp_err_t gpio_hold_en(gpio_num_t gpio_num);

/**
  * @brief Disable gpio pad hold function.
  *
  * When the chip is woken up from peripheral power-down sleep, the gpio will be set to the default mode,
  * so, the gpio will output the default level if this function is called. If you don't want the level changes, the
  * gpio should be configured to a known state before this function is called.
  *  e.g.
  *     If you hold gpio18 high during Deep-sleep, after the chip is woken up and `gpio_hold_dis` is called,
  *     gpio18 will output low level(because gpio18 is input mode by default). If you don't want this behavior,
  *     you should configure gpio18 as output mode and set it to high level before calling `gpio_hold_dis`.
  *
  * @param gpio_num GPIO number, only support output-capable GPIOs
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_NOT_SUPPORTED Not support pad hold function
  */
esp_err_t gpio_hold_dis(gpio_num_t gpio_num);

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
/**
  * @brief Enable all digital gpio pads hold function during Deep-sleep.
  *
  * Enabling this feature makes all digital gpio pads be at the holding state during Deep-sleep. The state of each pad
  * holds is its active configuration (not pad's sleep configuration!).
  *
  * Note that this pad hold feature only works when the chip is in Deep-sleep mode. When the chip is in active mode,
  * the digital gpio state can be changed freely even you have called this function.
  *
  * After this API is being called, the digital gpio Deep-sleep hold feature will work during every sleep process. You
  * should call `gpio_deep_sleep_hold_dis` to disable this feature.
  */
void gpio_deep_sleep_hold_en(void);

/**
  * @brief Disable all digital gpio pads hold function during Deep-sleep.
  */
void gpio_deep_sleep_hold_dis(void);
#endif //!SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP

/**
  * @brief Set pad input to a peripheral signal through the IOMUX.
  * @param gpio_num GPIO number of the pad.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
void gpio_iomux_in(uint32_t gpio_num, uint32_t signal_idx);

/**
  * @brief Set peripheral output to an GPIO pad through the IOMUX.
  * @param gpio_num gpio_num GPIO number of the pad.
  * @param func The function number of the peripheral pin to output pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  * @param out_en_inv True if the output enable needs to be inverted, otherwise False.
  */
void gpio_iomux_out(uint8_t gpio_num, int func, bool out_en_inv);

#if SOC_GPIO_SUPPORT_FORCE_HOLD
/**
  * @brief Force hold all digital and rtc gpio pads.
  *
  * GPIO force hold, no matter the chip in active mode or sleep modes.
  *
  * This function will immediately cause all pads to latch the current values of input enable, output enable,
  * output value, function, and drive strength values.
  *
  * @warning
  *   1. This function will hold flash and UART pins as well. Therefore, this function, and all code run afterwards
  *      (till calling `gpio_force_unhold_all` to disable this feature), MUST be placed in internal RAM as holding the flash
  *      pins will halt SPI flash operation, and holding the UART pins will halt any UART logging.
  *   2. The hold state of all pads will be cancelled during ROM boot, so it is not recommended to use this API to hold
  *      the pads state during deepsleep and reset.
  * */
esp_err_t gpio_force_hold_all(void);

/**
  * @brief Unhold all digital and rtc gpio pads.
  *
  * @note  The global hold signal and the hold signal of each IO act on the PAD through 'or' logic, so if a pad has already
  *        been configured to hold by `gpio_hold_en`, this API can't release its hold state.
  * */
esp_err_t gpio_force_unhold_all(void);
#endif

/**
  * @brief Enable SLP_SEL to change GPIO status automantically in lightsleep.
  * @param gpio_num GPIO number of the pad.
  *
  * @return
  *     - ESP_OK Success
  *
  */
esp_err_t gpio_sleep_sel_en(gpio_num_t gpio_num);

/**
  * @brief Disable SLP_SEL to change GPIO status automantically in lightsleep.
  * @param gpio_num GPIO number of the pad.
  *
  * @return
  *     - ESP_OK Success
  */
esp_err_t gpio_sleep_sel_dis(gpio_num_t gpio_num);

/**
 * @brief    GPIO set direction at sleep
 *
 * Configure GPIO direction,such as output_only,input_only,output_and_input
 *
 * @param  gpio_num  Configure GPIO pins number, it should be GPIO number. If you want to set direction of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  mode GPIO direction
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO error
 */
esp_err_t gpio_sleep_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);

/**
 * @brief  Configure GPIO pull-up/pull-down resistors at sleep
 *
 * @note ESP32: Only pins that support both input & output have integrated pull-up and pull-down resistors. Input-only GPIOs 34-39 do not.
 *
 * @param  gpio_num GPIO number. If you want to set pull up or down mode for e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  pull GPIO pull up/down mode.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG : Parameter error
 */
esp_err_t gpio_sleep_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull);

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

#define GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(gpio_num)    ((gpio_num >= 0) && \
                                                          (((1ULL << (gpio_num)) & SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK) != 0))

/**
 * @brief Enable GPIO deep-sleep wake-up function.
 *
 * @param gpio_num GPIO number.
 *
 * @param intr_type GPIO wake-up type. Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
 *
 * @note Called by the SDK. User shouldn't call this directly in the APP.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t gpio_deep_sleep_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief Disable GPIO deep-sleep wake-up function.
 *
 * @param gpio_num GPIO number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t gpio_deep_sleep_wakeup_disable(gpio_num_t gpio_num);

#endif //SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

/**
 * @brief Dump IO configuration information to console
 *
 * @param out_stream IO stream (e.g. stdout)
 * @param io_bit_mask IO pin bit mask, each bit maps to an IO
 *
 * @return
 *    - ESP_OK Success
 *    - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t gpio_dump_io_configuration(FILE *out_stream, uint64_t io_bit_mask);

#ifdef __cplusplus
}
#endif
