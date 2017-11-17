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

#ifndef _DRIVER_LEDC_H_
#define _DRIVER_LEDC_H_
#include "esp_err.h"
#include "soc/soc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_intr_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDC_APB_CLK_HZ (APB_CLK_FREQ)
#define LEDC_REF_CLK_HZ (1*1000000)
#define LEDC_ERR_DUTY   (0xFFFFFFFF)

typedef enum {
    LEDC_HIGH_SPEED_MODE = 0, /*!< LEDC high speed speed_mode */
    LEDC_LOW_SPEED_MODE,    /*!< LEDC low speed speed_mode */
    LEDC_SPEED_MODE_MAX,      /*!< LEDC speed limit */
} ledc_mode_t;

typedef enum {
    LEDC_INTR_DISABLE = 0,    /*!< Disable LEDC interrupt */
    LEDC_INTR_FADE_END,       /*!< Enable LEDC interrupt */
} ledc_intr_type_t;

typedef enum {
    LEDC_DUTY_DIR_DECREASE = 0,    /*!< LEDC duty decrease direction */
    LEDC_DUTY_DIR_INCREASE = 1,    /*!< LEDC duty increase direction */
} ledc_duty_direction_t;

typedef enum  {
    LEDC_REF_TICK = 0, /*!< LEDC timer clock divided from reference tick(1Mhz) */
    LEDC_APB_CLK,      /*!< LEDC timer clock divided from APB clock(80Mhz)*/
} ledc_clk_src_t;

typedef enum {
    LEDC_TIMER_0 = 0, /*!< LEDC source timer TIMER0 */
    LEDC_TIMER_1,     /*!< LEDC source timer TIMER1 */
    LEDC_TIMER_2,     /*!< LEDC source timer TIMER2 */
    LEDC_TIMER_3,     /*!< LEDC source timer TIMER3 */
} ledc_timer_t;

typedef enum {
    LEDC_CHANNEL_0 = 0, /*!< LEDC channel 0 */
    LEDC_CHANNEL_1,     /*!< LEDC channel 1 */
    LEDC_CHANNEL_2,     /*!< LEDC channel 2 */
    LEDC_CHANNEL_3,     /*!< LEDC channel 3 */
    LEDC_CHANNEL_4,     /*!< LEDC channel 4 */
    LEDC_CHANNEL_5,     /*!< LEDC channel 5 */
    LEDC_CHANNEL_6,     /*!< LEDC channel 6 */
    LEDC_CHANNEL_7,     /*!< LEDC channel 7 */
    LEDC_CHANNEL_MAX,
} ledc_channel_t;

typedef enum {
    LEDC_TIMER_10_BIT = 10, /*!< LEDC PWM depth 10Bit */
    LEDC_TIMER_11_BIT = 11, /*!< LEDC PWM depth 11Bit */
    LEDC_TIMER_12_BIT = 12, /*!< LEDC PWM depth 12Bit */
    LEDC_TIMER_13_BIT = 13, /*!< LEDC PWM depth 13Bit */
    LEDC_TIMER_14_BIT = 14, /*!< LEDC PWM depth 14Bit */
    LEDC_TIMER_15_BIT = 15, /*!< LEDC PWM depth 15Bit */
} ledc_timer_bit_t;

typedef enum {
    LEDC_FADE_NO_WAIT = 0,  /*!< LEDC fade function will return immediately */
    LEDC_FADE_WAIT_DONE,    /*!< LEDC fade function will block until fading to the target duty*/
    LEDC_FADE_MAX,
} ledc_fade_mode_t;
/**
 * @brief Configuration parameters of LEDC channel for ledc_channel_config function
 */
typedef struct {
    int gpio_num;                   /*!< the LEDC output gpio_num, if you want to use gpio16, gpio_num = 16*/
    ledc_mode_t speed_mode;         /*!< LEDC speed speed_mode, high-speed mode or low-speed mode*/
    ledc_channel_t channel;         /*!< LEDC channel(0 - 7)*/
    ledc_intr_type_t intr_type;     /*!< configure interrupt, Fade interrupt enable  or Fade interrupt disable*/
    ledc_timer_t timer_sel;         /*!< Select the timer source of channel (0 - 3)*/
    uint32_t duty;                  /*!< LEDC channel duty, the duty range is [0, (2**bit_num) - 1], */
} ledc_channel_config_t;

/**
 * @brief Configuration parameters of LEDC Timer timer for ledc_timer_config function
 */
typedef struct {
    ledc_mode_t speed_mode;         /*!< LEDC speed speed_mode, high-speed mode or low-speed mode*/
    ledc_timer_bit_t bit_num;       /*!< LEDC channel duty depth*/
    ledc_timer_t  timer_num;        /*!< The timer source of channel (0 - 3)*/
    uint32_t freq_hz;               /*!< LEDC timer frequency(Hz)*/
} ledc_timer_config_t;

typedef intr_handle_t ledc_isr_handle_t;

/**
 * @brief LEDC channel configuration
 *        Configure LEDC channel with the given channel/output gpio_num/interrupt/source timer/frequency(Hz)/LEDC depth
 *
 * @param ledc_conf Pointer of LEDC channel configure struct
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_channel_config(const ledc_channel_config_t* ledc_conf);

/**
 * @brief LEDC timer configuration
 *        Configure LEDC timer with the given source timer/frequency(Hz)/bit_num
 *
 * @param  timer_conf Pointer of LEDC timer configure struct
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Can not find a proper pre-divider number base on the given frequency and the current bit_num.
 */
esp_err_t ledc_timer_config(const ledc_timer_config_t* timer_conf);

/**
 * @brief LEDC update channel parameters
 *        Call this function to activate the LEDC updated parameters.
 *        After ledc_set_duty, ledc_set_fade, we need to call this function to update the settings.
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode,
 * @param channel LEDC channel(0-7), select from ledc_channel_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief LEDC stop.
 *        Disable LEDC output, and set idle level
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param  channel LEDC channel(0-7), select from ledc_channel_t
 * @param  idle_level Set output idle level after LEDC stops.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level);

/**
 * @brief LEDC set channel frequency(Hz)
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param  timer_num LEDC timer index(0-3), select from ledc_timer_t
 * @param  freq_hz Set the LEDC frequency
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Can not find a proper pre-divider number base on the given frequency and the current bit_num.
 */
esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz);

/**
 * @brief      LEDC get channel frequency(Hz)
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param timer_num LEDC timer index(0-3), select from ledc_timer_t
 *
 * @return
 *     - 0  error
 *     - Others Current LEDC frequency
 */
uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num);

/**
 * @brief LEDC set duty
 *        Only after calling ledc_update_duty will the duty update.
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param channel LEDC channel(0-7), select from ledc_channel_t
 * @param duty Set the LEDC duty, the duty range is [0, (2**bit_num) - 1]
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);

/**
 * @brief LEDC get duty
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param channel LEDC channel(0-7), select from ledc_channel_t
 *
 * @return
 *     - LEDC_ERR_DUTY if parameter error
 *     - Others Current LEDC duty
 */
uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief LEDC set gradient
 *        Set LEDC gradient, After the function calls the ledc_update_duty function, the function can take effect.
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param  channel LEDC channel(0-7), select from ledc_channel_t
 * @param  duty Set the start of the gradient duty, the duty range is [0, (2**bit_num) - 1]
 * @param  gradule_direction Set the direction of the gradient
 * @param  step_num Set the number of the gradient
 * @param  duty_cyle_num Set how many LEDC tick each time the gradient lasts
 * @param  duty_scale Set gradient change amplitude
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, ledc_duty_direction_t gradule_direction,
                        uint32_t step_num, uint32_t duty_cyle_num, uint32_t duty_scale);

/**
 * @brief Register LEDC interrupt handler, the handler is an ISR.
 *        The handler will be attached to the same CPU core that this function is running on.
 *
 * @param fn Interrupt handler function.
 * @param arg User-supplied argument passed to the handler function.
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param arg Parameter for handler function
 * @param handle Pointer to return handle. If non-NULL, a handle for the interrupt will
 *        be returned here.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t ledc_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, ledc_isr_handle_t *handle);

/**
 * @brief Configure LEDC settings
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param timer_sel  Timer index(0-3), there are 4 timers in LEDC module
 * @param div_num Timer clock divide number, the timer clock is divided from the selected clock source
 * @param bit_num The count number of one period, counter range is 0 ~ ((2 ** bit_num) - 1)
 * @param clk_src Select LEDC source clock.
 *
 * @return
 *     - (-1) Parameter error
 *     - Other Current LEDC duty
 */
esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t div_num, uint32_t bit_num, ledc_clk_src_t clk_src);

/**
 * @brief Reset LEDC timer
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param  timer_sel LEDC timer index(0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, uint32_t timer_sel);

/**
 * @brief Pause LEDC timer counter
 *
 * @param  speed_mode  Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param  timer_sel LEDC timer index(0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *
 */
esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, uint32_t timer_sel);

/**
 * @brief Resume LEDC timer
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param timer_sel LEDC timer index(0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, uint32_t timer_sel);

/**
 * @brief Bind LEDC channel with the selected timer
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param channel LEDC channel index(0-7), select from ledc_channel_t
 * @param timer_idx LEDC timer index(0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, uint32_t channel, uint32_t timer_idx);

/**
 * @brief Set LEDC fade function. Should call ledc_fade_func_install() before calling this function.
 *        Call ledc_fade_start() after this to start fading.
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode,
 * @param channel LEDC channel index(0-7), select from ledc_channel_t
 * @param target_duty Target duty of fading.( 0 - (2 ** bit_num - 1)))
 * @param scale Controls the increase or decrease step scale.
 * @param cycle_num increase or decrease the duty every cycle_num cycles
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_FAIL Fade function init error
 */
esp_err_t ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int scale, int cycle_num);

/**
 * @brief Set LEDC fade function, with a limited time. Should call ledc_fade_func_install() before calling this function.
 *        Call ledc_fade_start() after this to start fading.
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode,
 * @param channel LEDC channel index(0-7), select from ledc_channel_t
 * @param target_duty Target duty of fading.( 0 - (2 ** bit_num - 1)))
 * @param max_fade_time_ms The maximum time of the fading ( ms ).
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_FAIL Fade function init error
 */
esp_err_t ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms);

/**
 * @brief Install ledc fade function. This function will occupy interrupt of LEDC module.
 *
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function already installed.
 */
esp_err_t ledc_fade_func_install(int intr_alloc_flags);

/**
 * @brief Uninstall LEDC fade function.
 *
 */
void ledc_fade_func_uninstall();

/**
 * @brief Start LEDC fading.
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode
 * @param channel LEDC channel number
 * @param wait_done Whether to block until fading done.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_ERR_INVALID_ARG Parameter error.
 */
esp_err_t ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t wait_done);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_LEDC_H_ */
