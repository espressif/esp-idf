// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_GPIO_H_
#define _DRIVER_GPIO_H_
#include "esp_err.h"
#include <esp_types.h>
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#include "soc/rtc_io_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "rom/gpio.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "soc/gpio_periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_SEL_0              (BIT(0))                         /*!< Pin 0 selected */
#define GPIO_SEL_1              (BIT(1))                         /*!< Pin 1 selected */
#define GPIO_SEL_2              (BIT(2))                         /*!< Pin 2 selected
                                                                      @note There are more macros
                                                                      like that up to pin 39,
                                                                      excluding pins 20, 24 and 28..31.
                                                                      They are not shown here
                                                                      to reduce redundant information. */
/** @cond */
#define GPIO_SEL_3              (BIT(3))                         /*!< Pin 3 selected */
#define GPIO_SEL_4              (BIT(4))                         /*!< Pin 4 selected */
#define GPIO_SEL_5              (BIT(5))                         /*!< Pin 5 selected */
#define GPIO_SEL_6              (BIT(6))                         /*!< Pin 6 selected */
#define GPIO_SEL_7              (BIT(7))                         /*!< Pin 7 selected */
#define GPIO_SEL_8              (BIT(8))                         /*!< Pin 8 selected */
#define GPIO_SEL_9              (BIT(9))                         /*!< Pin 9 selected */
#define GPIO_SEL_10             (BIT(10))                        /*!< Pin 10 selected */
#define GPIO_SEL_11             (BIT(11))                        /*!< Pin 11 selected */
#define GPIO_SEL_12             (BIT(12))                        /*!< Pin 12 selected */
#define GPIO_SEL_13             (BIT(13))                        /*!< Pin 13 selected */
#define GPIO_SEL_14             (BIT(14))                        /*!< Pin 14 selected */
#define GPIO_SEL_15             (BIT(15))                        /*!< Pin 15 selected */
#define GPIO_SEL_16             (BIT(16))                        /*!< Pin 16 selected */
#define GPIO_SEL_17             (BIT(17))                        /*!< Pin 17 selected */
#define GPIO_SEL_18             (BIT(18))                        /*!< Pin 18 selected */
#define GPIO_SEL_19             (BIT(19))                        /*!< Pin 19 selected */

#define GPIO_SEL_21             (BIT(21))                        /*!< Pin 21 selected */
#define GPIO_SEL_22             (BIT(22))                        /*!< Pin 22 selected */
#define GPIO_SEL_23             (BIT(23))                        /*!< Pin 23 selected */

#define GPIO_SEL_25             (BIT(25))                        /*!< Pin 25 selected */
#define GPIO_SEL_26             (BIT(26))                        /*!< Pin 26 selected */
#define GPIO_SEL_27             (BIT(27))                        /*!< Pin 27 selected */

#define GPIO_SEL_32             ((uint64_t)(((uint64_t)1)<<32))  /*!< Pin 32 selected */
#define GPIO_SEL_33             ((uint64_t)(((uint64_t)1)<<33))  /*!< Pin 33 selected */
#define GPIO_SEL_34             ((uint64_t)(((uint64_t)1)<<34))  /*!< Pin 34 selected */
#define GPIO_SEL_35             ((uint64_t)(((uint64_t)1)<<35))  /*!< Pin 35 selected */
#define GPIO_SEL_36             ((uint64_t)(((uint64_t)1)<<36))  /*!< Pin 36 selected */
#define GPIO_SEL_37             ((uint64_t)(((uint64_t)1)<<37))  /*!< Pin 37 selected */
#define GPIO_SEL_38             ((uint64_t)(((uint64_t)1)<<38))  /*!< Pin 38 selected */
#define GPIO_SEL_39             ((uint64_t)(((uint64_t)1)<<39))  /*!< Pin 39 selected */

#define GPIO_PIN_REG_0          IO_MUX_GPIO0_REG
#define GPIO_PIN_REG_1          IO_MUX_GPIO1_REG
#define GPIO_PIN_REG_2          IO_MUX_GPIO2_REG
#define GPIO_PIN_REG_3          IO_MUX_GPIO3_REG
#define GPIO_PIN_REG_4          IO_MUX_GPIO4_REG
#define GPIO_PIN_REG_5          IO_MUX_GPIO5_REG
#define GPIO_PIN_REG_6          IO_MUX_GPIO6_REG
#define GPIO_PIN_REG_7          IO_MUX_GPIO7_REG
#define GPIO_PIN_REG_8          IO_MUX_GPIO8_REG
#define GPIO_PIN_REG_9          IO_MUX_GPIO9_REG
#define GPIO_PIN_REG_10         IO_MUX_GPIO10_REG
#define GPIO_PIN_REG_11         IO_MUX_GPIO11_REG
#define GPIO_PIN_REG_12         IO_MUX_GPIO12_REG
#define GPIO_PIN_REG_13         IO_MUX_GPIO13_REG
#define GPIO_PIN_REG_14         IO_MUX_GPIO14_REG
#define GPIO_PIN_REG_15         IO_MUX_GPIO15_REG
#define GPIO_PIN_REG_16         IO_MUX_GPIO16_REG
#define GPIO_PIN_REG_17         IO_MUX_GPIO17_REG
#define GPIO_PIN_REG_18         IO_MUX_GPIO18_REG
#define GPIO_PIN_REG_19         IO_MUX_GPIO19_REG
#define GPIO_PIN_REG_20         IO_MUX_GPIO20_REG
#define GPIO_PIN_REG_21         IO_MUX_GPIO21_REG
#define GPIO_PIN_REG_22         IO_MUX_GPIO22_REG
#define GPIO_PIN_REG_23         IO_MUX_GPIO23_REG
#define GPIO_PIN_REG_25         IO_MUX_GPIO25_REG
#define GPIO_PIN_REG_26         IO_MUX_GPIO26_REG
#define GPIO_PIN_REG_27         IO_MUX_GPIO27_REG
#define GPIO_PIN_REG_32         IO_MUX_GPIO32_REG
#define GPIO_PIN_REG_33         IO_MUX_GPIO33_REG
#define GPIO_PIN_REG_34         IO_MUX_GPIO34_REG
#define GPIO_PIN_REG_35         IO_MUX_GPIO35_REG
#define GPIO_PIN_REG_36         IO_MUX_GPIO36_REG
#define GPIO_PIN_REG_37         IO_MUX_GPIO37_REG
#define GPIO_PIN_REG_38         IO_MUX_GPIO38_REG
#define GPIO_PIN_REG_39         IO_MUX_GPIO39_REG

#define GPIO_APP_CPU_INTR_ENA      (BIT(0))
#define GPIO_APP_CPU_NMI_INTR_ENA  (BIT(1))
#define GPIO_PRO_CPU_INTR_ENA      (BIT(2))
#define GPIO_PRO_CPU_NMI_INTR_ENA  (BIT(3))
#define GPIO_SDIO_EXT_INTR_ENA     (BIT(4))

#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)
#define GPIO_MODE_DEF_OUTPUT          (BIT1)
#define GPIO_MODE_DEF_OD              (BIT2)


/** @endcond */

#define GPIO_IS_VALID_GPIO(gpio_num)      ((gpio_num < GPIO_PIN_COUNT && GPIO_PIN_MUX_REG[gpio_num] != 0))   /*!< Check whether it is a valid GPIO number */
#define GPIO_IS_VALID_OUTPUT_GPIO(gpio_num)      ((GPIO_IS_VALID_GPIO(gpio_num)) && (gpio_num < 34))         /*!< Check whether it can be a valid GPIO number of output mode */

typedef enum {
    GPIO_NUM_0 = 0,     /*!< GPIO0, input and output */
    GPIO_NUM_1 = 1,     /*!< GPIO1, input and output */
    GPIO_NUM_2 = 2,     /*!< GPIO2, input and output
                             @note There are more enumerations like that
                             up to GPIO39, excluding GPIO20, GPIO24 and GPIO28..31.
                             They are not shown here to reduce redundant information.
                             @note GPIO34..39 are input mode only. */
/** @cond */
    GPIO_NUM_3 = 3,     /*!< GPIO3, input and output */
    GPIO_NUM_4 = 4,     /*!< GPIO4, input and output */
    GPIO_NUM_5 = 5,     /*!< GPIO5, input and output */
    GPIO_NUM_6 = 6,     /*!< GPIO6, input and output */
    GPIO_NUM_7 = 7,     /*!< GPIO7, input and output */
    GPIO_NUM_8 = 8,     /*!< GPIO8, input and output */
    GPIO_NUM_9 = 9,     /*!< GPIO9, input and output */
    GPIO_NUM_10 = 10,   /*!< GPIO10, input and output */
    GPIO_NUM_11 = 11,   /*!< GPIO11, input and output */
    GPIO_NUM_12 = 12,   /*!< GPIO12, input and output */
    GPIO_NUM_13 = 13,   /*!< GPIO13, input and output */
    GPIO_NUM_14 = 14,   /*!< GPIO14, input and output */
    GPIO_NUM_15 = 15,   /*!< GPIO15, input and output */
    GPIO_NUM_16 = 16,   /*!< GPIO16, input and output */
    GPIO_NUM_17 = 17,   /*!< GPIO17, input and output */
    GPIO_NUM_18 = 18,   /*!< GPIO18, input and output */
    GPIO_NUM_19 = 19,   /*!< GPIO19, input and output */

    GPIO_NUM_21 = 21,   /*!< GPIO21, input and output */
    GPIO_NUM_22 = 22,   /*!< GPIO22, input and output */
    GPIO_NUM_23 = 23,   /*!< GPIO23, input and output */

    GPIO_NUM_25 = 25,   /*!< GPIO25, input and output */
    GPIO_NUM_26 = 26,   /*!< GPIO26, input and output */
    GPIO_NUM_27 = 27,   /*!< GPIO27, input and output */

    GPIO_NUM_32 = 32,   /*!< GPIO32, input and output */
    GPIO_NUM_33 = 33,   /*!< GPIO33, input and output */
    GPIO_NUM_34 = 34,   /*!< GPIO34, input mode only */
    GPIO_NUM_35 = 35,   /*!< GPIO35, input mode only */
    GPIO_NUM_36 = 36,   /*!< GPIO36, input mode only */
    GPIO_NUM_37 = 37,   /*!< GPIO37, input mode only */
    GPIO_NUM_38 = 38,   /*!< GPIO38, input mode only */
    GPIO_NUM_39 = 39,   /*!< GPIO39, input mode only */
    GPIO_NUM_MAX = 40,
/** @endcond */
} gpio_num_t;

typedef enum {
    GPIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    GPIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    GPIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    GPIO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
    GPIO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
    GPIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
    GPIO_INTR_MAX,
} gpio_int_type_t;

typedef enum {
    GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                        /*!< GPIO mode : disable input and output             */
    GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                         /*!< GPIO mode : input only                           */
    GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                       /*!< GPIO mode : output only mode                     */
    GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT)|(GPIO_MODE_DEF_OD)),                             /*!< GPIO mode : output only with open-drain mode     */
    GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT)|(GPIO_MODE_DEF_OUTPUT)|(GPIO_MODE_DEF_OD)), /*!< GPIO mode : output and input with open-drain mode*/
    GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT)|(GPIO_MODE_DEF_OUTPUT)),                       /*!< GPIO mode : output and input mode                */
} gpio_mode_t;

typedef enum {
    GPIO_PULLUP_DISABLE = 0x0,     /*!< Disable GPIO pull-up resistor */
    GPIO_PULLUP_ENABLE = 0x1,      /*!< Enable GPIO pull-up resistor */
} gpio_pullup_t;

typedef enum {
    GPIO_PULLDOWN_DISABLE = 0x0,   /*!< Disable GPIO pull-down resistor */
    GPIO_PULLDOWN_ENABLE = 0x1,    /*!< Enable GPIO pull-down resistor  */
} gpio_pulldown_t;

/**
 * @brief Configuration parameters of GPIO pad for gpio_config function
 */
typedef struct {
    uint64_t pin_bit_mask;          /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    gpio_mode_t mode;               /*!< GPIO mode: set input/output mode                     */
    gpio_pullup_t pull_up_en;       /*!< GPIO pull-up                                         */
    gpio_pulldown_t pull_down_en;   /*!< GPIO pull-down                                       */
    gpio_int_type_t intr_type;      /*!< GPIO interrupt type                                  */
} gpio_config_t;

typedef enum {
    GPIO_PULLUP_ONLY,               /*!< Pad pull up            */
    GPIO_PULLDOWN_ONLY,             /*!< Pad pull down          */
    GPIO_PULLUP_PULLDOWN,           /*!< Pad pull up + pull down*/
    GPIO_FLOATING,                  /*!< Pad floating           */
} gpio_pull_mode_t;

typedef enum {
    GPIO_DRIVE_CAP_0       = 0,    /*!< Pad drive capability: weak          */
    GPIO_DRIVE_CAP_1       = 1,    /*!< Pad drive capability: stronger      */
    GPIO_DRIVE_CAP_2       = 2,    /*!< Pad drive capability: default value */
    GPIO_DRIVE_CAP_DEFAULT = 2,    /*!< Pad drive capability: default value */
    GPIO_DRIVE_CAP_3       = 3,    /*!< Pad drive capability: strongest     */
    GPIO_DRIVE_CAP_MAX,
} gpio_drive_cap_t;

typedef void (*gpio_isr_t)(void*);
typedef intr_handle_t gpio_isr_handle_t;

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
 * @note Please do not use the interrupt of GPIO36 and GPIO39 when using ADC or Wi-Fi with sleep mode enabled.
 *       Please refer to the comments of `adc1_get_raw`.
 *       Please refer to section 3.11 of 'ECO_and_Workarounds_for_Bugs_in_ESP32' for the description of this issue.
 *       As a workaround, call adc_power_acquire() in the app. This will result in higher power consumption (by ~1mA),
 *       but will remove the glitches on GPIO36 and GPIO39.
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
 * @brief	 GPIO set direction
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
 * Only pins that support both input & output have integrated pull-up and pull-down resistors. Input-only GPIOs 34-39 do not.
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
 * @param  intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param  arg  Parameter for handler function
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
esp_err_t gpio_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, gpio_isr_handle_t *handle);

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
  * @brief Install the driver's GPIO ISR handler service, which allows per-pin GPIO interrupt handlers.
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
void gpio_uninstall_isr_service();

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
esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void* args);

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
esp_err_t gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t* strength);

/**
  * @brief Enable gpio pad hold function.
  *
  * The gpio pad hold function works in both input and output modes, but must be output-capable gpios.
  * If pad hold enabled:
  *   in output mode: the output level of the pad will be force locked and can not be changed.
  *   in input mode: the input value read will not change, regardless the changes of input signal.
  *
  * The state of digital gpio cannot be held during Deep-sleep, and it will resume the hold function
  * when the chip wakes up from Deep-sleep. If the digital gpio also needs to be held during Deep-sleep,
  * `gpio_deep_sleep_hold_en` should also be called.
  *
  * Power down or call gpio_hold_dis will disable this function.
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
  * When the chip is woken up from Deep-sleep, the gpio will be set to the default mode, so, the gpio will output
  * the default level if this function is called. If you dont't want the level changes, the gpio should be configured to
  * a known state before this function is called.
  *  e.g.
  *     If you hold gpio18 high during Deep-sleep, after the chip is woken up and `gpio_hold_dis` is called, 
  *     gpio18 will output low level(because gpio18 is input mode by default). If you don't want this behavior,
  *     you should configure gpio18 as output mode and set it to hight level before calling `gpio_hold_dis`.
  *
  * @param gpio_num GPIO number, only support output-capable GPIOs
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_NOT_SUPPORTED Not support pad hold function
  */
esp_err_t gpio_hold_dis(gpio_num_t gpio_num);

/**
  * @brief Enable all digital gpio pad hold function during Deep-sleep.
  *
  * When the chip is in Deep-sleep mode, all digital gpio will hold the state before sleep, and when the chip is woken up,
  * the status of digital gpio will not be held. Note that the pad hold feature only works when the chip is in Deep-sleep mode,
  * when not in sleep mode, the digital gpio state can be changed even you have called this function.
  *
  * Power down or call gpio_hold_dis will disable this function, otherwise, the digital gpio hold feature works as long as the chip enter Deep-sleep.
  */
void gpio_deep_sleep_hold_en(void);

/**
  * @brief Disable all digital gpio pad hold function during Deep-sleep.
  *
  */
void gpio_deep_sleep_hold_dis(void);

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
  * @param oen_inv True if the output enable needs to be inversed, otherwise False.
  */
void gpio_iomux_out(uint8_t gpio_num, int func, bool oen_inv);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_GPIO_H_ */
