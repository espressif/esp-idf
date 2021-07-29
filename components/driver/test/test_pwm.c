/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <unistd.h>
#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
#if SOC_MCPWM_SUPPORTED
#include "soc/mcpwm_periph.h"
#include "driver/pcnt.h"
#include "driver/mcpwm.h"
#include "driver/gpio.h"

#define TEST_PWMA_PCNT_UNIT (0)
#define TEST_PWMB_PCNT_UNIT (1)
#define TEST_PWMA_GPIO (2)
#define TEST_PWMB_GPIO (4)
#define TEST_FAULT_GPIO (21)
#define TEST_SYNC_GPIO (21)
#define TEST_CAP_GPIO (21)

static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1}; // interrupt handling still lacks API to get/clear pending event, currently we have to read/write interrupt register
const static mcpwm_io_signals_t pwma[] = {MCPWM0A, MCPWM1A, MCPWM2A};
const static mcpwm_io_signals_t pwmb[] = {MCPWM0B, MCPWM1B, MCPWM2B};
const static mcpwm_fault_signal_t fault_sig_array[] = {MCPWM_SELECT_F0, MCPWM_SELECT_F1, MCPWM_SELECT_F2};
const static mcpwm_io_signals_t fault_io_sig_array[] = {MCPWM_FAULT_0, MCPWM_FAULT_1, MCPWM_FAULT_2};
const static mcpwm_sync_signal_t sync_sig_array[] = {MCPWM_SELECT_SYNC0, MCPWM_SELECT_SYNC1, MCPWM_SELECT_SYNC2};
const static mcpwm_io_signals_t sync_io_sig_array[] = {MCPWM_SYNC_0, MCPWM_SYNC_1, MCPWM_SYNC_2};
const static mcpwm_capture_signal_t cap_sig_array[] = {MCPWM_SELECT_CAP0, MCPWM_SELECT_CAP1, MCPWM_SELECT_CAP2};
const static mcpwm_io_signals_t cap_io_sig_array[] = {MCPWM_CAP_0, MCPWM_CAP_1, MCPWM_CAP_2};

// This GPIO init function is almost the same to public API `mcpwm_gpio_init()`, except that
// this function will configure all MCPWM GPIOs into output and input capable
// which is useful to simulate a trigger source
static esp_err_t test_mcpwm_gpio_init(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num)
{
    if (gpio_num < 0) { // ignore on minus gpio number
        return ESP_OK;
    }

    if (io_signal <= MCPWM2B) { // Generator output signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
        int operator_id = io_signal / 2;
        int generator_id = io_signal % 2;
        esp_rom_gpio_connect_out_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].operators[operator_id].generators[generator_id].pwm_sig, 0, 0);
    } else if (io_signal <= MCPWM_SYNC_2) { // External sync input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
        int gpio_sync_id = io_signal - MCPWM_SYNC_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].gpio_synchros[gpio_sync_id].sync_sig, 0);
    } else if (io_signal <= MCPWM_FAULT_2) { // Fault input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
        int fault_id = io_signal - MCPWM_FAULT_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].gpio_faults[fault_id].fault_sig, 0);
    } else if (io_signal >= MCPWM_CAP_0 && io_signal <= MCPWM_CAP_2) { // Capture input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
        int capture_id = io_signal - MCPWM_CAP_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].captures[capture_id].cap_sig, 0);
    }
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    return ESP_OK;
}

static void mcpwm_setup_testbench(mcpwm_unit_t group, mcpwm_timer_t timer, uint32_t pwm_freq, float pwm_duty)
{
    // PWMA <--> PCNT UNIT0
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = TEST_PWMA_GPIO,
        .ctrl_gpio_num = -1,       // don't care level signal
        .channel = PCNT_CHANNEL_0,
        .unit = TEST_PWMA_PCNT_UNIT,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 10000,
        .counter_l_lim = -10000,
    };
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));
    mcpwm_io_signals_t mcpwm_a = pwma[timer];
    TEST_ESP_OK(test_mcpwm_gpio_init(group, mcpwm_a, TEST_PWMA_GPIO));
    // PWMB <--> PCNT UNIT1
    pcnt_config.pulse_gpio_num = TEST_PWMB_GPIO;
    pcnt_config.unit = TEST_PWMB_PCNT_UNIT;
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));
    mcpwm_io_signals_t mcpwm_b = pwmb[timer];
    TEST_ESP_OK(test_mcpwm_gpio_init(group, mcpwm_b, TEST_PWMB_GPIO));

    // Set PWM freq and duty, start immediately
    mcpwm_config_t pwm_config = {
        .frequency = pwm_freq,
        .cmpr_a = pwm_duty,
        .cmpr_b = pwm_duty,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    TEST_ESP_OK(mcpwm_init(group, timer, &pwm_config));
}

static uint32_t mcpwm_pcnt_get_pulse_number(pcnt_unit_t pwm_pcnt_unit, int capture_window_ms)
{
    int16_t count_value = 0;
    TEST_ESP_OK(pcnt_counter_pause(pwm_pcnt_unit));
    TEST_ESP_OK(pcnt_counter_clear(pwm_pcnt_unit));
    TEST_ESP_OK(pcnt_counter_resume(pwm_pcnt_unit));
    usleep(capture_window_ms * 1000);
    TEST_ESP_OK(pcnt_get_counter_value(pwm_pcnt_unit, &count_value));
    printf("count value: %d\r\n", count_value);
    return (uint32_t)count_value;
}

static void mcpwm_timer_duty_test(mcpwm_unit_t unit, mcpwm_timer_t timer)
{
    mcpwm_setup_testbench(unit, timer, 1000, 50.0);
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(mcpwm_set_duty(unit, timer, MCPWM_OPR_A, 10.0));
    TEST_ESP_OK(mcpwm_set_duty(unit, timer, MCPWM_OPR_B, 20.0));
    TEST_ASSERT_EQUAL_FLOAT(10.0, mcpwm_get_duty(unit, timer, MCPWM_OPR_A));
    TEST_ASSERT_EQUAL_FLOAT(20.0, mcpwm_get_duty(unit, timer, MCPWM_OPR_B));
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(mcpwm_set_duty(unit, timer, MCPWM_OPR_A, 55.5f));
    TEST_ESP_OK(mcpwm_set_duty_type(unit, timer, MCPWM_OPR_A, MCPWM_DUTY_MODE_0));
    TEST_ASSERT_EQUAL_FLOAT(55.5, mcpwm_get_duty(unit, timer, MCPWM_OPR_A));
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(mcpwm_set_duty_in_us(unit, timer, MCPWM_OPR_B, 500));
    TEST_ASSERT_EQUAL_FLOAT(50.0, mcpwm_get_duty(unit, timer, MCPWM_OPR_B));
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(mcpwm_stop(unit, timer));
    vTaskDelay(pdMS_TO_TICKS(100));
}

TEST_CASE("MCPWM duty test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            mcpwm_timer_duty_test(i, j);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_start_stop_test(mcpwm_unit_t unit, mcpwm_timer_t timer)
{
    uint32_t pulse_number = 0;

    mcpwm_setup_testbench(unit, timer, 1000, 50.0); // Period: 1000us, 1ms
    // count the pulse number within 100ms
    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMA_PCNT_UNIT, 100);
    TEST_ASSERT_INT_WITHIN(2, 100, pulse_number);
    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMB_PCNT_UNIT, 100);
    TEST_ASSERT_INT_WITHIN(2, 100, pulse_number);

    TEST_ESP_OK(mcpwm_set_frequency(unit, timer, 100));
    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMB_PCNT_UNIT, 100);
    TEST_ASSERT_INT_WITHIN(2, 10, pulse_number);

    // stop timer, then no pwm pulse should be generating
    TEST_ESP_OK(mcpwm_stop(unit, timer));
    usleep(10000); // wait until timer stopped
    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMA_PCNT_UNIT, 100);
    TEST_ASSERT_INT_WITHIN(2, 0, pulse_number);
    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMB_PCNT_UNIT, 100);
    TEST_ASSERT_INT_WITHIN(2, 0, pulse_number);
}

TEST_CASE("MCPWM start and stop test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            mcpwm_start_stop_test(i, j);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_deadtime_test(mcpwm_unit_t unit, mcpwm_timer_t timer)
{
    mcpwm_setup_testbench(unit, timer, 1000, 50.0); // Period: 1000us, 1ms
    mcpwm_deadtime_type_t deadtime_type[] = {MCPWM_BYPASS_RED, MCPWM_BYPASS_FED, MCPWM_ACTIVE_HIGH_MODE,
                                             MCPWM_ACTIVE_LOW_MODE, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, MCPWM_ACTIVE_LOW_COMPLIMENT_MODE,
                                             MCPWM_ACTIVE_RED_FED_FROM_PWMXA, MCPWM_ACTIVE_RED_FED_FROM_PWMXB
                                            };

    for (size_t i = 0; i < sizeof(deadtime_type) / sizeof(deadtime_type[0]); i++) {
        mcpwm_stop(unit, timer);
        usleep(10000);
        mcpwm_deadtime_enable(unit, timer, deadtime_type[i], 1000, 1000);
        mcpwm_start(unit, timer);
        vTaskDelay(pdMS_TO_TICKS(100));
        mcpwm_deadtime_disable(unit, timer);
    }
    mcpwm_stop(unit, timer);
}

TEST_CASE("MCPWM deadtime test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            mcpwm_deadtime_test(i, j);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_carrier_test(mcpwm_unit_t unit, mcpwm_timer_t timer, mcpwm_carrier_out_ivt_t invert_or_not,
                               uint8_t period, uint8_t duty, uint8_t os_width)
{
    uint32_t pulse_number = 0;

    mcpwm_setup_testbench(unit, timer, 1000, 50.0);
    mcpwm_set_signal_high(unit, timer, MCPWM_GEN_A);
    mcpwm_set_signal_high(unit, timer, MCPWM_GEN_B);
    TEST_ESP_OK(mcpwm_carrier_enable(unit, timer));
    TEST_ESP_OK(mcpwm_carrier_set_period(unit, timer, period));   //carrier revolution
    TEST_ESP_OK(mcpwm_carrier_set_duty_cycle(unit, timer, duty)); // carrier duty
    TEST_ESP_OK(mcpwm_carrier_output_invert(unit, timer, invert_or_not));
    TEST_ESP_OK(mcpwm_carrier_oneshot_mode_enable(unit, timer, os_width));
    vTaskDelay(pdMS_TO_TICKS(100));

    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMA_PCNT_UNIT, 10);
    TEST_ASSERT_INT_WITHIN(50, 2500, pulse_number);
    usleep(10000);
    pulse_number = mcpwm_pcnt_get_pulse_number(TEST_PWMB_PCNT_UNIT, 10);
    TEST_ASSERT_INT_WITHIN(50, 2500, pulse_number);

    TEST_ESP_OK(mcpwm_carrier_disable(unit, timer));
    TEST_ESP_OK(mcpwm_stop(unit, timer));
}

TEST_CASE("MCPWM carrier test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            // carrier should be 10MHz/8/(4+1) = 250KHz, (10MHz is the group resolution, it's fixed in the driver), carrier duty cycle is 4/8 = 50%
            mcpwm_carrier_test(i, j, MCPWM_CARRIER_OUT_IVT_DIS, 4, 4, 3);
            mcpwm_carrier_test(i, j, MCPWM_CARRIER_OUT_IVT_EN, 4, 4, 3);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_check_generator_level_on_fault(mcpwm_action_on_pwmxa_t action_a, mcpwm_action_on_pwmxb_t action_b)
{
    if (action_a == MCPWM_ACTION_FORCE_LOW) {
        TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_PWMA_GPIO));
    } else if (action_a == MCPWM_ACTION_FORCE_HIGH) {
        TEST_ASSERT_EQUAL(1, gpio_get_level(TEST_PWMA_GPIO));
    }

    if (action_b == MCPWM_ACTION_FORCE_LOW) {
        TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_PWMB_GPIO));
    } else if (action_b == MCPWM_ACTION_FORCE_HIGH) {
        TEST_ASSERT_EQUAL(1, gpio_get_level(TEST_PWMB_GPIO));
    }
}

static void mcpwm_fault_cbc_test(mcpwm_unit_t unit, mcpwm_timer_t timer)
{
    mcpwm_action_on_pwmxa_t action_a[] = {MCPWM_ACTION_FORCE_LOW, MCPWM_ACTION_FORCE_HIGH};
    mcpwm_action_on_pwmxb_t action_b[] = {MCPWM_ACTION_FORCE_LOW, MCPWM_ACTION_FORCE_HIGH};

    mcpwm_fault_signal_t fault_sig = fault_sig_array[timer];
    mcpwm_io_signals_t fault_io_sig = fault_io_sig_array[timer];

    mcpwm_setup_testbench(unit, timer, 1000, 50.0);
    TEST_ESP_OK(test_mcpwm_gpio_init(unit, fault_io_sig, TEST_FAULT_GPIO));
    gpio_set_level(TEST_FAULT_GPIO, 0);
    TEST_ESP_OK(mcpwm_fault_init(unit, MCPWM_HIGH_LEVEL_TGR, fault_sig));
    for (int i = 0; i < sizeof(action_a) / sizeof(action_a[0]); i++) {
        for (int j = 0; j < sizeof(action_b) / sizeof(action_b[0]); j++) {
            TEST_ESP_OK(mcpwm_fault_set_cyc_mode(unit, timer, fault_sig, action_a[i], action_b[j]));
            gpio_set_level(TEST_FAULT_GPIO, 1); // trigger the fault event
            usleep(10000);
            mcpwm_check_generator_level_on_fault(action_a[i], action_b[j]);
            gpio_set_level(TEST_FAULT_GPIO, 0); // remove the fault signal
            usleep(10000);
        }
    }
    TEST_ESP_OK(mcpwm_fault_deinit(unit, fault_sig));
}

TEST_CASE("MCPWM fault cbc test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            mcpwm_fault_cbc_test(i, j);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_fault_ost_test(mcpwm_unit_t unit, mcpwm_timer_t timer)
{
    mcpwm_action_on_pwmxa_t action_a[] = {MCPWM_ACTION_FORCE_LOW, MCPWM_ACTION_FORCE_HIGH};
    mcpwm_action_on_pwmxb_t action_b[] = {MCPWM_ACTION_FORCE_LOW, MCPWM_ACTION_FORCE_HIGH};

    mcpwm_fault_signal_t fault_sig = fault_sig_array[timer];
    mcpwm_io_signals_t fault_io_sig = fault_io_sig_array[timer];

    mcpwm_setup_testbench(unit, timer, 1000, 50.0);
    TEST_ESP_OK(test_mcpwm_gpio_init(unit, fault_io_sig, TEST_FAULT_GPIO));
    gpio_set_level(TEST_FAULT_GPIO, 0);
    TEST_ESP_OK(mcpwm_fault_init(unit, MCPWM_HIGH_LEVEL_TGR, fault_sig));
    for (int i = 0; i < sizeof(action_a) / sizeof(action_a[0]); i++) {
        for (int j = 0; j < sizeof(action_b) / sizeof(action_b[0]); j++) {
            TEST_ESP_OK(mcpwm_fault_set_oneshot_mode(unit, timer, fault_sig, action_a[i], action_b[j]));
            gpio_set_level(TEST_FAULT_GPIO, 1); // trigger the fault event
            usleep(10000);
            mcpwm_check_generator_level_on_fault(action_a[i], action_b[j]);
            gpio_set_level(TEST_FAULT_GPIO, 0); // remove the fault signal
            usleep(10000);
        }
    }
    TEST_ESP_OK(mcpwm_fault_deinit(unit, fault_sig));
}

TEST_CASE("MCPWM fault ost test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            mcpwm_fault_ost_test(i, j);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_sync_test(mcpwm_unit_t unit, mcpwm_timer_t timer)
{
    mcpwm_sync_signal_t sync_sig = sync_sig_array[timer];
    mcpwm_io_signals_t sync_io_sig = sync_io_sig_array[timer];

    mcpwm_setup_testbench(unit, timer, 1000, 50.0);
    TEST_ESP_OK(test_mcpwm_gpio_init(unit, sync_io_sig, TEST_SYNC_GPIO));
    gpio_set_level(TEST_SYNC_GPIO, 0);

    TEST_ESP_OK(mcpwm_sync_enable(unit, timer, sync_sig, 200));
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(TEST_SYNC_GPIO, 1); // trigger an external sync event
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ESP_OK(mcpwm_sync_disable(unit, timer));
    TEST_ESP_OK(mcpwm_stop(unit, timer));
}

TEST_CASE("MCPWM timer sync test", "[mcpwm]")
{
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            mcpwm_sync_test(i, j);
        }
    }
}

// -------------------------------------------------------------------------------------

static void mcpwm_capture_test(mcpwm_unit_t unit, mcpwm_capture_signal_t cap_chan)
{
    typedef struct {
        mcpwm_unit_t unit;
        TaskHandle_t task_hdl;
    } test_capture_callback_data_t;

    void test_mcpwm_intr_handler(void *arg) {
        BaseType_t high_task_wakeup = pdFALSE;
        test_capture_callback_data_t *cb_data = (test_capture_callback_data_t *)arg;
        uint32_t status = MCPWM[cb_data->unit]->int_st.val;
        MCPWM[cb_data->unit]->int_clr.val = status;
        vTaskNotifyGiveFromISR(cb_data->task_hdl, &high_task_wakeup);
        if (high_task_wakeup == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }

    intr_handle_t mcpwm_intr = NULL;
    test_capture_callback_data_t callback_data = {
        .unit = unit,
        .task_hdl = xTaskGetCurrentTaskHandle(),
    };

    //each timer test the capture sig with the same id with it.
    mcpwm_io_signals_t cap_io = cap_io_sig_array[cap_chan];
    mcpwm_capture_signal_t cap_sig = cap_sig_array[cap_chan];

    TEST_ESP_OK(test_mcpwm_gpio_init(unit, cap_io, TEST_CAP_GPIO));
    TEST_ESP_OK(mcpwm_capture_enable(unit, cap_sig, MCPWM_POS_EDGE, 0));
    TEST_ESP_OK(mcpwm_isr_register(unit, test_mcpwm_intr_handler, &callback_data, 0, &mcpwm_intr));
    // generate an posage
    gpio_set_level(TEST_CAP_GPIO, 0);
    gpio_set_level(TEST_CAP_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(40)));
    uint32_t cap_val0 = mcpwm_capture_signal_get_value(unit, cap_chan);
    // generate another posage
    gpio_set_level(TEST_CAP_GPIO, 0);
    gpio_set_level(TEST_CAP_GPIO, 1);
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(40)));
    uint32_t cap_val1 = mcpwm_capture_signal_get_value(unit, cap_chan);
    // capture clock source is APB (80MHz), 100ms means 8000000 ticks
    TEST_ASSERT_UINT_WITHIN(100000, 8000000, cap_val1 - cap_val0);

    TEST_ESP_OK(mcpwm_capture_disable(unit, cap_sig));
    TEST_ESP_OK(esp_intr_free(mcpwm_intr));
}

TEST_CASE("MCPWM capture test", "[mcpwm]")
{
    // we assume each group has one capture timer
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; j++) {
            mcpwm_capture_test(i, j);
        }
    }
}

#endif // SOC_MCPWM_SUPPORTED
