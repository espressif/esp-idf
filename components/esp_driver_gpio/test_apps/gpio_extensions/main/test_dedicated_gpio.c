/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_rom_sys.h"
#include "soc/soc_caps.h"
#include "hal/dedic_gpio_cpu_ll.h"
#include "driver/gpio.h"
#include "driver/dedic_gpio.h"

TEST_CASE("Dedicated_GPIO_bundle_install/uninstall", "[dedic_gpio]")
{
    const int test_gpios[SOC_DEDIC_GPIO_OUT_CHANNELS_NUM / 2] = {0};
    const int test2_gpios[SOC_DEDIC_GPIO_OUT_CHANNELS_NUM / 2 + 1] = {0};
    const int test3_gpios[SOC_DEDIC_GPIO_OUT_CHANNELS_NUM + 1] = {0};
    dedic_gpio_bundle_handle_t test_bundle, test_bundle2, test_bundle3 = NULL;
    dedic_gpio_bundle_config_t bundle_config = {
        .gpio_array = test_gpios,
        .array_size = sizeof(test_gpios) / sizeof(test_gpios[0]),
    };
    dedic_gpio_bundle_config_t bundle_config2 = {
        .gpio_array = test2_gpios,
        .array_size = sizeof(test2_gpios) / sizeof(test2_gpios[0]),
        .flags = {
            .out_en = 1,
        },
    };
    dedic_gpio_bundle_config_t bundle_config3 = {
        .gpio_array = test3_gpios,
        .array_size = sizeof(test3_gpios) / sizeof(test3_gpios[0]),
        .flags = {
            .out_en = 1,
        },
    };

    TEST_ASSERT_EQUAL_MESSAGE(ESP_ERR_INVALID_ARG, dedic_gpio_new_bundle(&bundle_config, &test_bundle), "shouldn't create bundle if no mode is specified");

    bundle_config.flags.out_en = 1;
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, dedic_gpio_new_bundle(&bundle_config, &test_bundle), "create bundle with half channels failed");
    uint32_t mask = 0;
    TEST_ESP_OK(dedic_gpio_get_out_mask(test_bundle, &mask));
    TEST_ASSERT_EQUAL_MESSAGE((1 << (SOC_DEDIC_GPIO_OUT_CHANNELS_NUM / 2)) - 1, mask, "wrong out mask");
    TEST_ESP_OK(dedic_gpio_get_in_mask(test_bundle, &mask));
    TEST_ASSERT_EQUAL_MESSAGE(0, mask, "wrong in mask");

    TEST_ASSERT_EQUAL_MESSAGE(ESP_ERR_NOT_FOUND, dedic_gpio_new_bundle(&bundle_config2, &test_bundle2), "shouldn't create bundle if there's no enough channels");
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, dedic_gpio_del_bundle(test_bundle), "delete bundle failed");

    TEST_ASSERT_EQUAL_MESSAGE(ESP_ERR_INVALID_ARG, dedic_gpio_new_bundle(&bundle_config3, &test_bundle3), "shouldn't create bundle if the array size exceeds maximum");
}

#define TEST_GPIO_GROUP_SIZE (4)

typedef struct {
    SemaphoreHandle_t sem;
    const int gpios[TEST_GPIO_GROUP_SIZE];
} test_dedic_task_context_t;

static void test_dedic_gpio_on_specific_core(void *args)
{
    test_dedic_task_context_t *ctx = (test_dedic_task_context_t *)args;
    uint32_t value = 0;
    dedic_gpio_cpu_ll_write_all(0x0); // clear all out channels

    // configure a group of GPIOs, output only
    const int bundleA_gpios[] = {ctx->gpios[0], ctx->gpios[1]};
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
    };
    for (int i = 0; i < sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]); i++) {
        io_conf.pin_bit_mask = 1ULL << bundleA_gpios[i];
        gpio_config(&io_conf);
    }
    // Create bundleA, output only
    dedic_gpio_bundle_handle_t bundleA = NULL;
    dedic_gpio_bundle_config_t bundleA_config = {
        .gpio_array = bundleA_gpios,
        .array_size = sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]),
        .flags = {
            .out_en = 1,
        },
    };
    TEST_ESP_OK(dedic_gpio_new_bundle(&bundleA_config, &bundleA));

    // configure another group of GPIOs, input and output
    const int bundleB_gpios[] = {ctx->gpios[2], ctx->gpios[3]};
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    for (int i = 0; i < sizeof(bundleB_gpios) / sizeof(bundleB_gpios[0]); i++) {
        io_conf.pin_bit_mask = 1ULL << bundleB_gpios[i];
        gpio_config(&io_conf);
    }

    // GPIO bundleB, input and output
    dedic_gpio_bundle_handle_t bundleB = NULL;
    dedic_gpio_bundle_config_t bundleB_config = {
        .gpio_array = bundleB_gpios,
        .array_size = sizeof(bundleB_gpios) / sizeof(bundleB_gpios[0]),
        .flags = {
            .in_en = 1,
            .out_en = 1,
        },
    };
    TEST_ESP_OK(dedic_gpio_new_bundle(&bundleB_config, &bundleB));

    dedic_gpio_bundle_write(bundleA, 0x01, 0x01);
    dedic_gpio_bundle_write(bundleB, 0x03, 0x03);

    value = dedic_gpio_cpu_ll_read_out();
    TEST_ASSERT_EQUAL(0x0D, value); // 1101
    value = dedic_gpio_cpu_ll_read_in();
    TEST_ASSERT_EQUAL(0x03, value); // 11

    dedic_gpio_bundle_write(bundleB, 0x02, 0x0);
    value = dedic_gpio_cpu_ll_read_out();
    TEST_ASSERT_EQUAL(0x05, value); // 0101
    value = dedic_gpio_cpu_ll_read_in();
    TEST_ASSERT_EQUAL(0x01, value); // 01

    dedic_gpio_cpu_ll_write_all(0x0F); // Set all out channels
    value = dedic_gpio_cpu_ll_read_out();
    TEST_ASSERT_EQUAL(0x0F, value);
    value = dedic_gpio_cpu_ll_read_in();
    TEST_ASSERT_EQUAL(0x03, value);                               // 11
    TEST_ASSERT_EQUAL(0x03, dedic_gpio_bundle_read_out(bundleA)); // 11
    TEST_ASSERT_EQUAL(0x00, dedic_gpio_bundle_read_in(bundleA));  // input is not enabled for bundleA
    TEST_ASSERT_EQUAL(0x03, dedic_gpio_bundle_read_out(bundleB)); // 11
    TEST_ASSERT_EQUAL(0x03, dedic_gpio_bundle_read_in(bundleB));  // 11

    TEST_ESP_OK(dedic_gpio_del_bundle(bundleA));
    TEST_ESP_OK(dedic_gpio_del_bundle(bundleB));

    xSemaphoreGive(ctx->sem);
    vTaskSuspend(NULL);
}

TEST_CASE("Dedicated_GPIO_run_on_multiple_CPU_cores", "[dedic_gpio]")
{
    SemaphoreHandle_t sem = xSemaphoreCreateCounting(SOC_CPU_CORES_NUM, 0);
    TaskHandle_t task_handle[SOC_CPU_CORES_NUM];

    for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
        int start_gpio = i * TEST_GPIO_GROUP_SIZE;
        test_dedic_task_context_t isr_ctx = {
            .sem = sem,
            .gpios = {start_gpio, start_gpio + 1, start_gpio + 2, start_gpio + 3}
        };
        xTaskCreatePinnedToCore(test_dedic_gpio_on_specific_core, "dedic_gpio_test_tsk", 4096, &isr_ctx, 1,
                                &task_handle[i], i);
    }

    for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
        xSemaphoreTake(sem, pdMS_TO_TICKS(1000));
    }

    vSemaphoreDelete(sem);
    for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
        unity_utils_task_delete(task_handle[i]);
    }
}

IRAM_ATTR static void test_dedic_gpio_isr_callback(void *args)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)args;
    BaseType_t high_task_wakeup = pdFALSE;
    esp_rom_printf("GPIO event\r\n");
    xSemaphoreGiveFromISR(sem, &high_task_wakeup);
    if (high_task_wakeup) {
        esp_rom_printf("high priority task wake up\r\n");
    }
}

TEST_CASE("Dedicated_GPIO_interrupt_and_callback", "[dedic_gpio]")
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();

    // configure GPIO
    const int bundle_gpios[] = {0, 1};
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
    };
    for (int i = 0; i < sizeof(bundle_gpios) / sizeof(bundle_gpios[0]); i++) {
        io_conf.pin_bit_mask = 1ULL << bundle_gpios[i];
        gpio_config(&io_conf);
    }
    dedic_gpio_bundle_handle_t bundle = NULL;
    dedic_gpio_bundle_config_t bundle_config = {
        .gpio_array = bundle_gpios,
        .array_size = sizeof(bundle_gpios) / sizeof(bundle_gpios[0]),
        .flags = {
            .in_en = 1,
            .out_en = 1,
        },
    };
    TEST_ESP_OK(dedic_gpio_new_bundle(&bundle_config, &bundle));

    // enable interrupt on GPIO1
    TEST_ESP_OK(gpio_set_intr_type(1, GPIO_INTR_POSEDGE));
    // install gpio isr service
    TEST_ESP_OK(gpio_install_isr_service(0));
    // hook isr handler for specific gpio pin
    TEST_ESP_OK(gpio_isr_handler_add(1, test_dedic_gpio_isr_callback, sem));

    // trigger a posedge on GPIO1
    dedic_gpio_bundle_write(bundle, BIT(1), 0x00);
    dedic_gpio_bundle_write(bundle, BIT(1), 0xFF);
    // wait for done semaphore
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));

    // remove isr handler for gpio number
    TEST_ESP_OK(gpio_isr_handler_remove(1));
    // uninstall GPIO interrupt service
    gpio_uninstall_isr_service();

    TEST_ESP_OK(dedic_gpio_del_bundle(bundle));
    vSemaphoreDelete(sem);
}
