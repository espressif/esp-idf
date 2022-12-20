/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "unity.h"
#include "driver/uart.h"
#include "esp_pm.h"
#include "esp_log.h"
#include "test_common.h"
#include "esp_private/sleep_cpu.h" //for sleep_cpu_configure

#define UART_TAG         "Uart"
#define TEST_BUF_SIZE    256

//This should be larger than FIFO_SIZE + 2 * TEST_DRIVER_BUF_SIZE, so that blocking will happen
#define TEST_WRITE_SIZE 1024

#define TEST_RTS   UART_PIN_NO_CHANGE
#define TEST_CTS   UART_PIN_NO_CHANGE

#define TEST_UART_BAUD_RATE     115200

#define MAX_FREQ    (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

#if CONFIG_XTAL_FREQ_40
#define MIN_FREQ    10
#elif CONFIG_XTAL_FREQ_32
#define MIN_FREQ    8
#elif CONFIG_XTAL_FREQ_26
#define MIN_FREQ    13
#endif

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4)
#if CONFIG_PM_ENABLE

TEST_CASE("uart tx won't be blocked by auto light sleep", "[uart]")
{
    uart_port_param_t port_param = {};
    TEST_ASSERT(port_select(&port_param));

    uart_port_t port_num = port_param.port_num;

    // Configure dynamic frequency scaling:
    // maximum and minimum frequencies are set in sdkconfig,
    // automatic light sleep is enabled if tickless idle support is enabled.
    esp_pm_config_t pm_config = {
        .max_freq_mhz = MAX_FREQ,
        .min_freq_mhz = MIN_FREQ,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
#endif
    };
    TEST_ESP_OK(esp_pm_configure(&pm_config));

    uart_config_t uart_config = {
        .baud_rate = TEST_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = port_param.default_src_clk,
    };
    int intr_alloc_flags = 0;

    TEST_ESP_OK(uart_driver_install(port_num, TEST_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    TEST_ESP_OK(uart_param_config(port_num, &uart_config));
    TEST_ESP_OK(uart_set_pin(port_num, port_param.tx_pin_num, port_param.rx_pin_num, TEST_RTS, TEST_CTS));

    // Configure a temporary buffer for the incoming data
    const int len = TEST_WRITE_SIZE;
    uint8_t *data = (uint8_t *) malloc(len);

    //If auto lightsleep happen, there will be deadlock in either one of the two following functions
    uart_write_bytes(port_num, (const char *) data, len);
    uart_wait_tx_done(port_num, portMAX_DELAY);

    ESP_LOGI(UART_TAG, "return from uart_write_bytes");

    uart_driver_delete(port_num);
    free(data);

#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP
    //When PD_CPU enabled, retention may cause 14K memory leak. Workaround to release the memory
    sleep_cpu_configure(false);
#endif
}
#endif // CONFIG_PM_ENABLE
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4)
