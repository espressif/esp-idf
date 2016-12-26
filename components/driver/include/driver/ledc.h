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
#include "soc/ledc_reg.h"
#include "soc/ledc_reg.h"
#include "soc/ledc_struct.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_intr_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDC_APB_CLK_HZ (APB_CLK_FREQ)
#define LEDC_REF_CLK_HZ (1*1000000)

typedef enum {
    LEDC_HIGH_SPEED_MODE = 0, /*!< LEDC high speed speed_mode */
    //in this version, we only support high speed speed_mode. We will access low speed speed_mode later
    //LEDC_LOW_SPEED_MODE,    /*!< LEDC low speed speed_mode */
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
} ledc_channel_t;

typedef enum {
    LEDC_TIMER_10_BIT = 10, /*!< LEDC PWM depth 10Bit */
    LEDC_TIMER_11_BIT = 11, /*!< LEDC PWM depth 11Bit */
    LEDC_TIMER_12_BIT = 12, /*!< LEDC PWM depth 12Bit */
    LEDC_TIMER_13_BIT = 13, /*!< LEDC PWM depth 13Bit */
    LEDC_TIMER_14_BIT = 14, /*!< LEDC PWM depth 14Bit */
    LEDC_TIMER_15_BIT = 15, /*!< LEDC PWM depth 15Bit */
} ledc_timer_bit_t;

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
 * @brief      LEDC channel configuration
 *
 * User this Function, configure LEDC channel with the given channel/output gpio_num/interrupt/source timer/frequency(Hz)/LEDC depth
 *
 * @param  ledc_conf Pointer of LEDC channel configure struct
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t ledc_channel_config(ledc_channel_config_t* ledc_conf);

/**
 * @brief      LEDC timer configuration
 *
 * User this Function, configure LEDC timer with the given source timer/frequency(Hz)/bit_num
 *
 * @param  timer_conf Pointer of LEDC timer configure struct
 *
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Can not find a proper pre-divider number base on the given frequency and the current bit_num.
 *
 */
esp_err_t ledc_timer_config(ledc_timer_config_t* timer_conf);

/**
 * @brief      LEDC update channel parameters
 *
 * Call this function to activate the LEDC updated parameters.
 * After ledc_set_duty, ledc_set_fade, we need to call this function to update the settings.
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  channel LEDC channel(0-7), select from ledc_channel_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief   LEDC stop
 *
 * Disable LEDC output, and set idle level
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  channel LEDC channel(0-7), select from ledc_channel_t
 *
 * @param  idle_level Set output idle level after LEDC stops.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level);

/**
 * @brief      LEDC set channel frequency(Hz)
 *
 * Set LEDC frequency(Hz)
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  timer_num LEDC timer index(0-3), select from ledc_timer_t
 *
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
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param timer_num LEDC timer index(0-3), select from ledc_timer_t
 *
 * @return
 *     - 0  error
 *     - Others Current LEDC frequency
 *
 */
uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num);

/**
 * @brief      LEDC set duty
 *
 * Set LEDC duty, After the function calls the ledc_update_duty function, the function can take effect.
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param channel LEDC channel(0-7), select from ledc_channel_t
 *
 * @param duty Set the LEDC duty, the duty range is [0, (2**bit_num) - 1]
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);

/**
 * @brief      LEDC get duty
 *
 * @param speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param channel LEDC channel(0-7), select from ledc_channel_t
 *
 *
 * @return
 *     - (-1) parameter error
 *     - Others Current LEDC duty
 *
 */
int ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief      LEDC set gradient
 *
 * Set LEDC gradient, After the function calls the ledc_update_duty function, the function can take effect.
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  channel LEDC channel(0-7), select from ledc_channel_t
 *
 * @param  duty Set the start of the gradient duty, the duty range is [0, (2**bit_num) - 1]
 *
 * @param  gradule_direction Set the direction of the gradient
 *
 * @param  step_num Set the number of the gradient
 *
 * @param  duty_cyle_num Set how many LEDC tick each time the gradient lasts
 *
 * @param  duty_scale Set gradient change amplitude
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_fade(ledc_mode_t speed_mode, uint32_t channel, uint32_t duty, ledc_duty_direction_t gradule_direction,
                        uint32_t step_num, uint32_t duty_cyle_num, uint32_t duty_scale);

/**
 * @brief   register LEDC interrupt handler, the handler is an ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 *
 * @param   fn Interrupt handler function.
 * @param  arg User-supplied argument passed to the handler function.
 * @param  intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param   arg Parameter for handler function
 * @param  handle Pointer to return handle. If non-NULL, a handle for the interrupt will
 *            be returned here.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t ledc_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, ledc_isr_handle_t *handle);

/**
 * @brief      configure LEDC settings
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  timer_sel  Timer index(0-3), there are 4 timers in LEDC module
 *
 * @param  div_num Timer clock divide number, the timer clock is divided from the selected clock source
 *
 * @param  bit_num The count number of one period, counter range is 0 ~ ((2 ** bit_num) - 1)
 *
 * @param  clk_src Select LEDC source clock.
 *
 * @return
 *     - (-1) Parameter error
 *     - Other Current LEDC duty
 *
 */
esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t div_num, uint32_t bit_num, ledc_clk_src_t clk_src);

/**
 * @brief      reset LEDC timer
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  timer_sel LEDC timer index(0-3), select from ledc_timer_t
 *
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *
 */
esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, uint32_t timer_sel);

/**
 * @brief      pause LEDC timer counter
 *
 * @param  speed_mode  Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  timer_sel LEDC timer index(0-3), select from ledc_timer_t
 *
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *
 */
esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, uint32_t timer_sel);

/**
 * @brief      pause LEDC timer resume
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  timer_sel LEDC timer index(0-3), select from ledc_timer_t
 *
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *
 */
esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, uint32_t timer_sel);

/**
 * @brief      bind LEDC channel with the selected timer
 *
 * @param  speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mode, now we only support high-speed mode. We will access low-speed mode in next version
 *
 * @param  channel LEDC channel index(0-7), select from ledc_channel_t
 *
 * @param  timer_idx LEDC timer index(0-3), select from ledc_timer_t
 *
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *
 */
esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, uint32_t channel, uint32_t timer_idx);

/***************************EXAMPLE**********************************
 *
 *
 * ----------------EXAMPLE OF LEDC SETTING ---------------------
 * @code{c}
 * //1. enable LEDC
 * //enable LEDC module, or you can not set any register of it.
 * periph_module_enable(PERIPH_LEDC_MODULE);
 * @endcode
 *
 * @code{c}
 * //2. set LEDC timer
 * ledc_timer_config_t timer_conf = {
 *     .bit_num = LEDC_TIMER_12_BIT,                        //set timer counter bit number
 *     .freq_hz = 1000,                                     //set frequency of pwm, here, 1000Hz
 *     .speed_mode = LEDC_HIGH_SPEED_MODE,                  //timer mode,
 *     .timer_num = LEDC_TIMER_0,                           //timer number
 * };
 * ledc_timer_config(&timer_conf);                          //setup timer.
 * @endcode
 *
 * @code{c}
 * //3. set LEDC channel
 * ledc_channel_config_t ledc_conf = {
 *     .channel = LEDC_CHANNEL_0;                           //set LEDC channel 0
 *     .duty = 1000;                                        //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
 *     .gpio_num = 16;                                      //GPIO number
 *     .intr_type = LEDC_INTR_FADE_END;                     //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
 *     .speed_mode = LEDC_HIGH_SPEED_MODE;                  //set LEDC mode, from ledc_mode_t
 *     .timer_sel = LEDC_TIMER_0;                           //set LEDC timer source, if different channel use one timer, the frequency and bit_num of these channels should be the same
 * }
 * ledc_channel_config(&ledc_conf);                         //setup the configuration
 *
 * ----------------EXAMPLE OF SETTING DUTY --- -----------------
 * @code{c}
 * uint32_t ledc_channel = LEDC_CHANNEL_0;                  //LEDC channel(0-73)
 * uint32_t duty = 2000;                                    //duty range is 0 ~ ((2**bit_num)-1)
 * LEDC_set_duty(LEDC_HIGH_SPEED_MODE, ledc_channel, duty); //set speed mode, channel, and duty.
 * ledc_update_duty(LEDC_HIGH_SPEED_MODE, ledc_channel);    //after set duty, we need to call ledc_update_duty to update the settings.
 * @endcode
 *
 * ----------------EXAMPLE OF LEDC INTERRUPT ------------------
 * @code{c}
 * //we have fade_end interrupt and counter overflow interrupt. we just give an example of fade_end interrupt here.
 * ledc_isr_register(ledc_isr_handler, NULL, 0);           //hook the isr handler for LEDC interrupt
 * @endcode
 *
 * ----------------EXAMPLE OF INTERRUPT HANDLER ---------------
 * @code{c}
 * #include "esp_attr.h"
 * void IRAM_ATTR ledc_isr_handler(void* arg)                   //we should add 'IRAM_ATTR' attribution when we declare the isr function
 * {
 *    uint32_t intr_st = LEDC.int_st.val;                       //read LEDC interrupt status.
 *
 *    //you will find which channels have triggered fade_end interrupt here,
 *    //then, you can post some event to RTOS queue to process the event.
 *    //later we will add a queue in the driver code.
 *
 *    LEDC.int_clr.val = intr_st;                               //clear LEDC interrupt status.
 * }
 * @endcode
 *
 *--------------------------END OF EXAMPLE --------------------------
 */




#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_LEDC_H_ */
