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

#ifndef _DRIVER_TOUCH_PAD_H_
#define _DRIVER_TOUCH_PAD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/touch_periph.h"

#if CONFIG_IDF_TARGET_ESP32
typedef enum {
    TOUCH_PAD_NUM0 = 0, /*!< Touch pad channel 0 is GPIO4 */
    TOUCH_PAD_NUM1,     /*!< Touch pad channel 1 is GPIO0 */
    TOUCH_PAD_NUM2,     /*!< Touch pad channel 2 is GPIO2 */
    TOUCH_PAD_NUM3,     /*!< Touch pad channel 3 is GPIO15*/
    TOUCH_PAD_NUM4,     /*!< Touch pad channel 4 is GPIO13*/
    TOUCH_PAD_NUM5,     /*!< Touch pad channel 5 is GPIO12*/
    TOUCH_PAD_NUM6,     /*!< Touch pad channel 6 is GPIO14*/
    TOUCH_PAD_NUM7,     /*!< Touch pad channel 7 is GPIO27*/
    TOUCH_PAD_NUM8,     /*!< Touch pad channel 8 is GPIO33*/
    TOUCH_PAD_NUM9,     /*!< Touch pad channel 9 is GPIO32*/
    TOUCH_PAD_MAX,
} touch_pad_t;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
typedef enum {
    TOUCH_PAD_NUM0 = 0, /*!< Internal channel, be used for denoise */
#define TOUCH_DENOISE_CHANNEL TOUCH_PAD_NUM0 /*!< T0 is an internal channel that does not have a corresponding external GPIO. 
                                                  T0 will work simultaneously with the measured channel Tn. Finally, the actual 
                                                  measured value of Tn is the value after subtracting lower bits of T0. */
    TOUCH_PAD_NUM1,     /*!< Touch channel 1 is GPIO1  */
    TOUCH_PAD_NUM2,     /*!< Touch channel 2 is GPIO2  */
    TOUCH_PAD_NUM3,     /*!< Touch channel 3 is GPIO3  */
    TOUCH_PAD_NUM4,     /*!< Touch channel 4 is GPIO4  */
    TOUCH_PAD_NUM5,     /*!< Touch channel 5 is GPIO5  */
    TOUCH_PAD_NUM6,     /*!< Touch channel 6 is GPIO6  */
    TOUCH_PAD_NUM7,     /*!< Touch channel 7 is GPIO7  */
    TOUCH_PAD_NUM8,     /*!< Touch channel 8 is GPIO8  */
    TOUCH_PAD_NUM9,     /*!< Touch channel 9 is GPIO9  */
    TOUCH_PAD_NUM10,    /*!< Touch channel 9 is GPIO10 */
    TOUCH_PAD_NUM11,    /*!< Touch channel 9 is GPIO11 */
    TOUCH_PAD_NUM12,    /*!< Touch channel 9 is GPIO12 */
    TOUCH_PAD_NUM13,    /*!< Touch channel 9 is GPIO13 */
    TOUCH_PAD_NUM14,    /*!< Touch channel 9 is GPIO14 */
#define TOUCH_SHIELD_CHANNEL TOUCH_PAD_NUM14 /*!< The waterproof function includes a shielded channel (TOUCH_PAD_NUM14)
                                                  The shielded channel outputs the same signal as the channel being measured. 
                                                  It is generally designed as a grid and is placed around the touch buttons. */
    TOUCH_PAD_MAX,
} touch_pad_t;
#endif

typedef enum {
    TOUCH_HVOLT_KEEP = -1, /*!<Touch sensor high reference voltage, no change  */
    TOUCH_HVOLT_2V4 = 0,   /*!<Touch sensor high reference voltage, 2.4V  */
    TOUCH_HVOLT_2V5,       /*!<Touch sensor high reference voltage, 2.5V  */
    TOUCH_HVOLT_2V6,       /*!<Touch sensor high reference voltage, 2.6V  */
    TOUCH_HVOLT_2V7,       /*!<Touch sensor high reference voltage, 2.7V  */
    TOUCH_HVOLT_MAX,
} touch_high_volt_t;

typedef enum {
    TOUCH_LVOLT_KEEP = -1, /*!<Touch sensor low reference voltage, no change  */
    TOUCH_LVOLT_0V5 = 0,   /*!<Touch sensor low reference voltage, 0.5V  */
    TOUCH_LVOLT_0V6,       /*!<Touch sensor low reference voltage, 0.6V  */
    TOUCH_LVOLT_0V7,       /*!<Touch sensor low reference voltage, 0.7V  */
    TOUCH_LVOLT_0V8,       /*!<Touch sensor low reference voltage, 0.8V  */
    TOUCH_LVOLT_MAX,
} touch_low_volt_t;

typedef enum {
    TOUCH_HVOLT_ATTEN_KEEP = -1,  /*!<Touch sensor high reference voltage attenuation, no change  */
    TOUCH_HVOLT_ATTEN_1V5 = 0,    /*!<Touch sensor high reference voltage attenuation, 1.5V attenuation  */
    TOUCH_HVOLT_ATTEN_1V,         /*!<Touch sensor high reference voltage attenuation, 1.0V attenuation  */
    TOUCH_HVOLT_ATTEN_0V5,        /*!<Touch sensor high reference voltage attenuation, 0.5V attenuation  */
    TOUCH_HVOLT_ATTEN_0V,         /*!<Touch sensor high reference voltage attenuation,   0V attenuation  */
    TOUCH_HVOLT_ATTEN_MAX,
} touch_volt_atten_t;

typedef enum {
    TOUCH_PAD_SLOPE_0 = 0,       /*!<Touch sensor charge / discharge speed, always zero  */
    TOUCH_PAD_SLOPE_1 = 1,       /*!<Touch sensor charge / discharge speed, slowest  */
    TOUCH_PAD_SLOPE_2 = 2,       /*!<Touch sensor charge / discharge speed */
    TOUCH_PAD_SLOPE_3 = 3,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_4 = 4,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_5 = 5,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_6 = 6,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_7 = 7,       /*!<Touch sensor charge / discharge speed, fast  */
    TOUCH_PAD_SLOPE_MAX,
} touch_cnt_slope_t;

#if CONFIG_IDF_TARGET_ESP32
typedef enum {
    TOUCH_TRIGGER_BELOW = 0,   /*!<Touch interrupt will happen if counter value is less than threshold.*/
    TOUCH_TRIGGER_ABOVE = 1,   /*!<Touch interrupt will happen if counter value is larger than threshold.*/
    TOUCH_TRIGGER_MAX,
} touch_trigger_mode_t;

typedef enum {
    TOUCH_TRIGGER_SOURCE_BOTH = 0,  /*!< wakeup interrupt is generated if both SET1 and SET2 are "touched"*/
    TOUCH_TRIGGER_SOURCE_SET1 = 1,  /*!< wakeup interrupt is generated if SET1 is "touched"*/
    TOUCH_TRIGGER_SOURCE_MAX,
} touch_trigger_src_t;
#endif

typedef enum {
    TOUCH_PAD_TIE_OPT_LOW = 0,    /*!<Initial level of charging voltage, low level */
    TOUCH_PAD_TIE_OPT_HIGH = 1,   /*!<Initial level of charging voltage, high level */
    TOUCH_PAD_TIE_OPT_MAX,
} touch_tie_opt_t;

typedef enum {
    TOUCH_FSM_MODE_TIMER = 0,   /*!<To start touch FSM by timer */
    TOUCH_FSM_MODE_SW,          /*!<To start touch FSM by software trigger */
    TOUCH_FSM_MODE_MAX,
} touch_fsm_mode_t;

#if CONFIG_IDF_TARGET_ESP32S2BETA
typedef enum {
    TOUCH_PAD_INTR_DONE = 0,    /*!<Each enabled channel measure done */
    TOUCH_PAD_INTR_ACTIVE = 1,  /*!<Each enabled channel be touched */
    TOUCH_PAD_INTR_INACTIVE = 2,/*!<Each enabled channel be released */
    TOUCH_PAD_INTR_ALL,         /*!<All touch interrupt measure done & touched & released */
    TOUCH_PAD_INTR_MAX
} touch_pad_intr_type_t;

typedef enum {
    TOUCH_PAD_INTR_MASK_DONE = BIT(0),      /*!<Each enabled channel measure done */
    TOUCH_PAD_INTR_MASK_ACTIVE = BIT(1),    /*!<Each enabled channel be touched */
    TOUCH_PAD_INTR_MASK_INACTIVE = BIT(2),  /*!<Each enabled channel be released */
    TOUCH_PAD_INTR_MASK_ALL = BIT(2)|BIT(1)|BIT(0), /*!<All touch interrupt measure done & touched & released */
    TOUCH_PAD_INTR_MASK_MAX
} touch_pad_intr_mask_t;

typedef enum {
    TOUCH_PAD_DENOISE_BIT12 = 0,    /*!<Denoise range is 12bit */
    TOUCH_PAD_DENOISE_BIT10 = 1,    /*!<Denoise range is 10bit */
    TOUCH_PAD_DENOISE_BIT8 = 2,     /*!<Denoise range is 8bit */
    TOUCH_PAD_DENOISE_BIT4 = 3,     /*!<Denoise range is 4bit */
    TOUCH_PAD_DENOISE_MAX
} touch_pad_denoise_grade_t;

typedef enum {
    TOUCH_PAD_DENOISE_CAP_L0 = 0,   /*!<Denoise channel internal reference capacitance is 0pf */
    TOUCH_PAD_DENOISE_CAP_L1 = 4,   /*!<Denoise channel internal reference capacitance is 1.4pf */
    TOUCH_PAD_DENOISE_CAP_L2 = 2,   /*!<Denoise channel internal reference capacitance is 2.8pf */
    TOUCH_PAD_DENOISE_CAP_L3 = 6,   /*!<Denoise channel internal reference capacitance is 4.2pf */
    TOUCH_PAD_DENOISE_CAP_L4 = 1,   /*!<Denoise channel internal reference capacitance is 5.6pf */
    TOUCH_PAD_DENOISE_CAP_L5 = 5,   /*!<Denoise channel internal reference capacitance is 7.0pf */
    TOUCH_PAD_DENOISE_CAP_L6 = 3,   /*!<Denoise channel internal reference capacitance is 8.4pf */
    TOUCH_PAD_DENOISE_CAP_L7 = 7,   /*!<Denoise channel internal reference capacitance is 9.8pf */
    TOUCH_PAD_DENOISE_CAP_MAX
} touch_pad_denoise_cap_t;

typedef struct touch_pad_denoise {
    touch_pad_denoise_grade_t grade;    /*!<Select denoise channel denoise range.
                                            Determined by measuring the noise amplitude of the denoise channel. */
    touch_pad_denoise_cap_t cap_level;  /*!<Select denoise channel internal reference capacitance.
                                            Select the appropriate internal reference capacitance value so that 
                                            the reading of denoise channel is closest to the reading of the channel being measured. */
} touch_pad_denoise_t;

typedef enum {
    TOUCH_PAD_SHIELD_DRV_L0 = 0,/*!<The max equivalent capacitance in shield channel is 40pf */
    TOUCH_PAD_SHIELD_DRV_L1,    /*!<The max equivalent capacitance in shield channel is 80pf */
    TOUCH_PAD_SHIELD_DRV_L2,    /*!<The max equivalent capacitance in shield channel is 120pf */
    TOUCH_PAD_SHIELD_DRV_L3,    /*!<The max equivalent capacitance in shield channel is 160pf */
    TOUCH_PAD_SHIELD_DRV_L4,    /*!<The max equivalent capacitance in shield channel is 200pf */
    TOUCH_PAD_SHIELD_DRV_L5,    /*!<The max equivalent capacitance in shield channel is 240pf */
    TOUCH_PAD_SHIELD_DRV_L6,    /*!<The max equivalent capacitance in shield channel is 280pf */
    TOUCH_PAD_SHIELD_DRV_L7,    /*!<The max equivalent capacitance in shield channel is 320pf */
    TOUCH_PAD_SHIELD_DRV_MAX
} touch_pad_shield_driver_t;

typedef struct touch_pad_waterproof {
    touch_pad_t guard_ring_pad;             /*!<Waterproof. Select touch channel use for guard pad */
    touch_pad_shield_driver_t shield_driver;/*!<Waterproof. Select max equivalent capacitance for sheild pad
                                                Config the Touch14 to the touch sensor and compare the measured 
                                                reading to the Touch0 reading to estimate the equivalent capacitance.*/
} touch_pad_waterproof_t;

typedef struct touch_pad_proximity {
    touch_pad_t select_pad0;    /*!<Select touch channel use for proximity pad1 
                                    If clear the proximity channel, point this pad to `TOUCH_PAD_NUM0` */
    touch_pad_t select_pad1;    /*!<Select touch channel use for proximity pad1 */
    touch_pad_t select_pad2;    /*!<Select touch channel use for proximity pad1 */
    uint8_t meas_num;           /*!<Set cumulative number of measurements for proximity pad */
} touch_pad_proximity_t;

typedef enum {
    TOUCH_PAD_CONN_HIGHZ = 0,   /*!<Idel status of touch channel is high resistance state */
    TOUCH_PAD_CONN_GND = 1,     /*!<Idel status of touch channel is ground connection */
    TOUCH_PAD_CONN_MAX
} touch_pad_conn_type_t;

typedef enum {
    TOUCH_PAD_FILTER_IIR_2 = 0, /*!<The filter mode is first-order IIR filter. The coefficient is 2 */
    TOUCH_PAD_FILTER_IIR_4,     /*!<The filter mode is first-order IIR filter. The coefficient is 4 */
    TOUCH_PAD_FILTER_IIR_8,     /*!<The filter mode is first-order IIR filter. The coefficient is 8 */
    TOUCH_PAD_FILTER_JITTER,    /*!<The filter mode is jitter filter */
    TOUCH_PAD_FILTER_MAX
} touch_filter_mode_t;

typedef struct touch_filter_config {
    touch_filter_mode_t mode;   /*!<Set filter mode. The filtered value is baseline 
                                    The baseline is an important parameter of the touch algorithm */
    uint8_t debounce_cnt;       /*!<Set debounce count is n. If the measured values continue to exceed 
                                    the threshold for n times, it is determined that the touch sensor state changes. 
                                    Range: 0 ~ 7 */
    uint8_t hysteresis_thr;     /*!<Hysteresis threshold coefficient. hysteresis = hysteresis_thr * touch threshold.
                                    If (raw data - baseline) > (touch threshold + hysteresis), the touch channel be touched. 
                                    If (raw data - baseline) < (touch threshold - hysteresis), the touch channel be released.
                                    Range: 0 ~ 3. The coefficient is 0: 1/8;  1: 3/32;  2: 1/16;  3: 1/32 */
    uint8_t noise_thr;          /*!<Noise threshold coefficient. noise = noise_thr * touch threshold.
                                    If (raw data - baseline) > (noise), the baseline stop updating. 
                                    If (raw data - baseline) < (noise), the baseline start updating.
                                    Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8; */
    uint8_t noise_neg_thr;      /*!<Negative noise threshold coefficient. negative noise = noise_neg_thr * touch threshold.
                                    If (raw data - baseline) > (- negative noise), the baseline start updating. 
                                    If (raw data - baseline) < (- negative noise), the baseline stop updating.
                                    Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8; */
    uint8_t neg_noise_limit;    /*!<Set negative noise limit count is n. If the measured values continue to exceed 
                                    the negative noise threshold for n times, it is determined that the baseline reset to raw data. 
                                    Range: 0 ~ 15 */
    uint8_t jitter_step;        /*!<Set jitter filter step size. Range: 0 ~ 15 */
} touch_filter_config_t;

typedef struct {
    touch_pad_t touch_num;          /*!<touch pad index */
    uint32_t sleep_pad_threshold;   /*!<threshold in sleep mode */
    bool en_proximity;              /*!<enable proximity function for sleep pad */
} touch_pad_sleep_channel_t;

#define TOUCH_PAD_THRESHOLD_MAX 0x1FFFFF /*!<If set touch threshold max value, The touch sensor can't be in touched status */

#endif // CONFIG_IDF_TARGET_ESP32S2BETA

typedef intr_handle_t touch_isr_handle_t;

#if CONFIG_IDF_TARGET_ESP32

#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0x1000)  /*!<The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options), max value is 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (0x7fff)  /*!<The timer frequency is 8Mhz, the max value is 0x7fff */
#define TOUCH_PAD_MEASURE_WAIT_DEFAULT  (0xFF)    /*!<The timer frequency is 8Mhz, the max value is 0xff */
#define TOUCH_FSM_MODE_DEFAULT          (TOUCH_FSM_MODE_SW)  /*!<The touch FSM my be started by the software or timer */
#define TOUCH_TRIGGER_MODE_DEFAULT      (TOUCH_TRIGGER_BELOW)   /*!<Interrupts can be triggered if sensor value gets below or above threshold */
#define TOUCH_TRIGGER_SOURCE_DEFAULT    (TOUCH_TRIGGER_SOURCE_SET1)  /*!<The wakeup trigger source can be SET1 or both SET1 and SET2 */
#define TOUCH_PAD_BIT_MASK_MAX          (0x3ff)

#elif CONFIG_IDF_TARGET_ESP32S2BETA

#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0xf)   /*!<The number of sleep cycle in each measure process of touch channels.
                                                    The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options).
                                                    Range: 0 ~ 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (300)   /*!<The times of charge and discharge in each measure process of touch channels.
                                                    The timer frequency is 8Mhz. 
                                                    Range: 0 ~ 0xffff */
#define TOUCH_PAD_BIT_MASK_MAX          (0x7fff)/*! 15 Touch channels */

#endif // CONFIG_IDF_TARGET_ESP32

/**
 * @brief Initialize touch module.
 * @note  If default parameter don't match the usage scenario, it can be changed after this function.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_NO_MEM Touch pad init error
 */
esp_err_t touch_pad_init(void);

/**
 * @brief Un-install touch pad driver.
 * @note  After this function is called, other touch functions are prohibited from being called.
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Touch pad driver not initialized
 */
esp_err_t touch_pad_deinit(void);

/**
 * @brief Deregister the handler previously registered using touch_pad_isr_handler_register
 * @param fn  handler function to call (as passed to touch_pad_isr_handler_register)
 * @param arg  argument of the handler (as passed to touch_pad_isr_handler_register)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if a handler matching both fn and
 *        arg isn't registered
 */
esp_err_t touch_pad_isr_deregister(void(*fn)(void *), void *arg);

/**
 * @brief Get the touch pad which caused wakeup from sleep
 * @param pad_num pointer to touch pad which caused wakeup
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num);

/**
 * @brief Get the touch sensor status, usually used in ISR to decide which pads are 'touched'.
 * @return
 *      - touch status
 */
uint32_t touch_pad_get_status(void);

/**
 * @brief Set touch sensor FSM mode, the test action can be triggered by the timer,
 *        as well as by the software.
 * @param mode FSM mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode);

/**
 * @brief Get touch sensor FSM mode
 * @param mode pointer to accept FSM mode
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode);

#if CONFIG_IDF_TARGET_ESP32

/**
 * @brief Configure touch pad interrupt threshold.
 *
 * @note  If FSM mode is set to TOUCH_FSM_MODE_TIMER, this function will be blocked for one measurement cycle and wait for data to be valid.
 *
 * @param touch_num touch pad index
 * @param threshold interrupt threshold,
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG if argument wrong
 *     - ESP_FAIL if touch pad not initialized
 */
esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold);

/**
 * @brief get touch sensor counter value.
 *        Each touch sensor has a counter to count the number of charge/discharge cycles.
 *        When the pad is not 'touched', we can get a number of the counter.
 *        When the pad is 'touched', the value in counter will get smaller because of the larger equivalent capacitance.
 *
 * @note This API requests hardware measurement once. If IIR filter mode is enabled,
 *       please use 'touch_pad_read_raw_data' interface instead.
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad parameter error
 *     - ESP_ERR_INVALID_STATE This touch pad hardware connection is error, the value of "touch_value" is 0.
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t * touch_value);

/**
 * @brief get filtered touch sensor counter value by IIR filter.
 *
 * @note touch_pad_filter_start has to be called before calling touch_pad_read_filtered.
 *       This function can be called from ISR
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad parameter error
 *     - ESP_ERR_INVALID_STATE This touch pad hardware connection is error, the value of "touch_value" is 0.
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value);

/**
 * @brief get raw data (touch sensor counter value) from IIR filter process.
 *        Need not request hardware measurements.
 *
 * @note touch_pad_filter_start has to be called before calling touch_pad_read_raw_data.
 *       This function can be called from ISR
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad parameter error
 *     - ESP_ERR_INVALID_STATE This touch pad hardware connection is error, the value of "touch_value" is 0.
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint16_t *touch_value);

/**
  * @brief Callback function that is called after each IIR filter calculation.
  * @note This callback is called in timer task in each filtering cycle.
  * @note This callback should not be blocked.
  * @param raw_value  The latest raw data(touch sensor counter value) that
  *        points to all channels(raw_value[0..TOUCH_PAD_MAX-1]).
  * @param filtered_value  The latest IIR filtered data(calculated from raw data) that
  *        points to all channels(filtered_value[0..TOUCH_PAD_MAX-1]).
  *
  */
typedef void (* filter_cb_t)(uint16_t *raw_value, uint16_t *filtered_value);

/**
 * @brief Register the callback function that is called after each IIR filter calculation.
 * @note The 'read_cb' callback is called in timer task in each filtering cycle.
 * @param read_cb  Pointer to filtered callback function.
 *                 If the argument passed in is NULL, the callback will stop.
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG set error
 */
esp_err_t touch_pad_set_filter_read_cb(filter_cb_t read_cb);

/**
 * @brief   Register touch-pad ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 * @param fn  Pointer to ISR handler
 * @param arg  Parameter for ISR
 * @return
 *     - ESP_OK Success ;
 *     - ESP_ERR_INVALID_ARG GPIO error
 *     - ESP_ERR_NO_MEM No memory
 */
esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg);

/**
 * @brief Set touch sensor measurement and sleep time
 * @param sleep_cycle  The touch sensor will sleep after each measurement.
 *                     sleep_cycle decide the interval between each measurement.
 *                     t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                     The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_cycle The duration of the touch sensor measurement.
 *                   t_meas = meas_cycle / 8M, the maximum measure time is 0xffff / 8M = 8.19 ms
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle);

/**
 * @brief Get touch sensor measurement and sleep time
 * @param sleep_cycle  Pointer to accept sleep cycle number
 * @param meas_cycle Pointer to accept measurement cycle count.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle);

/**
 * @brief Set touch sensor reference voltage, if the voltage gap between high and low reference voltage get less,
 *        the charging and discharging time would be faster, accordingly, the counter value would be larger.
 *        In the case of detecting very slight change of capacitance, we can narrow down the gap so as to increase
 *        the sensitivity. On the other hand, narrow voltage gap would also introduce more noise, but we can use a
 *        software filter to pre-process the counter value.
 * @param refh the value of DREFH
 * @param refl the value of DREFL
 * @param atten the attenuation on DREFH
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten);

/**
 * @brief Get touch sensor reference voltage,
 * @param refh pointer to accept DREFH value
 * @param refl pointer to accept DREFL value
 * @param atten pointer to accept the attenuation on DREFH
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten);

/**
 * @brief Set touch sensor charge/discharge speed for each pad.
 *        If the slope is 0, the counter would always be zero.
 *        If the slope is 1, the charging and discharging would be slow, accordingly, the counter value would be small.
 *        If the slope is set 7, which is the maximum value, the charging and discharging would be fast, accordingly, the
 *        counter value would be larger.
 * @param touch_num touch pad index
 * @param slope touch pad charge/discharge speed
 * @param opt the initial voltage
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt);

/**
 * @brief Get touch sensor charge/discharge speed for each pad
 * @param touch_num touch pad index
 * @param slope pointer to accept touch pad charge/discharge slope
 * @param opt pointer to accept the initial voltage
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt);

/**
 * @brief Initialize touch pad GPIO
 * @param touch_num touch pad index
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_io_init(touch_pad_t touch_num);

/**
 * @brief Trigger a touch sensor measurement, only support in SW mode of FSM
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_sw_start(void);

/**
 * @brief Set touch sensor interrupt threshold
 * @param touch_num touch pad index
 * @param threshold threshold of touchpad count, refer to touch_pad_set_trigger_mode to see how to set trigger mode.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold);

/**
 * @brief Get touch sensor interrupt threshold
 * @param touch_num touch pad index
 * @param threshold pointer to accept threshold
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold);

/**
 * @brief Set touch sensor interrupt trigger mode.
 *        Interrupt can be triggered either when counter result is less than
 *        threshold or when counter result is more than threshold.
 * @param mode touch sensor interrupt trigger mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode);

/**
 * @brief Get touch sensor interrupt trigger mode
 * @param mode pointer to accept touch sensor interrupt trigger mode
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode);

/**
 * @brief Set touch sensor interrupt trigger source. There are two sets of touch signals.
 *        Set1 and set2 can be mapped to several touch signals. Either set will be triggered
 *        if at least one of its touch signal is 'touched'. The interrupt can be configured to be generated
 *        if set1 is triggered, or only if both sets are triggered.
 * @param src touch sensor interrupt trigger source
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src);

/**
 * @brief Get touch sensor interrupt trigger source
 * @param src pointer to accept touch sensor interrupt trigger source
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src);

/**
 * @brief Set touch sensor group mask.
 *        Touch pad module has two sets of signals, 'Touched' signal is triggered only if
 *        at least one of touch pad in this group is "touched".
 *        This function will set the register bits according to the given bitmask.
 * @param set1_mask bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask of touch sensor signal group2, it's a 10-bit value
 * @param en_mask bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask);

/**
 * @brief Get touch sensor group mask.
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 * @param en_mask pointer to accept bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask);

/**
 * @brief Clear touch sensor group mask.
 *        Touch pad module has two sets of signals, Interrupt is triggered only if
 *        at least one of touch pad in this group is "touched".
 *        This function will clear the register bits according to the given bitmask.
 * @param set1_mask bitmask touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask touch sensor signal group2, it's a 10-bit value
 * @param en_mask bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask);

/**
 * @brief To clear the touch status register, usually use this function in touch ISR to clear status.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_clear_status(void);

/**
 * @brief To enable touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_enable(void);

/**
 * @brief To disable touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_disable(void);

/**
 * @brief set touch pad filter calibration period, in ms.
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @param new_period_ms filter period, in ms
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 *      - ESP_ERR_INVALID_ARG parameter error
 */
esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms);

/**
 * @brief get touch pad filter calibration period, in ms
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @param p_period_ms pointer to accept period
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 *      - ESP_ERR_INVALID_ARG parameter error
 */
esp_err_t touch_pad_get_filter_period(uint32_t* p_period_ms);

/**
 * @brief start touch pad filter function
 *      This API will start a filter to process the noise in order to prevent false triggering
 *      when detecting slight change of capacitance.
 *      Need to call touch_pad_filter_start before all touch filter APIs
 *
 * @note This filter uses FreeRTOS timer, which is dispatched from a task with
 *       priority 1 by default on CPU 0. So if some application task with higher priority
 *       takes a lot of CPU0 time, then the quality of data obtained from this filter will be affected.
 *       You can adjust FreeRTOS timer task priority in menuconfig.
 * @param filter_period_ms filter calibration period, in ms
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG parameter error
 *      - ESP_ERR_NO_MEM No memory for driver
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_start(uint32_t filter_period_ms);

/**
 * @brief stop touch pad filter function
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_stop(void);

/**
 * @brief delete touch pad filter driver and release the memory
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_delete(void);

#elif CONFIG_IDF_TARGET_ESP32S2BETA

/**
 * @brief Set touch sensor FSM start
 * @note  Start FSM after the touch sensor FSM mode is set.
 * @note  Call this function will reset beseline of all touch channels.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_fsm_start(void);

/**
 * @brief Stop touch sensor FSM.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_fsm_stop(void);

/**
 * @brief Set touch sensor measurement and sleep time
 * @param sleep_cycle  The touch sensor will sleep after each measurement.
 *                     sleep_cycle decide the interval between each measurement.
 *                     t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                     The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_timers The times of charge and discharge in each measure process of touch channels.
 *                     The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_times);

/**
 * @brief Get touch sensor measurement and sleep time
 * @param sleep_cycle  Pointer to accept sleep cycle number
 * @param meas_times Pointer to accept measurement times count.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_times);

/**
 * @brief Set idel status of touch channel. The measurement of each touch channel is not in same time, 
 *        So, The value of touch channel can be effected by other's inactive status.
 *        The high resistance setting increases the sensitivity of adjacent touch channels. 
 *        The grounding setting increases the stability of adjacent touch channels.
 * @param type  Select connect to high resistance state or ground.
 *              Default: TOUCH_PAD_CONN_GND.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_inactive_connect(touch_pad_conn_type_t type);

/**
 * @brief Get idel status of touch channel. The measurement of each touch channel is not in same time, 
 *        So, The value of touch channel can be effected by other's inactive status.
 *        The high resistance setting increases the sensitivity of adjacent touch channels. 
 *        The grounding setting increases the stability of adjacent touch channels.
 * @param type  Pointer to connection type.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_inactive_connect(touch_pad_conn_type_t *type);

/**
 * @brief Set the trigger threshold of touch sensor.
 *        The threshold determines the sensitivity of the touch sensor.
 *        threshold < (touched raw data - released raw data).
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be trigered.
 * @param touch_num touch pad index
 * @param threshold threshold of touch sensor. Should be less than the max change value of touch.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint32_t threshold);

/**
 * @brief Get touch sensor trigger threshold
 * @param touch_num touch pad index
 * @param threshold pointer to accept threshold
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint32_t *threshold);

/**
 * @brief Register touch channel into touch sensor scan group.
 *        The working mode of the touch sensor is cyclically scanned.
 *        This function will set the scan bits according to the given bitmask.
 * @note  If set this mask, the FSM timer should be stop firsty.
 * @note  The touch sensor that in scan map, should be deinit GPIO function firstly by `touch_pad_io_init`.
 * @param enable_mask bitmask of touch sensor scan group. 
 *        e.g. TOUCH_PAD_NUM14 -> BIT(14)
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_group_mask(uint16_t enable_mask);

/**
 * @brief Get the touch sensor scan group bit mask.
 * @param enable_mask Pointer to bitmask of touch sensor scan group. 
 *        e.g. TOUCH_PAD_NUM14 -> BIT(14)
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_group_mask(uint16_t *enable_mask);

/**
 * @brief Clear touch channel from touch sensor scan group.
 *        The working mode of the touch sensor is cyclically scanned.
 *        This function will clear the scan bits according to the given bitmask.
 * @note  If clear all mask, the FSM timer should be stop firsty.
 * @param enable_mask bitmask of touch sensor scan group. 
 *        e.g. TOUCH_PAD_NUM14 -> BIT(14)
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_clear_group_mask(uint16_t enable_mask);

/**
 * @brief Configure parameter for each touch channel.
 * @note  Touch num 0 is denoise channel, please use `touch_pad_denoise_enable` to set denoise function
 * @param touch_num touch pad index
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG if argument wrong
 *     - ESP_FAIL if touch pad not initialized
 */
esp_err_t touch_pad_config(touch_pad_t touch_num);

/**
 * @brief Reset the whole of touch module.
 * @note  Call this funtion after `touch_pad_fsm_stop`,
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_reset(void);

/**
 * @brief Check touch sensor measurement status.
 *        If doing measurement, the flag will be clear.
 *        If finish measurement. the flag will be set.
 * @return
 *      - TRUE finish measurement
 *      - FALSE doing measurement
 */
bool touch_pad_meas_is_done(void);

/**
 * @brief Get the current scan channel. 
 *        usually used in ISR to decide channel scaning, and then, get the current measurement value.
 *        The role of each bit is reference to type `touch_pad_intr_mask_t`.
 * @return
 *      - touch channel number
 */
touch_pad_t touch_pad_get_scan_curr(void);

/**
 * @brief Get the touch sensor interrupt status mask. usually used in ISR to decide interrupt type.
 *        The role of each bit is reference to type `touch_pad_intr_mask_t`.
 * @return
 *      - touch intrrupt bit 
 */
uint32_t touch_pad_intr_status_get_mask(void);

/**
 * @brief Enable touch sensor interrupt.
 * @param type interrupt type
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_enable(touch_pad_intr_mask_t int_mask);

/**
 * @brief Disable touch sensor interrupt.
 * @param type interrupt type
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_disable(touch_pad_intr_mask_t int_mask);

/**
 * @brief   Register touch-pad ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 * @param fn  Pointer to ISR handler
 * @param arg  Parameter for ISR
 * @return
 *     - ESP_OK Success ;
 *     - ESP_ERR_INVALID_ARG GPIO error
 *     - ESP_ERR_NO_MEM No memory
 */
esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg, touch_pad_intr_mask_t intr_mask);

/**
 * @brief get raw data of touch sensor.
 * @note After the initialization is complete, the "raw_data" is max value. You need to wait for a measurement
 *       cycle before you can read the correct touch value.
 * @param touch_num touch pad index
 * @param raw_data pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Touch channel 0 havent this parameter.
 */

esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data);

/**
 * @brief get baseline of touch sensor.
 * @note After the initialization is complete, the "touch_value" is max value. You need to wait for a measurement
 *       cycle before you can read the correct touch value.
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch channel 0 havent this parameter.
 */
esp_err_t touch_pad_filter_baseline_read(touch_pad_t touch_num, uint32_t *basedata);

/**
 * @brief Reset baseline to raw data of touch sensor.
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset basaline of all channels
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_baseline_reset(touch_pad_t touch_num);

/**
 * @brief get debounce count of touch sensor.
 * @param touch_num touch pad index
 * @param debounce pointer to debounce value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch channel 0 havent this parameter.
 */
esp_err_t touch_pad_filter_debounce_read(touch_pad_t touch_num, uint32_t *debounce);

/**
 * @brief set parameter of touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @param filter_info select filter type and threshold of detection algorithm
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_set_config(touch_filter_config_t *filter_info);

/**
 * @brief get parameter of touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @param filter_info select filter type and threshold of detection algorithm
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_get_config(touch_filter_config_t *filter_info);

/**
 * @brief enable touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_enable(void);

/**
 * @brief diaable touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_disable(void);

/**
 * @brief set parameter of denoise pad (TOUCH_PAD_NUM0).
 *        T0 is an internal channel that does not have a corresponding external GPIO. 
 *        T0 will work simultaneously with the measured channel Tn. Finally, the actual 
 *        measured value of Tn is the value after subtracting lower bits of T0.
 *        This denoise function filters out interference introduced on all channels, 
 *        such as noise introduced by the power supply and external EMI.
 * @param denoise parameter of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_set_config(touch_pad_denoise_t denoise);

/**
 * @brief get parameter of denoise pad (TOUCH_PAD_NUM0).
 * @param denoise Pointer to parameter of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_get_config(touch_pad_denoise_t *denoise);

/**
 * @brief enable denoise function.
 *        T0 is an internal channel that does not have a corresponding external GPIO. 
 *        T0 will work simultaneously with the measured channel Tn. Finally, the actual 
 *        measured value of Tn is the value after subtracting lower bits of T0.
 *        This denoise function filters out interference introduced on all channels, 
 *        such as noise introduced by the power supply and external EMI.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_enable(void);

/**
 * @brief disable denoise function.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_disable(void);

/**
 * @brief get denoise measure value (TOUCH_PAD_NUM0).
 * @param denoise value of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_data_get(uint32_t *data);

/**
 * @brief set parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured. 
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel. 
 *        So that the guard channel can detect a large area of water.
 * @param waterproof parameter of waterproof
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_set_config(touch_pad_waterproof_t waterproof);

/**
 * @brief get parameter of waterproof function.
 * @param waterproof parameter of waterproof
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof);

/**
 * @brief Enable parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured. 
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel. 
 *        So that the guard channel can detect a large area of water.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_enable(void);

/**
 * @brief Enable parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured. 
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel. 
 *        So that the guard channel can detect a large area of water.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_disable(void);

/**
 * @brief Set parameter of proximity channel. Three proximity sensing channels can be set. 
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @note  If stop the proximity function for the channel, point this proximity channel to `TOUCH_PAD_NUM0`.
 * @param proximity parameter of proximity
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_proximity_set_config(touch_pad_proximity_t proximity);

/**
 * @brief Get parameter of proximity channel. Three proximity sensing channels can be set. 
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param proximity parameter of proximity
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_proximity_get_config(touch_pad_proximity_t *proximity);

/**
 * @brief Get measure count of proximity channel.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param touch_num touch pad index
 * @param proximity parameter of proximity
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_proximity_get_meas_cnt(touch_pad_t touch_num, uint32_t *cnt);

/**
 * @brief Get the accumulated measurement of the proximity sensor.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param touch_num touch pad index
 * @param measure_out If the accumulation process does not end, the `measure_out` is the process value.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_proximity_data_get(touch_pad_t touch_num, uint32_t *measure_out);

/**
 * @brief Set parameter of touch sensor in sleep mode. 
 *        In order to achieve low power consumption in sleep mode, other circuits except the RTC part of the register are in a power-off state.
 *        Only one touch channel is supported in the sleep state, which can be used as a wake-up function.
 *        If in non-sleep mode, the sleep parameters do not work.
 * @param slp_config touch pad config
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_config(touch_pad_sleep_channel_t slp_config);

/**
 * @brief get baseline of touch sensor in sleep mode.
 * @param baseline pointer to accept touch sensor baseline value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_baseline_get(uint32_t *baseline);

/**
 * @brief get debounce of touch sensor in sleep mode.
 * @param debounce pointer to accept touch sensor debounce value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_debounce_get(uint32_t *debounce);

/**
 * @brief get proximity count of touch sensor in sleep mode.
 * @param proximity_cnt pointer to accept touch sensor proximity count value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_proximity_cnt_get(uint32_t *proximity_cnt);

#endif // CONFIG_IDF_TARGET_ESP32S2BETA

#ifdef __cplusplus
}
#endif

#endif/*_DRIVER_TOUCH_PAD_H_*/

