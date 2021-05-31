/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "hal/ledc_types.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDC_APB_CLK_HZ  (APB_CLK_FREQ)
#define LEDC_REF_CLK_HZ  (REF_CLK_FREQ)
#define LEDC_ERR_DUTY    (0xFFFFFFFF)
#define LEDC_ERR_VAL     (-1)

typedef intr_handle_t ledc_isr_handle_t;

/**
 * @brief LEDC channel configuration
 *        Configure LEDC channel with the given channel/output gpio_num/interrupt/source timer/frequency(Hz)/LEDC duty resolution
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
 *        Configure LEDC timer with the given source timer/frequency(Hz)/duty_resolution
 *
 * @param  timer_conf Pointer of LEDC timer configure struct
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Can not find a proper pre-divider number base on the given frequency and the current duty_resolution.
 */
esp_err_t ledc_timer_config(const ledc_timer_config_t* timer_conf);

/**
 * @brief LEDC update channel parameters
 * @note  Call this function to activate the LEDC updated parameters.
 *        After ledc_set_duty, we need to call this function to update the settings.
 * @note  ledc_set_duty, ledc_set_duty_with_hpoint and ledc_update_duty are not thread-safe, do not call these functions to
 *        control one LEDC channel in different tasks at the same time.
 *        A thread-safe version of API is ledc_set_duty_and_update
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *
 */
esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief Set LEDC output gpio.
 * @deprecated This function is redundant, please use ledc_channel_config to set gpio pins.
 *
 * @param  gpio_num The LEDC output gpio
 * @param  speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param  ledc_channel LEDC channel (0-7), select from ledc_channel_t
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_pin(int gpio_num, ledc_mode_t speed_mode, ledc_channel_t ledc_channel)
    __attribute__((deprecated("use ledc_channel_config instead")));
/**
 * @brief LEDC stop.
 *        Disable LEDC output, and set idle level
 *
 * @param  speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param  channel LEDC channel (0-7), select from ledc_channel_t
 * @param  idle_level Set output idle level after LEDC stops.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level);

/**
 * @brief LEDC set channel frequency (Hz)
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param  timer_num LEDC timer index (0-3), select from ledc_timer_t
 * @param  freq_hz Set the LEDC frequency
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Can not find a proper pre-divider number base on the given frequency and the current duty_resolution.
 */
esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz);

/**
 * @brief      LEDC get channel frequency (Hz)
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param timer_num LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return
 *     - 0  error
 *     - Others Current LEDC frequency
 */
uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num);

/**
 * @brief LEDC set duty and hpoint value
 *        Only after calling ledc_update_duty will the duty update.
 * @note  ledc_set_duty, ledc_set_duty_with_hpoint and ledc_update_duty are not thread-safe, do not call these functions to
 *        control one LEDC channel in different tasks at the same time.
 *        A thread-safe version of API is ledc_set_duty_and_update
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 * @param duty Set the LEDC duty, the range of duty setting is [0, (2**duty_resolution)]
 * @param hpoint Set the LEDC hpoint value(max: 0xfffff)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_duty_with_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint);

/**
 * @brief LEDC get hpoint value, the counter value when the output is set high level.
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 * @return
 *     - LEDC_ERR_VAL if parameter error
 *     - Others Current hpoint value of LEDC channel
 */
int ledc_get_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief LEDC set duty
 *        This function do not change the hpoint value of this channel. if needed, please call ledc_set_duty_with_hpoint.
 *        only after calling ledc_update_duty will the duty update.
 * @note  ledc_set_duty, ledc_set_duty_with_hpoint and ledc_update_duty are not thread-safe, do not call these functions to
 *        control one LEDC channel in different tasks at the same time.
 *        A thread-safe version of API is ledc_set_duty_and_update.
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 * @param duty Set the LEDC duty, the range of duty setting is [0, (2**duty_resolution)]
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);

/**
 * @brief LEDC get duty
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 *
 * @return
 *     - LEDC_ERR_DUTY if parameter error
 *     - Others Current LEDC duty
 */
uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel);

/**
 * @brief LEDC set gradient
 *        Set LEDC gradient, After the function calls the ledc_update_duty function, the function can take effect.
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 * @param duty Set the start of the gradient duty, the range of duty setting is [0, (2**duty_resolution)]
 * @param fade_direction Set the direction of the gradient
 * @param step_num Set the number of the gradient
 * @param duty_cycle_num Set how many LEDC tick each time the gradient lasts
 * @param duty_scale Set gradient change amplitude
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ledc_set_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, ledc_duty_direction_t fade_direction,
                        uint32_t step_num, uint32_t duty_cycle_num, uint32_t duty_scale);

/**
 * @brief Register LEDC interrupt handler, the handler is an ISR.
 *        The handler will be attached to the same CPU core that this function is running on.
 *
 * @param fn Interrupt handler function.
 * @param arg User-supplied argument passed to the handler function.
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
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
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param timer_sel  Timer index (0-3), there are 4 timers in LEDC module
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 * @param duty_resolution Resolution of duty setting in number of bits. The range of duty values is [0, (2**duty_resolution)]
 * @param clk_src Select LEDC source clock.
 *
 * @return
 *     - (-1) Parameter error
 *     - Other Current LEDC duty
 */
esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider, uint32_t duty_resolution, ledc_clk_src_t clk_src);

/**
 * @brief Reset LEDC timer
 *
 * @param  speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param  timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, ledc_timer_t timer_sel);

/**
 * @brief Pause LEDC timer counter
 *
 * @param  speed_mode  Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param  timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *
 */
esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, ledc_timer_t timer_sel);

/**
 * @brief Resume LEDC timer
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, ledc_timer_t timer_sel);

/**
 * @brief Bind LEDC channel with the selected timer
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel index (0-7), select from ledc_channel_t
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_timer_t timer_sel);

/**
 * @brief Set LEDC fade function.
 * @note  Call ledc_fade_func_install() once before calling this function.
 *        Call ledc_fade_start() after this to start fading.
 * @note  ledc_set_fade_with_step, ledc_set_fade_with_time and ledc_fade_start are not thread-safe, do not call these functions to
 *        control one LEDC channel in different tasks at the same time.
 *        A thread-safe version of API is ledc_set_fade_step_and_start
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode. ,
 * @param channel LEDC channel index (0-7), select from ledc_channel_t
 * @param target_duty Target duty of fading [0, (2**duty_resolution) - 1]
 * @param scale Controls the increase or decrease step scale.
 * @param cycle_num increase or decrease the duty every cycle_num cycles
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_FAIL Fade function init error
 */
esp_err_t ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num);

/**
 * @brief Set LEDC fade function, with a limited time.
 * @note  Call ledc_fade_func_install() once before calling this function.
 *        Call ledc_fade_start() after this to start fading.
 * @note  ledc_set_fade_with_step, ledc_set_fade_with_time and ledc_fade_start are not thread-safe, do not call these functions to
 *        control one LEDC channel in different tasks at the same time.
 *        A thread-safe version of API is ledc_set_fade_step_and_start
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode. ,
 * @param channel LEDC channel index (0-7), select from ledc_channel_t
 * @param target_duty Target duty of fading.( 0 - (2 ** duty_resolution - 1)))
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
 * @brief Install LEDC fade function. This function will occupy interrupt of LEDC module.
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
void ledc_fade_func_uninstall(void);

/**
 * @brief Start LEDC fading.
 * @note  Call ledc_fade_func_install() once before calling this function.
 *        Call this API right after ledc_set_fade_with_time or ledc_set_fade_with_step before to start fading.
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel number
 * @param fade_mode Whether to block until fading done.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_ERR_INVALID_ARG Parameter error.
 */
esp_err_t ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode);

/**
 * @brief A thread-safe API to set duty for LEDC channel and return when duty updated.
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 *
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel (0-7), select from ledc_channel_t
 * @param duty Set the LEDC duty, the range of duty setting is [0, (2**duty_resolution)]
 * @param hpoint Set the LEDC hpoint value(max: 0xfffff)
 *
 */
esp_err_t ledc_set_duty_and_update(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint);

/**
 * @brief A thread-safe API to set and start LEDC fade function, with a limited time.
 * @note  Call ledc_fade_func_install() once, before calling this function.
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel index (0-7), select from ledc_channel_t
 * @param target_duty Target duty of fading.( 0 - (2 ** duty_resolution - 1)))
 * @param max_fade_time_ms The maximum time of the fading ( ms ).
 * @param fade_mode choose blocking or non-blocking mode
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_FAIL Fade function init error
 */
esp_err_t ledc_set_fade_time_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t max_fade_time_ms, ledc_fade_mode_t fade_mode);

/**
 * @brief A thread-safe API to set and start LEDC fade function.
 * @note  Call ledc_fade_func_install() once before calling this function.
 * @note  If a fade operation is running in progress on that channel, the driver would not allow it to be stopped.
 *        Other duty operations will have to wait until the fade operation has finished.
 * @param speed_mode Select the LEDC channel group with specified speed mode. Note that not all targets support high speed mode.
 * @param channel LEDC channel index (0-7), select from ledc_channel_t
 * @param target_duty Target duty of fading [0, (2**duty_resolution) - 1]
 * @param scale Controls the increase or decrease step scale.
 * @param cycle_num increase or decrease the duty every cycle_num cycles
 * @param fade_mode choose blocking or non-blocking mode
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE Fade function not installed.
 *     - ESP_FAIL Fade function init error
 */
esp_err_t ledc_set_fade_step_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num, ledc_fade_mode_t fade_mode);
#ifdef __cplusplus
}
#endif
