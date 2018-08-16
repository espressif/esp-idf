/**
 * About test environment UT_T1_GPIO:
 * Please connect GPIO18 and GPIO19
 */
#include <stdio.h>
#include <string.h>
#include "rom/uart.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "unity.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define WAKE_UP_IGNORE 1  // gpio_wakeup function development is not completed yet, set it deprecated.
#define GPIO_OUTPUT_IO   18  // default output GPIO
#define GPIO_INPUT_IO   19  // default input GPIO
#define GPIO_OUTPUT_MAX GPIO_NUM_34
static volatile int disable_intr_times = 0;  // use this to calculate how many times it go into interrupt
static volatile int level_intr_times = 0;  // use this to get how many times the level interrupt happened
static volatile int edge_intr_times = 0;   // use this to get how many times the edge interrupt happened
#if !WAKE_UP_IGNORE
static bool wake_up_result = false;  // use this to judge the wake up event happen or not
#endif

/**
 * do some initialization operation in this function
 * @param num: it is the destination GPIO wanted to be initialized
 *
 */
static gpio_config_t init_io(gpio_num_t num)
{
    TEST_ASSERT(num < GPIO_OUTPUT_MAX);
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    return io_conf;
}

// edge interrupt event
static void gpio_isr_edge_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    ets_printf("GPIO[%d] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num));
    edge_intr_times++;
}

// level interrupt event with "gpio_intr_disable"
static void gpio_isr_level_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    disable_intr_times++;
    ets_printf("GPIO[%d] intr, val: %d, disable_intr_times = %d\n", gpio_num, gpio_get_level(gpio_num), disable_intr_times);
    gpio_intr_disable(gpio_num);
}

// level interrupt event
static void gpio_isr_level_handler2(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    level_intr_times++;
    ets_printf("GPIO[%d] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num));
    if(gpio_get_level(gpio_num)) {
        gpio_set_level(GPIO_OUTPUT_IO, 0);
    }else{
        gpio_set_level(GPIO_OUTPUT_IO, 1);
    }
    ets_printf("GPIO[%d] intr, val: %d, level_intr_times = %d\n", GPIO_OUTPUT_IO, gpio_get_level(GPIO_OUTPUT_IO), level_intr_times);
    ets_printf("GPIO[%d] intr, val: %d, level_intr_times = %d\n", gpio_num, gpio_get_level(gpio_num), level_intr_times);
}

#if !WAKE_UP_IGNORE
// get result of waking up or not
static void sleep_wake_up(void *arg)
{
    gpio_config_t io_config = init_io(GPIO_INPUT_IO);
    io_config.mode = GPIO_MODE_INPUT;
    gpio_config(&io_config);
    TEST_ESP_OK(gpio_wakeup_enable(GPIO_INPUT_IO, GPIO_INTR_HIGH_LEVEL));
    esp_light_sleep_start();
    wake_up_result = true;
}

// wake up light sleep event
static void trigger_wake_up(void *arg)
{
    gpio_config_t io_config = init_io(GPIO_OUTPUT_IO);
    gpio_config(&io_config);
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_OUTPUT_IO, gpio_isr_level_handler, (void*) GPIO_INPUT_IO);
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
}
#endif

static void prompt_to_continue(const char* str)
{
    printf("%s , please press \"Enter\" to go on!\n", str);
    char sign[5] = {0};
    while(strlen(sign) == 0) {
        /* Flush anything already in the RX buffer */
        while(uart_rx_one_char((uint8_t *) sign) == OK) {
        }
        /* Read line */
        UartRxString((uint8_t*) sign, sizeof(sign) - 1);
    }
}

static void drive_capability_set_get(gpio_num_t num, gpio_drive_cap_t capability)
{
    gpio_config_t pad_io = init_io(num);
    TEST_ESP_OK(gpio_config(&pad_io));
    TEST_ASSERT(gpio_set_drive_capability(num, GPIO_DRIVE_CAP_MAX) == ESP_ERR_INVALID_ARG);

    gpio_drive_cap_t cap;
    TEST_ESP_OK(gpio_set_drive_capability(num, capability));
    TEST_ESP_OK(gpio_get_drive_capability(num, &cap));
    TEST_ASSERT_EQUAL_INT(cap, capability);
}


// test the basic configuration function with right parameters and error parameters
TEST_CASE("GPIO config parameters test", "[gpio]")
{
    //error param test
    //test 41 bit
    gpio_config_t io_config;
    io_config.pin_bit_mask = ((uint64_t)1<<(GPIO_NUM_MAX+1));
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);

    // test 0
    io_config.pin_bit_mask = 0;
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);

    //test 40 bit
    io_config.pin_bit_mask = ((uint64_t)1<<GPIO_NUM_MAX);
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);

    io_config.pin_bit_mask = (uint64_t)1<<23;
    TEST_ESP_OK(gpio_config(&io_config));

    io_config.pin_bit_mask = ((uint64_t)1 << 34);
    io_config.mode = GPIO_MODE_INPUT;
    TEST_ESP_OK(gpio_config(&io_config));
    io_config.mode = GPIO_MODE_OUTPUT;
    // 34-39 input only, once set as output should log something
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("GPIO rising edge interrupt test", "[gpio][test_env=UT_T1_GPIO]")
{
    edge_intr_times = 0;  // set it as 0 prepare to test
    //init input and output gpio
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 0));

    //rising edge intr
    TEST_ESP_OK(gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_POSEDGE));
    TEST_ESP_OK(gpio_install_isr_service(0));
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_edge_handler, (void*)GPIO_INPUT_IO);
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 1));
    TEST_ASSERT_EQUAL_INT(edge_intr_times, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_isr_handler_remove(GPIO_INPUT_IO);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO falling edge interrupt test", "[gpio][test_env=UT_T1_GPIO]")
{
    edge_intr_times = 0;
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 1));

    gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_edge_handler, (void*) GPIO_INPUT_IO);
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT(edge_intr_times, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_isr_handler_remove(GPIO_INPUT_IO);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO both rising and falling edge interrupt test", "[gpio][test_env=UT_T1_GPIO]")
{
    edge_intr_times = 0;
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 0));
    int level = 0;

    gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_edge_handler, (void*) GPIO_INPUT_IO);
    // for rising edge in GPIO_INTR_ANYEDGE
    while(1) {
        level = level + 1;
        gpio_set_level(GPIO_OUTPUT_IO, level*0.2);
        if(level > 10) {
            break;
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }
    vTaskDelay(100 / portTICK_RATE_MS);
    // for falling rdge in GPIO_INTR_ANYEDGE
    while(1) {
        level = level - 1;
        gpio_set_level(GPIO_OUTPUT_IO, level/5);
        if(level < 0) {
            break;
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT(edge_intr_times, 2);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_isr_handler_remove(GPIO_INPUT_IO);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO input high level trigger, cut the interrupt source exit interrupt test", "[gpio][test_env=UT_T1_GPIO]")
{
    level_intr_times=0;
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 0));

    gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_HIGH_LEVEL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_level_handler2, (void*) GPIO_INPUT_IO);
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(level_intr_times, 1, "go into high-level interrupt more than once with cur interrupt source way");
    gpio_isr_handler_remove(GPIO_INPUT_IO);
    gpio_uninstall_isr_service();

}

TEST_CASE("GPIO low level interrupt test", "[gpio][test_env=UT_T1_GPIO]")
{
    disable_intr_times=0;
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 1));

    gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_LOW_LEVEL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_level_handler, (void*) GPIO_INPUT_IO);
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    printf("get level:%d\n",gpio_get_level(GPIO_INPUT_IO));
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(disable_intr_times, 1, "go into low-level interrupt more than once with disable way");
    gpio_isr_handler_remove(GPIO_INPUT_IO);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO enable and disable interrupt test", "[gpio][test_env=UT_T1_GPIO]")
{
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));

    TEST_ESP_OK(gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_HIGH_LEVEL));
    TEST_ESP_OK(gpio_install_isr_service(0));
    TEST_ESP_OK(gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_level_handler, (void*) GPIO_INPUT_IO));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 1));
    TEST_ESP_OK(gpio_isr_handler_remove(GPIO_INPUT_IO));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 0));
    TEST_ASSERT_EQUAL_INT_MESSAGE(disable_intr_times, 1, "go into high-level interrupt more than once with disable way");

    // not install service now
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ESP_OK(gpio_intr_disable(GPIO_INPUT_IO));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 1));
    TEST_ASSERT_EQUAL_INT_MESSAGE(disable_intr_times, 1, "disable interrupt does not work, still go into interrupt!");

    gpio_uninstall_isr_service();  //uninstall the service
    TEST_ASSERT(gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_level_handler, (void*) GPIO_INPUT_IO) == ESP_ERR_INVALID_STATE);
    TEST_ASSERT(gpio_isr_handler_remove(GPIO_INPUT_IO) == ESP_ERR_INVALID_STATE);
}

// Connect GPIO18 with GPIO19
// use multimeter to test the voltage, so it is ignored in CI
TEST_CASE("GPIO set gpio output level test", "[gpio][ignore]")
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1<<GPIO_OUTPUT_IO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1<<GPIO_INPUT_IO);
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);

    gpio_set_level(GPIO_OUTPUT_IO, 0);
    // tested voltage is around 0v
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "get level error! the level should be low!");
    vTaskDelay(1000 / portTICK_RATE_MS);
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    // tested voltage is around 3.3v
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 1, "get level error! the level should be high!");

    //IO34-39 are just used for input
    io_conf.pin_bit_mask = ((uint64_t)1<<34);
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);
    TEST_ASSERT(gpio_config(&io_conf) == ESP_ERR_INVALID_ARG);

}

// gpio17 connects to 3.3v pin, gpio19 connects to the GND pin
// use multimeter to test the voltage, so it is ignored in CI
TEST_CASE("GPIO get input level test", "[gpio][ignore]")
{
    gpio_num_t num = 17;
    int level = gpio_get_level(num);
    printf("gpio17's level is: %d\n", level);
    TEST_ASSERT_EQUAL_INT_MESSAGE(level, 1, "get level error! the level should be high!");

    gpio_num_t num2 = 19;
    int level2 = gpio_get_level(num2);
    printf("gpio19's level is: %d\n", level2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(level2, 0, "get level error! the level should be low!");
    printf("the memory get: %d\n", esp_get_free_heap_size());

    gpio_num_t num3 = 34;  // connect with 3.3v
    int level3 = gpio_get_level(num3);
    printf("gpio19's level is: %d\n", level3);
    TEST_ASSERT_EQUAL_INT_MESSAGE(level3, 0, "get level error! the level should be low!");
    printf("the memory get: %d\n", esp_get_free_heap_size());
    //when case finish, get the result from multimeter, the pin17 is 3.3v, the pin19 is 0.00v
}

TEST_CASE("GPIO io pull up/down function", "[gpio]")
{
    gpio_config_t  io_conf = init_io(GPIO_INPUT_IO);
    gpio_config(&io_conf);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_INPUT);
    TEST_ESP_OK(gpio_pullup_en(GPIO_INPUT_IO));  // pull up first
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 1, "gpio_pullup_en error, it can't pull up");
    TEST_ESP_OK(gpio_pulldown_dis(GPIO_INPUT_IO)); //can't be pull down
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 1, "gpio_pulldown_dis error, it can pull down");
    TEST_ESP_OK(gpio_pulldown_en(GPIO_INPUT_IO)); // can be pull down now
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "gpio_pulldown_en error, it can't pull down");
    TEST_ESP_OK(gpio_pullup_dis(GPIO_INPUT_IO)); // can't be pull up
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "gpio_pullup_dis error, it can pull up");
}

TEST_CASE("GPIO output and input mode test", "[gpio][test_env=UT_T1_GPIO]")
{
    //connect io18 and io5
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    gpio_config(&output_io);
    gpio_config(&input_io);
    int level = gpio_get_level(GPIO_INPUT_IO);

    //disable mode
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_DISABLE);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_OUTPUT_IO, !level);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), level, "direction GPIO_MODE_DISABLE set error, it can output");

    //input mode and output mode
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_INPUT);
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 1, "direction GPIO_MODE_OUTPUT set error, it can't output");
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "direction GPIO_MODE_OUTPUT set error, it can't output");

    // open drain mode(output), can just output low level
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT_OD);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_INPUT);
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "direction GPIO_MODE_OUTPUT set error, it can't output");
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "direction GPIO_MODE_OUTPUT set error, it can't output");

    // open drain mode(output and input), can just output low level
    // output test
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_INPUT);
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "direction GPIO_MODE_OUTPUT set error, it can't output");
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), 0, "direction GPIO_MODE_OUTPUT set error, it can't output");

    // GPIO_MODE_INPUT_OUTPUT mode
    // output test
    level = gpio_get_level(GPIO_INPUT_IO);
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_INPUT);
    gpio_set_level(GPIO_OUTPUT_IO, !level);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), !level, "direction set error, it can't output");
    // input test
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_INPUT_IO, GPIO_MODE_INPUT_OUTPUT);
    level = gpio_get_level(GPIO_INPUT_IO);
    gpio_set_level(GPIO_OUTPUT_IO, !level);
    TEST_ASSERT_EQUAL_INT_MESSAGE(gpio_get_level(GPIO_INPUT_IO), !level, "direction set error, it can't output");
}

TEST_CASE("GPIO repeate call service and isr has no memory leak test","[gpio][test_env=UT_T1_GPIO][timeout=90]")
{
    gpio_config_t output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t input_io = init_io(GPIO_INPUT_IO);
    input_io.intr_type = GPIO_INTR_POSEDGE;
    input_io.mode = GPIO_MODE_INPUT;
    input_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&output_io));
    TEST_ESP_OK(gpio_config(&input_io));
    TEST_ESP_OK(gpio_set_level(GPIO_OUTPUT_IO, 0));
    //rising edge
    uint32_t size = esp_get_free_heap_size();
    for(int i=0;i<1000;i++) {
        TEST_ESP_OK(gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_POSEDGE));
        TEST_ESP_OK(gpio_install_isr_service(0));
        TEST_ESP_OK(gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_edge_handler, (void*)GPIO_INPUT_IO));
        gpio_set_level(GPIO_OUTPUT_IO, 1);
        TEST_ESP_OK(gpio_isr_handler_remove(GPIO_INPUT_IO));
        gpio_set_level(GPIO_OUTPUT_IO, 0);
        gpio_uninstall_isr_service();
    }
    TEST_ASSERT_INT32_WITHIN(size, esp_get_free_heap_size(), 100);
}

#if !WAKE_UP_IGNORE
//this function development is not completed yet, set it ignored
TEST_CASE("GPIO wake up enable and disenable test", "[gpio][ignore]")
{
    xTaskCreate(sleep_wake_up, "sleep_wake_up", 4096, NULL, 5, NULL);
    xTaskCreate(trigger_wake_up, "trigger_wake_up", 4096, NULL, 5, NULL);
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_TRUE(wake_up_result);

    wake_up_result = false;
    TEST_ESP_OK(gpio_wakeup_disable(GPIO_INPUT_IO));
    gpio_set_level(GPIO_OUTPUT_IO, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    TEST_ASSERT_FALSE(wake_up_result);
}
#endif

// this case need the resistance to pull up the voltage or pull down the voltage
// ignored because the voltage needs to be tested with multimeter
TEST_CASE("GPIO verify only the gpio with input ability can be set pull/down", "[gpio][ignore]")
{
    gpio_config_t  output_io = init_io(GPIO_OUTPUT_IO);
    gpio_config_t  input_io = init_io(GPIO_INPUT_IO);
    gpio_config(&output_io);
    input_io.mode = GPIO_MODE_INPUT;
    gpio_config(&input_io);

    printf("pull up test!\n");
    // pull up test
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLUP_ONLY));

    // open drain just can output low level
    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT_OD");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT_OUTPUT");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT");

    // after pull up the level is high now
    // pull down test
    printf("pull down test!\n");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT_OD");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT_OUTPUT_OD");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT_OUTPUT");

    gpio_set_direction(GPIO_OUTPUT_IO, GPIO_MODE_INPUT);
    TEST_ESP_OK(gpio_set_pull_mode(GPIO_OUTPUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT");
}

/**
 * There are 5 situation for the GPIO drive capability:
 * 1. GPIO drive weak capability test
 * 2. GPIO drive stronger capability test
 * 3. GPIO drive default capability test
 * 4. GPIO drive default capability test2
 * 5. GPIO drive strongest capability test
 *
 * How to test:
 * when testing, use the sliding resistor and a multimeter
 * adjust the resistor from low to high, 0-10k
 * watch the current change
 * the current test result:
 * weak capability: (0.32-10.1)mA
 * stronger capability: (0.32-20.0)mA
 * default capability: (0.33-39.8)mA
 * default capability2: (0.33-39.9)mA
 * strongest capability: (0.33-64.2)mA
 *
 * the data shows:
 * weak capability<stronger capability<default capability=default capability2<strongest capability
 *
 * all of these cases should be ignored that it will not run in CI
 */

// drive capability test
TEST_CASE("GPIO drive capability test", "[gpio][ignore]")
{
    printf("weak capability test! please view the current change!\n");
    drive_capability_set_get(GPIO_OUTPUT_IO, GPIO_DRIVE_CAP_0);
    prompt_to_continue("If this test finishes");

    printf("stronger capability test! please view the current change!\n");
    drive_capability_set_get(GPIO_OUTPUT_IO, GPIO_DRIVE_CAP_1);
    prompt_to_continue("If this test finishes");

    printf("default capability test! please view the current change!\n");
    drive_capability_set_get(GPIO_OUTPUT_IO, GPIO_DRIVE_CAP_2);
    prompt_to_continue("If this test finishes");

    printf("default capability2 test! please view the current change!\n");
    drive_capability_set_get(GPIO_OUTPUT_IO, GPIO_DRIVE_CAP_DEFAULT);
    prompt_to_continue("If this test finishes");

    printf("strongest capability test! please view the current change!\n");
    drive_capability_set_get(GPIO_OUTPUT_IO, GPIO_DRIVE_CAP_3);
    prompt_to_continue("If this test finishes");
}
