/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * No specific runner required to run GPIO unit test.
 * TEST_GPIO_EXT_OUT_IO and TEST_GPIO_EXT_IN_IO are connected internally through gpio matrix.
 *
 * If wants to externally connect TEST_GPIO_EXT_OUT_IO to TEST_GPIO_EXT_IN_IO (UT_T1_GPIO), please set
 * TEST_GPIO_INTERNAL_ROUTING to 0
 */

#include <stdio.h>
#include <string.h>
#include "test_gpio.h"
#include "esp_system.h"
#include "esp_cpu.h"
#include "esp_sleep.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/gpio.h"
#include "hal/gpio_ll.h"
#include "soc/gpio_periph.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "spi_flash_mmap.h"
#include "esp_attr.h"
#include "esp_private/spi_flash_os.h"

// Enable internal routing for the output and input gpio pins
#define TEST_GPIO_INTERNAL_ROUTING    1

// If there is any input-only pin, enable input-only pin part of some tests.
#define SOC_HAS_INPUT_ONLY_PIN (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

static volatile int disable_intr_times = 0;  // use this to calculate how many times it go into interrupt
static volatile int level_intr_times = 0;  // use this to get how many times the level interrupt happened
static volatile int edge_intr_times = 0;   // use this to get how many times the edge interrupt happened

/**
 * Do some initialization operation in this function
 * @param num it is the destination GPIO wanted to be initialized
 */
static gpio_config_t test_init_io(gpio_num_t num)
{
    TEST_ASSERT(GPIO_IS_VALID_OUTPUT_GPIO(num));
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << num),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    return io_conf;
}

/**
 * Configure gpio pin as GPIO_MODE_INPUT_OUTPUT for all the interrupt related tests to avoid runner requirements
 */
static void test_gpio_config_mode_input_output(gpio_num_t num)
{
    gpio_config_t input_output_io = test_init_io(num);
    input_output_io.mode = GPIO_MODE_INPUT_OUTPUT;
    input_output_io.pull_up_en = 1;
    TEST_ESP_OK(gpio_config(&input_output_io));
}

// test the basic configuration function with right parameters and error parameters
TEST_CASE("GPIO_config_parameters_test", "[gpio]")
{
    gpio_config_t io_config = { 0 };
    io_config.intr_type = GPIO_INTR_DISABLE;

    // test 0
    io_config.pin_bit_mask = 0;
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);

    // test a non-exist pin
    io_config.pin_bit_mask = ((uint64_t)1 << GPIO_NUM_MAX);
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);

    // test an available pin
    io_config.pin_bit_mask = ((uint64_t)1 << TEST_GPIO_EXT_OUT_IO);
    TEST_ESP_OK(gpio_config(&io_config));

    //This IO is just used for input, C3 and S3 doesn't have input only pin.
#if SOC_HAS_INPUT_ONLY_PIN
    io_config.pin_bit_mask = ((uint64_t)1 << TEST_GPIO_INPUT_ONLY_PIN);
    io_config.mode = GPIO_MODE_INPUT;
    TEST_ESP_OK(gpio_config(&io_config));
    io_config.mode = GPIO_MODE_OUTPUT;
    // The pin is input only, once set as output should log something
    TEST_ASSERT(gpio_config(&io_config) == ESP_ERR_INVALID_ARG);
#endif // SOC_HAS_INPUT_ONLY_PIN
}

// edge interrupt event
static void gpio_isr_edge_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    esp_rom_printf("GPIO[%d] intr on core %d, val: %d\n", gpio_num, esp_cpu_get_core_id(), gpio_get_level(gpio_num));
    edge_intr_times++;
}

// level interrupt event with "gpio_intr_disable"
static void gpio_isr_level_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    disable_intr_times++;
    esp_rom_printf("GPIO[%d] intr, val: %d, disable_intr_times = %d\n", gpio_num, gpio_get_level(gpio_num), disable_intr_times);
    gpio_intr_disable(gpio_num);
}

// level interrupt event with "gpio_set_level(!gpio_get_level)"
static void gpio_isr_level_handler2(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    level_intr_times++;
    esp_rom_printf("GPIO[%d] intr, val: %d, level_intr_times = %d\n", gpio_num, gpio_get_level(gpio_num), level_intr_times);
    if (gpio_get_level(gpio_num)) {
        gpio_set_level(gpio_num, 0);
    } else {
        gpio_set_level(gpio_num, 1);
    }
}

TEST_CASE("GPIO_rising_edge_interrupt_test", "[gpio]")
{
    edge_intr_times = 0;  // set it as 0 prepare to test
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    // Rising edge intr
    TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_POSEDGE));
    TEST_ESP_OK(gpio_install_isr_service(0));
    gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_edge_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT(1, edge_intr_times);
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO_falling_edge_interrupt_test", "[gpio]")
{
    edge_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));

    gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_edge_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT(1, edge_intr_times);
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO_both_rising_and_falling_edge_interrupt_test", "[gpio]")
{
    edge_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));
    int level = 0;

    gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_edge_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    // For rising edge in GPIO_INTR_ANYEDGE
    while (1) {
        level = level + 1;
        gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, level * 0.2);
        if (level > 10) {
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // For falling edge in GPIO_INTR_ANYEDGE
    while (1) {
        level = level - 1;
        gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, level / 5);
        if (level < 0) {
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT(2, edge_intr_times);
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO_input_high_level_trigger_cut_the_interrupt_source_exit_interrupt_test", "[gpio]")
{
    level_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_HIGH_LEVEL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_level_handler2, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, level_intr_times, "go into high-level interrupt more than once with cut interrupt source way");
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO_low_level_interrupt_test", "[gpio]")
{
    disable_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));

    gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_LOW_LEVEL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_level_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0);
    printf("get level:%d\n", gpio_get_level(TEST_GPIO_INPUT_OUTPUT_IO1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, disable_intr_times, "go into low-level interrupt more than once with disable way");
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO_multi-level_trigger_cut_the_interrupt_source_exit_interrupt_test", "[gpio]")
{
    level_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_HIGH_LEVEL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_level_handler2, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, level_intr_times, "go into high-level interrupt more than once with cut interrupt source way");
    gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, level_intr_times, "go into high-level interrupt more than once with cut interrupt source way");
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
}

TEST_CASE("GPIO_enable_and_disable_interrupt_test", "[gpio]")
{
    disable_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_HIGH_LEVEL));
    TEST_ESP_OK(gpio_install_isr_service(0));
    TEST_ESP_OK(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_level_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    TEST_ESP_OK(gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, disable_intr_times, "go into high-level interrupt more than once with disable way");

    // Interrupt disabled now
    TEST_ESP_OK(gpio_intr_disable(TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, disable_intr_times, "disable interrupt does not work, still go into interrupt!");

    // Uninstall interrupt service
    gpio_uninstall_isr_service();
    TEST_ASSERT(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_level_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1) == ESP_ERR_INVALID_STATE);
    TEST_ASSERT(gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1) == ESP_ERR_INVALID_STATE);
}

TEST_CASE("GPIO_repeatedly_call_service_and_isr_has_no_memory_leak_test", "[gpio][timeout=90]")
{
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    // Rising edge intr
    uint32_t size = esp_get_free_heap_size();
    for (int i = 0; i < 1000; i++) {
        TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_POSEDGE));
        TEST_ESP_OK(gpio_install_isr_service(0));
        TEST_ESP_OK(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_edge_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1));
        gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1);
        TEST_ESP_OK(gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1));
        gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0);
        gpio_uninstall_isr_service();
    }
    TEST_ASSERT_INT32_WITHIN(100, size, esp_get_free_heap_size());
}

typedef struct {
    int gpio_num;
    int isr_cnt;
} gpio_isr_param_t;

static void gpio_isr_per_pin_handler(void *arg)
{
    gpio_isr_param_t *param = (gpio_isr_param_t *)arg;
    esp_rom_printf("GPIO[%d] intr, val: %d\n", param->gpio_num, gpio_get_level(param->gpio_num));
    param->isr_cnt++;
}

/** The old GPIO interrupt service routine used to poll the interrupt raw status register to find the GPIO that
 * triggered the interrupt. But this will incorrectly handle the interrupt disabled GPIOs, because the raw interrupt
 * status register can still be set when the trigger signal arrives, even if the interrupt is disabled.
 *
 * Do the following steps：
 *     1. Configure TEST_GPIO_INPUT_OUTPUT_IO1 and TEST_GPIO_INPUT_OUTPUT_IO2 input_output mode.
 *     2. Enable TEST_GPIO_INPUT_OUTPUT_IO1 dual edge triggered interrupt, enable TEST_GPIO_INPUT_OUTPUT_IO2 falling edge triggered interrupt.
 *     3. Trigger TEST_GPIO_INPUT_OUTPUT_IO1 interrupt, then disable TEST_GPIO_INPUT_OUTPUT_IO1 interrupt, and then trigger TEST_GPIO_INPUT_OUTPUT_IO1 interrupt again (This time will not respond to the interrupt).
 *     4. Trigger TEST_GPIO_INPUT_OUTPUT_IO2 interrupt.
 *
 * If the bug is not fixed, you will see, in the step 4, the interrupt of TEST_GPIO_INPUT_OUTPUT_IO1 will also respond.
 */
TEST_CASE("GPIO_isr_responses_to_correct_gpios_test", "[gpio]")
{
    gpio_isr_param_t io1_param = {
        .gpio_num =  TEST_GPIO_INPUT_OUTPUT_IO1,
        .isr_cnt = 0,
    };
    gpio_isr_param_t io2_param = {
        .gpio_num =  TEST_GPIO_INPUT_OUTPUT_IO2,
        .isr_cnt = 0,
    };
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO2);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO2, 0));
    TEST_ESP_OK(gpio_install_isr_service(0));
    TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_ANYEDGE));
    TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO2, GPIO_INTR_NEGEDGE));
    TEST_ESP_OK(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_per_pin_handler, (void *) &io1_param));
    TEST_ESP_OK(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO2, gpio_isr_per_pin_handler, (void *) &io2_param));

    printf("Triggering the interrupt of GPIO%d\n", TEST_GPIO_INPUT_OUTPUT_IO1);
    // Rising edge
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    printf("Disable the interrupt of GPIO%d\n", TEST_GPIO_INPUT_OUTPUT_IO1);
    // Disable TEST_GPIO_INPUT_OUTPUT_IO1 interrupt, TEST_GPIO_INPUT_OUTPUT_IO1 will not respond to the next falling edge interrupt
    TEST_ESP_OK(gpio_intr_disable(TEST_GPIO_INPUT_OUTPUT_IO1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // Falling edge
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    printf("Triggering the interrupt of GPIO%d\n", TEST_GPIO_INPUT_OUTPUT_IO2);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO2, 1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // Falling edge
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO2, 0));
    vTaskDelay(100 / portTICK_PERIOD_MS);

    TEST_ESP_OK(gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ESP_OK(gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO2));
    gpio_uninstall_isr_service();
    TEST_ASSERT((io1_param.isr_cnt == 1) && (io2_param.isr_cnt == 1));
}

#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"

static void install_isr_service_task(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    // Rising edge intr
    TEST_ESP_OK(gpio_set_intr_type(gpio_num, GPIO_INTR_POSEDGE));
    TEST_ESP_OK(gpio_install_isr_service(0));
    gpio_isr_handler_add(gpio_num, gpio_isr_edge_handler, (void *) gpio_num);
    vTaskSuspend(NULL);
}

TEST_CASE("GPIO_interrupt_on_other_CPUs_test", "[gpio]")
{
    TaskHandle_t gpio_task_handle;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);

    for (int cpu_num = 1; cpu_num < portNUM_PROCESSORS; ++cpu_num) {
        // We assume unit-test task is running on core 0, so we install gpio interrupt on other cores
        edge_intr_times = 0;
        TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));
        xTaskCreatePinnedToCore(install_isr_service_task, "install_isr_service_task", 2048, (void *) TEST_GPIO_INPUT_OUTPUT_IO1, 1, &gpio_task_handle, cpu_num);

        vTaskDelay(200 / portTICK_PERIOD_MS);
        TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        TEST_ASSERT_EQUAL_INT(1, edge_intr_times);
        gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
        gpio_uninstall_isr_service();
        unity_utils_task_delete(gpio_task_handle);
    }
}

static void gpio_intr_enable_task(void *param)
{
    int gpio_num = (int) param;
    TEST_ESP_OK(gpio_intr_enable(gpio_num));
}

/** Test the GPIO Interrupt Enable API with dual core enabled. The GPIO ISR service routine is registered on one core.
 * When the GPIO interrupt on another core is enabled, the GPIO interrupt will be lost.
 * Note. This is only a problem for ESP32. On ESP32S3, interrupt enable is effective to both cores, therefore, no matter
 * which core the interrupt service is installed on, the GPIO interrupt won't be lost.
 *
 * First on the core 0, do the following steps:
 *     1. Configure TEST_GPIO_INPUT_OUTPUT_IO1 input_output mode, and enable the falling edge interrupt mode.
 *     2. Trigger TEST_GPIO_INPUT_OUTPUT_IO1 interrupt and check if the interrupt responds correctly.
 *     3. Disable TEST_GPIO_INPUT_OUTPUT_IO1 interrupt
 * Then on the core 1, do the following steps:
 *     1. Enable TEST_GPIO_INPUT_OUTPUT_IO1 interrupt again.
 *     2. Trigger TEST_GPIO_INPUT_OUTPUT_IO1 interrupt and check if the interrupt responds correctly.
 */
TEST_CASE("GPIO_crosscore_interrupt_test", "[gpio]")
{
    edge_intr_times = 0;
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_NEGEDGE));
    // GPIO interrupt service installed on core 0
    TEST_ESP_OK(gpio_install_isr_service(0));
    TEST_ESP_OK(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_edge_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ESP_OK(gpio_intr_disable(TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ASSERT(edge_intr_times == 1);
    // Here, interrupt is enabling from core 1, but since the isr is installed on core 0, core 0 interrupt enable bit
    // will still be set instead of core 1 interrupt enable bit
    esp_ipc_call_blocking((xPortGetCoreID() == 0), gpio_intr_enable_task, (void *) TEST_GPIO_INPUT_OUTPUT_IO1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ESP_OK(gpio_intr_disable(TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ESP_OK(gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1));
    gpio_uninstall_isr_service();
    TEST_ASSERT(edge_intr_times == 2);
}
#endif //!CONFIG_FREERTOS_UNICORE

#if CONFIG_GPIO_CTRL_FUNC_IN_IRAM
static volatile DRAM_ATTR bool isr_triggered = false;

static void IRAM_ATTR gpio_isr_level_iram_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    isr_triggered = true;
    gpio_intr_disable(gpio_num);
}

static void IRAM_ATTR gpio_wait_intr_done_task(void *arg)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t) arg;
    spi_flash_guard_get()->start(); // Disables flash cache
    // Since interrupt service is installed on core 0, we enable the gpio intr on core 0
    gpio_ll_intr_enable_on_core(&GPIO, 0, TEST_GPIO_INPUT_OUTPUT_IO1);
    // Wait until interrupt triggered
    while (!isr_triggered) {
        ;
    }
    spi_flash_guard_get()->end(); // Re-enables flash cache
    xSemaphoreGive(sem);
    vTaskSuspend(NULL);
}

TEST_CASE("GPIO_iram_interrupt_safe_test", "[gpio]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateBinary();
    TaskHandle_t task_handle;
    TEST_ASSERT_NOT_NULL(done_sem);
    test_gpio_config_mode_input_output(TEST_GPIO_INPUT_OUTPUT_IO1);
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 0));

    TEST_ESP_OK(gpio_set_intr_type(TEST_GPIO_INPUT_OUTPUT_IO1, GPIO_INTR_HIGH_LEVEL));
    // We assume unit-test task is running on core 0, so interrupt service is installed on core 0
    TEST_ESP_OK(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
    TEST_ESP_OK(gpio_isr_handler_add(TEST_GPIO_INPUT_OUTPUT_IO1, gpio_isr_level_iram_handler, (void *) TEST_GPIO_INPUT_OUTPUT_IO1));
    // Disable intr and set pin level high, such that once the intr is re-enabled, it will trigger isr
    TEST_ESP_OK(gpio_intr_disable(TEST_GPIO_INPUT_OUTPUT_IO1));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_INPUT_OUTPUT_IO1, 1));
    xTaskCreate(gpio_wait_intr_done_task, "gpio_wait_intr_done_task", 2048, done_sem, 1, &task_handle);

    xSemaphoreTake(done_sem, portMAX_DELAY);
    gpio_isr_handler_remove(TEST_GPIO_INPUT_OUTPUT_IO1);
    gpio_uninstall_isr_service();
    vSemaphoreDelete(done_sem);
    unity_utils_task_delete(task_handle);
}
#endif

#if TEST_GPIO_INTERNAL_ROUTING
// Inter-connect input pin and output pin through an internal signal
static void gpio_interconnect_input_output_pin(uint32_t input_pin, uint32_t output_pin, uint32_t signal_idx)
{
    // signal256 -> output pin -> signal_idx -> input_pin
    // Set output pin IE to be able to connect to the signal
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[output_pin]);
    esp_rom_gpio_connect_in_signal(output_pin, signal_idx, 0);
    // Input pin OE to be able to connect to the signal is done by the esp_rom_gpio_connect_out_signal function
    esp_rom_gpio_connect_out_signal(input_pin, signal_idx, 0, 0);
}
#endif

TEST_CASE("GPIO_set_output_level_get_input_level_test", "[gpio]")
{
    gpio_config_t output_io = test_init_io(TEST_GPIO_EXT_OUT_IO);
    gpio_config(&output_io);
    gpio_config_t input_io = test_init_io(TEST_GPIO_EXT_IN_IO);
    input_io.mode = GPIO_MODE_INPUT;
    gpio_config(&input_io);

#if TEST_GPIO_INTERNAL_ROUTING
    gpio_interconnect_input_output_pin(TEST_GPIO_EXT_IN_IO, TEST_GPIO_EXT_OUT_IO, TEST_GPIO_SIGNAL_IDX);
#endif

    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // tested voltage is around 0v
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "get level error! the level should be low!");
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // tested voltage is around 3.3v
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(TEST_GPIO_EXT_IN_IO), "get level error! the level should be high!");
}

// This test routes constant-high/low signal to pins, another way is to directly connect TEST_GPIO_EXT_IN_IO to
// 3.3v or GND pin
TEST_CASE("GPIO_get_level_from_fixed_voltage_test", "[gpio]")
{
#if !TEST_GPIO_INTERNAL_ROUTING
    // If TEST_GPIO_EXT_OUT_IO is connected to TEST_GPIO_EXT_IN_IO, prevent being affected
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_DISABLE);
#endif
    gpio_config_t input_io = test_init_io(TEST_GPIO_EXT_IN_IO);
    input_io.mode = GPIO_MODE_INPUT;
    gpio_config(&input_io);
    esp_rom_gpio_connect_out_signal(TEST_GPIO_EXT_IN_IO, TEST_GPIO_SIGNAL_IDX, 0, 0);

    // Connect TEST_GPIO_EXT_IN_IO to a constant-high signal (to simulate connection to 3.3v)
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, TEST_GPIO_SIGNAL_IDX, 0);
    int level1 = gpio_get_level(TEST_GPIO_EXT_IN_IO);
    printf("TEST_GPIO_EXT_IN_IO(GPIO%d)'s level is: %d\n", TEST_GPIO_EXT_IN_IO, level1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, level1, "get level error! the level should be high!");

    // Connect TEST_GPIO_EXT_IN_IO to a constant-low signal (to simulate connection to GND)
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, TEST_GPIO_SIGNAL_IDX, 0);
    int level2 = gpio_get_level(TEST_GPIO_EXT_IN_IO);
    printf("TEST_GPIO_EXT_IN_IO(GPIO%d)'s level is: %d\n", TEST_GPIO_EXT_IN_IO, level2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, level2, "get level error! the level should be low!");
}

TEST_CASE("GPIO_io_pull_up/down_function", "[gpio]")
{
    // First, ensure that the output IO will not affect the level
    gpio_config_t io_conf = test_init_io(TEST_GPIO_EXT_OUT_IO);
    gpio_config(&io_conf);
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT);
    io_conf = test_init_io(TEST_GPIO_EXT_IN_IO);
    gpio_config(&io_conf);
    gpio_set_direction(TEST_GPIO_EXT_IN_IO, GPIO_MODE_INPUT);
    TEST_ESP_OK(gpio_pullup_en(TEST_GPIO_EXT_IN_IO));  // pull up first
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(TEST_GPIO_EXT_IN_IO), "gpio_pullup_en error, it can't pull up");
    TEST_ESP_OK(gpio_pulldown_dis(TEST_GPIO_EXT_IN_IO)); //can't be pull down
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(TEST_GPIO_EXT_IN_IO), "gpio_pulldown_dis error, it can pull down");
    TEST_ESP_OK(gpio_pulldown_en(TEST_GPIO_EXT_IN_IO)); // can be pull down now
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "gpio_pulldown_en error, it can't pull down");
    TEST_ESP_OK(gpio_pullup_dis(TEST_GPIO_EXT_IN_IO)); // can't be pull up
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "gpio_pullup_dis error, it can pull up");
}

// This test case tests whether gpio_set_level() outputs correctly with all gpio modes (gpio_mode_t)
TEST_CASE("GPIO_mode_test", "[gpio]")
{
    gpio_config_t output_io = test_init_io(TEST_GPIO_EXT_OUT_IO);
    gpio_config_t input_io = test_init_io(TEST_GPIO_EXT_IN_IO);
    gpio_config(&output_io);
    gpio_config(&input_io);
    int level = gpio_get_level(TEST_GPIO_EXT_IN_IO);

    // Disable mode
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_DISABLE);
    gpio_set_direction(TEST_GPIO_EXT_IN_IO, GPIO_MODE_OUTPUT);
#if TEST_GPIO_INTERNAL_ROUTING
    gpio_interconnect_input_output_pin(TEST_GPIO_EXT_IN_IO, TEST_GPIO_EXT_OUT_IO, TEST_GPIO_SIGNAL_IDX);
#endif
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, !level);
    TEST_ASSERT_EQUAL_INT_MESSAGE(level, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_DISABLE set error, it can output");

    // Output mode
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(TEST_GPIO_EXT_IN_IO, GPIO_MODE_INPUT);
#if TEST_GPIO_INTERNAL_ROUTING
    gpio_interconnect_input_output_pin(TEST_GPIO_EXT_IN_IO, TEST_GPIO_EXT_OUT_IO, TEST_GPIO_SIGNAL_IDX);
#endif
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_OUTPUT set error, it can't output");
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_OUTPUT set error, it can't output");

    // Open drain mode(output), can just output low level
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_OUTPUT_OD);
    gpio_set_direction(TEST_GPIO_EXT_IN_IO, GPIO_MODE_INPUT);
#if TEST_GPIO_INTERNAL_ROUTING
    gpio_interconnect_input_output_pin(TEST_GPIO_EXT_IN_IO, TEST_GPIO_EXT_OUT_IO, TEST_GPIO_SIGNAL_IDX);
#endif
    // Outputs high level: w/ pull up, then must read high level; w/ pull down, then must read low level
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 1);
    gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_OUTPUT_OD with GPIO_PULLUP_ONLY set error, it outputs low level");
    gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_OUTPUT_OD with GPIO_PULLDOWN_ONLY set error, it outputs high level");
    // Outputs low level: must read low level
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 0);
    gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_FLOATING);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_OUTPUT_OD set error, it outputs high level");

    // Open drain mode(output and input), can just output low level
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_direction(TEST_GPIO_EXT_IN_IO, GPIO_MODE_INPUT);
#if TEST_GPIO_INTERNAL_ROUTING
    gpio_interconnect_input_output_pin(TEST_GPIO_EXT_IN_IO, TEST_GPIO_EXT_OUT_IO, TEST_GPIO_SIGNAL_IDX);
#endif
    // Outputs high level: w/ pull up, then must read high level; w/ pull down, then must read low level
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 1);
    gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_INPUT_OUTPUT_OD with GPIO_PULLDOWN_ONLY set error, it outputs high level");
    gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_INPUT_OUTPUT_OD with GPIO_PULLUP_ONLY set error, it outputs low level");
    // Outputs low level: must read low level
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, 0);
    gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_FLOATING);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_INPUT_OUTPUT_OD set error, it outputs high level");

    // GPIO_MODE_INPUT_OUTPUT mode
    level = gpio_get_level(TEST_GPIO_EXT_IN_IO);
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(TEST_GPIO_EXT_IN_IO, GPIO_MODE_INPUT);
#if TEST_GPIO_INTERNAL_ROUTING
    gpio_interconnect_input_output_pin(TEST_GPIO_EXT_IN_IO, TEST_GPIO_EXT_OUT_IO, TEST_GPIO_SIGNAL_IDX);
#endif
    gpio_set_level(TEST_GPIO_EXT_OUT_IO, !level);
    TEST_ASSERT_EQUAL_INT_MESSAGE(!level, gpio_get_level(TEST_GPIO_EXT_IN_IO), "direction GPIO_MODE_INPUT_OUTPUT set error, it gives incorrect output");
}

static void prompt_to_continue(const char *str)
{
    printf("%s , please press \"Enter\" to go on!\n", str);
    char sign[5] = {0};
    while (strlen(sign) == 0) {
        /* Flush anything already in the RX buffer */
        while (esp_rom_uart_rx_one_char((uint8_t *) sign) == 0) {
        }
        /* Read line */
        esp_rom_uart_rx_string((uint8_t *) sign, sizeof(sign) - 1);
    }
}

// This case needs the resistance to pull up the voltage or pull down the voltage
// Ignored in CI because the voltage needs to be tested with multimeter
TEST_CASE("GPIO_verify_only_the_gpio_with_input_ability_can_be_set_pull/down", "[gpio][ignore]")
{
    gpio_config_t output_io = test_init_io(TEST_GPIO_EXT_OUT_IO);
    gpio_config_t input_io = test_init_io(TEST_GPIO_EXT_IN_IO);
    gpio_config(&output_io);
    input_io.mode = GPIO_MODE_INPUT;
    gpio_config(&input_io);

    printf("pull up test!\n");
    // pull up test
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY));

    // open drain just can output low level
    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT_OD");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT_OUTPUT");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLUP_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT");

    // after pull up the level is high now
    // pull down test
    printf("pull down test!\n");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_OUTPUT_OD");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT_OUTPUT_OD);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT_OUTPUT_OD");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT_OUTPUT);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT_OUTPUT");

    gpio_set_direction(TEST_GPIO_EXT_OUT_IO, GPIO_MODE_INPUT);
    TEST_ESP_OK(gpio_set_pull_mode(TEST_GPIO_EXT_OUT_IO, GPIO_PULLDOWN_ONLY));
    prompt_to_continue("mode: GPIO_MODE_INPUT");
}

static void drive_capability_set_get(gpio_num_t num, gpio_drive_cap_t capability)
{
    gpio_config_t pad_io = test_init_io(num);
    TEST_ESP_OK(gpio_config(&pad_io));
    TEST_ASSERT(gpio_set_drive_capability(num, GPIO_DRIVE_CAP_MAX) == ESP_ERR_INVALID_ARG);

    gpio_drive_cap_t cap;
    TEST_ESP_OK(gpio_set_drive_capability(num, capability));
    TEST_ESP_OK(gpio_get_drive_capability(num, &cap));
    TEST_ASSERT_EQUAL_INT(capability, cap);
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
 * weak capability < stronger capability < default capability = default capability2 < strongest capability
 *
 * all of these cases should be ignored that it will not run in CI
 */
TEST_CASE("GPIO_drive_capability_test", "[gpio][ignore]")
{
    printf("weak capability test! please view the current change!\n");
    drive_capability_set_get(TEST_GPIO_EXT_OUT_IO, GPIO_DRIVE_CAP_0);
    prompt_to_continue("If this test finishes");

    printf("stronger capability test! please view the current change!\n");
    drive_capability_set_get(TEST_GPIO_EXT_OUT_IO, GPIO_DRIVE_CAP_1);
    prompt_to_continue("If this test finishes");

    printf("default capability test! please view the current change!\n");
    drive_capability_set_get(TEST_GPIO_EXT_OUT_IO, GPIO_DRIVE_CAP_2);
    prompt_to_continue("If this test finishes");

    printf("default capability2 test! please view the current change!\n");
    drive_capability_set_get(TEST_GPIO_EXT_OUT_IO, GPIO_DRIVE_CAP_DEFAULT);
    prompt_to_continue("If this test finishes");

    printf("strongest capability test! please view the current change!\n");
    drive_capability_set_get(TEST_GPIO_EXT_OUT_IO, GPIO_DRIVE_CAP_3);
    prompt_to_continue("If this test finishes");
}

#if SOC_USB_SERIAL_JTAG_SUPPORTED // TODO: replace with a more proper soc_caps (USB_PHY_INTERNAL_NUM > 0)
TEST_CASE("GPIO_input_and_output_of_USB_pins_test", "[gpio]")
{
    const int test_pins[] = {USB_INT_PHY0_DP_GPIO_NUM,
                             USB_INT_PHY0_DM_GPIO_NUM,
#if CONFIG_IDF_TARGET_ESP32P4 // TODO: Use proper soc_caps macro
                             USB_INT_PHY1_DP_GPIO_NUM,
                             USB_INT_PHY1_DM_GPIO_NUM
#endif
                            };

    for (int i = 0; i < sizeof(test_pins) / sizeof(int); i++) {
        int pin = test_pins[i];
        gpio_config_t io_conf = {
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT_OUTPUT,
            .pin_bit_mask = BIT64(pin),
            .pull_down_en = 0,
            .pull_up_en = 0,
        };
        gpio_config(&io_conf);

        // test pin
        gpio_set_level(pin, 0);
        esp_rom_delay_us(10);
        // tested voltage is around 0v
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(pin), "get level error! the level should be low!");
        gpio_set_level(pin, 1);
        esp_rom_delay_us(10);
        // tested voltage is around 3.3v
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(pin), "get level error! the level should be high!");
        gpio_set_level(pin, 0);
        esp_rom_delay_us(10);
        // tested voltage is around 0v
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, gpio_get_level(pin), "get level error! the level should be low!");
        gpio_set_level(pin, 1);
        esp_rom_delay_us(10);
        // tested voltage is around 3.3v
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, gpio_get_level(pin), "get level error! the level should be high!");
    }
}

TEST_CASE("GPIO_USB_DP_pin_pullup_disable_test", "[gpio]")
{
    // This test ensures the USB D+ pin pull-up can be disabled
    // The pull-up value of the D+ pin is controlled by the pin's pull-up value together with the USB pull-up value.
    // If any one of the pull-up value is 1, the pin’s pull-up resistor will be enabled.
    // USB D+ pull-up value is default to 1 (USB_SERIAL_JTAG_DP_PULLUP)
    // Therefore, when D+ pin's pull-up value is set to 0, it will also clear USB D+ pull-up value to allow
    // its full functionality as a normal gpio pin
    const int test_pins[] = {USB_INT_PHY0_DP_GPIO_NUM,
#if CONFIG_IDF_TARGET_ESP32P4 // TODO: Use proper soc_caps macro
                             USB_INT_PHY1_DP_GPIO_NUM,
#endif
                            };

    for (int i = 0; i < sizeof(test_pins) / sizeof(int); i++) {
        int pin = test_pins[i];
        gpio_config_t input_io = test_init_io(pin);
        input_io.mode = GPIO_MODE_INPUT;
        input_io.pull_up_en = 0;
        input_io.pull_down_en = 1;
        gpio_config(&input_io);

        TEST_ASSERT_EQUAL_INT(0, gpio_get_level(pin));
    }
}
#endif //SOC_USB_SERIAL_JTAG_SUPPORTED

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4) // TODO: IDF-7528 Remove when light sleep is supported on ESP32P4
// Ignored in CI because it needs manually connect TEST_GPIO_INPUT_LEVEL_LOW_PIN to 3.3v to wake up from light sleep
TEST_CASE("GPIO_light_sleep_wake_up_test", "[gpio][ignore]")
{
    gpio_config_t io_config = test_init_io(TEST_GPIO_INPUT_LEVEL_LOW_PIN);
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pull_down_en = 1;
    gpio_config(&io_config);
    TEST_ESP_OK(gpio_wakeup_enable(TEST_GPIO_INPUT_LEVEL_LOW_PIN, GPIO_INTR_HIGH_LEVEL));
    TEST_ESP_OK(esp_sleep_enable_gpio_wakeup());
    printf("Entering light sleep... Please connect GPIO%d to 3.3v to wake up...\n", TEST_GPIO_INPUT_LEVEL_LOW_PIN);
    // Wait for the complete line to be printed before entering sleep
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_light_sleep_start();
    printf("Waked up from light sleep\n");
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO);
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(...)
