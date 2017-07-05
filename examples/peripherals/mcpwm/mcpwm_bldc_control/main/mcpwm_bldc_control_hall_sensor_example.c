
/* MCPWM BLDC control Test code

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * The following examples uses mcpwm module to control bldc motor vary its speed continiously
 * The BLDC motor used for testing this code had sequence of 6-->4-->5-->1-->3-->2-->6-->4--> and so on
 * IR2136 3-ph bridge driver is used for testing this example code
 * User needs to make changes according to the motor and gate driver ic used
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "soc/rtc.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define INITIAL_DUTY 10.0   //initial duty cycle is 10.0%
#define MCPWM_GPIO_INIT 0    //select which function to use to initialize gpio signals

#define CAP_SIG_NUM 3   //three capture signals from HALL-A, HALL-B, HALL-C
#define CAP0_INT_EN BIT(27)  //Capture 0 interrupt bit
#define CAP1_INT_EN BIT(28)  //Capture 1 interrupt bit
#define CAP2_INT_EN BIT(29)  //Capture 2 interrupt bit

#define GPIO_PWM0A_OUT 15   //Set GPIO 15 as PWM0A
#define GPIO_PWM0B_OUT 02   //Set GPIO 02 as PWM0B
#define GPIO_PWM1A_OUT 00   //Set GPIO 00 as PWM1A
#define GPIO_PWM1B_OUT 04   //Set GPIO 04 as PWM1B
#define GPIO_PWM2A_OUT 16   //Set GPIO 16 as PWM2A
#define GPIO_PWM2B_OUT 17   //Set GPIO 17 as PWM2B
#define GPIO_CAP0_IN   25   //Set GPIO 25 as  CAP0
#define GPIO_CAP1_IN   26   //Set GPIO 26 as  CAP1
#define GPIO_CAP2_IN   27   //Set GPIO 27 as  CAP2

typedef struct {
    uint32_t capture_signal;
    mcpwm_capture_signal_t sel_cap_signal;
} capture;

static uint32_t hall_sensor_value = 0;
static uint32_t hall_sensor_previous = 0;

xQueueHandle cap_queue;

static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1};

static void mcpwm_example_gpio_initialize()
{
    printf("initializing mcpwm bldc control gpio...\n");
#if MCPWM_GPIO_INIT
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, GPIO_PWM1A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, GPIO_PWM1B_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2A, GPIO_PWM2A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2B, GPIO_PWM2B_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, GPIO_CAP0_IN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_1, GPIO_CAP1_IN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_2, GPIO_CAP2_IN);
#else
    mcpwm_pin_config_t pin_config = {
        .mcpwm0a_out_num = GPIO_PWM0A_OUT,
        .mcpwm0b_out_num = GPIO_PWM0B_OUT,
        .mcpwm1a_out_num = GPIO_PWM1A_OUT,
        .mcpwm1b_out_num = GPIO_PWM1B_OUT,
        .mcpwm2a_out_num = GPIO_PWM2A_OUT,
        .mcpwm2b_out_num = GPIO_PWM2B_OUT,
        .mcpwm_cap0_in_num   = GPIO_CAP0_IN,
        .mcpwm_cap1_in_num   = GPIO_CAP1_IN,
        .mcpwm_cap2_in_num   = GPIO_CAP2_IN,
        .mcpwm_sync0_in_num  = -1,  //Not used
        .mcpwm_sync1_in_num  = -1,  //Not used
        .mcpwm_sync2_in_num  = -1,  //Not used
        .mcpwm_fault0_in_num = -1,  //Not used
        .mcpwm_fault1_in_num = -1,  //Not used
        .mcpwm_fault2_in_num = -1   //Not used
    };
    mcpwm_set_pin(MCPWM_UNIT_0, &pin_config);
#endif
    gpio_pulldown_en(GPIO_CAP0_IN);    //Enable pull down on CAP0   signal
    gpio_pulldown_en(GPIO_CAP1_IN);    //Enable pull down on CAP1   signal
    gpio_pulldown_en(GPIO_CAP2_IN);    //Enable pull down on CAP2   signal
}

/**
 * @brief Set gpio 13, 12, 14  as our test signal of hall sensors, that generates high-low waveform continuously
 *        Attach this pins to GPIO 27, 26, 25 respectively for capture unit
 */
static void gpio_test_signal(void *arg)
{
    printf("intializing test signal...\n");
    gpio_config_t gp;
    gp.intr_type = GPIO_INTR_DISABLE;
    gp.mode = GPIO_MODE_OUTPUT;
    gp.pin_bit_mask = GPIO_SEL_13 | GPIO_SEL_12 | GPIO_SEL_14;
    gpio_config(&gp);
    while (1) {
        gpio_set_level(GPIO_NUM_13, 1); //Set H1 high
        gpio_set_level(GPIO_NUM_12, 0); //Set H2 low
        gpio_set_level(GPIO_NUM_14, 1); //Set H3 high
        vTaskDelay(1);
        gpio_set_level(GPIO_NUM_14, 0); //Set H3 low
        vTaskDelay(1);
        gpio_set_level(GPIO_NUM_12, 1); //Set H2 high
        vTaskDelay(1);
        gpio_set_level(GPIO_NUM_13, 0); //Set H1 low
        vTaskDelay(1);
        gpio_set_level(GPIO_NUM_14, 1); //Set H3 high
        vTaskDelay(1);
        gpio_set_level(GPIO_NUM_12, 0); //Set H2 high
        vTaskDelay(1);
    }
}

/**
 * @brief When interrupt occurs, we receive the counter value and display the time between two rising edge
 */
static void disp_captured_signal(void *arg)
{
    uint32_t *current_cap_value = (uint32_t *)malloc(sizeof(CAP_SIG_NUM));
    uint32_t *previous_cap_value = (uint32_t *)malloc(sizeof(CAP_SIG_NUM));
    capture evt;
    while (1) {
        xQueueReceive(cap_queue, &evt, portMAX_DELAY);
        if (evt.sel_cap_signal == MCPWM_SELECT_CAP0) {
            current_cap_value[0] = evt.capture_signal - previous_cap_value[0];
            previous_cap_value[0] = evt.capture_signal;
            current_cap_value[0] = (current_cap_value[0] / 10000) * (10000000000 / rtc_clk_apb_freq_get());
            //printf("CAP0 : %d us\n", current_cap_value[0]);
        }
        if (evt.sel_cap_signal == MCPWM_SELECT_CAP1) {
            current_cap_value[1] = evt.capture_signal - previous_cap_value[1];
            previous_cap_value[1] = evt.capture_signal;
            current_cap_value[1] = (current_cap_value[1] / 10000) * (10000000000 / rtc_clk_apb_freq_get());
            //printf("CAP1 : %d us\n", current_cap_value[1]);
        }
        if (evt.sel_cap_signal == MCPWM_SELECT_CAP2) {
            current_cap_value[2] = evt.capture_signal -  previous_cap_value[2];
            previous_cap_value[2] = evt.capture_signal;
            current_cap_value[2] = (current_cap_value[2] / 10000) * (10000000000 / rtc_clk_apb_freq_get());
            //printf("CAP2 : %d us\n", current_cap_value[2]);
        }
    }
}

/**
 * @brief this is ISR handler function, here we check for interrupt that triggers rising edge on CAP0 signal and according take action
 */
static void IRAM_ATTR isr_handler()
{
    uint32_t mcpwm_intr_status;
    capture evt;
    mcpwm_intr_status = MCPWM[MCPWM_UNIT_0]->int_st.val; //Read interrupt status
    if (mcpwm_intr_status & CAP0_INT_EN) { //Check for interrupt on rising edge on CAP0 signal
        evt.capture_signal = mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP0); //get capture signal counter value
        evt.sel_cap_signal = MCPWM_SELECT_CAP0;
        xQueueSendFromISR(cap_queue, &evt, NULL);
    }
    if (mcpwm_intr_status & CAP1_INT_EN) { //Check for interrupt on rising edge on CAP1 signal
        evt.capture_signal = mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP1); //get capture signal counter value
        evt.sel_cap_signal = MCPWM_SELECT_CAP1;
        xQueueSendFromISR(cap_queue, &evt, NULL);
    }
    if (mcpwm_intr_status & CAP2_INT_EN) { //Check for interrupt on rising edge on CAP2 signal
        evt.capture_signal = mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP2); //get capture signal counter value
        evt.sel_cap_signal = MCPWM_SELECT_CAP2;
        xQueueSendFromISR(cap_queue, &evt, NULL);
    }
    MCPWM[MCPWM_UNIT_0]->int_clr.val = mcpwm_intr_status;
}

static void change_duty(void *arg)
{
    int j;
    while (1) {
        for (j = 0; j < 18; j++) {
            //printf("duty cycle: %d\n", (0 +j*50));
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, (INITIAL_DUTY + j * 5.0));
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, (INITIAL_DUTY + j * 5.0));
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, (INITIAL_DUTY + j * 5.0));
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, (INITIAL_DUTY + j * 5.0));
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, (INITIAL_DUTY + j * 5.0));
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B, (INITIAL_DUTY + j * 5.0));
            vTaskDelay(500 / portTICK_RATE_MS);
        }
    }
}

/**
 * @brief Configure whole MCPWM module for bldc motor control
 */
static void mcpwm_example_bldc_control(void *arg)
{
    //1. mcpwm gpio initialization
    mcpwm_example_gpio_initialize();

    //2. initial mcpwm configuration
    printf("Configuring Initial Parameters of mcpwm bldc control...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    //frequency = 1000Hz
    pwm_config.cmpr_a = 50.0;    //duty cycle of PWMxA = 50.0%
    pwm_config.cmpr_b = 50.0;    //duty cycle of PWMxb = 50.0%
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_1;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);    //Configure PWM1A & PWM1B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &pwm_config);    //Configure PWM2A & PWM2B with above settings

    //3. Capture configuration
    //configure CAP0, CAP1 and CAP2 signal to start capture counter on rising edge
    //we generate a gpio_test_signal of 20ms on GPIO 12 and connect it to one of the capture signal, the disp_captured_function displays the time between rising edge
    //In general practice you can connect Capture  to external signal, measure time between rising edge or falling edge and take action accordingly
    mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 0);  //capture signal on rising edge, pulse num = 0 i.e. 800,000,000 counts is equal to one second
    mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP1, MCPWM_POS_EDGE, 0);  //capture signal on rising edge, pulse num = 0 i.e. 800,000,000 counts is equal to one second
    mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP2, MCPWM_POS_EDGE, 0);  //capture signal on rising edge, pulse num = 0 i.e. 800,000,000 counts is equal to one second
    //enable interrupt, so each this a rising edge occurs interrupt is triggered
    MCPWM[MCPWM_UNIT_0]->int_ena.val = (CAP0_INT_EN | CAP1_INT_EN | CAP2_INT_EN);  //Enable interrupt on  CAP0, CAP1 and CAP2 signal
    mcpwm_isr_register(MCPWM_UNIT_0, isr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);  //Set ISR Handler
    //According to the hall sensor input value take action on PWM0A/0B/1A/1B/2A/2B
    while (1) {
        hall_sensor_value = (gpio_get_level(GPIO_NUM_27) * 1) + (gpio_get_level(GPIO_NUM_26) * 2) + (gpio_get_level(GPIO_NUM_25) * 4);
        if (hall_sensor_value != hall_sensor_previous) {
            //printf("hall_sen val: %d\n", hall_sensor_value);
            if (hall_sensor_value == 2) {
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);
                mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);
                //MCPWMXA to duty mode 1 and MCPWMXB to duty mode 0 or vice versa will generate MCPWM compliment signal of each other, there are also other ways to do it
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_1); //Set PWM0A to duty mode one
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //Set PWM0B back to duty mode zero
                mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_BYPASS_FED, 100, 100);   //Deadtime of 10us
            }
            if (hall_sensor_value == 6) {
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
                mcpwm_deadtime_disable(MCPWM_UNIT_0, MCPWM_TIMER_0);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);
                mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);
                //MCPWMXA to duty mode 1 and MCPWMXB to duty mode 0 or vice versa will generate MCPWM compliment signal of each other, there are also other ways to do it
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, MCPWM_DUTY_MODE_1); //Set PWM2A to duty mode one
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //Set PWM2B back to duty mode zero
                mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_BYPASS_FED, 100, 100);   //Deadtime of 10us
            }
            if (hall_sensor_value == 4) {
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
                mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
                //MCPWMXA to duty mode 1 and MCPWMXB to duty mode 0 or vice versa will generate MCPWM compliment signal of each other, there are also other ways to do it
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, MCPWM_DUTY_MODE_1); //Set PWM2A to duty mode one
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //Set PWM2B back to duty mode zero
                mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_BYPASS_FED, 100, 100);   //Deadtime of 10us
            }
            if (hall_sensor_value == 5) {
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B);
                mcpwm_deadtime_disable(MCPWM_UNIT_0, MCPWM_TIMER_2);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
                mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
                //MCPWMXA to duty mode 1 and MCPWMXB to duty mode 0 or vice versa will generate MCPWM compliment signal of each other, there are also other ways to do it
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_1); //Set PWM1A to duty mode one
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //Set PWM1B back to duty mode zero
                mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_BYPASS_FED, 100, 100);   //Deadtime of 10uss
            }
            if (hall_sensor_value == 1) {
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A);
                mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B);
                //MCPWMXA to duty mode 1 and MCPWMXB to duty mode 0 or vice versa will generate MCPWM compliment signal of each other, there are also other ways to do it
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_1); //Set PWM1A to duty mode one
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //Set PWM1B back to duty mode zero
                mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_BYPASS_FED, 100, 100);   //Deadtime of 10uss
            }
            if (hall_sensor_value == 3) {
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);
                mcpwm_deadtime_disable(MCPWM_UNIT_0, MCPWM_TIMER_1);
                mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A);
                mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B);
                //MCPWMXA to duty mode 1 and MCPWMXB to duty mode 0 or vice versa will generate MCPWM compliment signal of each other, there are also other ways to do it
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_1); //Set PWM0A to duty mode one
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //Set PWM0B back to duty mode zero
                mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_BYPASS_FED, 100, 100);   //Deadtime of 10us
            }
            hall_sensor_previous = hall_sensor_value;
        }
    }
}

void app_main()
{
    printf("Testing MCPWM BLDC Control...\n");
    //xTaskCreate(change_duty, "change_duty", 2048, NULL, 2, NULL); //uncomment to change duty continuously
    cap_queue = xQueueCreate(1, sizeof(capture)); //comment if you don't want to use capture module
    //xTaskCreate(gpio_test_signal, "gpio_test_signal", 2048, NULL, 2, NULL);
    xTaskCreate(disp_captured_signal, "mcpwm_config", 4096, NULL, 2, NULL);  //comment if you don't want to use capture module
    xTaskCreate(mcpwm_example_bldc_control, "mcpwm_example_bldc_control", 4096, NULL, 2, NULL);
}

