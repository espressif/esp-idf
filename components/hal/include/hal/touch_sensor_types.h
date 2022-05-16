/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

/** Touch pad channel */
typedef enum {
    TOUCH_PAD_NUM0 = 0, /*!< Touch pad channel 0 is GPIO4(ESP32) */
    TOUCH_PAD_NUM1,     /*!< Touch pad channel 1 is GPIO0(ESP32) / GPIO1(ESP32-S2) */
    TOUCH_PAD_NUM2,     /*!< Touch pad channel 2 is GPIO2(ESP32) / GPIO2(ESP32-S2) */
    TOUCH_PAD_NUM3,     /*!< Touch pad channel 3 is GPIO15(ESP32) / GPIO3(ESP32-S2) */
    TOUCH_PAD_NUM4,     /*!< Touch pad channel 4 is GPIO13(ESP32) / GPIO4(ESP32-S2) */
    TOUCH_PAD_NUM5,     /*!< Touch pad channel 5 is GPIO12(ESP32) / GPIO5(ESP32-S2) */
    TOUCH_PAD_NUM6,     /*!< Touch pad channel 6 is GPIO14(ESP32) / GPIO6(ESP32-S2) */
    TOUCH_PAD_NUM7,     /*!< Touch pad channel 7 is GPIO27(ESP32) / GPIO7(ESP32-S2) */
    TOUCH_PAD_NUM8,     /*!< Touch pad channel 8 is GPIO33(ESP32) / GPIO8(ESP32-S2) */
    TOUCH_PAD_NUM9,     /*!< Touch pad channel 9 is GPIO32(ESP32) / GPIO9(ESP32-S2) */
#if SOC_TOUCH_SENSOR_NUM > 10
    TOUCH_PAD_NUM10,    /*!< Touch channel 10 is GPIO10(ESP32-S2) */
    TOUCH_PAD_NUM11,    /*!< Touch channel 11 is GPIO11(ESP32-S2) */
    TOUCH_PAD_NUM12,    /*!< Touch channel 12 is GPIO12(ESP32-S2) */
    TOUCH_PAD_NUM13,    /*!< Touch channel 13 is GPIO13(ESP32-S2) */
    TOUCH_PAD_NUM14,    /*!< Touch channel 14 is GPIO14(ESP32-S2) */
#endif
    TOUCH_PAD_MAX,
} touch_pad_t;

/** Touch sensor high reference voltage */
typedef enum {
    TOUCH_HVOLT_KEEP = -1, /*!<Touch sensor high reference voltage, no change  */
    TOUCH_HVOLT_2V4 = 0,   /*!<Touch sensor high reference voltage, 2.4V  */
    TOUCH_HVOLT_2V5,       /*!<Touch sensor high reference voltage, 2.5V  */
    TOUCH_HVOLT_2V6,       /*!<Touch sensor high reference voltage, 2.6V  */
    TOUCH_HVOLT_2V7,       /*!<Touch sensor high reference voltage, 2.7V  */
    TOUCH_HVOLT_MAX,
} touch_high_volt_t;

/** Touch sensor low reference voltage */
typedef enum {
    TOUCH_LVOLT_KEEP = -1, /*!<Touch sensor low reference voltage, no change  */
    TOUCH_LVOLT_0V5 = 0,   /*!<Touch sensor low reference voltage, 0.5V  */
    TOUCH_LVOLT_0V6,       /*!<Touch sensor low reference voltage, 0.6V  */
    TOUCH_LVOLT_0V7,       /*!<Touch sensor low reference voltage, 0.7V  */
    TOUCH_LVOLT_0V8,       /*!<Touch sensor low reference voltage, 0.8V  */
    TOUCH_LVOLT_MAX,
} touch_low_volt_t;

/** Touch sensor high reference voltage attenuation */
typedef enum {
    TOUCH_HVOLT_ATTEN_KEEP = -1,  /*!<Touch sensor high reference voltage attenuation, no change  */
    TOUCH_HVOLT_ATTEN_1V5 = 0,    /*!<Touch sensor high reference voltage attenuation, 1.5V attenuation  */
    TOUCH_HVOLT_ATTEN_1V,         /*!<Touch sensor high reference voltage attenuation, 1.0V attenuation  */
    TOUCH_HVOLT_ATTEN_0V5,        /*!<Touch sensor high reference voltage attenuation, 0.5V attenuation  */
    TOUCH_HVOLT_ATTEN_0V,         /*!<Touch sensor high reference voltage attenuation,   0V attenuation  */
    TOUCH_HVOLT_ATTEN_MAX,
} touch_volt_atten_t;

/** Touch sensor charge/discharge speed */
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

/** Touch sensor initial charge level */
typedef enum {
    TOUCH_PAD_TIE_OPT_LOW = 0,    /*!<Initial level of charging voltage, low level */
    TOUCH_PAD_TIE_OPT_HIGH = 1,   /*!<Initial level of charging voltage, high level */
    TOUCH_PAD_TIE_OPT_MAX,
} touch_tie_opt_t;

/** Touch sensor FSM mode */
typedef enum {
    TOUCH_FSM_MODE_TIMER = 0,   /*!<To start touch FSM by timer */
    TOUCH_FSM_MODE_SW,          /*!<To start touch FSM by software trigger */
    TOUCH_FSM_MODE_MAX,
} touch_fsm_mode_t;

/**** ESP32 Only *****/

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

/********************************/
#define TOUCH_PAD_BIT_MASK_ALL              ((1<<SOC_TOUCH_SENSOR_NUM)-1)
#define TOUCH_PAD_SLOPE_DEFAULT             (TOUCH_PAD_SLOPE_7)
#define TOUCH_PAD_TIE_OPT_DEFAULT           (TOUCH_PAD_TIE_OPT_LOW)
#define TOUCH_PAD_BIT_MASK_MAX              (TOUCH_PAD_BIT_MASK_ALL)
#define TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD    (TOUCH_HVOLT_2V7)
#define TOUCH_PAD_LOW_VOLTAGE_THRESHOLD     (TOUCH_LVOLT_0V5)
#define TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD   (TOUCH_HVOLT_ATTEN_0V5)
#define TOUCH_PAD_IDLE_CH_CONNECT_DEFAULT   (TOUCH_PAD_CONN_GND)
#define TOUCH_PAD_THRESHOLD_MAX             (SOC_TOUCH_PAD_THRESHOLD_MAX) /*!<If set touch threshold max value, The touch sensor can't be in touched status */

#ifdef CONFIG_IDF_TARGET_ESP32

#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0x1000)  /*!<The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options), max value is 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (0x7fff)  /*!<The timer frequency is 8Mhz, the max value is 0x7fff */
#define TOUCH_FSM_MODE_DEFAULT          (TOUCH_FSM_MODE_SW)  /*!<The touch FSM my be started by the software or timer */
#define TOUCH_TRIGGER_MODE_DEFAULT      (TOUCH_TRIGGER_BELOW)   /*!<Interrupts can be triggered if sensor value gets below or above threshold */
#define TOUCH_TRIGGER_SOURCE_DEFAULT    (TOUCH_TRIGGER_SOURCE_SET1)  /*!<The wakeup trigger source can be SET1 or both SET1 and SET2 */

#endif // CONFIG_IDF_TARGET ESP32

#if !CONFIG_IDF_TARGET_ESP32
/**
 * Excessive total time will slow down the touch response.
 * Too small measurement time will not be sampled enough, resulting in inaccurate measurements.
 *
 * @note The greater the duty cycle of the measurement time, the more system power is consumed.
 */
#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0xf)   /*!<The number of sleep cycle in each measure process of touch channels.
                                                    The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options).
                                                    Range: 0 ~ 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (500)   /*!<The times of charge and discharge in each measure process of touch channels.
                                                    The timer frequency is 8Mhz.
                                                    Recommended typical value: Modify this value to make the measurement time around 1ms.
                                                    Range: 0 ~ 0xffff */

typedef enum {
    TOUCH_PAD_INTR_MASK_DONE = BIT(0),      /*!<Measurement done for one of the enabled channels. */
    TOUCH_PAD_INTR_MASK_ACTIVE = BIT(1),    /*!<Active for one of the enabled channels. */
    TOUCH_PAD_INTR_MASK_INACTIVE = BIT(2),  /*!<Inactive for one of the enabled channels. */
    TOUCH_PAD_INTR_MASK_SCAN_DONE = BIT(3), /*!<Measurement done for all the enabled channels. */
    TOUCH_PAD_INTR_MASK_TIMEOUT = BIT(4),   /*!<Timeout for one of the enabled channels. */
#if SOC_TOUCH_PROXIMITY_MEAS_DONE_SUPPORTED
    TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE = BIT(5),   /*!<For proximity sensor, when the number of measurements reaches the set count of measurements, an interrupt will be generated. */
    TOUCH_PAD_INTR_MASK_MAX
#define TOUCH_PAD_INTR_MASK_ALL (TOUCH_PAD_INTR_MASK_TIMEOUT    \
                                | TOUCH_PAD_INTR_MASK_SCAN_DONE \
                                | TOUCH_PAD_INTR_MASK_INACTIVE  \
                                | TOUCH_PAD_INTR_MASK_ACTIVE    \
                                | TOUCH_PAD_INTR_MASK_DONE      \
                                | TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE) /*!<All touch interrupt type enable. */
#else
    TOUCH_PAD_INTR_MASK_MAX
#define TOUCH_PAD_INTR_MASK_ALL (TOUCH_PAD_INTR_MASK_TIMEOUT    \
                                | TOUCH_PAD_INTR_MASK_SCAN_DONE \
                                | TOUCH_PAD_INTR_MASK_INACTIVE  \
                                | TOUCH_PAD_INTR_MASK_ACTIVE    \
                                | TOUCH_PAD_INTR_MASK_DONE) /*!<All touch interrupt type enable. */
#endif
} touch_pad_intr_mask_t;
FLAG_ATTR(touch_pad_intr_mask_t)

typedef enum {
    TOUCH_PAD_DENOISE_BIT12 = 0,    /*!<Denoise range is 12bit */
    TOUCH_PAD_DENOISE_BIT10 = 1,    /*!<Denoise range is 10bit */
    TOUCH_PAD_DENOISE_BIT8 = 2,     /*!<Denoise range is 8bit */
    TOUCH_PAD_DENOISE_BIT4 = 3,     /*!<Denoise range is 4bit */
    TOUCH_PAD_DENOISE_MAX
} touch_pad_denoise_grade_t;

typedef enum {
    TOUCH_PAD_DENOISE_CAP_L0 = 0,   /*!<Denoise channel internal reference capacitance is 5pf */
    TOUCH_PAD_DENOISE_CAP_L1 = 1,   /*!<Denoise channel internal reference capacitance is 6.4pf */
    TOUCH_PAD_DENOISE_CAP_L2 = 2,   /*!<Denoise channel internal reference capacitance is 7.8pf */
    TOUCH_PAD_DENOISE_CAP_L3 = 3,   /*!<Denoise channel internal reference capacitance is 9.2pf */
    TOUCH_PAD_DENOISE_CAP_L4 = 4,   /*!<Denoise channel internal reference capacitance is 10.6pf */
    TOUCH_PAD_DENOISE_CAP_L5 = 5,   /*!<Denoise channel internal reference capacitance is 12.0pf */
    TOUCH_PAD_DENOISE_CAP_L6 = 6,   /*!<Denoise channel internal reference capacitance is 13.4pf */
    TOUCH_PAD_DENOISE_CAP_L7 = 7,   /*!<Denoise channel internal reference capacitance is 14.8pf */
    TOUCH_PAD_DENOISE_CAP_MAX = 8
} touch_pad_denoise_cap_t;

/** Touch sensor denoise configuration */
typedef struct touch_pad_denoise {
    touch_pad_denoise_grade_t grade;    /*!<Select denoise range of denoise channel.
                                            Determined by measuring the noise amplitude of the denoise channel. */
    touch_pad_denoise_cap_t cap_level;  /*!<Select internal reference capacitance of denoise channel.
                                            Ensure that the denoise readings are closest to the readings of the channel being measured.
                                            Use `touch_pad_denoise_read_data` to get the reading of denoise channel.
                                            The equivalent capacitance of the shielded channel can be calculated
                                            from the reading of denoise channel. */
} touch_pad_denoise_t;

/** Touch sensor shield channel drive capability level */
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

/** Touch sensor waterproof configuration */
typedef struct touch_pad_waterproof {
    touch_pad_t guard_ring_pad;             /*!<Waterproof. Select touch channel use for guard pad.
                                                Guard pad is used to detect the large area of water covering the touch panel. */
    touch_pad_shield_driver_t shield_driver;/*!<Waterproof. Shield channel drive capability configuration.
                                                Shield pad is used to shield the influence of water droplets covering the touch panel.
                                                When the waterproof function is enabled, Touch14 is set as shield channel by default.
                                                The larger the parasitic capacitance on the shielding channel, the higher the drive capability needs to be set.
                                                The equivalent capacitance of the shield channel can be estimated through the reading value of the denoise channel(Touch0).*/
} touch_pad_waterproof_t;

/** Touch sensor proximity detection configuration */
#define TOUCH_PROXIMITY_MEAS_NUM_MAX (0xFF)

/** Touch channel idle state configuration */
typedef enum {
    TOUCH_PAD_CONN_HIGHZ = 0,   /*!<Idle status of touch channel is high resistance state */
    TOUCH_PAD_CONN_GND = 1,     /*!<Idle status of touch channel is ground connection */
    TOUCH_PAD_CONN_MAX
} touch_pad_conn_type_t;

/**
 * @brief Touch channel IIR filter coefficient configuration.
 * @note On ESP32S2. There is an error in the IIR calculation. The magnitude of the error is twice the filter coefficient.
 *       So please select a smaller filter coefficient on the basis of meeting the filtering requirements.
 *       Recommended filter coefficient selection `IIR_16`.
 */
typedef enum {
    TOUCH_PAD_FILTER_IIR_4 = 0, /*!<The filter mode is first-order IIR filter. The coefficient is 4. */
    TOUCH_PAD_FILTER_IIR_8,     /*!<The filter mode is first-order IIR filter. The coefficient is 8. */
    TOUCH_PAD_FILTER_IIR_16,    /*!<The filter mode is first-order IIR filter. The coefficient is 16 (Typical value). */
    TOUCH_PAD_FILTER_IIR_32,    /*!<The filter mode is first-order IIR filter. The coefficient is 32. */
    TOUCH_PAD_FILTER_IIR_64,    /*!<The filter mode is first-order IIR filter. The coefficient is 64. */
    TOUCH_PAD_FILTER_IIR_128,   /*!<The filter mode is first-order IIR filter. The coefficient is 128. */
    TOUCH_PAD_FILTER_IIR_256,   /*!<The filter mode is first-order IIR filter. The coefficient is 256. */
    TOUCH_PAD_FILTER_JITTER,    /*!<The filter mode is jitter filter */
    TOUCH_PAD_FILTER_MAX
} touch_filter_mode_t;

/**
 * @brief Level of filter applied on the original data against large noise interference.
 * @note On ESP32S2. There is an error in the IIR calculation. The magnitude of the error is twice the filter coefficient.
 *       So please select a smaller filter coefficient on the basis of meeting the filtering requirements.
 *       Recommended filter coefficient selection `IIR_2`.
 */
typedef enum {
    TOUCH_PAD_SMOOTH_OFF   = 0, /*!<No filtering of raw data. */
    TOUCH_PAD_SMOOTH_IIR_2 = 1, /*!<Filter the raw data. The coefficient is 2 (Typical value). */
    TOUCH_PAD_SMOOTH_IIR_4 = 2, /*!<Filter the raw data. The coefficient is 4. */
    TOUCH_PAD_SMOOTH_IIR_8 = 3, /*!<Filter the raw data. The coefficient is 8. */
    TOUCH_PAD_SMOOTH_MAX,
} touch_smooth_mode_t;

/** Touch sensor filter configuration */
typedef struct touch_filter_config {
    touch_filter_mode_t mode;   /*!<Set filter mode. The input of the filter is the raw value of touch reading,
                                    and the output of the filter is involved in the judgment of the touch state. */
    uint32_t debounce_cnt;      /*!<Set debounce count, such as `n`. If the measured values continue to exceed
                                    the threshold for `n+1` times, the touch sensor state changes.
                                    Range: 0 ~ 7 */
    uint32_t noise_thr;         /*!<Noise threshold coefficient. Higher = More noise resistance.
                                    The actual noise should be less than (noise coefficient * touch threshold).
                                    Range: 0 ~ 3. The coefficient is 0: 4/8;  1: 3/8;   2: 2/8;   3: 1; */
    uint32_t jitter_step;       /*!<Set jitter filter step size. Range: 0 ~ 15 */
    touch_smooth_mode_t smh_lvl;/*!<Level of filter applied on the original data against large noise interference. */
#define TOUCH_DEBOUNCE_CNT_MAX      (7)
#define TOUCH_NOISE_THR_MAX         (3)
#define TOUCH_JITTER_STEP_MAX       (15)
} touch_filter_config_t;

/** Touch sensor channel sleep configuration */
typedef struct {
    touch_pad_t touch_num;          /*!<Set touch channel number for sleep pad.
                                        Only one touch sensor channel is supported in deep sleep mode.
                                        If clear the sleep channel, point this pad to `TOUCH_PAD_NUM0`  */
    bool en_proximity;              /*!<enable proximity function for sleep pad */
} touch_pad_sleep_channel_t;

#endif // !CONFIG_IDF_TARGET_ESP32
