/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <sys/param.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/ledc_periph.h"
#include "esp_clk_tree.h"
#include "soc/soc_caps.h"
#include "hal/ledc_hal.h"
#include "hal/gpio_hal.h"
#include "driver/ledc.h"
#include "esp_rom_gpio.h"
#include "clk_ctrl_os.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/gpio.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_memory_utils.h"

static __attribute__((unused)) const char *LEDC_TAG = "ledc";

#define LEDC_CHECK(a, str, ret_val) ESP_RETURN_ON_FALSE(a, ret_val, LEDC_TAG, "%s", str)
#define LEDC_ARG_CHECK(a, param) ESP_RETURN_ON_FALSE(a, ESP_ERR_INVALID_ARG, LEDC_TAG, param " argument is invalid")
#define LEDC_CHECK_ISR(a, str, ret_val) ESP_RETURN_ON_FALSE_ISR(a, ret_val, LEDC_TAG, "%s", str)
#define LEDC_ARG_CHECK_ISR(a, param) ESP_RETURN_ON_FALSE_ISR(a, ESP_ERR_INVALID_ARG, LEDC_TAG, param " argument is invalid")

#define LEDC_CLK_NOT_FOUND  0
#define LEDC_SLOW_CLK_UNINIT -1
#define LEDC_TIMER_SPECIFIC_CLK_UNINIT -1

// Precision degree only affects RC_FAST, other clock sources' frequencies are fixed values
// For targets that do not support RC_FAST calibration, can only use its approx. value. Precision degree other than
// APPROX will trigger LOGW during the call to `esp_clk_tree_src_get_freq_hz`.
#if SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
#define LEDC_CLK_SRC_FREQ_PRECISION     ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED
#else
#define LEDC_CLK_SRC_FREQ_PRECISION     ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define LEDC_BUS_CLOCK_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define LEDC_BUS_CLOCK_ATOMIC()
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
#define LEDC_FUNC_CLOCK_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define LEDC_FUNC_CLOCK_ATOMIC()
#endif

typedef enum {
    LEDC_FSM_IDLE,
    LEDC_FSM_HW_FADE,
    LEDC_FSM_ISR_CAL,
    LEDC_FSM_KILLED_PENDING,
} ledc_fade_fsm_t;

typedef struct {
    ledc_mode_t speed_mode;
    ledc_duty_direction_t direction;
    uint32_t target_duty;
    int cycle_num;
    int scale;
    ledc_fade_mode_t mode;
    SemaphoreHandle_t ledc_fade_sem;
    SemaphoreHandle_t ledc_fade_mux;
    ledc_cb_t ledc_fade_callback;
    void *cb_user_arg;
    volatile ledc_fade_fsm_t fsm;
} ledc_fade_t;

typedef struct {
    ledc_hal_context_t ledc_hal;                        /*!< LEDC hal context */
    ledc_slow_clk_sel_t glb_clk;                        /*!< LEDC global clock selection */
    bool timer_is_stopped[LEDC_TIMER_MAX];              /*!< Indicates whether each timer has been stopped */
    bool glb_clk_is_acquired[LEDC_TIMER_MAX];           /*!< Tracks whether the global clock is being acquired by each timer */
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    ledc_clk_src_t timer_specific_clk[LEDC_TIMER_MAX];  /*!< Tracks the timer-specific clock selection for each timer */
#endif
} ledc_obj_t;

static ledc_obj_t *p_ledc_obj[LEDC_SPEED_MODE_MAX] = {
    [0 ... LEDC_SPEED_MODE_MAX - 1] = NULL,
};
static ledc_fade_t *s_ledc_fade_rec[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];
static ledc_isr_handle_t s_ledc_fade_isr_handle = NULL;
static portMUX_TYPE ledc_spinlock = portMUX_INITIALIZER_UNLOCKED;
static _lock_t s_ledc_mutex[LEDC_SPEED_MODE_MAX];

#define LEDC_VAL_NO_CHANGE        (-1)
#define LEDC_DUTY_NUM_MAX         LEDC_LL_DUTY_NUM_MAX            // Maximum steps per hardware fade
#define LEDC_DUTY_DECIMAL_BIT_NUM (4)
#define LEDC_TIMER_DIV_NUM_MAX    (0x3FFFF)
#define LEDC_FADE_TOO_SLOW_STR    "LEDC FADE TOO SLOW"
#define LEDC_FADE_TOO_FAST_STR    "LEDC FADE TOO FAST"
#define DIM(array)                (sizeof(array)/sizeof(*array))
#define LEDC_IS_DIV_INVALID(div)  ((div) <= LEDC_LL_FRACTIONAL_MAX || (div) > LEDC_TIMER_DIV_NUM_MAX)

static __attribute__((unused)) const char *LEDC_NOT_INIT = "LEDC is not initialized";
static __attribute__((unused)) const char *LEDC_FADE_SERVICE_ERR_STR = "LEDC fade service not installed";
static __attribute__((unused)) const char *LEDC_FADE_INIT_ERROR_STR = "LEDC fade channel init error, not enough memory or service not installed";

//This value will be calibrated when in use.
static uint32_t s_ledc_slow_clk_rc_fast_freq = 0;

static const ledc_slow_clk_sel_t s_glb_clks[] = LEDC_LL_GLOBAL_CLOCKS;
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
static const ledc_clk_src_t s_timer_specific_clks[] = LEDC_LL_TIMER_SPECIFIC_CLOCKS;
#endif

static void ledc_ls_timer_update(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        ledc_hal_ls_timer_update(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    }
}

static IRAM_ATTR void ledc_ls_channel_update(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        ledc_hal_ls_channel_update(&(p_ledc_obj[speed_mode]->ledc_hal), channel);
    }
}

//We know that RC_FAST is about 8M/20M, but don't know the actual value. So we need to do a calibration.
static bool ledc_slow_clk_calibrate(void)
{
    if (periph_rtc_dig_clk8m_enable()) {
        s_ledc_slow_clk_rc_fast_freq = periph_rtc_dig_clk8m_get_freq();
#if !SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
        /* Workaround: RC_FAST calibration cannot be performed, we can only use its theoretic freq */
        ESP_LOGD(LEDC_TAG, "Calibration cannot be performed, approximate RC_FAST_CLK : %"PRIu32" Hz", s_ledc_slow_clk_rc_fast_freq);
#else
        ESP_LOGD(LEDC_TAG, "Calibrate RC_FAST_CLK : %"PRIu32" Hz", s_ledc_slow_clk_rc_fast_freq);
#endif
        return true;
    }
    ESP_LOGE(LEDC_TAG, "Calibrate RC_FAST_CLK failed");
    return false;
}

static esp_err_t ledc_enable_intr_type(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_intr_type_t type)
{
    if (type == LEDC_INTR_FADE_END) {
        ledc_hal_set_fade_end_intr(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
    } else {
        ledc_hal_set_fade_end_intr(&(p_ledc_obj[speed_mode]->ledc_hal), channel, false);
    }
    return ESP_OK;
}

static void _ledc_fade_hw_acquire(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreTake(fade->ledc_fade_sem, portMAX_DELAY);
        portENTER_CRITICAL(&ledc_spinlock);
        ledc_enable_intr_type(mode, channel, LEDC_INTR_DISABLE);
        portEXIT_CRITICAL(&ledc_spinlock);
    }
}

static void _ledc_fade_hw_release(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreGive(fade->ledc_fade_sem);
    }
}

static void _ledc_op_lock_acquire(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreTake(fade->ledc_fade_mux, portMAX_DELAY);
    }
}

static void _ledc_op_lock_release(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreGive(fade->ledc_fade_mux);
    }
}

static uint32_t ledc_get_max_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    // The arguments are checked before internally calling this function.
    ledc_timer_t timer_sel;
    ledc_hal_get_channel_timer(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &timer_sel);
    uint32_t max_duty;
    ledc_hal_get_max_duty(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, &max_duty);
    return max_duty;
}

esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider, uint32_t duty_resolution,
                         ledc_clk_src_t clk_src)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_set_clock_divider(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, clock_divider);
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    /* Clock source can only be configured on targets which support timer-specific source clock. */
    ledc_hal_set_clock_source(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, clk_src);
    // TODO: acquire clk_src, and release old clk_src if initialized and different than new one [clk_tree]
    p_ledc_obj[speed_mode]->timer_specific_clk[timer_sel] = clk_src;
#endif
    ledc_hal_set_duty_resolution(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, duty_resolution);
    ledc_ls_timer_update(speed_mode, timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t ledc_duty_config(ledc_mode_t speed_mode, ledc_channel_t channel, int hpoint_val,
                                            int duty_val, ledc_duty_direction_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    if (hpoint_val >= 0) {
        ledc_hal_set_hpoint(&(p_ledc_obj[speed_mode]->ledc_hal), channel, hpoint_val);
    }
    if (duty_val >= 0) {
        ledc_hal_set_duty_int_part(&(p_ledc_obj[speed_mode]->ledc_hal), channel, duty_val);
    }
    ledc_hal_set_fade_param(&(p_ledc_obj[speed_mode]->ledc_hal), channel, 0, duty_direction, duty_cycle, duty_scale, duty_num);
#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
    ledc_hal_set_range_number(&(p_ledc_obj[speed_mode]->ledc_hal), channel, 1);
#endif
    return ESP_OK;
}

esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_bind_channel_timer(&(p_ledc_obj[speed_mode]->ledc_hal), channel, timer_sel);
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_timer_rst(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    p_ledc_obj[speed_mode]->timer_is_stopped[timer_sel] = true;
    ledc_hal_timer_pause(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    p_ledc_obj[speed_mode]->timer_is_stopped[timer_sel] = false;
    ledc_hal_timer_resume(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, ledc_isr_handle_t *handle)
{
    esp_err_t ret;
    LEDC_ARG_CHECK(fn, "fn");
    portENTER_CRITICAL(&ledc_spinlock);
    ret = esp_intr_alloc(ETS_LEDC_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

static bool ledc_speed_mode_ctx_create(ledc_mode_t speed_mode)
{
    bool new_ctx = false;

    // Prevent p_ledc_obj malloc concurrently
    _lock_acquire(&s_ledc_mutex[speed_mode]);
    if (!p_ledc_obj[speed_mode]) {
        ledc_obj_t *ledc_new_mode_obj = (ledc_obj_t *) heap_caps_calloc(1, sizeof(ledc_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (ledc_new_mode_obj) {
            new_ctx = true;
            LEDC_BUS_CLOCK_ATOMIC() {
                ledc_ll_enable_bus_clock(true);
                ledc_ll_enable_reset_reg(false);
            }
            ledc_hal_init(&(ledc_new_mode_obj->ledc_hal), speed_mode);
            ledc_new_mode_obj->glb_clk = LEDC_SLOW_CLK_UNINIT;
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
            memset(ledc_new_mode_obj->timer_specific_clk, LEDC_TIMER_SPECIFIC_CLK_UNINIT, sizeof(ledc_clk_src_t) * LEDC_TIMER_MAX);
#endif
            p_ledc_obj[speed_mode] = ledc_new_mode_obj;
        }
    }
    _lock_release(&s_ledc_mutex[speed_mode]);

    return new_ctx;
}

static inline uint32_t ledc_calculate_divisor(uint32_t src_clk_freq, int freq_hz, uint32_t precision)
{
    /**
     * In order to find the right divisor, we need to divide the source clock
     * frequency by the desired frequency. However, two things to note here:
     * - The lowest LEDC_LL_FRACTIONAL_BITS bits of the result are the FRACTIONAL
     *   part. The higher bits represent the integer part, this is why we need
     *   to right shift the source frequency.
     * - The `precision` parameter represents the granularity of the clock. It
     *   **must** be a power of 2. It means that the resulted divisor is
     *   a multiplier of `precision`.
     *
     * Let's take a concrete example, we need to generate a 5KHz clock out of
     * a 80MHz clock (APB).
     * If the precision is 1024 (10 bits), the resulted multiplier is:
     * (80000000 << 8) / (5000 * 1024) = 4000 (0xfa0)
     * Let's ignore the fractional part to simplify the explanation, so we get
     * a result of 15 (0xf).
     * This can be interpreted as: every 15 "precision" ticks, the resulted
     * clock will go high, where one precision tick is made out of 1024 source
     * clock ticks.
     * Thus, every `15 * 1024` source clock ticks, the resulted clock will go
     * high.
     *
     * NOTE: We are also going to round up the value when necessary, thanks to:
     * (freq_hz * precision / 2)
     */
    return (((uint64_t) src_clk_freq << LEDC_LL_FRACTIONAL_BITS) + freq_hz * precision / 2)
           / (freq_hz * precision);
}

static inline uint32_t ledc_auto_global_clk_divisor(int freq_hz, uint32_t precision, ledc_slow_clk_sel_t *clk_target)
{
    uint32_t ret = LEDC_CLK_NOT_FOUND;
    uint32_t clk_freq = 0;

    /* This function will go through all the following clock sources to look
     * for a valid divisor which generates the requested frequency. */
    for (int i = 0; i < DIM(s_glb_clks); i++) {
        /* Before calculating the divisor, we need to have the RC_FAST frequency.
         * If it hasn't been measured yet, try calibrating it now. */
        if (s_glb_clks[i] == LEDC_SLOW_CLK_RC_FAST && s_ledc_slow_clk_rc_fast_freq == 0 && !ledc_slow_clk_calibrate()) {
            ESP_LOGD(LEDC_TAG, "Unable to retrieve RC_FAST clock frequency, skipping it");
            continue;
        }

        esp_clk_tree_src_get_freq_hz((soc_module_clk_t)s_glb_clks[i], LEDC_CLK_SRC_FREQ_PRECISION, &clk_freq);
        uint32_t div_param = ledc_calculate_divisor(clk_freq, freq_hz, precision);

        /* If the divisor is valid, we can return this value. */
        if (!LEDC_IS_DIV_INVALID(div_param)) {
            *clk_target = s_glb_clks[i];
            ret = div_param;
            break;
        }
    }

    return ret;
}

#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
static inline uint32_t ledc_auto_timer_specific_clk_divisor(ledc_mode_t speed_mode, int freq_hz, uint32_t precision,
                                                            ledc_clk_src_t *clk_source)
{
    uint32_t ret = LEDC_CLK_NOT_FOUND;
    uint32_t clk_freq = 0;

    for (int i = 0; i < DIM(s_timer_specific_clks); i++) {
        esp_clk_tree_src_get_freq_hz((soc_module_clk_t)s_timer_specific_clks[i], LEDC_CLK_SRC_FREQ_PRECISION, &clk_freq);
        uint32_t div_param = ledc_calculate_divisor(clk_freq, freq_hz, precision);

        /* If the divisor is valid, we can return this value. */
        if (!LEDC_IS_DIV_INVALID(div_param)) {
            *clk_source = s_timer_specific_clks[i];
            ret = div_param;
            break;
        }
    }

#if SOC_LEDC_SUPPORT_HS_MODE
    /* On board that support LEDC high-speed mode, APB clock becomes a timer-
     * specific clock when in high speed mode. Check if it is necessary here
     * to test APB. */
    if (speed_mode == LEDC_HIGH_SPEED_MODE && ret == LEDC_CLK_NOT_FOUND) {
        /* No divider was found yet, try with APB! */
        esp_clk_tree_src_get_freq_hz((soc_module_clk_t)LEDC_APB_CLK, LEDC_CLK_SRC_FREQ_PRECISION, &clk_freq);
        uint32_t div_param = ledc_calculate_divisor(clk_freq, freq_hz, precision);

        if (!LEDC_IS_DIV_INVALID(div_param)) {
            *clk_source = LEDC_APB_CLK;
            ret = div_param;
        }
    }
#endif

    return ret;
}
#endif

/**
 * @brief Try to find the clock with its divisor giving the frequency requested
 * by the caller.
 */
static uint32_t ledc_auto_clk_divisor(ledc_mode_t speed_mode, int freq_hz, uint32_t precision,
                                      ledc_clk_src_t *clk_source, ledc_slow_clk_sel_t *clk_target)
{
    uint32_t ret = LEDC_CLK_NOT_FOUND;

#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    /* If the SoC presents timer-specific clock(s), try to achieve the given frequency
     * thanks to it/them.
     * clk_source parameter will returned by this function. */
    uint32_t div_param_timer = ledc_auto_timer_specific_clk_divisor(speed_mode, freq_hz, precision, clk_source);

    if (div_param_timer != LEDC_CLK_NOT_FOUND) {
        /* The divider is valid, no need try any other clock, return directly. */
        ret = div_param_timer;
    }
#endif

    /* On ESP32, only low speed channel can use the global clocks. For other
     * chips, there are no high speed channels. */
    if (ret == LEDC_CLK_NOT_FOUND && speed_mode == LEDC_LOW_SPEED_MODE) {
        uint32_t div_param_global = ledc_auto_global_clk_divisor(freq_hz, precision, clk_target);
        if (div_param_global != LEDC_CLK_NOT_FOUND) {
            *clk_source = LEDC_SCLK;
            ret = div_param_global;
        }
    }

    return ret;
}

/**
 * @brief Function setting the LEDC timer divisor with the given source clock,
 * frequency and resolution. If the clock configuration passed is
 * LEDC_AUTO_CLK, the clock will be determined automatically (if possible).
 */
static esp_err_t ledc_set_timer_div(ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_clk_cfg_t clk_cfg, int freq_hz, int duty_resolution)
{
    uint32_t div_param = 0;
    const uint32_t precision = (0x1 << duty_resolution);
    /* The clock sources are not initialized on purpose. To produce compiler warning if used but the selector functions
     * don't set them properly. */
    /* Timer-specific mux. Set to timer-specific clock or LEDC_SCLK if a global clock is used. */
    ledc_clk_src_t timer_clk_src;
    /* Global clock mux. Should be set when LEDC_SCLK is used in LOW_SPEED_MODE. Otherwise left uninitialized. */
    ledc_slow_clk_sel_t glb_clk = LEDC_SLOW_CLK_UNINIT;

    if (clk_cfg == LEDC_AUTO_CLK) {
        /* User hasn't specified the speed, we should try to guess it. */
        div_param = ledc_auto_clk_divisor(speed_mode, freq_hz, precision, &timer_clk_src, &glb_clk);
    } else if (clk_cfg == LEDC_USE_RC_FAST_CLK) {
        /* User specified source clock(RC_FAST_CLK) for low speed channel.
         * Make sure the speed mode is correct. */
        ESP_RETURN_ON_FALSE((speed_mode == LEDC_LOW_SPEED_MODE), ESP_ERR_INVALID_ARG, LEDC_TAG, "RC_FAST clock can only be used in low speed mode");

        /* Before calculating the divisor, we need to have the RC_FAST frequency.
         * If it hasn't been measured yet, try calibrating it now. */
        if (s_ledc_slow_clk_rc_fast_freq == 0 && ledc_slow_clk_calibrate() == false) {
            goto error;
        }

        /* Set the global clock source */
        timer_clk_src = LEDC_SCLK;
        glb_clk =  LEDC_SLOW_CLK_RC_FAST;

        /* We have the RC_FAST clock frequency now. */
        div_param = ledc_calculate_divisor(s_ledc_slow_clk_rc_fast_freq, freq_hz, precision);
        if (LEDC_IS_DIV_INVALID(div_param)) {
            div_param = LEDC_CLK_NOT_FOUND;
        }
    } else {
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
        if (LEDC_LL_IS_TIMER_SPECIFIC_CLOCK(speed_mode, clk_cfg)) {
            /* Currently we can convert a timer-specific clock to a source clock that
             * easily because their values are identical in the enumerations (on purpose)
             * If we decide to change the values in the future, we should consider defining
             * a macro/function to convert timer-specific clock to clock source .*/
            timer_clk_src = (ledc_clk_src_t) clk_cfg;
        } else
#endif
        {
            timer_clk_src = LEDC_SCLK;
#if SOC_LEDC_SUPPORT_REF_TICK
            assert(clk_cfg != LEDC_USE_REF_TICK); // REF_TICK is NOT a global clock, it is a timer-specific clock
#endif
            glb_clk = (ledc_slow_clk_sel_t)clk_cfg;
        }

        uint32_t src_clk_freq = 0;
        esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_cfg, LEDC_CLK_SRC_FREQ_PRECISION, &src_clk_freq);
        div_param = ledc_calculate_divisor(src_clk_freq, freq_hz, precision);
        if (LEDC_IS_DIV_INVALID(div_param)) {
            div_param = LEDC_CLK_NOT_FOUND;
        }
    }

    if (div_param == LEDC_CLK_NOT_FOUND) {
        goto error;
    }

    /* The following debug message makes more sense for AUTO mode. */
    ESP_LOGD(LEDC_TAG, "Using clock source %d (in %s mode), divisor: 0x%"PRIx32,
             timer_clk_src, (speed_mode == LEDC_LOW_SPEED_MODE ? "slow" : "fast"), div_param);

    /* The following block configures the global clock.
     * Thus, in theory, this only makes sense when configuring the LOW_SPEED timer and the source clock is LEDC_SCLK (as
     * HIGH_SPEED timers won't be clocked by the global clock). However, there are some limitations due to HW design.
     */
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
        /* On ESP32 and ESP32-S2, when the source clock of LOW_SPEED timer is a timer-specific one (i.e. REF_TICK), the
         * global clock MUST be set to APB_CLK. For HIGH_SPEED timers, this is not necessary.
         */
        if (timer_clk_src != LEDC_SCLK) {
            glb_clk = LEDC_SLOW_CLK_APB;
        }
#else
        /* On later chips, there is only one type of timer/channel (referred as LOW_SPEED in the code), which can only be
         * clocked by the global clock. So there's no limitation on the global clock, except that it must be set.
         */
        assert(timer_clk_src == LEDC_SCLK);
#endif
        // Arriving here, variable glb_clk must have been assigned to one of the ledc_slow_clk_sel_t enum values
        assert(glb_clk != LEDC_SLOW_CLK_UNINIT);

        portENTER_CRITICAL(&ledc_spinlock);
        if (p_ledc_obj[speed_mode]->glb_clk != LEDC_SLOW_CLK_UNINIT && p_ledc_obj[speed_mode]->glb_clk != glb_clk) {
            for (int i = 0; i < LEDC_TIMER_MAX; i++) {
                if (i != timer_num && p_ledc_obj[speed_mode]->glb_clk_is_acquired[i]) {
                    portEXIT_CRITICAL(&ledc_spinlock);
                    ESP_RETURN_ON_FALSE(false, ESP_FAIL, LEDC_TAG,
                                        "timer clock conflict, already is %d but attempt to %d", p_ledc_obj[speed_mode]->glb_clk, glb_clk);
                }
            }
        }
        p_ledc_obj[speed_mode]->glb_clk_is_acquired[timer_num] = true;
        if (p_ledc_obj[speed_mode]->glb_clk != glb_clk) {
            // TODO: release old glb_clk (if not UNINIT), and acquire new glb_clk [clk_tree]
            p_ledc_obj[speed_mode]->glb_clk = glb_clk;
            LEDC_FUNC_CLOCK_ATOMIC() {
                ledc_ll_enable_clock(p_ledc_obj[speed_mode]->ledc_hal.dev, true);
                ledc_hal_set_slow_clk_sel(&(p_ledc_obj[speed_mode]->ledc_hal), glb_clk);
            }
        }
        portEXIT_CRITICAL(&ledc_spinlock);

        ESP_LOGD(LEDC_TAG, "In slow speed mode, global clk set: %d", glb_clk);

        /* keep ESP_PD_DOMAIN_RC_FAST on during light sleep */
#if SOC_LIGHT_SLEEP_SUPPORTED
        extern void esp_sleep_periph_use_8m(bool use_or_not);
        esp_sleep_periph_use_8m(glb_clk == LEDC_SLOW_CLK_RC_FAST);
#endif
    }

    /* The divisor is correct, we can write in the hardware. */
    ledc_timer_set(speed_mode, timer_num, div_param, duty_resolution, timer_clk_src);
    return ESP_OK;

error:
    ESP_LOGE(LEDC_TAG, "requested frequency %d and duty resolution %d can not be achieved, try reducing freq_hz or duty_resolution. div_param=%"PRIu32, freq_hz, duty_resolution, div_param);
    return ESP_FAIL;
}

static esp_err_t ledc_timer_del(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    bool is_configured = true;
    bool is_deleted = false;
    portENTER_CRITICAL(&ledc_spinlock);
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    if (p_ledc_obj[speed_mode]->glb_clk_is_acquired[timer_sel] == false && p_ledc_obj[speed_mode]->timer_specific_clk[timer_sel] == LEDC_TIMER_SPECIFIC_CLK_UNINIT)
#else
    if (p_ledc_obj[speed_mode]->glb_clk_is_acquired[timer_sel] == false)
#endif
    {
        is_configured = false;
    } else if (p_ledc_obj[speed_mode]->timer_is_stopped[timer_sel] == true) {
        is_deleted = true;
        p_ledc_obj[speed_mode]->glb_clk_is_acquired[timer_sel] = false;
        // TODO: release timer specific clk and global clk if possible [clk_tree]
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    ESP_RETURN_ON_FALSE(is_configured && is_deleted, ESP_ERR_INVALID_STATE, LEDC_TAG, "timer hasn't been configured, or it is still running, please stop it with ledc_timer_pause first");
    return ESP_OK;
}

esp_err_t ledc_timer_config(const ledc_timer_config_t *timer_conf)
{
    LEDC_ARG_CHECK(timer_conf != NULL, "timer_conf");
    uint32_t freq_hz = timer_conf->freq_hz;
    uint32_t duty_resolution = timer_conf->duty_resolution;
    uint32_t timer_num = timer_conf->timer_num;
    uint32_t speed_mode = timer_conf->speed_mode;
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_num < LEDC_TIMER_MAX, "timer_num");
    if (timer_conf->deconfigure) {
        return ledc_timer_del(speed_mode, timer_num);
    }
    LEDC_ARG_CHECK(!((timer_conf->clk_cfg == LEDC_USE_RC_FAST_CLK) && (speed_mode != LEDC_LOW_SPEED_MODE)), "Only low speed channel support RC_FAST_CLK");
    if (freq_hz == 0 || duty_resolution == 0 || duty_resolution >= LEDC_TIMER_BIT_MAX) {
        ESP_LOGE(LEDC_TAG, "freq_hz=%"PRIu32" duty_resolution=%"PRIu32, freq_hz, duty_resolution);
        return ESP_ERR_INVALID_ARG;
    }

    if (!ledc_speed_mode_ctx_create(speed_mode) && !p_ledc_obj[speed_mode]) {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = ledc_set_timer_div(speed_mode, timer_num, timer_conf->clk_cfg, freq_hz, duty_resolution);
    if (ret == ESP_OK) {
        /* Make sure timer is running and reset the timer. */
        ledc_timer_resume(speed_mode, timer_num);
        ledc_timer_rst(speed_mode, timer_num);
    }
    return ret;
}

esp_err_t _ledc_set_pin(int gpio_num, bool out_inv, ledc_mode_t speed_mode, ledc_channel_t channel)
{
    gpio_func_sel(gpio_num, PIN_FUNC_GPIO);
    gpio_set_level(gpio_num, out_inv);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    // reserve the GPIO output path, because we don't expect another peripheral to signal to the same GPIO
    uint64_t old_gpio_rsv_mask = esp_gpio_reserve(BIT64(gpio_num));
    // check if the GPIO is already used by others, LEDC signal only uses the output path of the GPIO
    if (old_gpio_rsv_mask & BIT64(gpio_num)) {
        ESP_LOGW(LEDC_TAG, "GPIO %d is not usable, maybe conflict with others", gpio_num);
    }
    esp_rom_gpio_connect_out_signal(gpio_num, ledc_periph_signal[speed_mode].sig_out0_idx + channel, out_inv, 0);
    return ESP_OK;
}

// One LEDC channel signal can be directed to multiple IOs as outputs
esp_err_t ledc_set_pin(int gpio_num, ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "gpio_num");
    return _ledc_set_pin(gpio_num, false, speed_mode, channel);
}

esp_err_t ledc_channel_config(const ledc_channel_config_t *ledc_conf)
{
    LEDC_ARG_CHECK(ledc_conf, "ledc_conf");
    uint32_t speed_mode = ledc_conf->speed_mode;
    int gpio_num = ledc_conf->gpio_num;
    uint32_t ledc_channel = ledc_conf->channel;
    uint32_t timer_select = ledc_conf->timer_sel;
    uint32_t intr_type = ledc_conf->intr_type;
    uint32_t duty = ledc_conf->duty;
    uint32_t hpoint = ledc_conf->hpoint;
    bool output_invert = ledc_conf->flags.output_invert;
    LEDC_ARG_CHECK(ledc_channel < LEDC_CHANNEL_MAX, "ledc_channel");
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "gpio_num");
    LEDC_ARG_CHECK(timer_select < LEDC_TIMER_MAX, "timer_select");
    LEDC_ARG_CHECK(intr_type < LEDC_INTR_MAX, "intr_type");

    esp_err_t ret = ESP_OK;

    bool new_speed_mode_ctx_created = ledc_speed_mode_ctx_create(speed_mode);
    if (!new_speed_mode_ctx_created && !p_ledc_obj[speed_mode]) {
        return ESP_ERR_NO_MEM;
    }
#if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61)
    // On such targets, the default ledc core(global) clock does not connect to any clock source
    // Set channel configurations and update bits before core clock is on could lead to error
    // Therefore, we should connect the core clock to a real clock source to make it on before any ledc register operation
    // It can be switched to the other desired clock sources to meet the output pwm freq requirement later at timer configuration
    // So we consider the glb_clk still as LEDC_SLOW_CLK_UNINIT
    else if (new_speed_mode_ctx_created) {
        portENTER_CRITICAL(&ledc_spinlock);
        if (p_ledc_obj[speed_mode]->glb_clk == LEDC_SLOW_CLK_UNINIT) {
            ledc_hal_set_slow_clk_sel(&(p_ledc_obj[speed_mode]->ledc_hal), LEDC_LL_GLOBAL_CLK_DEFAULT);
        }
        portEXIT_CRITICAL(&ledc_spinlock);
    }
#endif

    /*set channel parameters*/
    /*   channel parameters decide how the waveform looks like in one period */
    /*   set channel duty and hpoint value, duty range is [0, (2**duty_res)], hpoint range is [0, (2**duty_res)-1] */
    /*   Note: On ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2, ESP32C6, ESP32H2, ESP32P4, due to a hardware bug,
     *         100% duty cycle (i.e. 2**duty_res) is not reachable when the binded timer selects the maximum duty
     *         resolution. For example, the max duty resolution on ESP32C3 is 14-bit width, then set duty to (2**14)
     *         will mess up the duty calculation in hardware.
    */
    ledc_set_duty_with_hpoint(speed_mode, ledc_channel, duty, hpoint);
    /*update duty settings*/
    ledc_update_duty(speed_mode, ledc_channel);
    /*bind the channel with the timer*/
    ledc_bind_channel_timer(speed_mode, ledc_channel, timer_select);
    /*set interrupt type*/
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_enable_intr_type(speed_mode, ledc_channel, intr_type);
    portEXIT_CRITICAL(&ledc_spinlock);
    ESP_LOGD(LEDC_TAG, "LEDC_PWM CHANNEL %"PRIu32"|GPIO %02u|Duty %04"PRIu32"|Time %"PRIu32,
             ledc_channel, gpio_num, duty, timer_select);
    /*set LEDC signal in gpio matrix*/
    _ledc_set_pin(gpio_num, output_invert, speed_mode, ledc_channel);

    return ret;
}

static void _ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    ledc_hal_set_sig_out_en(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
    ledc_hal_set_duty_start(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
    ledc_ls_channel_update(speed_mode, channel);
}

esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK_ISR(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK_ISR(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK_ISR(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL_SAFE(&ledc_spinlock);
    _ledc_update_duty(speed_mode, channel);
    portEXIT_CRITICAL_SAFE(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level)
{
    LEDC_ARG_CHECK_ISR(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK_ISR(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK_ISR(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL_SAFE(&ledc_spinlock);
    ledc_hal_set_idle_level(&(p_ledc_obj[speed_mode]->ledc_hal), channel, idle_level);
    ledc_hal_set_sig_out_en(&(p_ledc_obj[speed_mode]->ledc_hal), channel, false);
    ledc_hal_set_duty_start(&(p_ledc_obj[speed_mode]->ledc_hal), channel, false);
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL_SAFE(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_set_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, ledc_duty_direction_t fade_direction,
                        uint32_t step_num, uint32_t duty_cycle_num, uint32_t duty_scale)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_direction < LEDC_DUTY_DIR_MAX, "fade_direction");
    LEDC_ARG_CHECK(step_num <= LEDC_LL_DUTY_NUM_MAX, "step_num");
    LEDC_ARG_CHECK(duty_cycle_num <= LEDC_LL_DUTY_CYCLE_MAX, "duty_cycle_num");
    LEDC_ARG_CHECK(duty_scale <= LEDC_LL_DUTY_SCALE_MAX, "duty_scale");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,        //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty,           //uint32_t duty_val,
                     fade_direction, //uint32_t increase,
                     step_num,       //uint32_t duty_num,
                     duty_cycle_num,  //uint32_t duty_cycle,
                     duty_scale      //uint32_t duty_scale
                    );
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_duty_with_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(hpoint <= LEDC_LL_HPOINT_VAL_MAX, "hpoint");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    /* The channel configuration should not be changed before the fade operation is done. */
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     hpoint,          //uint32_t hpoint_val,
                     duty,           //uint32_t duty_val,
                     1,               //uint32_t increase,
                     1,               //uint32_t duty_num,
                     1,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                    );
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    /* The channel configuration should not be changed before the fade operation is done. */
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty,           //uint32_t duty_val,
                     1,               //uint32_t increase,
                     1,               //uint32_t duty_num,
                     1,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                    );
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    uint32_t duty = 0;
    ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty);
    return duty;
}

int ledc_get_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode argument is invalid", LEDC_ERR_VAL);
    LEDC_CHECK(channel < LEDC_CHANNEL_MAX, "channel argument is invalid", LEDC_ERR_VAL);
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    uint32_t hpoint = 0;
    ledc_hal_get_hpoint(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &hpoint);
    return hpoint;
}

esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_num < LEDC_TIMER_MAX, "timer_num");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    ledc_clk_cfg_t clk_cfg = LEDC_AUTO_CLK;
    uint32_t duty_resolution = 0;
    ledc_hal_get_clk_cfg(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &clk_cfg);
    ledc_hal_get_duty_resolution(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &duty_resolution);
    return ledc_set_timer_div(speed_mode, timer_num, clk_cfg, freq_hz, duty_resolution);
}

uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_num < LEDC_TIMER_MAX, "timer_num");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t clock_divider = 0;
    uint32_t duty_resolution = 0;
    ledc_clk_cfg_t clk_cfg = LEDC_AUTO_CLK;
    ledc_hal_get_clock_divider(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &clock_divider);
    ledc_hal_get_duty_resolution(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &duty_resolution);
    ledc_hal_get_clk_cfg(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &clk_cfg);
    uint32_t precision = (0x1 << duty_resolution);
    uint32_t src_clk_freq = 0;
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_cfg, LEDC_CLK_SRC_FREQ_PRECISION, &src_clk_freq);
    portEXIT_CRITICAL(&ledc_spinlock);
    if (clock_divider == 0) {
        ESP_LOGW(LEDC_TAG, "LEDC timer not configured, call ledc_timer_config to set timer frequency");
        return 0;
    }
    return (((uint64_t) src_clk_freq << LEDC_LL_FRACTIONAL_BITS) + precision * clock_divider / 2) / (precision * clock_divider);
}

static inline uint32_t ilog2(uint32_t i)
{
    assert(i > 0);
    uint32_t log = 0;
    while (i >>= 1) {
        ++log;
    }
    return log;
}

// https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#ledpwm
uint32_t ledc_find_suitable_duty_resolution(uint32_t src_clk_freq, uint32_t timer_freq)
{
    // Highest resolution is calculated when LEDC_CLK_DIV = 1 (i.e. div_param = 1 << LEDC_LL_FRACTIONAL_BITS)
    uint32_t div = (src_clk_freq + timer_freq / 2) / timer_freq; // rounded
    uint32_t duty_resolution = MIN(ilog2(div), SOC_LEDC_TIMER_BIT_WIDTH);
    uint32_t div_param = ledc_calculate_divisor(src_clk_freq, timer_freq, 1 << duty_resolution);
    if (LEDC_IS_DIV_INVALID(div_param)) {
        div = src_clk_freq / timer_freq; // truncated
        duty_resolution = MIN(ilog2(div), SOC_LEDC_TIMER_BIT_WIDTH);
        div_param = ledc_calculate_divisor(src_clk_freq, timer_freq, 1 << duty_resolution);
        if (LEDC_IS_DIV_INVALID(div_param)) {
            duty_resolution = 0;
        }
    }
    return duty_resolution;
}

static inline void IRAM_ATTR ledc_calc_fade_end_channel(uint32_t *fade_end_status, uint32_t *channel)
{
    uint32_t i = __builtin_ffs((*fade_end_status)) - 1;
    (*fade_end_status) &= ~(1 << i);
    *channel = i;
}

static void IRAM_ATTR ledc_fade_isr(void *arg)
{
    bool cb_yield = false;
    BaseType_t HPTaskAwoken = pdFALSE;
    uint32_t speed_mode = 0;
    uint32_t channel = 0;
    uint32_t intr_status = 0;
    ledc_fade_fsm_t state;

    for (speed_mode = 0; speed_mode < LEDC_SPEED_MODE_MAX; speed_mode++) {
        if (p_ledc_obj[speed_mode] == NULL) {
            continue;
        }
        ledc_hal_get_fade_end_intr_status(&(p_ledc_obj[speed_mode]->ledc_hal), &intr_status);
        while (intr_status) {
            ledc_calc_fade_end_channel(&intr_status, &channel);

            // clear interrupt
            ledc_hal_clear_fade_end_intr_status(&(p_ledc_obj[speed_mode]->ledc_hal), channel);

            if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
                //fade object not initialized yet.
                continue;
            }

            // Switch fade state to ISR_CAL if current state is HW_FADE
            bool already_stopped = false;
            portENTER_CRITICAL_ISR(&ledc_spinlock);
            state = s_ledc_fade_rec[speed_mode][channel]->fsm;
            assert(state != LEDC_FSM_ISR_CAL && state != LEDC_FSM_KILLED_PENDING);
            if (state == LEDC_FSM_HW_FADE) {
                s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_ISR_CAL;
            } else if (state == LEDC_FSM_IDLE) {
                // interrupt seen, but has already been stopped by task
                already_stopped = true;
            }
            portEXIT_CRITICAL_ISR(&ledc_spinlock);
            if (already_stopped) {
                continue;
            }

            bool set_to_idle = false;
            int cycle = 0;
            int delta = 0;
            int step = 0;
            int next_duty = 0;
            uint32_t duty_cur = 0;
            ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty_cur);
            uint32_t duty_tar = s_ledc_fade_rec[speed_mode][channel]->target_duty;
#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
            // If a multi-fade is done, check that target duty computed in sw is equal to the duty at the end of the fade
            uint32_t range_num;
            ledc_hal_get_range_number(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &range_num);
            if (range_num > 1) {
                assert(duty_cur == duty_tar);
            }
#endif
            int scale = s_ledc_fade_rec[speed_mode][channel]->scale;
            if (duty_cur == duty_tar || scale == 0) {
                // Target duty has reached
                set_to_idle = true;
            } else {
                // Calculate new duty config parameters
                delta = (s_ledc_fade_rec[speed_mode][channel]->direction == LEDC_DUTY_DIR_DECREASE) ?
                        (duty_cur - duty_tar) : (duty_tar - duty_cur);
                if (delta > scale) {
                    next_duty = duty_cur;
                    step = (delta / scale > LEDC_DUTY_NUM_MAX) ? LEDC_DUTY_NUM_MAX : (delta / scale);
                    cycle = s_ledc_fade_rec[speed_mode][channel]->cycle_num;
                } else {
                    next_duty = duty_tar;
                    step = 1;
                    cycle = 1;
                    scale = 0;
                }
            }

            bool finished = false;
            portENTER_CRITICAL_ISR(&ledc_spinlock);
            state = s_ledc_fade_rec[speed_mode][channel]->fsm;
            assert(state != LEDC_FSM_IDLE && state != LEDC_FSM_HW_FADE);
            if (set_to_idle || state == LEDC_FSM_KILLED_PENDING) {
                // Either fade has completed or has been killed, skip HW duty config
                finished = true;
                s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_IDLE;
            } else if (state == LEDC_FSM_ISR_CAL) {
                // Loading new fade to start
                ledc_duty_config(speed_mode,
                                 channel,
                                 LEDC_VAL_NO_CHANGE,
                                 next_duty,
                                 s_ledc_fade_rec[speed_mode][channel]->direction,
                                 step,
                                 cycle,
                                 scale);
                s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_HW_FADE;
                ledc_hal_set_duty_start(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
                ledc_ls_channel_update(speed_mode, channel);
            }
            portEXIT_CRITICAL_ISR(&ledc_spinlock);
            if (finished) {
                xSemaphoreGiveFromISR(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem, &HPTaskAwoken);
                ledc_cb_t fade_cb = s_ledc_fade_rec[speed_mode][channel]->ledc_fade_callback;
                if (fade_cb) {
                    ledc_cb_param_t param = {
                        .event = LEDC_FADE_END_EVT,
                        .speed_mode = speed_mode,
                        .channel = channel,
                        .duty = duty_cur
                    };
                    cb_yield |= fade_cb(&param, s_ledc_fade_rec[speed_mode][channel]->cb_user_arg);
                }
            }
        }
    }
    if (HPTaskAwoken == pdTRUE || cb_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t ledc_fade_channel_deinit(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (s_ledc_fade_rec[speed_mode][channel]) {
        if (s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux) {
            vSemaphoreDelete(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux);
            s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux = NULL;
        }
        if (s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem) {
            vSemaphoreDeleteWithCaps(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem);
            s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = NULL;
        }
        free(s_ledc_fade_rec[speed_mode][channel]);
        s_ledc_fade_rec[speed_mode][channel] = NULL;
    }
    return ESP_OK;
}

static esp_err_t ledc_fade_channel_init_check(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (s_ledc_fade_isr_handle == NULL) {
        ESP_LOGE(LEDC_TAG, "Fade service not installed, call ledc_fade_func_install");
        return ESP_FAIL;
    }
    if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
        // Always malloc internally since LEDC ISR is always placed in IRAM
        s_ledc_fade_rec[speed_mode][channel] = (ledc_fade_t *) heap_caps_calloc(1, sizeof(ledc_fade_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
            ledc_fade_channel_deinit(speed_mode, channel);
            return ESP_ERR_NO_MEM;
        }
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = xSemaphoreCreateBinaryWithCaps(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux = xSemaphoreCreateMutex();
        xSemaphoreGive(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem);
        s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_IDLE;
    }
    if (s_ledc_fade_rec[speed_mode][channel]
            && s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux
            && s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem) {
        return ESP_OK;
    } else {
        ledc_fade_channel_deinit(speed_mode, channel);
        return ESP_FAIL;
    }
}

static esp_err_t _ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int scale, int cycle_num)
{
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t duty_cur = 0;
    ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty_cur);
    // When duty == max_duty, meanwhile, if scale == 1 and fade_down == 1, counter would overflow.
    if (duty_cur == ledc_get_max_duty(speed_mode, channel)) {
        duty_cur -= 1;
    }
    s_ledc_fade_rec[speed_mode][channel]->speed_mode = speed_mode;
    s_ledc_fade_rec[speed_mode][channel]->target_duty = target_duty;
    s_ledc_fade_rec[speed_mode][channel]->cycle_num = cycle_num;
    s_ledc_fade_rec[speed_mode][channel]->scale = scale;
    int step_num = 0;
    int dir = LEDC_DUTY_DIR_DECREASE;
    if (scale > 0) {
        if (duty_cur > target_duty) {
            s_ledc_fade_rec[speed_mode][channel]->direction = LEDC_DUTY_DIR_DECREASE;
            step_num = (duty_cur - target_duty) / scale;
            step_num = step_num > LEDC_DUTY_NUM_MAX ? LEDC_DUTY_NUM_MAX : step_num;
        } else {
            s_ledc_fade_rec[speed_mode][channel]->direction = LEDC_DUTY_DIR_INCREASE;
            dir = LEDC_DUTY_DIR_INCREASE;
            step_num = (target_duty - duty_cur) / scale;
            step_num = step_num > LEDC_DUTY_NUM_MAX ? LEDC_DUTY_NUM_MAX : step_num;
        }
    }

    portEXIT_CRITICAL(&ledc_spinlock);
    if (scale > 0 && step_num > 0) {
        portENTER_CRITICAL(&ledc_spinlock);
        ledc_duty_config(speed_mode, channel, LEDC_VAL_NO_CHANGE, duty_cur, dir, step_num, cycle_num, scale);
        portEXIT_CRITICAL(&ledc_spinlock);
        ESP_LOGD(LEDC_TAG, "cur duty: %"PRIu32"; target: %"PRIu32", step: %d, cycle: %d; scale: %d; dir: %d",
                 duty_cur, target_duty, step_num, cycle_num, scale, dir);
    } else {
        // Directly set duty to the target, does not care on the dir
        portENTER_CRITICAL(&ledc_spinlock);
        ledc_duty_config(speed_mode, channel, LEDC_VAL_NO_CHANGE, target_duty, 1, 1, 1, 0);
        portEXIT_CRITICAL(&ledc_spinlock);
        ESP_LOGD(LEDC_TAG, "Set to target duty: %"PRIu32, target_duty);
    }
    return ESP_OK;
}

static esp_err_t _ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    ledc_timer_t timer_sel;
    uint32_t duty_cur = 0;
    ledc_hal_get_channel_timer(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &timer_sel);
    ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty_cur);
    uint32_t freq = ledc_get_freq(speed_mode, timer_sel);
    uint32_t duty_delta = target_duty > duty_cur ? target_duty - duty_cur : duty_cur - target_duty;

    if (duty_delta == 0) {
        return _ledc_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }
    uint32_t total_cycles = max_fade_time_ms * freq / 1000;
    if (total_cycles == 0) {
        ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
        return _ledc_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }
    int scale, cycle_num;
    if (total_cycles > duty_delta) {
        scale = 1;
        cycle_num = total_cycles / duty_delta;
        if (cycle_num > LEDC_LL_DUTY_CYCLE_MAX) {
            ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_SLOW_STR);
            cycle_num = LEDC_LL_DUTY_CYCLE_MAX;
        }
    } else {
        cycle_num = 1;
        scale = duty_delta / total_cycles;
        if (scale > LEDC_LL_DUTY_SCALE_MAX) {
            ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
            scale = LEDC_LL_DUTY_SCALE_MAX;
        }
    }
    return _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
}

static void _ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode)
{
    ledc_fade_t *fade = s_ledc_fade_rec[speed_mode][channel];
    fade->mode = fade_mode;
    // Clear interrupt status of channel
    ledc_hal_clear_fade_end_intr_status(&(p_ledc_obj[speed_mode]->ledc_hal), channel);
    // Enable interrupt for channel
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_enable_intr_type(speed_mode, channel, LEDC_INTR_FADE_END);
    // Set fade state to HW_FADE state for starting the fade
    assert(fade->fsm == LEDC_FSM_IDLE);
    fade->fsm = LEDC_FSM_HW_FADE;
    portEXIT_CRITICAL(&ledc_spinlock);
    // Trigger the fade
    ledc_update_duty(speed_mode, channel);
    if (fade_mode == LEDC_FADE_WAIT_DONE) {
        // Waiting for fade done
        _ledc_fade_hw_acquire(speed_mode, channel);
        // Release hardware to support next time fade configure
        _ledc_fade_hw_release(speed_mode, channel);
    }
}

esp_err_t ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_time(speed_mode, channel, target_duty, max_fade_time_ms);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK((scale > 0) && (scale <= LEDC_LL_DUTY_SCALE_MAX), "fade scale");
    LEDC_ARG_CHECK((cycle_num > 0) && (cycle_num <= LEDC_LL_DUTY_CYCLE_MAX), "cycle_num");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode)
{
    LEDC_CHECK(s_ledc_fade_rec[speed_mode][channel] != NULL, LEDC_FADE_SERVICE_ERR_STR, ESP_ERR_INVALID_STATE);
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    return ESP_OK;
}

// ESP32 does not support this functionality, fade cannot be overwritten with new duty config
#if SOC_LEDC_SUPPORT_FADE_STOP
esp_err_t ledc_fade_stop(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    ledc_fade_t *fade = s_ledc_fade_rec[speed_mode][channel];
    ledc_fade_fsm_t state = fade->fsm;
    bool wait_for_idle = false;
    assert(state != LEDC_FSM_KILLED_PENDING);
    if (state == LEDC_FSM_IDLE) {
        // if there is no fade going on, do nothing
        return ESP_OK;
    }
    // Fade state is either HW_FADE or ISR_CAL (there is a fade in process)
    portENTER_CRITICAL(&ledc_spinlock);
    // Disable ledc channel interrupt first
    ledc_enable_intr_type(speed_mode, channel, LEDC_INTR_DISABLE);
    // Config duty to the duty cycle at this moment
    uint32_t duty_cur = ledc_get_duty(speed_mode, channel);
    ledc_duty_config(speed_mode,
                     channel,              //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty_cur,             //uint32_t duty_val,
                     1,                    //uint32_t increase,
                     1,                    //uint32_t duty_num,
                     1,                    //uint32_t duty_cycle,
                     0                     //uint32_t duty_scale
                    );
    _ledc_update_duty(speed_mode, channel);
    state = fade->fsm;
    assert(state != LEDC_FSM_IDLE && state != LEDC_FSM_KILLED_PENDING);
    if (state == LEDC_FSM_HW_FADE) {
        fade->fsm = LEDC_FSM_IDLE;
    } else if (state == LEDC_FSM_ISR_CAL) {
        fade->fsm = LEDC_FSM_KILLED_PENDING;
        wait_for_idle = true;
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    if (wait_for_idle) {
        // Wait for ISR return, which gives the semaphore and switches state to IDLE
        _ledc_fade_hw_acquire(speed_mode, channel);
        assert(fade->fsm == LEDC_FSM_IDLE);
    }
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}
#endif

esp_err_t ledc_fade_func_install(int intr_alloc_flags)
{
    LEDC_CHECK(s_ledc_fade_isr_handle == NULL, "fade function already installed", ESP_ERR_INVALID_STATE);
    //OR intr_alloc_flags with ESP_INTR_FLAG_IRAM because the fade isr is in IRAM
    return ledc_isr_register(ledc_fade_isr, NULL, intr_alloc_flags | ESP_INTR_FLAG_IRAM, &s_ledc_fade_isr_handle);
}

void ledc_fade_func_uninstall(void)
{
    if (s_ledc_fade_isr_handle) {
        esp_intr_free(s_ledc_fade_isr_handle);
        s_ledc_fade_isr_handle = NULL;
    }
    int channel, mode;
    for (mode = 0; mode < LEDC_SPEED_MODE_MAX; mode++) {
        for (channel = 0; channel < LEDC_CHANNEL_MAX; channel++) {
            ledc_fade_channel_deinit(mode, channel);
        }
    }
    return;
}

esp_err_t ledc_cb_register(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_cbs_t *cbs, void *user_arg)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(cbs, "callback");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    if (cbs->fade_cb && !esp_ptr_in_iram(cbs->fade_cb)) {
        ESP_LOGW(LEDC_TAG, "fade callback not in IRAM");
    }
    if (user_arg && !esp_ptr_internal(user_arg)) {
        ESP_LOGW(LEDC_TAG, "user context not in internal RAM");
    }
    s_ledc_fade_rec[speed_mode][channel]->ledc_fade_callback = cbs->fade_cb;
    s_ledc_fade_rec[speed_mode][channel]->cb_user_arg = user_arg;
    return ESP_OK;
}

/*
 * The functions below are thread-safe version of APIs for duty and fade control.
 * These APIs can be called from different tasks.
 */
esp_err_t ledc_set_duty_and_update(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_ARG_CHECK(hpoint <= LEDC_LL_HPOINT_VAL_MAX, "hpoint");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode, channel, hpoint, duty, 1, 1, 1, 0);
    _ledc_update_duty(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_time_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t max_fade_time_ms, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_time(speed_mode, channel, target_duty, max_fade_time_ms);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_step_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    LEDC_ARG_CHECK((scale > 0) && (scale <= LEDC_LL_DUTY_SCALE_MAX), "fade scale");
    LEDC_ARG_CHECK((cycle_num > 0) && (cycle_num <= LEDC_LL_DUTY_CYCLE_MAX), "cycle_num");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}

#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
static esp_err_t _ledc_set_multi_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t start_duty, const ledc_fade_param_config_t *fade_params_list, uint32_t list_len)
{
    uint32_t max_duty = ledc_get_max_duty(speed_mode, channel);
    LEDC_ARG_CHECK(start_duty <= max_duty, "start_duty");
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_set_duty_int_part(&(p_ledc_obj[speed_mode]->ledc_hal), channel, start_duty);
    for (int i = 0; i < list_len; i++) {
        ledc_fade_param_config_t fade_param = fade_params_list[i];
        ledc_hal_set_fade_param(&(p_ledc_obj[speed_mode]->ledc_hal), channel, i, fade_param.dir, fade_param.cycle_num, fade_param.scale, fade_param.step_num);
    }
    ledc_hal_set_range_number(&(p_ledc_obj[speed_mode]->ledc_hal), channel, list_len);
    portEXIT_CRITICAL(&ledc_spinlock);
    // Calculate target duty, and take account for overflow
    uint32_t target_duty = start_duty;
    for (int i = 0; i < list_len; i++) {
        uint32_t delta_duty = (fade_params_list[i].step_num * fade_params_list[i].scale) % (max_duty + 1);
        if (fade_params_list[i].dir == LEDC_DUTY_DIR_INCREASE) {
            target_duty += delta_duty;
            if (target_duty > max_duty) {
                target_duty -= max_duty + 1;
            }
        } else {
            if (delta_duty > target_duty) {
                target_duty += max_duty + 1;
            }
            target_duty -= delta_duty;
        }
    }
    // Set interrupt exit criteria
    s_ledc_fade_rec[speed_mode][channel]->target_duty = target_duty;
    s_ledc_fade_rec[speed_mode][channel]->scale = fade_params_list[list_len - 1].scale;
    return ESP_OK;
}

esp_err_t ledc_set_multi_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t start_duty, const ledc_fade_param_config_t *fade_params_list, uint32_t list_len)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(list_len <= SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, "list_len");
    LEDC_ARG_CHECK(fade_params_list, "fade_params_list");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    esp_err_t ret = _ledc_set_multi_fade(speed_mode, channel, start_duty, fade_params_list, list_len);
    _ledc_fade_hw_release(speed_mode, channel);
    return ret;
}

esp_err_t ledc_set_multi_fade_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t start_duty, const ledc_fade_param_config_t *fade_params_list, uint32_t list_len, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(list_len <= SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, "list_len");
    LEDC_ARG_CHECK(fade_params_list, "fade_params_list");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    esp_err_t ret = _ledc_set_multi_fade(speed_mode, channel, start_duty, fade_params_list, list_len);
    if (ret != ESP_OK) {
        _ledc_fade_hw_release(speed_mode, channel);
    } else {
        _ledc_fade_start(speed_mode, channel, fade_mode);
    }
    _ledc_op_lock_release(speed_mode, channel);
    return ret;
}

esp_err_t ledc_fill_multi_fade_param_list(ledc_mode_t speed_mode, ledc_channel_t channel,
                                          uint32_t start_duty, uint32_t end_duty,
                                          uint32_t linear_phase_num, uint32_t max_fade_time_ms,
                                          uint32_t (* gamma_correction_operator)(uint32_t),
                                          uint32_t fade_params_list_size,
                                          ledc_fade_param_config_t *fade_params_list, uint32_t *hw_fade_range_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(linear_phase_num > 0 && linear_phase_num <= SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, "linear_phase_num");
    LEDC_ARG_CHECK(gamma_correction_operator, "gamma_correction_operator");
    LEDC_ARG_CHECK(fade_params_list_size <= SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, "fade_params_list_size");
    LEDC_ARG_CHECK(fade_params_list, "fade_params_list");
    LEDC_ARG_CHECK(hw_fade_range_num, "hw_fade_range_num");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);

    uint32_t max_duty = ledc_get_max_duty(speed_mode, channel);
    LEDC_ARG_CHECK(start_duty <= max_duty && end_duty <= max_duty, "duty");

    esp_err_t ret = ESP_OK;

    ledc_timer_t timer_sel;
    ledc_hal_get_channel_timer(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &timer_sel);
    uint32_t freq = ledc_get_freq(speed_mode, timer_sel);

    uint32_t dir = (end_duty > start_duty) ? LEDC_DUTY_DIR_INCREASE : LEDC_DUTY_DIR_DECREASE;
    uint32_t total_cycles = max_fade_time_ms * freq / 1000;
    // If no duty change is need, then simplify the case
    if (start_duty == end_duty) {
        total_cycles = 1;
        linear_phase_num = 1;
    }
    uint32_t avg_cycles_per_phase = total_cycles / linear_phase_num;
    if (avg_cycles_per_phase == 0) {
        ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
        avg_cycles_per_phase = 1;
    }
    int sgn = (dir == LEDC_DUTY_DIR_INCREASE) ? 1 : (-1);
    int32_t delta_brightness_per_phase = sgn * ((sgn * (end_duty - start_duty)) / linear_phase_num);

    // First phase start and end values
    uint32_t gamma_corrected_phase_head = gamma_correction_operator(start_duty);
    uint32_t gamma_corrected_phase_tail = 0;
    int32_t phase_tail = start_duty + delta_brightness_per_phase;

    // Compute raw fade parameters for each linear phase
    uint32_t total_fade_range = 0; // To record the required hw fade ranges
    uint32_t surplus_cycles_last_phase = 0;
    for (int i = 0; i < linear_phase_num; i++) {
        uint32_t cycle, scale, step;
        gamma_corrected_phase_tail = gamma_correction_operator(phase_tail);
        uint32_t duty_delta = (dir == LEDC_DUTY_DIR_INCREASE) ? (gamma_corrected_phase_tail - gamma_corrected_phase_head) :
                              (gamma_corrected_phase_head - gamma_corrected_phase_tail);
        uint32_t cycles_per_phase = avg_cycles_per_phase + surplus_cycles_last_phase;
        if (duty_delta == 0) {
            scale = 0;
            cycle = (cycles_per_phase > LEDC_LL_DUTY_CYCLE_MAX) ? LEDC_LL_DUTY_CYCLE_MAX : cycles_per_phase;
            step = 1;
        } else if (cycles_per_phase > duty_delta) {
            scale = 1;
            step = duty_delta;
            cycle = cycles_per_phase / duty_delta;
            if (cycle > LEDC_LL_DUTY_CYCLE_MAX) {
                ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_SLOW_STR);
                cycle = LEDC_LL_DUTY_CYCLE_MAX;
            }
        } else {
            cycle = 1;
            scale = duty_delta / cycles_per_phase;
            if (scale > LEDC_LL_DUTY_SCALE_MAX) {
                ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
                scale = LEDC_LL_DUTY_SCALE_MAX;
            }
            step = duty_delta / scale;
        }

        // Prepare for next phase calculation
        phase_tail = phase_tail + delta_brightness_per_phase;
        if (dir == LEDC_DUTY_DIR_INCREASE) {
            gamma_corrected_phase_head += step * scale;
        } else {
            gamma_corrected_phase_head -= step * scale;
        }
        surplus_cycles_last_phase = cycles_per_phase - step * cycle;
        // If next phase is the last one, then account for all remaining duty and cycles
        if (i == linear_phase_num - 2) {
            phase_tail = end_duty;
            surplus_cycles_last_phase += total_cycles - avg_cycles_per_phase * linear_phase_num;
        }

        // Fill into the fade parameter list
        // One linear phase might need multiple hardware fade ranges
        do {
            if (total_fade_range >= fade_params_list_size) {
                ret = ESP_FAIL;
                break;
            }
            fade_params_list[total_fade_range].dir = dir;
            fade_params_list[total_fade_range].cycle_num = cycle;
            fade_params_list[total_fade_range].scale = scale;
            fade_params_list[total_fade_range].step_num = (step > LEDC_LL_DUTY_NUM_MAX) ? LEDC_LL_DUTY_NUM_MAX : step;
            step -= fade_params_list[total_fade_range].step_num;
            total_fade_range += 1;
        } while (step > 0);

        if (ret != ESP_OK) {
            break;
        }
    }

    uint32_t remaining_duty_delta = (dir == LEDC_DUTY_DIR_INCREASE) ? (gamma_corrected_phase_tail - gamma_corrected_phase_head) :
                                    (gamma_corrected_phase_head - gamma_corrected_phase_tail);
    if (remaining_duty_delta) {
        total_fade_range += 1;
    }

    ESP_RETURN_ON_FALSE(total_fade_range <= fade_params_list_size, ESP_FAIL, LEDC_TAG,
                        "hw fade ranges required exceeds the space offered to fill the fade params."
                        " Please allocate more space, or split into smaller multi-fades, or reduce linear_phase_num");

    if (remaining_duty_delta) {
        fade_params_list[total_fade_range].dir = dir;
        fade_params_list[total_fade_range].step_num = 1;
        fade_params_list[total_fade_range].cycle_num = 1;
        fade_params_list[total_fade_range].scale = remaining_duty_delta;
    }

    *hw_fade_range_num = total_fade_range;
    return ret;
}

esp_err_t ledc_read_fade_param(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t range, uint32_t *dir, uint32_t *cycle, uint32_t *scale, uint32_t *step)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(range < SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX, "range");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);

    ledc_hal_get_fade_param(&(p_ledc_obj[speed_mode]->ledc_hal), channel, range, dir, cycle, scale, step);
    return ESP_OK;
}
#endif // SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
