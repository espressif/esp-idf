/**
 * To test PWM, use the PCNT to calculateit to judge it work right or not.
 * e.g: judge the start and stop.
 *      If started right, the PCNT will count the pulse.
 *      If stopped right, the PCNT will count no pulse.
 *
 *
 * test environment UT_T1_MCPWM:
 * 1. connect GPIO4 to GPIO5
 * 2. connect GPIO13 to GPIO12
 * 3. connect GPIO27 to GPIO14
 *
 * all of case separate different timer to test in case that one case cost too much time
 */
#include <stdio.h>
#include "esp_system.h"
#include "driver/mcpwm.h"
#include "driver/pcnt.h"
#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/mcpwm_periph.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/rtc.h"

#define GPIO_PWMA_OUT  4
#define GPIO_PWMB_OUT  13
#define GPIO_CAP_IN   27
#define GPIO_SYNC_IN   27
#define GPIO_FAULT_IN 27

#define CAP_SIG_NUM 14
#define SYN_SIG_NUM 14
#define FAULT_SIG_NUM 14

#define GPIO_PWMA_PCNT_INPUT 5
#define GPIO_PWMB_PCNT_INPUT 12

#define PCNT_CTRL_FLOATING_IO1 25
#define PCNT_CTRL_FLOATING_IO2 26

#define CAP0_INT_EN BIT(27)
#define CAP1_INT_EN BIT(28)
#define CAP2_INT_EN BIT(29)

#define INITIAL_DUTY 10.0
#define MCPWM_GPIO_INIT 0


#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000

static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1};

static xQueueHandle cap_queue;
static volatile int cap0_times = 0;
static volatile int cap1_times = 0;
static volatile int cap2_times = 0;

typedef struct {
    uint32_t capture_signal;
    mcpwm_capture_signal_t sel_cap_signal;
} capture;

// universal settings of mcpwm
static void mcpwm_basic_config(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer)
{
    mcpwm_gpio_init(unit, mcpwm_a, GPIO_PWMA_OUT);
    mcpwm_gpio_init(unit, mcpwm_b, GPIO_PWMB_OUT);
    mcpwm_config_t pwm_config = {
        .frequency = 1000,
        .cmpr_a = 50.0,  //duty cycle of PWMxA = 50.0%
        .cmpr_b = 50.0,  //duty cycle of PWMxb = 50.0%
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    mcpwm_init(unit, timer, &pwm_config);
}

static void pcnt_init(int pulse_gpio_num, int ctrl_gpio_num)
{
    pcnt_config_t pcnt_config = {
       .pulse_gpio_num = pulse_gpio_num,
       .ctrl_gpio_num = ctrl_gpio_num,
       .channel = PCNT_CHANNEL_0,
       .unit = PCNT_UNIT_0,
       .pos_mode = PCNT_COUNT_INC,
       .neg_mode = PCNT_COUNT_DIS,
       .lctrl_mode = PCNT_MODE_REVERSE,
       .hctrl_mode = PCNT_MODE_KEEP,
       .counter_h_lim = HIGHEST_LIMIT,
       .counter_l_lim = LOWEST_LIMIT,
   };
   TEST_ESP_OK(pcnt_unit_config(&pcnt_config));
}

// initialize the PCNT
// PCNT is used to count the MCPWM pulse
static int16_t pcnt_count(int pulse_gpio_num, int ctrl_gpio_num, int last_time)
{
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = pulse_gpio_num,
        .ctrl_gpio_num = ctrl_gpio_num,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = HIGHEST_LIMIT,
        .counter_l_lim = LOWEST_LIMIT,
    };
    TEST_ESP_OK(pcnt_unit_config(&pcnt_config));
    int16_t test_counter;
    TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("COUNT: %d\n", test_counter);
    vTaskDelay(last_time / portTICK_RATE_MS);
    TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &test_counter));
    printf("COUNT: %d\n", test_counter);
    return test_counter;
}

// judge the counting value right or not in specific error
static void judge_count_value(int allow_error ,int expect_freq)
{
    int16_t countA, countB;

    countA = pcnt_count(GPIO_PWMA_PCNT_INPUT, PCNT_CTRL_FLOATING_IO1, 1000);
    countB = pcnt_count(GPIO_PWMB_PCNT_INPUT, PCNT_CTRL_FLOATING_IO2, 1000);

    TEST_ASSERT_INT16_WITHIN(allow_error, countA, expect_freq);
    TEST_ASSERT_INT16_WITHIN(allow_error, countB, expect_freq);
}

// test the duty configuration
static void timer_duty_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer)
{
    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);
    vTaskDelay(1000 / portTICK_RATE_MS); // stay this status for a while so that can view its waveform by logic anylyzer

    TEST_ESP_OK(mcpwm_set_duty(unit, timer, MCPWM_OPR_A, (INITIAL_DUTY * 1)));
    TEST_ESP_OK(mcpwm_set_duty(unit, timer, MCPWM_OPR_B, (INITIAL_DUTY * 2)));

    TEST_ASSERT_EQUAL_INT(mcpwm_get_duty(unit, timer, MCPWM_OPR_A), INITIAL_DUTY * 1);
    TEST_ASSERT_EQUAL_INT(mcpwm_get_duty(unit, timer, MCPWM_OPR_B), INITIAL_DUTY * 2);
    vTaskDelay(1000 / portTICK_RATE_MS);  // stay this status for a while so that can view its waveform by logic anylyzer

    mcpwm_set_duty(unit, timer, MCPWM_OPR_A, 55.5f);
    mcpwm_set_duty_type(unit, timer, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    printf("mcpwm check = %f\n", mcpwm_get_duty(unit, timer, MCPWM_OPR_A));

    mcpwm_set_duty_in_us(unit, timer, MCPWM_OPR_B, 500);
    printf("mcpwm check = %f\n", mcpwm_get_duty(unit, timer, MCPWM_OPR_B));
    vTaskDelay(1000 / portTICK_RATE_MS);  // stay this status for a while so that can view its waveform by logic anylyzer
}

// test the start and stop function work or not
static void start_stop_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer)
{
    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);
    judge_count_value(2, 1000);
    TEST_ESP_OK(mcpwm_stop(unit, timer));
    vTaskDelay(10 / portTICK_RATE_MS); // wait for a while, stop totally
    judge_count_value(0, 0);
    TEST_ESP_OK(mcpwm_start(unit, timer));
    vTaskDelay(10 / portTICK_RATE_MS); // wait for a while, start totally
    judge_count_value(2, 1000);
}

// test the deadtime
static void deadtime_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer)
{
    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);
    mcpwm_deadtime_type_t deadtime_type[8] = {MCPWM_BYPASS_RED, MCPWM_BYPASS_FED, MCPWM_ACTIVE_HIGH_MODE,
            MCPWM_ACTIVE_LOW_MODE, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, MCPWM_ACTIVE_LOW_COMPLIMENT_MODE,
            MCPWM_ACTIVE_RED_FED_FROM_PWMXA, MCPWM_ACTIVE_RED_FED_FROM_PWMXB};

    for(int i=0; i<8; i++) {
        mcpwm_deadtime_enable(unit, timer, deadtime_type[i], 1000, 1000);
        vTaskDelay(1000 / portTICK_RATE_MS);
        mcpwm_deadtime_disable(unit, timer);
    }
}

/**
 * there are two kind of methods to set the carrier:
 * 1. by mcpwm_carrier_init
 * 2. by different single setting function
 */
static void carrier_with_set_function_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer,
        mcpwm_carrier_out_ivt_t invert_or_not, uint8_t period, uint8_t duty, uint8_t os_width)
{
    // no inversion and no one shot
    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);
    TEST_ESP_OK(mcpwm_carrier_enable(unit, timer));
    TEST_ESP_OK(mcpwm_carrier_set_period(unit, timer, period)); //carrier revolution
    TEST_ESP_OK(mcpwm_carrier_set_duty_cycle(unit, timer, duty)); // carrier duty
    judge_count_value(500, 50000/5.6);

    // with invert
    TEST_ESP_OK(mcpwm_carrier_output_invert(unit, timer, invert_or_not));
    vTaskDelay(2000 / portTICK_RATE_MS);
}

static void carrier_with_configuration_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer,
        mcpwm_carrier_os_t oneshot_or_not, mcpwm_carrier_out_ivt_t invert_or_not, uint8_t period, uint8_t duty, uint8_t os_width)
{
    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);

    mcpwm_carrier_config_t chop_config;
    chop_config.carrier_period = period;         //carrier period = (period + 1)*800ns
    chop_config.carrier_duty = duty;           // carrier duty cycle, carrier_duty should be less then 8(increment every 12.5%).  carrier duty = (3)*12.5%
    chop_config.carrier_os_mode = oneshot_or_not; //If one shot mode is enabled then set pulse width, if disabled no need to set pulse width
    chop_config.pulse_width_in_os = os_width;      //pulse width of first pulse in one shot mode = (carrier period)*(pulse_width_in_os + 1), should be less then 16.first pulse width = (3 + 1)*carrier_period
    chop_config.carrier_ivt_mode = invert_or_not; //output signal inversion enable
    mcpwm_carrier_init(unit, timer, &chop_config);

    if(!oneshot_or_not) {
        // the pwm frequency is 1000
        // the carrrier duration in one second is 500ms
        // the carrier wave count is: 500ms/carrier_period = 500ms/(period + 1)*800ns
        //                                                 = 62500/(period + 1)
        judge_count_value(500, 62500/(period + 1));
    } else {
        judge_count_value(500, 40000/((period + 1)));  // (500-500*0.125*3)/((period + 1)*800)
    }

    TEST_ESP_OK(mcpwm_carrier_disable(unit, timer));
    judge_count_value(2, 1000);
}

static void get_action_level(mcpwm_fault_input_level_t input_sig, mcpwm_action_on_pwmxa_t action_a, mcpwm_action_on_pwmxb_t action_b, int freq, int allow_err)
{
    if(action_a == MCPWM_NO_CHANGE_IN_MCPWMXA) {
        TEST_ASSERT_INT16_WITHIN(allow_err, pcnt_count(GPIO_PWMA_PCNT_INPUT, PCNT_CTRL_FLOATING_IO1, 1000), freq);
    } else if(action_a == MCPWM_FORCE_MCPWMXA_LOW) {
        TEST_ASSERT(gpio_get_level(GPIO_PWMA_PCNT_INPUT) == 0);
    } else if(action_a == MCPWM_FORCE_MCPWMXA_HIGH) {
        TEST_ASSERT(gpio_get_level(GPIO_PWMA_PCNT_INPUT) == 1);
    }else {
        int level =  gpio_get_level(GPIO_PWMA_PCNT_INPUT);
        vTaskDelay(100 / portTICK_RATE_MS);
        TEST_ASSERT(gpio_get_level(GPIO_PWMA_PCNT_INPUT) == level);
    }

    if(action_b == MCPWM_NO_CHANGE_IN_MCPWMXB) {
        TEST_ASSERT_INT16_WITHIN(allow_err, pcnt_count(GPIO_PWMB_PCNT_INPUT, PCNT_CTRL_FLOATING_IO1, 1000), freq);
    } else if(action_b == MCPWM_FORCE_MCPWMXB_LOW) {
        TEST_ASSERT(gpio_get_level(GPIO_PWMB_PCNT_INPUT) == 0);
    } else if(action_b == MCPWM_FORCE_MCPWMXB_HIGH) {
        TEST_ASSERT(gpio_get_level(GPIO_PWMB_PCNT_INPUT) == 1);
    }else {
        int level =  gpio_get_level(GPIO_PWMB_PCNT_INPUT);
        vTaskDelay(100 / portTICK_RATE_MS);
        TEST_ASSERT(gpio_get_level(GPIO_PWMB_PCNT_INPUT) == level);
    }
}

// test the fault event
static void cycle_fault_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer,
        mcpwm_fault_signal_t fault_sig, mcpwm_fault_input_level_t input_sig, mcpwm_io_signals_t fault_io,
        mcpwm_action_on_pwmxa_t action_a, mcpwm_action_on_pwmxb_t action_b)
{
    gpio_config_t gp;
    gp.intr_type = GPIO_INTR_DISABLE;
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = (1 << FAULT_SIG_NUM);
    gpio_config(&gp); // gpio configure should be more previous than mcpwm configuration
    gpio_set_level(FAULT_SIG_NUM, !input_sig);

    pcnt_init(GPIO_PWMA_PCNT_INPUT, PCNT_CTRL_FLOATING_IO1);
    pcnt_init(GPIO_PWMB_PCNT_INPUT, PCNT_CTRL_FLOATING_IO2);

    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);
    mcpwm_gpio_init(unit, fault_io, GPIO_FAULT_IN);

    // cycle mode, it can be triggered more than once
    printf("cyc test:\n");
    gpio_set_level(FAULT_SIG_NUM, !input_sig);
    TEST_ESP_OK(mcpwm_fault_init(unit, input_sig, fault_sig));
    TEST_ESP_OK(mcpwm_fault_set_cyc_mode(unit, timer, fault_sig, action_a, action_b));
    vTaskDelay(1000 / portTICK_RATE_MS);
    gpio_set_level(FAULT_SIG_NUM, input_sig); // trigger the fault event
    vTaskDelay(1000 / portTICK_RATE_MS);
    get_action_level(input_sig, action_a, action_b, 1000, 5);
    TEST_ESP_OK(mcpwm_fault_deinit(unit, fault_sig));
}

static void oneshot_fault_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer,
        mcpwm_fault_signal_t fault_sig, mcpwm_fault_input_level_t input_sig, mcpwm_io_signals_t fault_io,
        mcpwm_action_on_pwmxa_t action_a, mcpwm_action_on_pwmxb_t action_b)
{
    gpio_config_t gp;
    gp.intr_type = GPIO_INTR_DISABLE;
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = (1 << FAULT_SIG_NUM);
    gpio_config(&gp); // gpio configure should be more previous than mcpwm configuration
    gpio_set_level(FAULT_SIG_NUM, !input_sig);

    pcnt_init(GPIO_PWMA_PCNT_INPUT, PCNT_CTRL_FLOATING_IO1);
    pcnt_init(GPIO_PWMB_PCNT_INPUT, PCNT_CTRL_FLOATING_IO2);

    mcpwm_basic_config(unit, mcpwm_a, mcpwm_b, timer);
    mcpwm_gpio_init(unit, fault_io, GPIO_FAULT_IN);

    // one shot mode, it just can be triggered once
    TEST_ESP_OK(mcpwm_fault_init(unit, input_sig, fault_sig));
    TEST_ESP_OK(mcpwm_fault_set_oneshot_mode(unit, timer, fault_sig, action_a, action_b));
    vTaskDelay(1000 / portTICK_RATE_MS);
    // trigger it
    gpio_set_level(FAULT_SIG_NUM, input_sig);
    vTaskDelay(1000 / portTICK_RATE_MS);
    get_action_level(input_sig, action_a, action_b, 1000, 5);
    TEST_ESP_OK(mcpwm_fault_deinit(unit, fault_sig));
}

// test the sync event
static void sync_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_timer_t timer,
        mcpwm_sync_signal_t sync_sig, mcpwm_io_signals_t sync_io)
{
    gpio_config_t gp;
    gp.intr_type = GPIO_INTR_DISABLE;
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = (1 << SYN_SIG_NUM);
    gpio_config(&gp);
    gpio_set_level(SYN_SIG_NUM, 0);

    mcpwm_gpio_init(unit, mcpwm_a, GPIO_PWMA_OUT);
    mcpwm_gpio_init(unit, mcpwm_b, GPIO_PWMB_OUT);
    mcpwm_gpio_init(unit, sync_io, GPIO_SYNC_IN);
    mcpwm_config_t pwm_config = {
        .frequency = 1000,
        .cmpr_a = 50.0,  //duty cycle of PWMxA = 50.0%
        .cmpr_b = 50.0,  //duty cycle of PWMxb = 50.0%
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    mcpwm_init(unit, timer, &pwm_config);
    gpio_pulldown_en(GPIO_SYNC_IN);

    mcpwm_sync_enable(unit, timer, sync_sig, 200);
    gpio_set_level(SYN_SIG_NUM, 1);
    vTaskDelay(2000 / portTICK_RATE_MS);
    mcpwm_sync_disable(unit, timer);
    vTaskDelay(2000 / portTICK_RATE_MS);
}

/**
 * use interruption to test the capture event
 * there are two kinds of methods to trigger the capture event:
 * 1. high level trigger
 * 2. low level trigger
 */
static volatile int flag = 0;

// once capture event happens, will show it
static void disp_captured_signal(void *arg)
{

    uint32_t *current_cap_value = (uint32_t *)malloc(sizeof(uint32_t) * CAP_SIG_NUM);
    uint32_t *previous_cap_value = (uint32_t *)malloc(sizeof(uint32_t) * CAP_SIG_NUM);
    capture evt;
    for (int i=0; i<1000; i++) {
        xQueueReceive(cap_queue, &evt, portMAX_DELAY);
        if (evt.sel_cap_signal == MCPWM_SELECT_CAP0) {
            current_cap_value[0] = evt.capture_signal - previous_cap_value[0];
            previous_cap_value[0] = evt.capture_signal;
            current_cap_value[0] = (current_cap_value[0] / 10000) * (10000000000 / rtc_clk_apb_freq_get());
            printf("CAP0 : %d us\n", current_cap_value[0]);
            cap0_times++;
        }
        if (evt.sel_cap_signal == MCPWM_SELECT_CAP1) {
            current_cap_value[1] = evt.capture_signal - previous_cap_value[1];
            previous_cap_value[1] = evt.capture_signal;
            current_cap_value[1] = (current_cap_value[1] / 10000) * (10000000000 / rtc_clk_apb_freq_get());
            printf("CAP1 : %d us\n", current_cap_value[1]);
            cap1_times++;
        }
        if (evt.sel_cap_signal == MCPWM_SELECT_CAP2) {
            current_cap_value[2] = evt.capture_signal -  previous_cap_value[2];
            previous_cap_value[2] = evt.capture_signal;
            current_cap_value[2] = (current_cap_value[2] / 10000) * (10000000000 / rtc_clk_apb_freq_get());
            printf("CAP2 : %d us\n", current_cap_value[2]);
            cap2_times++;
        }
    }
    free(current_cap_value);
    free(previous_cap_value);
    vTaskDelete(NULL);
}

// mcpwm event
static void IRAM_ATTR isr_handler(void *arg)
{
	mcpwm_unit_t unit = (mcpwm_unit_t)arg;
    uint32_t mcpwm_intr_status;
    capture evt;
    mcpwm_intr_status = MCPWM[unit]->int_st.val; //Read interrupt status
    if (mcpwm_intr_status & CAP0_INT_EN) { //Check for interrupt on rising edge on CAP0 signal
        evt.capture_signal = mcpwm_capture_signal_get_value(unit, MCPWM_SELECT_CAP0); //get capture signal counter value
        evt.sel_cap_signal = MCPWM_SELECT_CAP0;
        xQueueSendFromISR(cap_queue, &evt, NULL);
    }
    if (mcpwm_intr_status & CAP1_INT_EN) { //Check for interrupt on rising edge on CAP0 signal
        evt.capture_signal = mcpwm_capture_signal_get_value(unit, MCPWM_SELECT_CAP1); //get capture signal counter value
        evt.sel_cap_signal = MCPWM_SELECT_CAP1;
        xQueueSendFromISR(cap_queue, &evt, NULL);
    }
    if (mcpwm_intr_status & CAP2_INT_EN) { //Check for interrupt on rising edge on CAP0 signal
        evt.capture_signal = mcpwm_capture_signal_get_value(unit, MCPWM_SELECT_CAP2); //get capture signal counter value
        evt.sel_cap_signal = MCPWM_SELECT_CAP2;
        xQueueSendFromISR(cap_queue, &evt, NULL);
    }
    MCPWM[unit]->int_clr.val = mcpwm_intr_status;
}

// the produce the capture triggering signal to trigger the capture event
static void gpio_test_signal(void *arg)
{

    printf("intializing test signal...\n");
    gpio_config_t gp;
    gp.intr_type = GPIO_INTR_DISABLE;
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = 1<<CAP_SIG_NUM;
    gpio_config(&gp);
    for (int i=0; i<1000; i++) {
        //here the period of test signal is 20ms
        gpio_set_level(CAP_SIG_NUM, 1); //Set high
        vTaskDelay(10);             //delay of 10ms
        gpio_set_level(CAP_SIG_NUM, 0); //Set low
        vTaskDelay(10);         //delay of 10ms
    }
    flag = 1;
    vTaskDelete(NULL);
}

// capture event test function
static void capture_test(mcpwm_unit_t unit, mcpwm_io_signals_t mcpwm_a, mcpwm_io_signals_t mcpwm_b, mcpwm_io_signals_t cap_io, mcpwm_timer_t timer,
        mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge)
{
	// initialize the capture times
    cap0_times = 0;
    cap1_times = 0;
    cap2_times = 0;

    mcpwm_gpio_init(unit, cap_io, GPIO_CAP_IN);

    cap_queue = xQueueCreate(1, sizeof(capture));
    xTaskCreate(disp_captured_signal, "mcpwm_config", 4096, (void *)unit, 5, NULL);
    xTaskCreate(gpio_test_signal, "gpio_test_signal", 4096, NULL, 5, NULL);
    mcpwm_capture_enable(unit, cap_sig, cap_edge, 0);
    MCPWM[unit]->int_ena.val = CAP0_INT_EN | CAP1_INT_EN | CAP2_INT_EN;  //Enable interrupt on  CAP0, CAP1 and CAP2 signal
    mcpwm_isr_register(unit, isr_handler, (void *)unit, ESP_INTR_FLAG_IRAM, NULL);

    while(flag != 1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    if(cap_sig == MCPWM_SELECT_CAP0) {
    	TEST_ASSERT(1000 == cap0_times);
    } else if(cap_sig == MCPWM_SELECT_CAP1) {
    	TEST_ASSERT(1000 == cap1_times);
    }else {
    	TEST_ASSERT(1000 == cap2_times);
    }
    flag = 0; // set flag to 0 that it can be used in other case
    mcpwm_capture_disable(unit, cap_sig);
}

/**
 *  duty test:
 *  1. mcpwm_set_duty
 *  2. mcpwm_get_duty
 *
 *  This case's phenomenon should be viewed by logic analyzer
 *  so set it ignore
 */
TEST_CASE("MCPWM timer0 duty test and each timer works or not test(logic analyzer)", "[mcpwm][ignore]")
{
    timer_duty_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0);
    timer_duty_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0);
}

TEST_CASE("MCPWM timer1 duty test and each timer works or not test(logic analyzer)", "[mcpwm][ignore]")
{
    timer_duty_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1);
    timer_duty_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1);
}
TEST_CASE("MCPWM timer2 duty test and each timer works or not test(logic analyzer)", "[mcpwm][ignore]")
{
    timer_duty_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2);
    timer_duty_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2);
}

// the deadtime configuration test
// use the logic analyzer to make sure it goes right
TEST_CASE("MCPWM timer0 deadtime configuration(logic analyzer)", "[mcpwm][ignore]")
{
    deadtime_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0);
    deadtime_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0);
}

TEST_CASE("MCPWM timer1 deadtime configuration(logic analyzer)", "[mcpwm][ignore]")
{
    deadtime_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1);
    deadtime_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1);
}

TEST_CASE("MCPWM timer2 deadtime configuration(logic analyzer)", "[mcpwm][ignore]")
{
    deadtime_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2);
    deadtime_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2);
}

TEST_CASE("MCPWM timer0 start and stop test", "[mcpwm][test_env=UT_T1_MCPWM]")
{
    start_stop_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0);
    start_stop_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0);
}

// mcpwm start and stop test
TEST_CASE("MCPWM timer1 start and stop test", "[mcpwm][test_env=UT_T1_MCPWM]")
{
    start_stop_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1);
    start_stop_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1);
}

TEST_CASE("MCPWM timer2 start and stop test", "[mcpwm][test_env=UT_T1_MCPWM]")
{
    start_stop_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2);
    start_stop_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2);
}

TEST_CASE("MCPWM timer0 carrier test with set function", "[mcpwm][test_env=UT_T1_MCPWM]")
{
    carrier_with_set_function_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
            MCPWM_CARRIER_OUT_IVT_DIS, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
            MCPWM_CARRIER_OUT_IVT_EN, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
            MCPWM_CARRIER_OUT_IVT_DIS, 6, 3, 3);
            carrier_with_set_function_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
            MCPWM_CARRIER_OUT_IVT_EN, 6, 3, 3);
}

TEST_CASE("MCPWM timer1 carrier test with set function", "[mcpwm][test_env=UT_T1_MCPWM]")
{
    carrier_with_set_function_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
            MCPWM_CARRIER_OUT_IVT_DIS, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
            MCPWM_CARRIER_OUT_IVT_EN, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
            MCPWM_CARRIER_OUT_IVT_DIS, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
            MCPWM_CARRIER_OUT_IVT_EN, 6, 3, 3);
}

TEST_CASE("MCPWM timer2 carrier test with set function", "[mcpwm][test_env=UT_T1_MCPWM]")
{
    carrier_with_set_function_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
            MCPWM_CARRIER_OUT_IVT_DIS, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
            MCPWM_CARRIER_OUT_IVT_EN, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
            MCPWM_CARRIER_OUT_IVT_DIS, 6, 3, 3);
    carrier_with_set_function_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
            MCPWM_CARRIER_OUT_IVT_EN, 6, 3, 3);
}

TEST_CASE("MCPWM timer0 carrier test with configuration function", "[mcpwm][test_env=UT_T1_MCPWM][timeout=120]")
{
    mcpwm_carrier_os_t oneshot[2] = {MCPWM_ONESHOT_MODE_DIS, MCPWM_ONESHOT_MODE_EN};
    mcpwm_carrier_out_ivt_t invert[2] = {MCPWM_CARRIER_OUT_IVT_DIS, MCPWM_CARRIER_OUT_IVT_EN};
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++) {
            printf("i=%d, j=%d\n", i, j);
            carrier_with_configuration_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
                    oneshot[i], invert[j], 6, 3, 3);
            carrier_with_configuration_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
                    oneshot[i], invert[j], 6, 3, 3);
        }
    }
}

TEST_CASE("MCPWM timer1 carrier test with configuration function", "[mcpwm][test_env=UT_T1_MCPWM][timeout=120]")
{
    mcpwm_carrier_os_t oneshot[2] = {MCPWM_ONESHOT_MODE_DIS, MCPWM_ONESHOT_MODE_EN};
    mcpwm_carrier_out_ivt_t invert[2] = {MCPWM_CARRIER_OUT_IVT_DIS, MCPWM_CARRIER_OUT_IVT_EN};
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++) {
            carrier_with_configuration_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
                    oneshot[i], invert[j], 6, 3, 3);
            carrier_with_configuration_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
                    oneshot[i], invert[j], 6, 3, 3);
        }
    }
}

TEST_CASE("MCPWM timer2 carrier test with configuration function", "[mcpwm][test_env=UT_T1_MCPWM][timeout=120]")
{
    mcpwm_carrier_os_t oneshot[2] = {MCPWM_ONESHOT_MODE_DIS, MCPWM_ONESHOT_MODE_EN};
    mcpwm_carrier_out_ivt_t invert[2] = {MCPWM_CARRIER_OUT_IVT_DIS, MCPWM_CARRIER_OUT_IVT_EN};
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++) {
            carrier_with_configuration_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
                    oneshot[i], invert[j], 6, 3, 3);
            carrier_with_configuration_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
                    oneshot[i], invert[j], 6, 3, 3);
        }
    }
}

/**
 * Fault event:
 * Just support high level triggering
 * There are two types fault event:
 * 1. one-shot: it just can be triggered once, its effect is forever and it will never be changed although the fault signal change
 * 2. cycle: it can be triggered more than once, it will changed just as the fault signal changes. If set it triggered by high level,
 *           when the fault signal is high level, the event will be triggered. But the event will disappear as the fault signal disappears
 */

TEST_CASE("MCPWM timer0 cycle fault test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=180]")
{
    // API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++) {
            printf("i=%d, j=%d\n",i, j);
            cycle_fault_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
                    MCPWM_SELECT_F0, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_0,
                    action_a[i], action_b[j]);
            cycle_fault_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
                    MCPWM_SELECT_F0, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_0,
                    action_a[i], action_b[j]);
        }
    }
}

TEST_CASE("MCPWM timer1 cycle fault test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=180]")
{
    // API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++) {
        	cycle_fault_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
                    MCPWM_SELECT_F1, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_1,
                    action_a[i], action_b[j]);
        	cycle_fault_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
                    MCPWM_SELECT_F1, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_1,
                    action_a[i], action_b[j]);
        }
    }
}

TEST_CASE("MCPWM timer2 cycle fault test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=180][ignore]")
{
//    API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++) {
        	cycle_fault_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
                    MCPWM_SELECT_F2, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_2,
                    action_a[i], action_b[j]);
        	cycle_fault_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
                    MCPWM_SELECT_F2, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_2,
                    action_a[i], action_b[j]);
        }
    }
}

// to debug the "mcpwm_fault_deinit" case. The "MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXB_HIGH" scenario can work right
// however, the mcpwm_fault_deinit can not release the status after "MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXB_LOW" scenario
TEST_CASE("MCPWM timer0 one shot fault test single", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{
    // API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    oneshot_fault_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
    		MCPWM_SELECT_F0, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_0,
			action_a[0], action_b[2]);
}

// the mcpwm_fault_deinit can not release the status after "MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXB_LOW" scenario
// set it ignore
// same as the case "MCPWM timer1 one shot fault test" and case "MCPWM timer2 one shot fault test"
TEST_CASE("MCPWM timer0 one shot fault test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60][ignore]")
{
    // API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++) {
            printf("i=%d, j=%d\n",i, j);
            oneshot_fault_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
                    MCPWM_SELECT_F0, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_0,
                    action_a[i], action_b[j]);
            oneshot_fault_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0,
                    MCPWM_SELECT_F0, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_0,
                    action_a[i], action_b[j]);
        }
    }
}

TEST_CASE("MCPWM timer1 one shot fault test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60][ignore]")
{
    // API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++) {
        	oneshot_fault_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
                    MCPWM_SELECT_F1, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_1,
                    action_a[i], action_b[j]);
        	oneshot_fault_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1,
                    MCPWM_SELECT_F1, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_1,
                    action_a[i], action_b[j]);
        }
    }
}

TEST_CASE("MCPWM timer2 one shot fault test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60][ignore]")
{
//    API just supports the high level trigger now, so comment it
//    mcpwm_fault_input_level_t fault_input[2] = {MCPWM_LOW_LEVEL_TGR, MCPWM_HIGH_LEVEL_TGR};
    mcpwm_action_on_pwmxa_t action_a[4] = {MCPWM_NO_CHANGE_IN_MCPWMXA, MCPWM_FORCE_MCPWMXA_LOW, MCPWM_FORCE_MCPWMXA_HIGH, MCPWM_TOG_MCPWMXA};
    mcpwm_action_on_pwmxb_t action_b[4] = {MCPWM_NO_CHANGE_IN_MCPWMXB, MCPWM_FORCE_MCPWMXB_LOW, MCPWM_FORCE_MCPWMXB_HIGH, MCPWM_TOG_MCPWMXB};

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++) {
        	oneshot_fault_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
                    MCPWM_SELECT_F2, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_2,
                    action_a[i], action_b[j]);
        	oneshot_fault_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2,
                    MCPWM_SELECT_F2, MCPWM_HIGH_LEVEL_TGR, MCPWM_FAULT_2,
                    action_a[i], action_b[j]);
        }
    }
}

// need to view its phenomenon in logic analyzer
// set it ignore
TEST_CASE("MCPWM timer0 sync test(logic analyzer)", "[mcpwm][ignore]")
{
    sync_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_TIMER_0, MCPWM_SELECT_SYNC0, MCPWM_SYNC_0);
    TEST_ESP_OK(mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0)); // make sure can view the next sync signal clearly
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ESP_OK(mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0));
    sync_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_TIMER_0, MCPWM_SELECT_SYNC0, MCPWM_SYNC_0);
}

// need to view its phenomenon in logic analyzer
// set it ignore
TEST_CASE("MCPWM timer1 sync test(logic analyzer)", "[mcpwm][ignore]")
{
    sync_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_TIMER_1, MCPWM_SELECT_SYNC1, MCPWM_SYNC_1);
    TEST_ESP_OK(mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_1)); // make sure can view the next sync signal clearly
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ESP_OK(mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_1));
    sync_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_TIMER_1, MCPWM_SELECT_SYNC1, MCPWM_SYNC_1);
}

// need to view its phenomenon in logic analyzer
// set it ignore
TEST_CASE("MCPWM timer2 sync test(logic analyzer)", "[mcpwm][ignore]")
{
    sync_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_TIMER_2, MCPWM_SELECT_SYNC2, MCPWM_SYNC_2);
    TEST_ESP_OK(mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_2)); // make sure can view the next sync signal clearly
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ESP_OK(mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_2));
    sync_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_TIMER_2, MCPWM_SELECT_SYNC2, MCPWM_SYNC_2);
}

TEST_CASE("MCPWM unit0, timer0 capture test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{
    capture_test(MCPWM_UNIT_0, MCPWM0A, MCPWM0B, MCPWM_CAP_0, MCPWM_TIMER_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE);
}

TEST_CASE("MCPWM uni0, timer1 capture test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{
    capture_test(MCPWM_UNIT_0, MCPWM1A, MCPWM1B, MCPWM_CAP_1, MCPWM_TIMER_1, MCPWM_SELECT_CAP1, MCPWM_POS_EDGE);
}

TEST_CASE("MCPWM unit0, timer2 capture test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{

    capture_test(MCPWM_UNIT_0, MCPWM2A, MCPWM2B, MCPWM_CAP_2, MCPWM_TIMER_2, MCPWM_SELECT_CAP2, MCPWM_POS_EDGE);
}

TEST_CASE("MCPWM unit1, timer0 capture test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{
    capture_test(MCPWM_UNIT_1, MCPWM0A, MCPWM0B, MCPWM_CAP_0, MCPWM_TIMER_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE);
}

TEST_CASE("MCPWM unit1, timer1 capture test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{
    capture_test(MCPWM_UNIT_1, MCPWM1A, MCPWM1B, MCPWM_CAP_1, MCPWM_TIMER_1, MCPWM_SELECT_CAP1, MCPWM_POS_EDGE);
}

TEST_CASE("MCPWM unit1, timer2 capture test", "[mcpwm][test_env=UT_T1_MCPWM][timeout=60]")
{
    capture_test(MCPWM_UNIT_1, MCPWM2A, MCPWM2B, MCPWM_CAP_2, MCPWM_TIMER_2, MCPWM_SELECT_CAP2, MCPWM_POS_EDGE);
}

